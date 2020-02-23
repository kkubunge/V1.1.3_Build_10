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

#include <VECTOR>
#include <algorithm>

extern "C"
{
#include <Kutlstr.h>
#include <Kutlanal.h>
#include <Kutlfile.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>
#include <CimSeqncKNFS.h>
#include "cimseqnc.h"
}
//#include "interface.h"
//#include "iodefine.h"

#include "resource.h"
#include "TextParser.h"

using namespace std;
 


//------------------------------------------------------------------------------------------
#define  PMC_DATALOG_CONFIG_FILE ".\\Function\\Function_Module_CTC\\OBJ_DataLog\\PMCDataLogConfig.ini"
#define  MAX_LOG_ITEM_COUNT		100

//------------------------------------------------------------------------------------------
typedef struct _tagLogData
{
	int  index;
	char stime[25];
	char values[MAX_LOG_ITEM_COUNT][32];
}LOGDATA, *LPLOGDATA;

//------------------------------------------------------------------------------------------
int  gColumnCnt = 0;
// long glogDatafp = -1;
vector < long > glogDatafp;
long glogDataCnt = 0;
LPLOGDATA gpLogData = NULL;

//------------------------------------------------------------------------------------------
typedef enum {
	FEF_FORWORD,
	FEF_BACKWORD
}FEF_ENUM;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef struct tagLOGGERLIST
{
	char szPMName[32];
	BOOL bLogStatus;
}LOGGERLIST;

LOGGERLIST g_LoggerList[5];
char       g_szFilePath[128];
int  g_nPMCount = 0;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	""
};
//------------------------------------------------------------------------------------------
HINSTANCE	BackBmpLib = NULL;
void		KGUI_Set_BACKGROUND( char *file ) { BackBmpLib = LoadLibrary( file ); }
HINSTANCE	KGUI_Get_BACKGROUND() { return BackBmpLib; }
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	MINI_SCREEN_MODE = FALSE;
BOOL	TITLE_MODE = FALSE;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
int		AUTO_DELETE_MONTH	= 0;
//---------------------------------------------------------------------------------------
// char	HISTORY_DIRECTORY[256];
char	IDSTR_NAME[256]; // 2004.01.07


char	SELECT_PMNAME[16];
char	SELECT_LOG_FNAME[256];


//--------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL    MONITOR_MODE = FALSE; // 2003.12.16
char    MONITOR_MODE_STRING[256]; // 2003.12.16
//--------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int  PROGRAM_GUI_SPAWNING = 0; // 2003.07.12



//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void Set_Prm_MINISCREEN_MODE( BOOL data ) { MINI_SCREEN_MODE = data; }
BOOL Get_Prm_MINISCREEN_MODE() { return MINI_SCREEN_MODE; }
//--------------------------------------------------------------------------------------------------
void Set_Prm_TITLE_MODE( BOOL data ) { TITLE_MODE = data; }
BOOL Get_Prm_TITLE_MODE() { return TITLE_MODE; }
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//void Set_Prm_AUTO_DELETE_MONTH( int data ) { AUTO_DELETE_MONTH = data; }
int  Get_Prm_AUTO_DELETE_MONTH() { return AUTO_DELETE_MONTH; }
//--------------------------------------------------------------------------------------------------
void Set_Prm_MONITOR_MODE( int data ) { // 2003.12.16
	MONITOR_MODE = data;
}
BOOL Get_Prm_MONITOR_MODE() { // 2003.12.16
	return MONITOR_MODE;
}

