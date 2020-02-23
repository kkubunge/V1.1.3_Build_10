#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>
#include <Kutlgui.h>

#include "cimseqnc.h"
#include "iodefine.h"
#include "resource.h"
#include "IPS_Log.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define DEFAULT_MID_CHECK_CONFIG	".\\Parameter\\MID_List.cfg"
//------------------------------------------------------------------------------------------
#define	CM1_MID_Double_Checked_Error			272
#define	CM2_MID_Double_Checked_Error			572
#define	CM3_MID_Double_Checked_Error			872
//------------------------------------------------------------------------------------------
#define MAX_CMT_CHAR	32
#define MAX_LOT			101
//------------------------------------------------------------------------------------------
typedef struct{
	char szMID[MAX_CMT_CHAR];
}SPRJobInfo;

SPRJobInfo g_nMID_List[MAX_LOT];
//------------------------------------------------------------------------------------------
int g_nMID_CurrentNo;
char m_szConfigPath[80];
//------------------------------------------------------------------------------------------
enum {Check, NotCheck};
//------------------------------------------------------------------------------------------
BOOL	CONFIRM_OK				= FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "e:SCHEDULER"						, _K_S_IO	,	S_SCHEDULER						,	0	} ,
	{ "PRM_System_MID_Check"			, _K_D_IO	,	PRM_System_MID_Check			,	0	} ,
	{ "PRM_System_MID_CheckParam"		, _K_S_IO	,	PRM_System_MID_CheckParam		,	0	} ,
	""
};
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_Confirm_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	static int Type;
	char Buffer[256];
	
	switch ( msg ) {
		case WM_INITDIALOG:
			//MoveCenterWindow( hdlg );
			 SetWindowPos( hdlg , HWND_TOPMOST , 257 , 257 , 0 , 0 , SWP_NOSIZE );
			CONFIRM_OK = FALSE;
			sprintf( Buffer , "MID Check LOAD CONFIRM WINDOW");
			SetWindowText( hdlg , Buffer );
			SendDlgItemMessage( hdlg , IDC_MESSAGE , WM_SETTEXT , 0 , lParam );
			return TRUE;
				
		case WM_COMMAND :
			switch (wParam) {
				case IDOK :
					CONFIRM_OK = TRUE;
					EndDialog( hdlg , 0 );
					return TRUE;
				
				case IDCANCEL :
					CONFIRM_OK	= FALSE;
					EndDialog( hdlg , 0 );
					return FALSE;
			}
			return TRUE;

		case WM_DESTROY:
			return TRUE;
	}

	return FALSE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {

	int i, nDatalength, nCommStatus, nCheckNotCheck, nMIDCheckDataLength, nResult;
	BOOL bConfirmPressed = FALSE;
	char *pszEd;
	char szEvent[256], szBuffer1[256], szPortNum[256], szMID[256], szFOUPID[32];
	char szMessage[256], szMIDCKD[16];

	memset(szEvent,			0,  sizeof(szEvent));
	memset(szBuffer1,		0,  sizeof(szBuffer1));
	memset(szPortNum,		0,	sizeof(szPortNum));
    memset(szMID,			0,	sizeof(szMID));
	memset(szFOUPID,		0,	sizeof(szFOUPID));
	memset(szMessage,		0,	sizeof(szMessage));
	memset(szMIDCKD,		0,	sizeof(szMIDCKD));

	pszEd = PROGRAM_PARAMETER_READ();
	nDatalength = strlen(pszEd);
	pszEd[ nDatalength ] = 0x00;

	if ( nDatalength < 1 ) {
		printf( "===###===> MID Check Manager PROGRAM_PARAMETER_READ Error. nDatalength is 0\n");
		return SYS_ABORTED;
	}
	

	READ_STRING(PRM_System_MID_CheckParam, szMIDCKD, &nCommStatus);

	STR_SEPERATE( pszEd , szEvent , szBuffer1	, 255 );
	STR_SEPERATE_CHAR( szBuffer1 ,  '|', szPortNum , szMID, 255 );

//	printf("===###===###===> [pszEd : %s ]\n",pszEd);
	
	sprintf(szFOUPID, szMID, &nCommStatus);
//	printf("===###===###===> [FOUPID : %s ]\n",szFOUPID);

	if ( MAX_LOT == g_nMID_CurrentNo ) g_nMID_CurrentNo = 1;

	nCheckNotCheck = READ_DIGITAL(PRM_System_MID_Check, &nCommStatus);
	if ( Check == nCheckNotCheck ) {
		nMIDCheckDataLength = strlen(szMIDCKD);
		for( i=1; i<MAX_LOT; i++) {
			if( STRCMP_L(g_nMID_List[i].szMID, szFOUPID) ) {
				nResult = STRNCMP_L(szMIDCKD, szFOUPID, nMIDCheckDataLength);
				if ( (nMIDCheckDataLength != 0) && (nResult == TRUE) ) {
//					printf("===###===###===> szMIDCKD Parameter is Same  [FOUPID: %s] = [MIDCHK: %s]\n",szFOUPID, szMIDCKD);
					return SYS_SUCCESS;
				}
				else {
//					printf("===###===###===> szMIDCKD Parameter is Same  [FOUPID: %s] = [MIDCHK: %s]\n",szFOUPID, szMIDCKD);
					if(STRCMP_L(szPortNum, "CM1")) {
						ALARM_POST(CM1_MID_Double_Checked_Error);
						return SYS_ABORTED;
					}
					else if(STRCMP_L(szPortNum, "CM2")) {
						ALARM_POST(CM2_MID_Double_Checked_Error);
						return SYS_ABORTED;
					}
					else if(STRCMP_L(szPortNum, "CM3")) {
						ALARM_POST(CM3_MID_Double_Checked_Error);
						return SYS_ABORTED;
					}
				}
/*				ALARM_POST(MID_Double_Checked_Error);
				while(TRUE)
				{
					sprintf( szMessage, "Current FOUPID : [ %s ]!!!", szFOUPID);
					GoModalDialogBoxParam( GETHINST(NULL) , MAKEINTRESOURCE( IDD_USER_PAD), GET_MAIN_WINDOW() /*GetTopWindow(NULL)*/ //, Console_Gui_For_Confirm_Proc , (LPARAM)szMessage );
//					if ( TRUE == CONFIRM_OK ) 
//					{
//						bConfirmPressed = TRUE;
//						return SYS_SUCCESS;
//						break;
//					}
//					else 
//					{
//						if(STRCMP_L(szPortNum, "CM1")) {
//							WRITE_STRING( S_SCHEDULER , "CANCEL" , &nCommStatus );
//							return SYS_ABORTED;
//							bConfirmPressed = FALSE;
//							break;
//						}
//						else if(STRCMP_L(szPortNum, "CM2")) {
//							WRITE_STRING( S_SCHEDULER , "CANCEL2" , &nCommStatus );
//							return SYS_ABORTED;
//							bConfirmPressed = FALSE;
//							break;
//						}
//						else if(STRCMP_L(szPortNum, "CM3")) {
//							WRITE_STRING( S_SCHEDULER , "CANCEL3" , &nCommStatus );
//							return SYS_ABORTED;
//							bConfirmPressed = FALSE;
//							break;
//						}
//					}
//				}
//				return SYS_ABORTED;
			}
		}
	}
	
	for( i=1; i<MAX_LOT; i++) {
		if( STRCMP_L(g_nMID_List[i].szMID, "NULL") ) {
		    sprintf(g_nMID_List[i].szMID, szFOUPID);
			g_nMID_CurrentNo = i+1;
			break;
		}
		else {
			sprintf(g_nMID_List[g_nMID_CurrentNo].szMID, szFOUPID);
			g_nMID_CurrentNo = g_nMID_CurrentNo + 1;
			break;
		}
	}
/*	for( i=1; i<MAX_LOT; i++) {
		nDatalength = strlen(g_nMID_List[i].szMID);
		if( 0 < nDatalength ) {
			printf("===###===###===> [MID : %s ]\n",g_nMID_List[i].szMID);
		}
	}
*/	return SYS_SUCCESS;
}
BOOL Load_MID_List() {
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[2048];
	int nDatalength, i;

	memset(szRead, 0, sizeof(szRead));

	printf("===###===###===> Load_MID_List()!!!\n");

	do {
		Sleep(500);
		fp = fopen(m_szConfigPath, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 2048, fp)) {
//			printf("===###===###===> [szRead : NULL ]\n");
			g_nMID_CurrentNo = 1;
			bRet = TRUE;
			break;
		}
//		printf("===###===###===> [szRead : %s ]\n",szRead);
		for(i=0;i<MAX_LOT;i++) {
			STR_SEPERATE_CHAR(szRead, '|', g_nMID_List[i].szMID, szRead, 2048);
//			printf("===###===###===> [MID : %s ]\n",g_nMID_List[i].szMID);
			nDatalength = strlen(szRead);
			if( 0 == nDatalength ) {
//				printf("===###===###===> Load_MID_List() - nDatalength = 0 \n");
				bRet = TRUE;
				break;
			}
		}
		g_nMID_CurrentNo = atoi(g_nMID_List[0].szMID);
		bRet = TRUE;
	} while(0);
	if(fp != NULL) fclose(fp);
	return bRet;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Save_MID_List() {
	BOOL bRet = FALSE;
	FILE *fp;
	char szSave[2048];
	char szSeperateCh[] = "|";
	char szMID_CurrentNo[3];
	int nDatalength, nDatalength_Next, i;

	memset(szSave, 0, sizeof(szSave));
	memset(szMID_CurrentNo, 0, sizeof(szMID_CurrentNo));

	printf("===###===###===> Save_MID_List()!!!\n");

	do {
		Sleep(500);
		fp = fopen(m_szConfigPath, "wt");
		if(fp == NULL) break;
		itoa(g_nMID_CurrentNo, szMID_CurrentNo, 10);
		sprintf(szSave, "%s|", szMID_CurrentNo);
		for(i=1;i<MAX_LOT;i++) {
			nDatalength = strlen(g_nMID_List[i].szMID);
			if( 0 != nDatalength ) {
				strcat(szSave, g_nMID_List[i].szMID);
				nDatalength_Next = strlen(g_nMID_List[i+1].szMID);
				if( (MAX_LOT > i) && (0 != nDatalength_Next) ) {
					strcat(szSave, szSeperateCh);
				}
			}
			else {
//				printf("===###===###===> Save_MID_List()!!! - nDatalength = 0 \n");
				bRet = TRUE;
				break;
			}
		}
//		printf("===###===###===> [szSave : %s ]\n",szSave);
		fputs(szSave, fp);
		bRet = TRUE;
	} while(0);
	if(fp != NULL) fclose(fp);
	return bRet;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		Save_MID_List();
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Program_Enter_Code() {
		SYSTEM_KILL_WITH_MESSAGE();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	BOOL bRet = FALSE;
	strcpy(m_szConfigPath, DEFAULT_MID_CHECK_CONFIG);
//	printf("===###===###===> Program_Init_Code!!!\n");
	bRet = Load_MID_List();
	return bRet;
}
