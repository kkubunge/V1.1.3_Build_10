#include <stdio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>
#include <io.h>

#include <windows.h>
#include <wingdi.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>

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

BOOL APIENTRY InfoDisplay_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int RcpDown , result , N2Purge_USE, NF3Seprate ;
	static int SIM_Sel ;
	static int ATM_Comm_Sel ;
	static int SFEM_TYPE_SEL ; //SFEM Type
	static int EDA_CONTROL; //->2012/04/12 EES CONTROL OFF/ON 기능 추가 by JTM
	FILE *fpt = NULL;
	FILE *fpt2 = NULL; 
	char Path[MAX_PATH] ;
	char Buffer[1024] ;
	char chr_return1[ 1024 ] = "\0";
	char chr_return2[ 1024 ] = "\0";
	char chr_return3[ 1024 ] = "\0";
	char chr_return4[ 1024 ] = "\0";
	char Path_ = 34;
	char Path_Aligner[ 256 ] = "TMC_FMALIGNER_CONTROL|%s";
	char Path_CM1[ 16 ] = "CM1";
	char Path_CM2[ 16 ] = "CM2";
	char Path_CM3[ 16 ] = "CM3";
	int Line , ReadCnt, i = 0 ;
	BOOL FileEnd = TRUE;
	char Radio_Bt_Chk_Msg[ 256 ] ;
	BOOL FINE_CHECK = FALSE;
	char Process[24] = "Cims97_h.exe *32";

	if (g_bExcuteCheck == TRUE)
	{
		ShowWindow(GetDlgItem(hdlg , IDOK), SW_HIDE);

		//for ( i = IDC_RADIO_RCPDOWN_NO; i <= IDC_RADIO_NF3_YES; i++ )	EnableWindow(GetDlgItem(hdlg, i), FALSE);
	}

	switch ( msg ) {
	case WM_INITDIALOG:

		if ( ( fpt = fopen( "System/Equipment_Set.cfg" , "r+t" ) ) == NULL ) {
			sprintf( Buffer , "0" );
			MessageBox( hdlg , "Cannot Find Equipment_Set.cfg File!!" , "ERROR" , MB_ICONHAND );
			RcpDown = 0 ;
			SFEM_TYPE_SEL = 0;
			EDA_CONTROL = 0;
			SIM_Sel = 0 ;
			ATM_Comm_Sel = 0;
			N2Purge_USE	= 0;
			NF3Seprate = 0;
			MoveCenterWindow( hdlg );
			return TRUE;
		}
		else { 
			for ( Line = 1 ; FileEnd ; Line++ ) {
				FileEnd = Get_Line_String_From_File( fpt , Buffer , &ReadCnt );
				if ( ReadCnt > 0 ) {
					if ( !Get_Data_From_String( Buffer , chr_return1 , 0 ) ) {
					}else {
						// SFEM TYPE
						if( STRCMP_L( chr_return1 , "SFEM_TYPE:" )){
							if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
								printf( "Unknown Error 2 - [%s]-[%d]\n" , Buffer , Line );
							}else {
								if( atoi(chr_return2) == 1 ) {
									CheckRadioButton(hdlg, IDC_RADIO_SFEM_VER1 , IDC_RADIO_SFEM_VER2 , IDC_RADIO_SFEM_VER1 ) ;
									SFEM_TYPE_SEL = 1 ;
								}else if( atoi(chr_return2) == 2 ) {
									CheckRadioButton(hdlg, IDC_RADIO_SFEM_VER1 , IDC_RADIO_SFEM_VER2 , IDC_RADIO_SFEM_VER2 ) ;
									SFEM_TYPE_SEL = 2 ;
								}else SFEM_TYPE_SEL = 1 ;
							}
						}

						// Recipe DownLoad System 
						else if ( STRCMP_L ( chr_return1 , "RECIPE_SEL:" )) {
							if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
							}else {
								if( atoi(chr_return2) == 1 ) {
									CheckRadioButton(hdlg, IDC_RADIO_RCPDOWN_NO , IDC_RADIO_RCPDOWN_YES , IDC_RADIO_RCPDOWN_NO ) ;
									RcpDown = 1 ;
								}else if( atoi(chr_return2) == 2 ){
									CheckRadioButton(hdlg, IDC_RADIO_RCPDOWN_NO , IDC_RADIO_RCPDOWN_YES , IDC_RADIO_RCPDOWN_YES ) ;
									RcpDown = 2 ;
								}else RcpDown = 0 ;
							}
						}
						
						// EDA CONTROL
						else if ( STRCMP_L ( chr_return1 , "EDA_CONTROL:" )) {
							if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
							}else {
								if( atoi(chr_return2) == 1 ) {
									CheckRadioButton(hdlg, IDC_RADIO_EDA_OFF , IDC_RADIO_EDA_ON , IDC_RADIO_EDA_OFF ) ;
									EDA_CONTROL = 1 ;
								}else if( atoi(chr_return2) == 2 ){
									CheckRadioButton(hdlg, IDC_RADIO_EDA_OFF , IDC_RADIO_EDA_ON , IDC_RADIO_EDA_ON ) ;
									EDA_CONTROL = 2 ;
								}else EDA_CONTROL = 1 ;
							}
						}
						
						// SIMMULATION MODE
						else if ( STRCMP_L ( chr_return1 , "SIMMULATION_MODE:" )) {
							if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
							}else {
								if( atoi(chr_return2) == 1 ) {
									CheckRadioButton(hdlg, IDC_RADIO_SIM_OFF , IDC_RADIO_SIM_ON , IDC_RADIO_SIM_OFF ) ;
									SIM_Sel = 1 ;
								}else if( atoi(chr_return2) == 2 ){
									CheckRadioButton(hdlg, IDC_RADIO_SIM_OFF , IDC_RADIO_SIM_ON , IDC_RADIO_SIM_ON ) ;
									SIM_Sel = 2 ;
								}else SIM_Sel = 1 ;
							}
						}
					
						// ATM COMM TYPE 
						else if ( STRCMP_L ( chr_return1 , "ATM_COMM_TYPE:" )) {
							if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
							}else {
								if( atoi(chr_return2) == 1 ) {
									CheckRadioButton(hdlg, IDC_RADIO_ATM_RS232_NO , IDC_RADIO_ATM_ETHERNET_ALIGN , IDC_RADIO_ATM_RS232_NO ) ;
									ATM_Comm_Sel = 1 ;
								}else if( atoi(chr_return2) == 2 ){
									CheckRadioButton(hdlg, IDC_RADIO_ATM_RS232_NO , IDC_RADIO_ATM_ETHERNET_ALIGN , IDC_RADIO_ATM_RS232_ALIGN ) ;
									ATM_Comm_Sel = 2 ;
								}else if( atoi(chr_return2) == 3 ){
									CheckRadioButton(hdlg, IDC_RADIO_ATM_RS232_NO , IDC_RADIO_ATM_ETHERNET_ALIGN , IDC_RADIO_ATM_ETHERNET_NO ) ;
									ATM_Comm_Sel = 3 ;
								}else if( atoi(chr_return2) == 4 ){
									CheckRadioButton(hdlg, IDC_RADIO_ATM_RS232_NO , IDC_RADIO_ATM_ETHERNET_ALIGN , IDC_RADIO_ATM_ETHERNET_ALIGN ) ;
									ATM_Comm_Sel = 4 ;
								}else ATM_Comm_Sel = 1 ;
							}
						}

						// LPM N2 PURGE 
						else if ( STRCMP_L ( chr_return1 , "N2PURGE_USE:" )) {
							if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
							}else {
								if( atoi(chr_return2) == 1 ) {
									CheckRadioButton(hdlg, IDC_RADIO_N2_PURGE_NO , IDC_RADIO_N2_PURGE_USE , IDC_RADIO_N2_PURGE_NO ) ;
									N2Purge_USE = 1 ;
								}else if( atoi(chr_return2) == 2 ){
									CheckRadioButton(hdlg, IDC_RADIO_N2_PURGE_NO , IDC_RADIO_N2_PURGE_USE , IDC_RADIO_N2_PURGE_USE ) ;
									N2Purge_USE = 2 ;
								}else N2Purge_USE = 1;
							}
						}

						// PM NF3 SEPARATE 
						else if ( STRCMP_L ( chr_return1 , "NF3SEPA_USE:" )) {
							if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
							}else {
								if( atoi(chr_return2) == 1 ) {
									CheckRadioButton(hdlg, IDC_RADIO_NF3_NO , IDC_RADIO_NF3_YES , IDC_RADIO_NF3_NO ) ;
									NF3Seprate = 1 ;
								}else if( atoi(chr_return2) == 2 ){
									CheckRadioButton(hdlg, IDC_RADIO_NF3_NO , IDC_RADIO_NF3_YES , IDC_RADIO_NF3_YES ) ;
									NF3Seprate = 2 ;
								}else NF3Seprate = 1;
							}
						}

					}
				}
			}
			fclose ( fpt ) ;
		}
		
		MoveCenterWindow( hdlg );
		return TRUE;

	case WM_COMMAND :

		if ( HIWORD(wParam) == BN_CLICKED ){
			switch(LOWORD(wParam)) {
			case IDC_RADIO_SFEM_VER1 :
				SFEM_TYPE_SEL = 1 ;
				break; 
			case IDC_RADIO_SFEM_VER2 :
				SFEM_TYPE_SEL = 2 ;
				break;
			case IDC_RADIO_RCPDOWN_NO:
				RcpDown = 1 ;
				break;
			case IDC_RADIO_RCPDOWN_YES:
				RcpDown = 2 ;
				break;
			case IDC_RADIO_EDA_OFF:
				EDA_CONTROL = 1 ; 
				break;
			case IDC_RADIO_EDA_ON:
				EDA_CONTROL = 2 ;
				break;
			case IDC_RADIO_SIM_OFF:
				SIM_Sel = 1 ;
				break;
			case IDC_RADIO_SIM_ON:
				SIM_Sel = 2 ;
				break;
			case IDC_RADIO_ATM_RS232_NO:
				ATM_Comm_Sel = 1;
				break;
			case IDC_RADIO_ATM_RS232_ALIGN:
				ATM_Comm_Sel = 2;
				break;
			case IDC_RADIO_ATM_ETHERNET_NO:
				ATM_Comm_Sel = 3;	
				break;	
			case IDC_RADIO_ATM_ETHERNET_ALIGN:
				ATM_Comm_Sel = 4;	
				break;	
			case IDC_RADIO_N2_PURGE_NO:
				N2Purge_USE = 1;
				break;
			case IDC_RADIO_N2_PURGE_USE:
				N2Purge_USE = 2;
				break;
			case IDC_RADIO_NF3_NO:
				NF3Seprate = 1;
				break;
			case IDC_RADIO_NF3_YES:
				NF3Seprate = 2;
				break;
			}
		}
		else {
			RcpDown = 0 ;
			SFEM_TYPE_SEL = 0 ;
			SIM_Sel = 0 ;
			EDA_CONTROL = 0;
			ATM_Comm_Sel = 0;
			N2Purge_USE = 0;
			NF3Seprate = 0;
		}

		switch( wParam ) {

		//OK Button
		case IDOK :
			result = TRUE;

			
			
			// EQ 설정하지 않은 항목 존재 시 Error Message 처리 (2012/12/21,LJH)
			// File 변경 전 Check해야, EasyCluster.cfg 내용이 삭제되지 않음.			
			sprintf ( Radio_Bt_Chk_Msg , "# NO SELECT ERROR\n- LIST\n" ) ;
			if ( !SIM_Sel ) { strcat ( Radio_Bt_Chk_Msg , ". SIMMULATION MODE\n" ); }
			if ( !ATM_Comm_Sel ) { strcat ( Radio_Bt_Chk_Msg , ". ATM COMM TYPE\n" ); }
			if ( !RcpDown ) { strcat ( Radio_Bt_Chk_Msg , ". RECIPE DOWNLOAD\n" ); }			
			if ( !EDA_CONTROL ) { strcat ( Radio_Bt_Chk_Msg , ". EES CONTROL\n" ); }
			if ( !SFEM_TYPE_SEL ) { strcat ( Radio_Bt_Chk_Msg , ". SFEM TYPE\n" ); }		
			if ( !N2Purge_USE ) { strcat ( Radio_Bt_Chk_Msg , ". N2 PURGE USE OPTION\n"); }
			if ( !NF3Seprate ) { strcat ( Radio_Bt_Chk_Msg , ". NF3 SEPARATE USE OPTION\n"); }

			if ( !RcpDown || !EDA_CONTROL || !SFEM_TYPE_SEL || !SIM_Sel || !ATM_Comm_Sel || !N2Purge_USE || !NF3Seprate ) 
			{
				MessageBox ( hdlg , Radio_Bt_Chk_Msg , "ERROR" , MB_ICONHAND ) ;
				return FALSE ;
			}			
			
			// SFEM1
			if(SFEM_TYPE_SEL == 1)
			{
				fpt  = fopen( "IO/Setup/Cfgs/System.cfg" , "r+t" );
				fpt2 = fopen( "IO/Setup/Cfgs/System_Backup.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : IO/Setup/Cfgs/System.cf)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						if ( strstr( Buffer, "IO/TMC/TM/Cfgs/DeviceNet.cfg" ) )
						{
							fprintf( fpt2, "$$$  F    IO/TMC/TM/Cfgs/DeviceNet.cfg\n");
						}
						else if( strstr( Buffer, "IO/TMC/TM/Cfgs/DeviceNet_SFEM2.cfg" ) )
						{
							fprintf( fpt2, "#$$$  F    IO/TMC/TM/Cfgs/DeviceNet_SFEM2.cfg\n");
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}	
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "IO/Setup/Cfgs/System.cfg" );
				rename( "IO/Setup/Cfgs/System_Backup.cfg", "IO/Setup/Cfgs/System.cfg" );
			}
			// SFEM2
			else if(SFEM_TYPE_SEL == 2)
			{
				fpt  = fopen( "IO/Setup/Cfgs/System.cfg" , "r+t" );
				fpt2 = fopen( "IO/Setup/Cfgs/System_Backup.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : (IO/Setup/Cfgs/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						if ( strstr( Buffer, "IO/TMC/TM/Cfgs/DeviceNet.cfg" ) )
						{
							fprintf( fpt2, "#$$$  F    IO/TMC/TM/Cfgs/DeviceNet.cfg\n");
						}
						else if( strstr( Buffer, "IO/TMC/TM/Cfgs/DeviceNet_SFEM2.cfg" ) )
						{
							fprintf( fpt2, "$$$  F    IO/TMC/TM/Cfgs/DeviceNet_SFEM2.cfg\n");
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}	
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "IO/Setup/Cfgs/System.cfg" );
				rename( "IO/Setup/Cfgs/System_Backup.cfg", "IO/Setup/Cfgs/System.cfg" );
			}
			// EDA Use
			if(EDA_CONTROL == 2)
			{
				if ( result ) result = CopyFile( "System\\Startup\\Engine\\F_GEM_EDA.dat" , "Function\\Function_Module_GEM\\F_GEM.dat", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Engine\\F_GEM_EDA.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// system
				fpt  = fopen( "Function/Function_Module_GEM/Cfg/System.cfg" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_GEM/Cfg/System_BackUp.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : (Function/Function_Module_GEM/Cfg/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Remote_FM(Align)
						if ( strstr( Buffer, "EDA_SYSTEM_USE" ) )
						{
							fprintf( fpt2, "EDA_SYSTEM_USE	YES\n");
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}	
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_GEM/Cfg/System.cfg" );
				rename( "Function/Function_Module_GEM/Cfg/System_BackUp.cfg", "Function/Function_Module_GEM/Cfg/System.cfg" );

			}
			// EDA Not Use
			else if(EDA_CONTROL == 1)
			{
				if ( result ) result = CopyFile( "System\\Startup\\Engine\\F_GEM.dat" , "Function\\Function_Module_GEM\\F_GEM.dat", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Engine\\F_GEM.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				// system
				fpt  = fopen( "Function/Function_Module_GEM/Cfg/System.cfg" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_GEM/Cfg/System_BackUp.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : (Function/Function_Module_GEM/Cfg/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Remote_FM(Align)
						if ( strstr( Buffer, "EDA_SYSTEM_USE" ) )
						{
							fprintf( fpt2, "#EDA_SYSTEM_USE	YES\n");
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}	
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_GEM/Cfg/System.cfg" );
				rename( "Function/Function_Module_GEM/Cfg/System_BackUp.cfg", "Function/Function_Module_GEM/Cfg/System.cfg" );

			}
			// Recipe download
			// Use
			if ( RcpDown == 2 )
			{
				if ( result ) result = CopyFile( "System\\Startup\\Engine\\File_RecipeDownLoad.cfg" , "Scheduler\\File.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Engine\\File_RecipeDownLoad.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\Startup\\Engine\\Recipe_RecipeDownload.cfg" , "Function\\Function_Module_GEM\\Cfg\\Recipe.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Engine\\Recipe_RecipeDownload.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }


				// system
				fpt  = fopen( "Function/Function_Module_GEM/Cfg/System.cfg" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_GEM/Cfg/System_BackUp.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : (Function/Function_Module_GEM/Cfg/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Remote_FM(Align)
						if ( strstr( Buffer, "PPID_FULLDOWN_OVERWRITE" ) )
						{
							fprintf( fpt2, "PPID_FULLDOWN_OVERWRITE	YES\n");
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}	
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_GEM/Cfg/System.cfg" );
				rename( "Function/Function_Module_GEM/Cfg/System_BackUp.cfg", "Function/Function_Module_GEM/Cfg/System.cfg" );

			}
			// Recipe download
			// No Use
			else if ( RcpDown == 1 )
			{
				if ( result ) result = CopyFile( "System\\Startup\\Engine\\File.cfg" , "Scheduler\\File.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Engine\\File.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\Startup\\Engine\\Recipe.cfg" , "Function\\Function_Module_GEM\\Cfg\\Recipe.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Engine\\Recipe.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// system
				fpt  = fopen( "Function/Function_Module_GEM/Cfg/System.cfg" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_GEM/Cfg/System_BackUp.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : (Function/Function_Module_GEM/Cfg/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Remote_FM(Align)
						if ( strstr( Buffer, "PPID_FULLDOWN_OVERWRITE" ) )
						{
							fprintf( fpt2, "PPID_FULLDOWN_OVERWRITE	NO\n");
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}	
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_GEM/Cfg/System.cfg" );
				rename( "Function/Function_Module_GEM/Cfg/System_BackUp.cfg", "Function/Function_Module_GEM/Cfg/System.cfg" );


			}
			// LoadPort LPM
			// No Use
			if ( N2Purge_USE == 1 )
			{
				// Cfgs
				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM1\\Cfgs\\Duraport.cfg" , "IO\\Tmc\\CM1\\Cfgs\\Duraport.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM1\\Cfgs\\Duraport.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM2\\Cfgs\\Duraport2.cfg" , "IO\\Tmc\\CM2\\Cfgs\\Duraport2.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM2\\Cfgs\\Duraport2.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM3\\Cfgs\\Duraport3.cfg" , "IO\\Tmc\\CM3\\Cfgs\\Duraport3.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM3\\Cfgs\\Duraport3.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM1\\Cfgs\\TEST-Duraport.cfg" , "IO\\Tmc\\CM1\\Cfgs\\TEST-Duraport.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM1\\Cfgs\\TEST-Duraport.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM2\\Cfgs\\TEST-Duraport2.cfg" , "IO\\Tmc\\CM2\\Cfgs\\TEST-Duraport2.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM2\\Cfgs\\TEST-Duraport2.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM3\\Cfgs\\TEST-Duraport3.cfg" , "IO\\Tmc\\CM3\\Cfgs\\TEST-Duraport3.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM3\\Cfgs\\TEST-Duraport3.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
	
				// Function
				fpt  = fopen( "Function/Function_Module_TMC/F_TMC.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_TMC/F_TMC_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_TMC\\F_TMC.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						if ( STRCMP_L( chr_return1, "CM1_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else if ( STRCMP_L( chr_return1, "CM2_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else if ( STRCMP_L( chr_return1, "CM3_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else if ( STRCMP_L( chr_return1, "CM1_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else if ( STRCMP_L( chr_return1, "CM2_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else if ( STRCMP_L( chr_return1, "CM3_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
					
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_TMC/F_TMC.dat" );
				rename( "Function/Function_Module_TMC/F_TMC_Backup.dat", "Function/Function_Module_TMC/F_TMC.dat" );
				
				// Function - Test
				fpt  = fopen( "Function/Function_Module_TMC/TEST-F_TMC.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_TMC/TEST-F_TMC_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : (Function\\Function_Module_TMC\\TEST-F_TMC.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						if ( STRCMP_L( chr_return1, "CM1_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else if ( STRCMP_L( chr_return1, "CM2_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else if ( STRCMP_L( chr_return1, "CM3_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else if ( STRCMP_L( chr_return1, "CM1_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else if ( STRCMP_L( chr_return1, "CM2_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else if ( STRCMP_L( chr_return1, "CM3_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	-	SUCCESS\n", chr_return1 );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
					
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_TMC/TEST-F_TMC.dat" );
				rename( "Function/Function_Module_TMC/TEST-F_TMC_Backup.dat", "Function/Function_Module_TMC/TEST-F_TMC.dat" );
				

				fpt  = fopen( "IO/Setup/Cfgs/System.cfg" , "r+t" );
				fpt2 = fopen( "IO/Setup/Cfgs/System_Backup.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : IO/Setup/Cfgs/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '.', chr_return1, chr_return2, 256 );
						
						// Load Driver
						if ( strstr(Buffer, "N2Duraport.dll"))
						{
							fprintf( fpt2, "#$$$  Module	DURA		LOAD	Driver/N2Duraport.dll		5	9600    8   2   0   0   0   0   0	2000\n");
						}
						else if ( strstr( Buffer, "N2Duraport2.dll" ) )
						{
							fprintf( fpt2, "#$$$  Module	DURA2		LOAD	Driver/N2Duraport2.dll		6	9600    8   2   0   0   0   0   0	2000\n");
						}
						else if ( strstr( Buffer, "N2Duraport3.dll" ) )
						{
							fprintf( fpt2, "#$$$  Module	DURA3		LOAD	Driver/N2Duraport3.dll		7	9600    8   2   0   0   0   0   0	2000\n");
						}
						else if ( strstr(Buffer, "Duraport.dll"))
						{
							fprintf( fpt2, "$$$  Module	DURA		LOAD	Driver/Duraport.dll		5	9600    8   2   0   0   0   0   0	2000\n");
						}
						else if ( strstr( Buffer, "Duraport2.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	DURA2		LOAD	Driver/Duraport2.dll		6	9600    8   2   0   0   0   0   0	2000\n");
						}
						else if ( strstr( Buffer, "Duraport3.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	DURA3		LOAD	Driver/Duraport3.dll		7	9600    8   2   0   0   0   0   0	2000\n");
						}
						
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "IO/Setup/Cfgs/System.cfg" );
				rename( "IO/Setup/Cfgs/System_Backup.cfg", "IO/Setup/Cfgs/System.cfg" );
			}
			// Use
			else if ( N2Purge_USE == 2 )
			{																
				// Cfgs
				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM1\\Cfgs\\Duraport(N2).cfg" , "IO\\Tmc\\CM1\\Cfgs\\Duraport.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM1\\Cfgs\\Duraport(N2).cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM2\\Cfgs\\Duraport2(N2).cfg" , "IO\\Tmc\\CM2\\Cfgs\\Duraport2.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM2\\Cfgs\\Duraport2(N2).cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM3\\Cfgs\\Duraport3(N2).cfg" , "IO\\Tmc\\CM3\\Cfgs\\Duraport3.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM3\\Cfgs\\Duraport3(N2).cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM1\\Cfgs\\TEST-Duraport(N2).cfg" , "IO\\Tmc\\CM1\\Cfgs\\TEST-Duraport.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM1\\Cfgs\\TEST-Duraport(N2).cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM2\\Cfgs\\TEST-Duraport2(N2).cfg" , "IO\\Tmc\\CM2\\Cfgs\\TEST-Duraport2.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM2\\Cfgs\\TEST-Duraport2(N2).cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				
				if ( result ) result = CopyFile( "System\\Startup\\N2Purge\\IO\\Tmc\\CM3\\Cfgs\\TEST-Duraport3(N2).cfg" , "IO\\Tmc\\CM3\\Cfgs\\TEST-Duraport3.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\N2Purge\\IO\\Tmc\\CM3\\Cfgs\\TEST-Duraport3(N2).cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// Function
				fpt  = fopen( "Function/Function_Module_TMC/F_TMC.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_TMC/F_TMC_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_TMC\\F_TMC.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						if ( STRCMP_L( chr_return1, "CM1_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Purge/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM1, Path_ );
						}
						else if ( STRCMP_L( chr_return1, "CM2_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Purge/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM2, Path_ );
						}
						else if ( STRCMP_L( chr_return1, "CM3_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Purge/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM3, Path_ );
						}
						else if ( STRCMP_L( chr_return1, "CM1_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Monitor/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM1, Path_ );
						}
						else if ( STRCMP_L( chr_return1, "CM2_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Monitor/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM2, Path_ );
						}
						else if ( STRCMP_L( chr_return1, "CM3_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Monitor/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM3, Path_ );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
					
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_TMC/F_TMC.dat" );
				rename( "Function/Function_Module_TMC/F_TMC_Backup.dat", "Function/Function_Module_TMC/F_TMC.dat" );


				// Function - Test
				fpt  = fopen( "Function/Function_Module_TMC/TEST-F_TMC.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_TMC/TEST-F_TMC_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_TMC\\TEST-F_TMC.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						if ( STRCMP_L( chr_return1, "CM1_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Purge/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM1, Path_ );
						}
						else if ( STRCMP_L( chr_return1, "CM2_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Purge/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM2, Path_ );
						}
						else if ( STRCMP_L( chr_return1, "CM3_N2_PURGE" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Purge/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM3, Path_ );
						}
						else if ( STRCMP_L( chr_return1, "CM1_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Monitor/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM1, Path_ );
						}
						else if ( STRCMP_L( chr_return1, "CM2_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Monitor/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM2, Path_ );
						}
						else if ( STRCMP_L( chr_return1, "CM3_N2_MONITOR" ) )
						{
							fprintf( fpt2, "%s	1	FUNCTION/FUNCTION_MODULE_TMC/OBJ_CM_N2Monitor/Sequence	ARGUMENT	%c%s%c\n", chr_return1, Path_, Path_CM3, Path_ );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
					
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_TMC/TEST-F_TMC.dat" );
				rename( "Function/Function_Module_TMC/TEST-F_TMC_Backup.dat", "Function/Function_Module_TMC/TEST-F_TMC.dat" );

				
				fpt  = fopen( "IO/Setup/Cfgs/System.cfg" , "r+t" );
				fpt2 = fopen( "IO/Setup/Cfgs/System_Backup.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : IO/Setup/Cfgs/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '.', chr_return1, chr_return2, 256 );
						
						// Load Driver
						if ( strstr(Buffer, "N2Duraport.dll"))
						{
							fprintf( fpt2, "$$$  Module	DURA		LOAD	Driver/N2Duraport.dll		5	9600    8   2   0   0   0   0   0	2000\n");
						}
						else if ( strstr( Buffer, "N2Duraport2.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	DURA2		LOAD	Driver/N2Duraport2.dll		6	9600    8   2   0   0   0   0   0	2000\n");
						}
						else if ( strstr( Buffer, "N2Duraport3.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	DURA3		LOAD	Driver/N2Duraport3.dll		7	9600    8   2   0   0   0   0   0	2000\n");
						}
						else if ( strstr(Buffer, "Duraport.dll"))
						{
							fprintf( fpt2, "#$$$  Module	DURA		LOAD	Driver/Duraport.dll		5	9600    8   2   0   0   0   0   0	2000\n");
						}
						else if ( strstr( Buffer, "Duraport2.dll" ) )
						{
							fprintf( fpt2, "#$$$  Module	DURA2		LOAD	Driver/Duraport2.dll		6	9600    8   2   0   0   0   0   0	2000\n");
						}
						else if ( strstr( Buffer, "Duraport3.dll" ) )
						{
							fprintf( fpt2, "#$$$  Module	DURA3		LOAD	Driver/Duraport3.dll		7	9600    8   2   0   0   0   0   0	2000\n");
						}

						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "IO/Setup/Cfgs/System.cfg" );
				rename( "IO/Setup/Cfgs/System_Backup.cfg", "IO/Setup/Cfgs/System.cfg" );
			}

			// Robot Option (ATM[Align/No], Ethernet[Align/No])
			if (ATM_Comm_Sel == 1)	// RS232 - No Align
			{
				// EQ_SETTING
				if ( result ) result = CopyFile( "System\\Startup\\Robot\\Parameter\\EQ_SETTING\\YASKAWA_RS232_NoAL.dat" , "Parameter\\EQ_SETTING\\YASKAWA_CFG.dat", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Robot\\Parameter\\EQ_SETTING\\YASKAWA_RS232_NoAL.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }


				// System.cfg
				if ( result ) result = CopyFile( "System\\Startup\\Engine\\System.cfg" , "Scheduler\\System.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Engine\\System.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }


				// Function
				fpt  = fopen( "Function/Function_Module_TMC/F_TMC.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_TMC/F_TMC_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_TMC\\F_TMC.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Align
						if ( STRCMP_L( chr_return1, "TMC_FMALIGNER_CONTROL" ) )
						{
							fprintf( fpt2, "%s	2	-	SUCCESS\n", chr_return1 );
						}
						// Rblow
						else if ( STRCMP_L( chr_return1, "TMC_FMRBLOW_CONTROL" ) )
						{
							fprintf( fpt2, "%s	2	FUNCTION/FUNCTION_MODULE_TMC/OBJ_FM_Rblow/Sequence\n", chr_return1 );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
					
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_TMC/F_TMC.dat" );
				rename( "Function/Function_Module_TMC/F_TMC_Backup.dat", "Function/Function_Module_TMC/F_TMC.dat" );
				
				// Function
				fpt  = fopen( "Function/Function_Module_Remote/Remote_FM.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_Remote/Remote_FM_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_Remote\\Remote_FM.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Remote_FM(Align)
						if ( STRCMP_L( chr_return1, "FM.ALIGN_WAFER" ) )
						{
							fprintf( fpt2, "%s	50	-	SUCCESS\n", chr_return1 );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}	
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_Remote/Remote_FM.dat" );
				rename( "Function/Function_Module_Remote/Remote_FM_Backup.dat", "Function/Function_Module_Remote/Remote_FM.dat" );
								
				
				// Cfgs
				fpt  = fopen( "IO/Setup/Cfgs/System.cfg" , "r+t" );
				fpt2 = fopen( "IO/Setup/Cfgs/System_Backup.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : IO/Setup/Cfgs/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '.', chr_return1, chr_return2, 256 );
						
						// Load Driver
						if ( strstr(Buffer, "YASKAWA_ATM_232.dll"))
						{
							fprintf( fpt2, "$$$  Module	YASKAWA		LOAD	Driver/YASKAWA_ATM_232.dll		2	9600	8   1   0   0   0   0   20	1000\n");
						}
						else if ( strstr( Buffer, "YASKAWA_ATM.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	YSKWE		uLOAD	Driver/YASKAWA_ATM.dll			0	0	0   0   0   0   0   0   20	1000\n");
						}
						else if ( strstr( Buffer, "YASKAWA_ATM_AL.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	YSKWE_AL	uLOAD	Driver/YASKAWA_ATM_AL.dll		0	0	0   0   0   0   0   0   20	1000\n");
						}
						// FM
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM.cfg" ) )
						{
							fprintf( fpt2, "#$$$	F    IO/TMC/FM/Cfgs/YASKAWA_ATM.cfg\n");
						}
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM_AL.cfg" ) )
						{
							fprintf( fpt2, "#$$$    F    IO/TMC/FM/Cfgs/YASKAWA_ATM_AL.cfg\n");
						}
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM_232.cfg" ) )
						{
							fprintf( fpt2, "$$$    F    IO/TMC/FM/Cfgs/YASKAWA_ATM_232.cfg\n");
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "IO/Setup/Cfgs/System.cfg" );
				rename( "IO/Setup/Cfgs/System_Backup.cfg", "IO/Setup/Cfgs/System.cfg" );
			
			}
			else if (ATM_Comm_Sel == 2)	// RS232 - Align
			{
				// EQ_SETTING
				if ( result ) result = CopyFile( "System\\Startup\\Robot\\Parameter\\EQ_SETTING\\YASKAWA_RS232_AL.dat" , "Parameter\\EQ_SETTING\\YASKAWA_CFG.dat", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Robot\\Parameter\\EQ_SETTING\\YASKAWA_RS232_AL.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }


				// System.cfg
				if ( result ) result = CopyFile( "System\\Startup\\Engine\\System_AL.cfg" , "Scheduler\\System.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Engine\\System_AL.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }


				// Function
				fpt  = fopen( "Function/Function_Module_TMC/F_TMC.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_TMC/F_TMC_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_TMC\\F_TMC.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );

					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
					
						// Align
						if ( STRCMP_L( chr_return1, "TMC_FMALIGNER_CONTROL" ) )
						{
							fprintf( fpt2, "%s	2	FUNCTION/FUNCTION_MODULE_TMC/OBJ_FM_Align/Sequence\n", chr_return1 );
						}
						// Rblow
						else if ( STRCMP_L( chr_return1, "TMC_FMRBLOW_CONTROL" ) )
						{
							fprintf( fpt2, "%s	2	FUNCTION/FUNCTION_MODULE_TMC/OBJ_FM_Rblow/Sequence\n", chr_return1 );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
					
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_TMC/F_TMC.dat" );
				rename( "Function/Function_Module_TMC/F_TMC_Backup.dat", "Function/Function_Module_TMC/F_TMC.dat" );

				
				// Function
				fpt  = fopen( "Function/Function_Module_Remote/Remote_FM.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_Remote/Remote_FM_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_Remote\\Remote_FM.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Remote_FM(Align)
						if ( STRCMP_L( chr_return1, "FM.ALIGN_WAFER" ) )
						{
							fprintf( fpt2, "%s	50	Engine/EXEC/MsgReplacer	ARGUEMENT	%c%s%c\n", chr_return1, Path_, Path_Aligner, Path_ );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}	
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_Remote/Remote_FM.dat" );
				rename( "Function/Function_Module_Remote/Remote_FM_Backup.dat", "Function/Function_Module_Remote/Remote_FM.dat" );

				// Cfgs
				fpt  = fopen( "IO/Setup/Cfgs/System.cfg" , "r+t" );
				fpt2 = fopen( "IO/Setup/Cfgs/System_Backup.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : IO/Setup/Cfgs/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '.', chr_return1, chr_return2, 256 );
						
						// Load Driver
						if ( strstr(Buffer, "YASKAWA_ATM_232.dll"))
						{
							fprintf( fpt2, "$$$  Module	YASKAWA		LOAD	Driver/YASKAWA_ATM_232.dll		2	9600	8   1   0   0   0   0   20	1000\n");
						}
						else if ( strstr( Buffer, "YASKAWA_ATM.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	YSKWE		uLOAD	Driver/YASKAWA_ATM.dll			0	0	0   0   0   0   0   0   20	1000\n");
						}
						else if ( strstr( Buffer, "YASKAWA_ATM_AL.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	YSKWE_AL	uLOAD	Driver/YASKAWA_ATM_AL.dll		0	0	0   0   0   0   0   0   20	1000\n");
						}
						// FM
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM.cfg" ) )
						{
							fprintf( fpt2, "#$$$	F    IO/TMC/FM/Cfgs/YASKAWA_ATM.cfg\n");
						}
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM_AL.cfg" ) )
						{
							fprintf( fpt2, "#$$$    F    IO/TMC/FM/Cfgs/YASKAWA_ATM_AL.cfg\n");
						}
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM_232.cfg" ) )
						{
							fprintf( fpt2, "$$$    F    IO/TMC/FM/Cfgs/YASKAWA_ATM_232.cfg\n");
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "IO/Setup/Cfgs/System.cfg" );
				rename( "IO/Setup/Cfgs/System_Backup.cfg", "IO/Setup/Cfgs/System.cfg" );

			}
			else if (ATM_Comm_Sel == 3)	// Ethernet - No Align
			{
				// EQ_SETTING
				if ( result ) result = CopyFile( "System\\Startup\\Robot\\Parameter\\EQ_SETTING\\YASKAWA_Ethernet.dat" , "Parameter\\EQ_SETTING\\YASKAWA_Ethernet.dat", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Robot\\Parameter\\EQ_SETTING\\YASKAWA_Ethernet.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// System.cfg
				if ( result ) result = CopyFile( "System\\Startup\\Engine\\System.cfg" , "Scheduler\\System.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Engine\\System.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// Function
				fpt  = fopen( "Function/Function_Module_TMC/F_TMC.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_TMC/F_TMC_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_TMC\\F_TMC.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Align
						if ( STRCMP_L( chr_return1, "TMC_FMALIGNER_CONTROL" ) )
						{
							fprintf( fpt2, "%s	2	-	SUCCESS\n", chr_return1 );
						}
						// Rblow
						else if ( STRCMP_L( chr_return1, "TMC_FMRBLOW_CONTROL" ) )
						{
							fprintf( fpt2, "%s	2	FUNCTION/FUNCTION_MODULE_TMC/OBJ_FM_Rblow_Ethernet/Sequence\n", chr_return1 );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
					
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_TMC/F_TMC.dat" );
				rename( "Function/Function_Module_TMC/F_TMC_Backup.dat", "Function/Function_Module_TMC/F_TMC.dat" );


				// Function
				fpt  = fopen( "Function/Function_Module_Remote/Remote_FM.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_Remote/Remote_FM_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_Remote\\Remote_FM.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Remote_FM(Align)
						if ( STRCMP_L( chr_return1, "FM.ALIGN_WAFER" ) )
						{
							fprintf( fpt2, "%s	50	-	SUCCESS\n", chr_return1 );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
					
					
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_Remote/Remote_FM.dat" );
				rename( "Function/Function_Module_Remote/Remote_FM_Backup.dat", "Function/Function_Module_Remote/Remote_FM.dat" );

				// Cfgs
				fpt  = fopen( "IO/Setup/Cfgs/System.cfg" , "r+t" );
				fpt2 = fopen( "IO/Setup/Cfgs/System_Backup.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : IO/Setup/Cfgs/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '.', chr_return1, chr_return2, 256 );

						// Load Driver
						if ( strstr(Buffer, "YASKAWA_ATM_232.dll"))
						{
							fprintf( fpt2, "$$$  Module	YASKAWA		uLOAD	Driver/YASKAWA_ATM_232.dll		2	9600	8   1   0   0   0   0   20	1000\n");
						}
						else if ( strstr( Buffer, "YASKAWA_ATM.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	YSKWE		LOAD	Driver/YASKAWA_ATM.dll			0	0	0   0   0   0   0   0   20	1000\n");
						}
						else if ( strstr( Buffer, "YASKAWA_ATM_AL.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	YSKWE_AL	uLOAD	Driver/YASKAWA_ATM_AL.dll		0	0	0   0   0   0   0   0   20	1000\n");
						}
						// FM
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM.cfg" ) )
						{
							fprintf( fpt2, "$$$		F    IO/TMC/FM/Cfgs/YASKAWA_ATM.cfg\n");
						}
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM_AL.cfg" ) )
						{
							fprintf( fpt2, "$$$    F    IO/TMC/FM/Cfgs/YASKAWA_ATM_AL.cfg\n");
						}
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM_232.cfg" ) )
						{
							fprintf( fpt2, "#$$$    F    IO/TMC/FM/Cfgs/YASKAWA_ATM_232.cfg\n");
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "IO/Setup/Cfgs/System.cfg" );
				rename( "IO/Setup/Cfgs/System_Backup.cfg", "IO/Setup/Cfgs/System.cfg" );

			}
			else if (ATM_Comm_Sel == 4)	// Ethernet - Align
			{
				// EQ_SETTING
				if ( result ) result = CopyFile( "System\\Startup\\Robot\\Parameter\\EQ_SETTING\\YASKAWA_Ethernet.dat" , "Parameter\\EQ_SETTING\\YASKAWA_Ethernet.dat", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Robot\\Parameter\\EQ_SETTING\\YASKAWA_Ethernet.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// System.cfg
				if ( result ) result = CopyFile( "System\\Startup\\Engine\\System_AL.cfg" , "Scheduler\\System.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\Engine\\System_AL.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				// Function
				fpt  = fopen( "Function/Function_Module_TMC/F_TMC.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_TMC/F_TMC_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_TMC\\F_TMC.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Align
						if ( STRCMP_L( chr_return1, "TMC_FMALIGNER_CONTROL" ) )
						{
							fprintf( fpt2, "%s	2	FUNCTION/FUNCTION_MODULE_TMC/OBJ_FM_Align_Ethernet/Sequence\n", chr_return1 );
						}
						// Rblow
						else if ( STRCMP_L( chr_return1, "TMC_FMRBLOW_CONTROL" ) )
						{
							fprintf( fpt2, "%s	2	FUNCTION/FUNCTION_MODULE_TMC/OBJ_FM_Rblow_Ethernet/Sequence\n", chr_return1 );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
					
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_TMC/F_TMC.dat" );
				rename( "Function/Function_Module_TMC/F_TMC_Backup.dat", "Function/Function_Module_TMC/F_TMC.dat" );

		



				// Function
				fpt  = fopen( "Function/Function_Module_Remote/Remote_FM.dat" , "r+t" );
				fpt2 = fopen( "Function/Function_Module_Remote/Remote_FM_Backup.dat", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : Function\\Function_Module_Remote\\Remote_FM.dat)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						STR_SEPERATE_CHAR( Buffer, '\t', chr_return1, chr_return2, 256 );
						
						// Remote_FM(Align)
						if ( STRCMP_L( chr_return1, "FM.ALIGN_WAFER" ) )
						{
							fprintf( fpt2, "%s	50	Engine/EXEC/MsgReplacer	ARGUEMENT	%c%s%c\n", chr_return1, Path_, Path_Aligner, Path_ );
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}	
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "Function/Function_Module_Remote/Remote_FM.dat" );
				rename( "Function/Function_Module_Remote/Remote_FM_Backup.dat", "Function/Function_Module_Remote/Remote_FM.dat" );


				// Cfgs
				fpt  = fopen( "IO/Setup/Cfgs/System.cfg" , "r+t" );
				fpt2 = fopen( "IO/Setup/Cfgs/System_Backup.cfg", "w+t" ); 
				
				if ( fpt == NULL || fpt2 == NULL )
				{
					sprintf( Buffer, "0" );
					MessageBox( hdlg , "Can't Open File : IO/Setup/Cfgs/System.cfg)!!" , "ERROR" , MB_ICONHAND );
					
					return FALSE;
				}
				else
				{
					fgets ( Buffer, sizeof(Buffer), fpt );
					
					while ( !feof( fpt) )
					{
						//STR_SEPERATE_CHAR( Buffer, '.', chr_return1, chr_return2, 256 );
						
						// Load Driver
						if ( strstr(Buffer, "YASKAWA_ATM_232.dll"))
						{
							fprintf( fpt2, "$$$  Module	YASKAWA		uLOAD	Driver/YASKAWA_ATM_232.dll		2	9600	8   1   0   0   0   0   20	1000\n");
						}
						else if ( strstr( Buffer, "YASKAWA_ATM.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	YSKWE		LOAD	Driver/YASKAWA_ATM.dll			0	0	0   0   0   0   0   0   20	1000\n");
						}
						else if ( strstr( Buffer, "YASKAWA_ATM_AL.dll" ) )
						{
							fprintf( fpt2, "$$$  Module	YSKWE_AL	LOAD	Driver/YASKAWA_ATM_AL.dll		0	0	0   0   0   0   0   0   20	1000\n");
						}
						// FM
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM.cfg" ) )
						{
							fprintf( fpt2, "$$$	F    IO/TMC/FM/Cfgs/YASKAWA_ATM.cfg\n");
						}
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM_AL.cfg" ) )
						{
							fprintf( fpt2, "$$$    F    IO/TMC/FM/Cfgs/YASKAWA_ATM_AL.cfg\n");
						}
						else if ( strstr( Buffer, "IO/TMC/FM/Cfgs/YASKAWA_ATM_232.cfg" ) )
						{
							fprintf( fpt2, "#$$$    F    IO/TMC/FM/Cfgs/YASKAWA_ATM_232.cfg\n");
						}
						else
						{
							fprintf( fpt2, "%s", Buffer );
						}
						fgets ( Buffer, sizeof(Buffer), fpt );
					}
				}
				
				fclose ( fpt2 );
				fclose ( fpt );
				
				remove( "IO/Setup/Cfgs/System.cfg" );
				rename( "IO/Setup/Cfgs/System_Backup.cfg", "IO/Setup/Cfgs/System.cfg" );
			}

			// NF3 Separate 
			if (NF3Seprate == 1)	// NO
			{
				// Function [Function_Module_Recipe_Editor], GUI
				if ( result ) result = CopyFile( "System\\Startup\\NF3Separate\\Function\\F_GUI_beforeNF3.dat" , "Function\\F_GUI.dat", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\NF3Separate\\Function\\F_GUI_beforeNF3.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
			}
			else if (NF3Seprate == 2)	// YES
			{
				// Function [Function_Module_Recipe_Editor], GUI
				if ( result ) result = CopyFile( "System\\Startup\\NF3Separate\\Function\\F_GUI.dat" , "Function\\F_GUI.dat", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\NF3Separate\\Function\\F_GUI.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
			}

			// SIM MODE
			if (SIM_Sel == 1)	// NO
			{
				// SIM 파일 제거
				/*if ( 0 != access("Sim-STARTUP.cfg", 0) )*/		remove("Sim-STARTUP.cfg");
				/*if ( 0 != access("Sim-RunModule97.bat", 0) )*/	remove("Sim-RunModule97.bat");

			}
			else if (SIM_Sel == 2)	// YES
			{
				// 
				if ( result ) result = CopyFile( "System\\Startup\\SImmulation\\Sim-STARTUP.cfg" , "Sim-STARTUP.cfg", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\SImmulation\\Sim-STARTUP.cfg)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				if ( result ) result = CopyFile( "System\\Startup\\SImmulation\\Sim-RunModule97.bat" , "Sim-RunModule97.bat", FALSE );
				else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\SImmulation\\Sim-RunModule97.bat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }

				if (NF3Seprate == 1)	// NO
				{
					// Function [Function_Module_Recipe_Editor], GUI
					if ( result ) result = CopyFile( "System\\Startup\\SImmulation\\Function\\F_SimGUI_beforeNF3.dat" , "Function\\F_SimGUI.dat", FALSE );
					else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\NF3Separate\\Function\\F_SimGUI_beforeNF3.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				}
				else if (NF3Seprate == 2)	// YES
				{
					// Function [Function_Module_Recipe_Editor], GUI
					if ( result ) result = CopyFile( "System\\Startup\\SImmulation\\Function\\F_SimGUI.dat" , "Function\\F_SimGUI.dat", FALSE );
					else { MessageBox( hdlg , "Cannot Copy File(System\\Startup\\NF3Separate\\Function\\F_SimGUI.dat)!!" , "ERROR" , MB_ICONHAND );	return FALSE; }
				}
			}
			
			sprintf( Path , "System\\Equipment_Set.cfg");
			fpt = fopen( Path , "w" );
			if( !fpt ) {/* printf( "Please Make Equipment_Set.cfg!! \n" ); */}
			else {
				fprintf( fpt , "#-- RECIPE DOWNLOAD				=> OFF : 1  , ON : 2\n");
				fprintf( fpt , "#-- SFEM TYPE                   => VER1 : 1 , VER2 : 2\n" );
				fprintf( fpt , "#-- EDA CONTROL                 => OFF : 1  , ON : 2\n" );
				fprintf( fpt , "#-- SIMMULATION MODE			=> OFF : 1  , ON : 2\n" ) ;
				fprintf( fpt , "#-- ATM COMM TYPE				=> RS232 - NO ALIGN : 1 , RS232 - ALIGN : 2 , ETHERNET - NO ALIGN : 3 , ETHERNET - ALIGN : 4\n" ) ;
				fprintf( fpt , "#-- LOAD PORT N2 PURGE USE		=> USE : 2  , NOTUSE : 1\n" ) ;
				fprintf( fpt , "#-- PM CHAMBER NF3 SEPARATE USE	=> USE : 2  , NOTUSE : 1\n" ) ;

				fprintf( fpt , "RECIPE_SEL: %d\n" , RcpDown ) ;
				fprintf( fpt , "SFEM_TYPE: %d\n" , SFEM_TYPE_SEL ) ;
				fprintf( fpt , "EDA_CONTROL: %d\n" , EDA_CONTROL ) ;
				fprintf( fpt , "SIMMULATION_MODE: %d\n" , SIM_Sel ) ;
				fprintf( fpt , "ATM_COMM_TYPE: %d\n" , ATM_Comm_Sel ) ;
				fprintf( fpt , "N2PURGE_USE: %d\n" , N2Purge_USE ) ;
				fprintf( fpt , "NF3SEPA_USE: %d\n" , NF3Seprate ) ;
				
				fclose( fpt );
			}
	
			if ( !result ) {
				MessageBox( hdlg , "Definition File Copy Fail! Last Success Information will be loaded." , "ERROR" , MB_ICONHAND );
			}

		//Cancel Button
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