void Set_Prm_MONITOR_FOLDER( char *data ) { // 2003.12.16
	strncpy( MONITOR_MODE_STRING , data , 255 );
}
char *Get_Prm_MONITOR_FOLDER() { // 2003.12.16
	return MONITOR_MODE_STRING;
}
//--------------------------------------------------------------------------------------------------
extern "C" void Screen_Nav_Info( int no ) {
	int i;
	if ( no == -1 ) {
		KGUI_STANDARD_Window_Kill_Signal();
	}
	if ( ( no > 0 ) && ( PROGRAM_GUI_SPAWNING == 0 ) ) { // 2003.07.12
		PROGRAM_GUI_SPAWNING = 1;
		for ( i = 0 ; i < 200 ; i++ ) {
			_sleep(100);
			if ( PROGRAM_GUI_SPAWNING == 2 ) break;
		}
	}
	KGUI_STANDARD_Screen_Nav_Info( no );
/*
	if ( no > 0 ) {
		SendMessage( KGUI_STANDARD_Get_Screen_Handle() , WM_COMMAND , IDIGNORE , (LPARAM) NULL );
	}
*/
}
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
extern BOOL ReadManualDataLogConfig();
BOOL FileData_ListView_Init( HWND hWnd ) 
{
	/*
	 *	Manual Log를 기록하는 PMC 목록을 갱신한다.
	 */

	ListBox_ResetContent( hWnd );

	if(!ReadManualDataLogConfig())	return FALSE;

	for(int i=0; i<5; i++)
	{
		if(g_LoggerList[i].bLogStatus && g_LoggerList[i].szPMName[0] != 0)
		{
			ListBox_AddString( hWnd , g_LoggerList[i].szPMName );
		}
	}

	return TRUE;
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
/*
// 2013.09.04 Windows7 Crash Modify by hyuk
void LView_Insert_Column( HWND hWnd , int k , char *Msg , char *Msg2 , char *Msg3 , char *Msg4 , char *Msg5, char *Msg6) {

#ifdef _DEBUG
	printf("--->Insert Column [%d] [%s] [%s] [%s] [%s] [%s] [%s]\n", k, Msg, Msg2, Msg3, Msg4, Msg5, Msg6);
#endif

	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= k;
	lvi.iSubItem	= 0;
	lvi.pszText		= Msg;
	lvi.cchTextMax	= 300;
	lvi.iImage		= 0;
	lvi.lParam		= 0;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return;
	ListView_SetItemText( hWnd , k , 1 , Msg2 );
	ListView_SetItemText( hWnd , k , 2 , Msg3 );
	ListView_SetItemText( hWnd , k , 3 , Msg4 );
	ListView_SetItemText( hWnd , k , 4 , Msg5 );
	ListView_SetItemText( hWnd , k , 5 , Msg6 );
}
// 2013.09.04 Windows7 Crash Modify by hyuk
*/

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

// 2013.09.04 Windows7 Crash Modify by hyuk
void LView_Insert_Column( HWND hWnd ,int k , char *Msg , ...) {
	//2013.09.04	
	int Msg_Size = 6;
	char *pText = Msg;
	va_list marker;

	va_start(marker, Msg);

	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= k;
	lvi.iSubItem	= 0;
	lvi.pszText		= pText;
	lvi.cchTextMax	= 300;
	lvi.iImage		= 0;
	lvi.lParam		= 0;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return;

	pText = va_arg(marker, char*);
	//2013.09.04
	//for(int i=1; *pText != NULL; i++)
	for(int i=1; i < Msg_Size ; i++)
	{
		ListView_SetItemText( hWnd , k , i , pText );
#ifdef _DEBUG
		printf("pText : %s\n", pText);
#endif
		pText = va_arg(marker, char*);
		//if(*pText == NULL) break;
	}
	// return
	va_end(marker);
}
// 2013.09.04 Windows7 Crash Modify by hyuk

void LView_Insert_Column2( HWND hWnd , int dCnt, int k , char *Msg[]) {

	LV_ITEM lvi;
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= k;
	lvi.iSubItem	= 0;
	lvi.pszText		= Msg[0];
	lvi.cchTextMax	= 300;
	lvi.iImage		= 0;
	lvi.lParam		= 0;
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return;

	for(int i=1; i<dCnt; i++)
	{
		ListView_SetItemText( hWnd , k , i , Msg[i]);
	}
}



//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void InitializePMList()
{
	for(int i = 0 ; i < 5 ; i++)
	{
		memset(g_LoggerList[i].szPMName , 0 , sizeof(g_LoggerList[i].szPMName));
		g_LoggerList[i].bLogStatus  = FALSE;
	}	

	memset(g_szFilePath , 0 , sizeof(g_szFilePath));
	g_nPMCount = 0;
}


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
BOOL ReadManualDataLogConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead    [256];
	char szItem    [256];
	char szIOName  [32];
	char szFilePath[128];

	int  nIOType       = 0;
	int  nLogPeriod    = 1000;
	int  nDeletePeriod = 30;
	bool bLogOption    = FALSE;	
	char szPM[8]       = {0};

	InitializePMList();

	do {
		fp = fopen(PMC_DATALOG_CONFIG_FILE, "rt");
		if(fp == NULL)
		{
			printf("file %s open fail \n" , PMC_DATALOG_CONFIG_FILE);
			break;
		}
		if(NULL == fgets(szRead, 255, fp))
		{
			printf("file %s fgets fail \n" , PMC_DATALOG_CONFIG_FILE);
			break;
		}

		// printf("Reading --> %s \n" , szRead);
		do
		{
			CTextParser tp(szRead);
			tp.GetWord(szItem, 0);
			//printf("Reading --> %s \n" , szItem);

			if(szRead[0] == ';' || szRead[0] == '#' || szRead[0] == '\n' || szRead[0] == '\t')
			{
			}
			/*
			 *	Manual Log Main Directory 를 읽어 오고 있다.
			 */
			else if(0 == strcmp(szItem , "$_FILE_PATH"))
			{
				tp.GetString(szFilePath);
				strcpy(g_szFilePath , szFilePath);				
				// printf("File Path %s \n" , g_szFilePath);
			}
			else if(szItem[0] == '[')
			{
				tp.SetText(szItem);
				tp.GetChar();
				tp.GetString(szItem, ']');				
				// printf("Reading --> %s \n" , szItem);

				/*
				 *	PM Name을 읽고 있다.
				 */
				if((strcmp("PM1", szItem) != 0) && (strcmp("PM2" , szItem) != 0) && (strcmp("PM3" , szItem) != 0)
				   && (strcmp("PM4" , szItem) != 0) && (strcmp("PM5" , szItem) != 0))
				{
				   printf("Invalide PM configuration comming  \n");
				   return FALSE;
				}
				else
				{
					strcpy(szPM , szItem);					
					// printf("Reading --> %s \n" , szPM);
				}
			}
			/*
			 *	PM별 Manual Log Logging 가부
			 */
			else if(0 == strcmp(szItem , "$_LOG_OPTION"))
			{
				tp.GetBool(&bLogOption , 0);
			}
			/*
			 *	Logging 해야 할 I/O 정의를 시작한다는 표시
			 */
			else if(0 == strcmp(szItem , "$_IO_START"))
			{
				do
				{
					if(NULL == fgets(szRead, 255, fp))
					{
						printf("fgets fail --> %s \n" , szRead);
						break;
					}
					// printf("Reading --> %s \n" , szRead);

					CTextParser tp1(szRead);
					tp1.GetWord(szIOName, 0);
					
					if(szIOName[0] == ';' || szIOName[0] == '#' || szIOName[0] == '\n' || szIOName[0] == '\t' || szIOName[0] == NULL)
					{
					}
					/*
					 *	Logging 해야 할 I/O 정의를 끝냄을 표시
					 */
					else if(0 == strcmp(szIOName , "$_IO_END"))
					{
						if(bLogOption)
						{
							strcpy(g_LoggerList[g_nPMCount].szPMName, szPM);
							g_LoggerList[g_nPMCount].bLogStatus = TRUE;
						}
						if(++g_nPMCount>=5) return TRUE;
						break;
					}
				}while(feof(fp) == 0);
			}

			if(NULL == fgets(szRead, 255, fp)) //end of file
				break;

		} while(feof(fp) == 0);

		fclose(fp);
		bRet = TRUE;
	} while(0);
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : %s", PMC_DATALOG_CONFIG_FILE);
	}
	return bRet;

}

BOOL GetRecipeName(char *pPath, char *pFileName, char *pRecipeName)
{
	char szFile[MAX_PATH];
	char szBuf[512];
	char *pRecipe;
	FILE *fp;
	BOOL bRet = FALSE;

	sprintf(szFile, "%s/%s", pPath, pFileName);

	if((fp = fopen(szFile, "rt")) == NULL)
	{
		pRecipeName[0] = 0;
		return FALSE;
	}

	while(1)
	{
		fgets(szBuf, sizeof(szBuf)-1, fp);
		if( strstr(szBuf, "LOG START") != NULL)
		{
			pRecipe = strrchr(szBuf, ':');
			if(pRecipe != NULL)
			{
				CTextParser tp;
				tp.SetText(pRecipe+1);
				if(!tp.GetString(pRecipeName, '\r'))
				{
					pRecipeName[0] = 0;
					break;
				}
				bRet = TRUE;
				break;
			}
		}
	}

	fclose(fp);
	return bRet;
}

// added by ycchoi 2006.06.22 < 1.0.1.0 >
BOOL GetRecipeNameEndTimeEndState(HWND hdlg, char *pPath, char *pFileName, char *pRecipeName, char *pEndTime, BOOL *pbEndState)
{
	char szFile[MAX_PATH];
	char szBuf[512], szBuf2[512];
	char *pRecipe;
	FILE *fp;
	BOOL bRet = FALSE;
	long fsize;
	char LMonth[3], LDay[3], LHour[3], LMin[3], LSec[3], LmSec[4];

	sprintf(szFile, "%s/%s", pPath, pFileName);

	if((fp = fopen(szFile, "rt")) == NULL)
	{
		pRecipeName[0] = 0;
		return FALSE;
	}

	do
	{
		// Find Receipe Name
		while(!feof(fp))
		{
			fgets(szBuf, sizeof(szBuf)-1, fp);
			if( strstr(szBuf, "LOG START") != NULL)
			{
				pRecipe = strrchr(szBuf, ':');
				if(pRecipe != NULL)
				{
					CTextParser tp;
					tp.SetText(pRecipe+1);
					if(!tp.GetString(pRecipeName, '\r'))
					{
						pRecipeName[0] = 0;
						break;
					}
					bRet = TRUE;
					break;
				}
			}
		}
		if(!bRet) break;

		// Find End Time & End State
		fseek(fp, 0, SEEK_END);
		fsize = ftell(fp);

		// 정상 End시 Log size 326byte 보다 전체 file size가 컨지 확인하고 
		// End Log Size보다 작으면 File Start로 이동하고, 
		// End Log Size보다 크면, Log Size 보다 조금 큰 400byte 뒤고 이동 한다.
		if(fsize < 500)	fseek(fp, 0, SEEK_SET);
		else			fseek(fp, -500, SEEK_END);

		bRet = FALSE;

		while(!feof(fp))
		{
			szBuf[0] = 0;
			if(!fgets(szBuf, sizeof(szBuf)-1, fp)) break;

			strcpy(szBuf2, szBuf);	// complete 되지 못한 Recipe의 경우를 대비해서 이전 Line Backup

			if( strstr(szBuf, "LOG STOP") != NULL)
			{
				*pbEndState = TRUE;

				CTextParser tp(szBuf);

				// Parsing Log Month				
				if(!tp.GetWord(LMonth, '/')) {
					printf("Invalid Log File Format(Column-Month).");
					break;
				}
				// Parsing Log Day				
				if(!tp.GetWord(LDay, ' ')) {
					printf("Invalid Log File Format(Column-Day).");
					break;
				}
				// Parsing Log Hour				
				if(!tp.GetWord(LHour, ':')) {
					printf("Invalid Log File Format(Column-Hour).");
					break;
				}
				// Parsing Log Minute				
				if(!tp.GetWord(LMin, ':')) {
					printf("Invalid Log File Format(Column-Min).");
					break;
				}
				// Parsing Log Second				
				if(!tp.GetWord(LSec, ':')) {
					printf("Invalid Log File Format(Column-Sec).");
					break;
				}
				// Parsing Log mSecond				
				if(!tp.GetWord(LmSec, ':')) {
					printf("Invalid Log File Format(Column-mSec).");
					break;
				}

				sprintf(pEndTime, "%02d-%02d %02d:%02d:%02d:%03d", atoi(LMonth), atoi(LDay), atoi(LHour), atoi(LMin), atoi(LSec), atoi(LmSec));
				bRet = TRUE;
				break;
			}
		}

		if(!bRet)
		{
			*pbEndState = FALSE;

			CTextParser tp(szBuf2);

			// Parsing Log Month				
			if(!tp.GetWord(LMonth, '/')) {
				printf("Invalid Log File Format(Column-Month).");
				break;
			}
			// Parsing Log Day				
			if(!tp.GetWord(LDay, ' ')) {
				printf("Invalid Log File Format(Column-Day).");
				break;
			}
			// Parsing Log Hour				
			if(!tp.GetWord(LHour, ':')) {
				printf("Invalid Log File Format(Column-Hour).");
				break;
			}
			// Parsing Log Minute				
			if(!tp.GetWord(LMin, ':')) {
				printf("Invalid Log File Format(Column-Min).");
				break;
			}
			// Parsing Log Second				
			if(!tp.GetWord(LSec, ':')) {
				printf("Invalid Log File Format(Column-Sec).");
				break;
			}
			// Parsing Log mSecond				
			if(!tp.GetWord(LmSec, ':')) {
				printf("Invalid Log File Format(Column-mSec).");
				break;
			}

			sprintf(pEndTime, "%02d-%02d %02d:%02d:%02d:%03d", atoi(LMonth), atoi(LDay), atoi(LHour), atoi(LMin), atoi(LSec), atoi(LmSec));
			bRet = TRUE;		
		}
	}while(0);

	fclose(fp);
	return bRet;
}

typedef struct _tagLogFile
{
//	char no[10];
	char stime[25];
	char etime[25];
	char recipeName[MAX_PATH];
	char fileName[256];
	char endstate[25];
}LOGFILE,*LPLOGFILE;

class StartTimeLess{
public:
	BOOL operator()(const LOGFILE& p, const LOGFILE& q) const
	{
		return (strcmp(p.stime, q.stime)<0) ? TRUE : FALSE;
	}
};

// added by ycchoi 2006.06.22 < 1.0.1.0 >
class StartTimeMore{
public:
	BOOL operator()(const LOGFILE& p, const LOGFILE& q) const
	{
		return (strcmp(p.stime, q.stime)>0) ? TRUE : FALSE;
	}
};

BOOL LoadLogFileList( HWND hdlg, char *pPath )
{
	WIN32_FIND_DATA wfd;
	HANDLE hSrch;
	BOOL bRet = TRUE;
	CTextParser tp;
	char no[10], stime[25], etime[25], year[5], month[3], day[3], hour[3], minitue[3], second[3];
	BOOL bEndState = FALSE;
	char szSearchFiles[MAX_PATH], szRecipeName[MAX_PATH], buf[10], szBufFile[MAX_PATH] /*, *pp*/;
	vector<LOGFILE> vecLogF;
	LOGFILE logF;
	int i=0;
	
	const int OLD_TYPE_LOG_FILE_LENTH = 23;

	KWIN_Item_Disable(hdlg, IDRETRY);
	KWIN_Item_Disable(hdlg, IDC_OK);

	ListView_DeleteAllItems( GetDlgItem( hdlg , IDC_LOG_FILE_LIST ) );
	sprintf(szSearchFiles, "%s/*.log", pPath);
	hSrch = FindFirstFile(szSearchFiles, &wfd);
	while(hSrch != INVALID_HANDLE_VALUE && bRet)
	{
		if(!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			if(wfd.nFileSizeLow <= 0){				
				sprintf(szBufFile, "%s/%s", pPath, wfd.cFileName);
				DeleteFile(szBufFile);
			}
			else
			{
				// 2006_3_15_10_25_11.log	(old type)
				// 2006_05_08_18_33_42_Dummy PM2.rcp.log (new type)
				tp.SetText(wfd.cFileName);
				tp.GetWord(year, '_');
				tp.GetWord(month, '_');
				tp.GetWord(day, '_');
				tp.GetWord(hour, '_');
				tp.GetWord(minitue, '_');
				if(strlen(wfd.cFileName) > OLD_TYPE_LOG_FILE_LENTH)	tp.GetWord(second, '_');
				else												tp.GetWord(second, '.');
				sprintf(stime, "%04d-%02d-%02d %02d:%02d:%02d", atoi(year), atoi(month), atoi(day), atoi(hour), atoi(minitue), atoi(second));
				
				/*	< 1.0.1.0 >
				if(strlen(wfd.cFileName) > OLD_TYPE_LOG_FILE_LENTH){
					tp.GetWord(szBufFile);
					pp = strrchr(szBufFile, '.');
					if(pp) { *pp = 0;	strcpy(szRecipeName, szBufFile); }
					else	GetRecipeName(pPath, wfd.cFileName, szRecipeName);
				}
				else GetRecipeName(pPath, wfd.cFileName, szRecipeName);
				*/
				if(GetRecipeNameEndTimeEndState(hdlg, pPath, wfd.cFileName, szRecipeName, etime, &bEndState))
				{
					// added by ycchoi 2006.06.22 < 1.0.1.0 >
					char *pExt = szRecipeName + strlen(szRecipeName) - 3;
					//if(!STRCMP_L(pExt, "lpr"))
					{				
						memset(&logF, 0x00, sizeof(LOGFILE));
						strcpy(logF.stime, stime);
						strcpy(logF.recipeName, szRecipeName);
						strcpy(logF.fileName, wfd.cFileName);
						strcpy(logF.etime, etime);
						strcpy(logF.endstate, bEndState ? "Completed" : "Not Completed");

						vecLogF.push_back(logF);
					}
				}
			}
		}

		bRet = FindNextFile(hSrch, &wfd);

	}
	FindClose( hSrch );

//	{
//		printf("vecLogF before Sort.......\n");
//		for(i=0; i<vecLogF.size(); i++){
//			printf("[%d] - stime[%s] - recipe[%s] - file[%s]\n", i, vecLogF[i].stime, vecLogF[i].recipeName, vecLogF[i].fileName);
//		}
//	}

	// modified by ycchoi 2006.06.22 < 1.0.1.0 >
	// sort(vecLogF.begin(), vecLogF.end(), StartTimeLess());
	sort(vecLogF.begin(), vecLogF.end(), StartTimeMore());

//	{
//		printf("vecLogF after Sort.......\n");
//		for(i=0; i<vecLogF.size(); i++){
//			printf("[%d] - stime[%s] - recipe[%s] - file[%s]\n", i, vecLogF[i].stime, vecLogF[i].recipeName, vecLogF[i].fileName);
//		}
//	}


	for(i=0; i<vecLogF.size(); i++)
	{
		sprintf(no, "%d", i+1);
		LView_Insert_Column(GetDlgItem( hdlg , IDC_LOG_FILE_LIST ), i, no, vecLogF[i].stime, vecLogF[i].etime, vecLogF[i].endstate, vecLogF[i].recipeName, vecLogF[i].fileName);
	}
	sprintf(buf, "%d", i);
	SetWindowText(GetDlgItem(hdlg, IDC_DATA_TOTAL), buf);
	KWIN_Item_Enable(hdlg, IDRETRY);
	KWIN_Item_Enable(hdlg, IDC_OK);

	return TRUE;
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------	
BOOL FindEmptyField(int start, int end, int *rst, FEF_ENUM direct)
{
	int i;

#ifdef _DEBUG
	printf("FindEmptyField:: init start[%d] direct[%d]\n", start, direct);
#endif


	if(direct == FEF_FORWORD)
	{
		for(i=start; i<=end && i<glogDataCnt; i++)
		{
#ifdef _DEBUG
	printf("FindEmptyField:: search Direct[%d] - gpLogData[%d].index = [%d]\n", direct, i, gpLogData[i].index);
#endif

			if(gpLogData[i].index == 0){
				*rst = i;
				return TRUE;
			}
		}
	}
	else
	{
		for(i=start; i>=end && i>=0; i--)
		{
#ifdef _DEBUG
	printf("FindEmptyField:: search Direct[%d] - gpLogData[%d].index = [%d]\n", direct, i, gpLogData[i].index);
#endif

			if(gpLogData[i].index == 0){
				*rst = i;
				return TRUE;
			}
		}
	}

	return FALSE;
}


//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------	
BOOL ValidCachData(int iFrom, int iTo, int *rFrom, int *rTo)
{
	if(FindEmptyField(iFrom, iTo, rFrom, FEF_FORWORD))
		if(FindEmptyField(iTo, *rFrom, rTo, FEF_BACKWORD))
			return TRUE;

	return FALSE;
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void FillCacheData(int iFrom, int iTo)
{
	int rFrom, rTo;
	int i,j;
	FILE *fp;
	char szFile[MAX_PATH];
	char buf[2048];
	char LMonth[3], LDay[3], LHour[3], LMin[3], LSec[3], LmSec[4];
	CTextParser tp;



	if(ValidCachData(iFrom, iTo, &rFrom, &rTo))
	{
#ifdef _DEBUG
			printf("FillCacheData:: from[%d] to[%d] rFrom[%d] rTo[%d]\n", iFrom, iTo, rFrom, rTo);
#endif

		sprintf(szFile, "%s/%s/%s", g_szFilePath, SELECT_PMNAME, SELECT_LOG_FNAME);
		if((fp = fopen(szFile, "rt")) == NULL)
		{
			printf("Can not find the log file[%s]!!!", szFile);
			return;
		}
		// goto log data fp
		fseek(fp, glogDatafp[rFrom], SEEK_SET);

		// skip lines to rFrom
		for(i=rFrom; i<=rTo && i<glogDataCnt && !feof(fp); i++)
		{
#ifdef _DEBUG
			printf("FillCacheData:: Line[%d]\n", i);
#endif
			fgets(buf, sizeof(buf)-1, fp);
			
			tp.SetText(buf);
			// Parsing Log Month				
			if(!tp.GetWord(LMonth, '/')) {
				printf("Invalid Log File Format(Data-Month).");
				return;
			}
			// Parsing Log Day				
			if(!tp.GetWord(LDay, ' ')) {
				printf("Invalid Log File Format(Data-Day).");
				return;
			}
			// Parsing Log Hour				
			if(!tp.GetWord(LHour, ':')) {
				printf("Invalid Log File Format(Data-Hour).");
				return;
			}
			// Parsing Log Minute				
			if(!tp.GetWord(LMin, ':')) {
				printf("Invalid Log File Format(Data-Min).");
				return;
			}
			// Parsing Log Second				
			if(!tp.GetWord(LSec, ':')) {
				printf("Invalid Log File Format(Data-Second).");
				return;
			}
			// Parsing Log mSecond				
			if(!tp.GetWord(LmSec, ':')) {
				printf("Invalid Log File Format(Data-mSec).");
				return;
			}			

			gpLogData[i].index = i+1;
			sprintf(gpLogData[i].stime, "%02d/%02d %02d:%02d:%02d:%03d", atoi(LMonth), atoi(LDay), atoi(LHour), atoi(LMin), atoi(LSec), atoi(LmSec));
#ifdef _DEBUG
			printf("LogData INDEX[%d] TIME[%s]\n", gpLogData[i].index, gpLogData[i].stime);
#endif
			for(j=0; j<gColumnCnt; j++)
			{
				
				

				if(!tp.GetWord_F(gpLogData[i].values[j], 0x09)) break;
#ifdef _DEBUG
				printf("LogData Value[%d] = [%s] Total[%d]\n",  j, gpLogData[i].values[j], glogDataCnt);
#endif
			}	

		}
		fclose(fp);

	}
#ifdef _DEBUG
	else
	{
		printf("FillCacheData:: <<FAIL>> from[%d] to[%d] \n", iFrom, iTo);
	}
#endif
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_ManualLog_Detail_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam )
{

	switch ( msg )
	{
		case WM_INITDIALOG:
			{
				FILE *fp;
				char szFile[MAX_PATH];
				char buf[2048];
				int  nLines = 0;
				int	 i;
				char LMonth[3], LDay[3], LHour[3], LMin[3], LSec[3], LmSec[4];

				//<<-----------------------------------------------------------
				// KGUI_STANDARD_Set_Function_Size( hdlg );
				//>>-----------------------------------------------------------

				// data count init
				glogDataCnt = 0;
				glogDatafp.resize(0);
				
#ifdef _DEBUG
				S_TIMER_READY();
#endif

				sprintf(szFile, "%s/%s/%s", g_szFilePath, SELECT_PMNAME, SELECT_LOG_FNAME);

				// dialog text set
				sprintf(buf, "[%s]-[%s]", SELECT_PMNAME, SELECT_LOG_FNAME);
				SetWindowText(hdlg, buf);

				if((fp = fopen(szFile, "rt")) == NULL)
				{
					MessageBox(hdlg, "Can not find the log file!!!", "Notice", MB_OK);
					return FALSE;
				}

				// skip recipe name lines
				for(i=0; i<3; i++)	fgets(buf, sizeof(buf)-1, fp);

				// parsing colume name
				fgets(buf, sizeof(buf)-1, fp);
				if(strstr(buf, SELECT_PMNAME) == NULL)
				{
					fclose(fp);
					MessageBox(hdlg, "Invalid Log File Format(Column).", "Notice", MB_OK);
					return FALSE;
				}

				CTextParser tp(buf);

				// Parsing Log Month				
				if(!tp.GetWord(LMonth, '/')) {
					MessageBox(hdlg, "Invalid Log File Format(Column-Month).", "Notice", MB_OK);
					return FALSE;
				}
				// Parsing Log Day				
				if(!tp.GetWord(LDay, ' ')) {
					MessageBox(hdlg, "Invalid Log File Format(Column-Day).", "Notice", MB_OK);
					return FALSE;
				}
				// Parsing Log Hour				
				if(!tp.GetWord(LHour, ':')) {
					MessageBox(hdlg, "Invalid Log File Format(Column-Hour).", "Notice", MB_OK);
					return FALSE;
				}
				// Parsing Log Minute				
				if(!tp.GetWord(LMin, ':')) {
					MessageBox(hdlg, "Invalid Log File Format(Column-Min).", "Notice", MB_OK);
					return FALSE;
				}
				// Parsing Log Second				
				if(!tp.GetWord(LSec, ':')) {
					MessageBox(hdlg, "Invalid Log File Format(Column-Sec).", "Notice", MB_OK);
					return FALSE;
				}
				// Parsing Log mSecond				
				if(!tp.GetWord(LmSec, ':')) {
					MessageBox(hdlg, "Invalid Log File Format(Column-mSec).", "Notice", MB_OK);
					return FALSE;
				}

				char szTmpColumn[40];
				char *p;
				CTextParser tp2;
				int idx = 0;
				char *columns[MAX_LOG_ITEM_COUNT+2] = { NULL, };	// +2 (No, Start Time 포함)
				int  columns_size[MAX_LOG_ITEM_COUNT+2];

				// no column 추가
				columns[idx++] = (char*)malloc(5);
				strcpy(columns[0], "No");
				// time Column 추가
				columns[idx++] = (char*)malloc(25);
				strcpy(columns[1], "Time");
				while(tp.GetWord_F(szTmpColumn, 0x09) && idx<MAX_LOG_ITEM_COUNT+2)
				{
					p = strchr(szTmpColumn, '.');
					if(p == NULL){
						MessageBox(hdlg, "Invalid Log File Format(Data).", "Notice", MB_OK);
						return FALSE;
					}
					columns[idx] = (char*)malloc(strlen(p+1)+1);
					strcpy(columns[idx], p+1);
					idx++;
				}

				if(idx <= 2 ){
					MessageBox(hdlg, "Invalid Log File Format.", "Notice", MB_OK);
					for(i=0; i<idx; i++)
					{
						if(columns[i])		free(columns[i]);
						columns[i] = NULL;
					}
					return FALSE;
				}

				// set item column count
				gColumnCnt = idx-2;

				// set column width
				columns_size[0] = 50;
				columns_size[1] = 150;
				for(i=2; i<idx; i++)
				{
					columns_size[i] = 100;
				}
				
				KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LOG_DETAIL ) , idx , columns , columns_size );

				// column memory free
				for(i=0; i<MAX_LOG_ITEM_COUNT+2; i++)
				{
					if(columns[i])	free(columns[i]);
					columns[i] = NULL;
				}

				// count data line
				glogDatafp.push_back(ftell(fp));

				while(!feof(fp))
				{
					fgets(buf, sizeof(buf)-1, fp);

					// Log Data 종료 Part 확인
					if(strstr(buf, "LOG STOP") || strstr(buf,"----------"))
					{						
						break;
					}

					glogDatafp.push_back(ftell(fp));
					glogDataCnt++;
				}
				fclose(fp);

				// memrory alloc for log data
				gpLogData = (LPLOGDATA)VirtualAlloc(NULL, sizeof(LOGDATA)*glogDataCnt, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
				memset(gpLogData, 0x00, sizeof(LOGDATA)*glogDataCnt);
				
				// List Type set(GridLine)
				ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LOG_DETAIL ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
				// List Data Set
				ListView_SetItemCount(GetDlgItem(hdlg, IDC_LOG_DETAIL), glogDataCnt);
				
#ifdef _DEBUG
				printf("----> [ELAPSED TIME ; %.3f][Lines : %d]\n", S_TIMER_ELAPSED(), glogDataCnt);
#endif
			}
			break;

		case WM_COMMAND:
			{
				switch(wParam)
				{
					case IDOK :
						// VirtualFree(gpLogData, sizeof(LOGDATA)*glogDataCnt, MEM_DECOMMIT);
						EndDialog(hdlg, wParam);
						//2013.09.04 Memory Leak Bug by hyuk
						//VirtualFree(gpLogData, sizeof(LOGDATA)*glogDataCnt, MEM_RELEASE);			// 2009.11.11
						VirtualFree(gpLogData, 0, MEM_RELEASE);			// 2009.11.11
						return TRUE;
				}
			}
			break;

		case WM_NOTIFY:
			{
				LPNMHDR hdr;
				NMLVDISPINFO *ndi;
				NMLVCACHEHINT *nch;
				hdr = (LPNMHDR)lParam;
				ndi = (NMLVDISPINFO *)lParam;
				nch = (NMLVCACHEHINT *)lParam;

				if(hdr->hwndFrom == GetDlgItem(hdlg, IDC_LOG_DETAIL))
				{
					switch(hdr->code)
					{
						case LVN_GETDISPINFO:

							switch(ndi->item.iSubItem)
							{
								case 0:	
									if(ndi->item.mask & LVIF_TEXT == LVIF_TEXT){
										// lstrcpy(ndi->item.pszText, itoa(gpLogData[ndi->item.iItem].index, NULL, 10));
										sprintf(ndi->item.pszText, "%d", gpLogData[ndi->item.iItem].index);
									}
									break;
										
								case 1: lstrcpy(ndi->item.pszText, gpLogData[ndi->item.iItem].stime);						break;
								default :
										lstrcpy(ndi->item.pszText, gpLogData[ndi->item.iItem].values[ndi->item.iSubItem-2]);	break;

							}

							break;

						case LVN_ODCACHEHINT:
							FillCacheData(nch->iFrom, nch->iTo);
							break;
					}
				}
			}
			break;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
BOOL APIENTRY Console_Gui_For_OnlineLog_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) {
	PAINTSTRUCT	ps;
	int i;
	
	// TCHAR       *szString[] = { "No" , "Start Time" , "Recipe Name", "File Name"};
	TCHAR       *szString[] = { "No" , "Start Time" , "End Time", "End Status", "Recipe Name", "File Name"};

	//int         szSize[]   = {  60  ,    200 ,    250,	250 };
	int         szSize[]   = {  60  ,    200 ,	200,	100,	250,	250 };

	int         szSize_M[] = {  40  ,    140 ,    700,	700 };
	char Buffer[256];
	HBITMAP hBitmap;
	RECT r;

	switch ( msg ) {
		case WM_INITDIALOG:
			SetWindowText( hdlg , "GUI_ManualLog" );
			//<<-----------------------------------------------------------
			KGUI_STANDARD_Set_Function_Size( hdlg );
			//>>-----------------------------------------------------------
/*
			if ( Get_Prm_TITLE_MODE() ) i = 40;
			else                        i = 0;
*/
			i = 40; // Title Mode 적용

			
			GetWindowRect(GetDlgItem(hdlg, IDC_STATIC_PM), &r);
			KWIN_Item_Resize( hdlg , IDC_STATIC_PM		, r.left   ,  r.top + i , r.right-r.left  , r.bottom-r.top ); // String(Process Module)

			GetWindowRect(GetDlgItem(hdlg, IDRETRY), &r);
			KWIN_Item_Resize( hdlg , IDRETRY            , r.left   ,  r.top + i , r.right-r.left  , r.bottom-r.top ); // read again

			GetWindowRect(GetDlgItem(hdlg, IDC_STATIC_STATUS2), &r);
			KWIN_Item_Resize( hdlg , IDC_STATIC_STATUS2 , r.left   ,  r.top + i , r.right-r.left  , r.bottom-r.top ); // Frame

			GetWindowRect(GetDlgItem(hdlg, IDC_STATIC_STATUS), &r);
			KWIN_Item_Resize( hdlg , IDC_STATIC_STATUS  , r.left   ,  r.top + i , r.right-r.left  , r.bottom-r.top ); // string(Total)

			GetWindowRect(GetDlgItem(hdlg, IDC_DATA_TOTAL), &r);
			KWIN_Item_Resize( hdlg , IDC_DATA_TOTAL     , r.left   ,  r.top + i , r.right-r.left  , r.bottom-r.top ); // data count

			GetWindowRect(GetDlgItem(hdlg, IDC_OK), &r);
			KWIN_Item_Resize( hdlg , IDC_OK             , r.left   ,  r.top + i , r.right-r.left  , r.bottom-r.top ); // Button(OK)

			GetWindowRect(GetDlgItem(hdlg, IDC_LIST_FILE), &r);
			KWIN_Item_Resize( hdlg , IDC_LIST_FILE      , r.left   ,  r.top + i , r.right-r.left  , r.bottom-r.top-i ); // file list

			GetWindowRect(GetDlgItem(hdlg, IDC_LOG_FILE_LIST), &r);
			KWIN_Item_Resize( hdlg , IDC_LOG_FILE_LIST  , r.left   ,  r.top + i , r.right-r.left  , r.bottom-r.top-i ); // file list
		
//			if ( Get_Prm_MINISCREEN_MODE() )
//				KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_DATA_LIST ) , 4 , szString , szSize_M );
//			else
//				KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_DATA_LIST ) , 4 , szString , szSize );

//			ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_DATA_LIST ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
			//
			
			// KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LOG_FILE_LIST ) , 4 , szString , szSize );
			KWIN_LView_Init_Header( GetDlgItem( hdlg , IDC_LOG_FILE_LIST ) , 6 , szString , szSize );

			ListView_SetExtendedListViewStyleEx( GetDlgItem( hdlg , IDC_LOG_FILE_LIST ) , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
			

			FileData_ListView_Init( GetDlgItem( hdlg , IDC_LIST_FILE ));
			//>>-----------------------------------------------------------
//			ShowWindow( hdlg , SW_HIDE ); // 2003.07.12
			//----------------------------------
			PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
			//----------------------------------
			return TRUE;
			
		case WM_PAINT:
			if ( !KGUI_STANDARD_Window_Signal_Mode() ) { // 2005.02.28
				BeginPaint( hdlg, &ps );
				EndPaint( hdlg , &ps );
				return TRUE;
			}
			//=============================================================================================
			BeginPaint( hdlg, &ps );
			if ( KGUI_Get_BACKGROUND() != NULL ) {
				hBitmap = LoadBitmap( KGUI_Get_BACKGROUND() , "IDB_BACKGROUND" );
				if ( hBitmap ) {
					KWIN_DRAW_BITMAP( ps.hdc , 0 , 0 , hBitmap , SRCCOPY );
					DeleteObject( hBitmap );
				}
			}
			EndPaint( hdlg , &ps );
			return TRUE;

		case WM_COMMAND :
			if ( GET_WM_COMMAND_CMD( wParam , lParam ) == LBN_SELCHANGE ) {
				if ( GET_WM_COMMAND_ID( wParam , lParam ) == IDC_LIST_FILE ) {
					i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , IDC_LIST_FILE ) );
					if ( i >= 0 ) {
						ListBox_GetText( GetDlgItem( hdlg , IDC_LIST_FILE ) , i , SELECT_PMNAME );
						sprintf( Buffer , "%s/%s" , g_szFilePath , SELECT_PMNAME );
						LoadLogFileList( hdlg, Buffer );						
					}
				}				
				return TRUE;
			}
			switch (wParam) 
			{
				case IDRETRY :
					i = (int) ListBox_GetCurSel( GetDlgItem( hdlg , IDC_LIST_FILE ) );
					if ( i >= 0 ) {
						ListBox_GetText( GetDlgItem( hdlg , IDC_LIST_FILE ) , i , SELECT_PMNAME );
						sprintf( Buffer , "%s/%s" , g_szFilePath , SELECT_PMNAME );
						LoadLogFileList( hdlg, Buffer );
					}
					return TRUE;
					
				case IDC_OK :
					{
						int idx;
						idx = ListView_GetNextItem(GetDlgItem(hdlg, IDC_LOG_FILE_LIST), -1, LVNI_ALL | LVNI_SELECTED);
						if(idx == -1)
						{
							MessageBox(hdlg, "make a selection from the file list", "NOTICE", MB_OK);
						}
						else
						{
							// < 1.0.1.0 >
							// ListView_GetItemText(GetDlgItem(hdlg, IDC_LOG_FILE_LIST), idx, 3, SELECT_LOG_FNAME, sizeof(SELECT_LOG_FNAME)-1);
							ListView_GetItemText(GetDlgItem(hdlg, IDC_LOG_FILE_LIST), idx, 5, SELECT_LOG_FNAME, sizeof(SELECT_LOG_FNAME)-1);
							GoModalDialogBoxParam( GETHINST(hdlg) , MAKEINTRESOURCE(IDD_LOG_DETAIL) , hdlg , Console_Gui_For_ManualLog_Detail_Proc , (LPARAM) NULL );
						}
					}
					return TRUE;
			}
			break;
	}
	return FALSE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status Program_Main() {
	HWND hWnd;

	//----------------------------------
	REGIST_FILE_INFO( "Gui Manual Log View 1.0(2006.03.13)" );
	//----------------------------------

	//----------------------------------
	while( PROGRAM_GUI_SPAWNING == 0 ) { _sleep(100); }
	//----------------------------------
	//----------------------------------
	InitCommonControls();

	//----------------------------------
	KGUI_Set_BACKGROUND( PROGRAM_PARAMETER_READ() );
	strcpy( SELECT_PMNAME , "" );
	strcpy( g_szFilePath  , "" );
	hWnd = GET_MAIN_WINDOW();

	GoModalDialogBoxParam( GETHINST(hWnd) , MAKEINTRESOURCE(IDD_MANUAL_LOG) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL );

/*
	if ( Get_Prm_MINISCREEN_MODE() ) {
		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_MANUAL_LOG , IDD_E_ONLINE_HISTORY_M ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY_M , IDD_E_ONLINE_HISTORY_M ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY_M , IDD_E_ONLINE_HISTORY_M ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY_M , IDD_E_ONLINE_HISTORY_M ) , GET_MAIN_WINDOW() , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2002.06.17
	}
	else {
		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_MANUAL_LOG , IDD_E_ONLINE_HISTORY ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY , IDD_E_ONLINE_HISTORY ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19 // 2004.08.10
//		GoModalDialogBoxParam( GETHINST(hWnd) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY , IDD_E_ONLINE_HISTORY ) , hWnd , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2004.01.19
//		GoModalDialogBoxParam( GetModuleHandle(NULL) , KWIN_MAKEINTRESOURCE( IDD_ONLINE_HISTORY , IDD_E_ONLINE_HISTORY ) , GET_MAIN_WINDOW() , Console_Gui_For_OnlineLog_Proc , (LPARAM) NULL ); // 2002.06.17
	}
*/
	SYSTEM_KILL_MYSELF();
	//----------------------------------
	PROGRAM_GUI_SPAWNING = 2; // 2003.07.12
	//----------------------------------
	return( SYS_SUCCESS );
}


void Program_Enter_Code() 
{	
}