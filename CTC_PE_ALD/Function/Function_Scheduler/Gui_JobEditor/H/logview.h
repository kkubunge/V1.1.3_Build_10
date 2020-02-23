#ifndef LOGVIEW_H
#define LOGVIEW_H

//============================================================================================================================
//============================================================================================================================

BOOL xListView_Enter();
BOOL xListView_Leave();

void xListView_Clear( HWND hWnd );

//============================================================================================================================
//============================================================================================================================
#define SEP_ONE_TIME_MAX_READ_ITEM_COUNT	64

enum { LOG_OPEN_STS_READY , LOG_OPEN_STS_READING , LOG_OPEN_STS_DONE , LOG_OPEN_STS_FAIL };

typedef BOOL (*CALLBACK_LOG_OPEN_CONTROL)	( HWND , HWND , int , char * , int , int , int , int , int , int );

void LOG_OPEN_INIT();

BOOL LOG_OPEN_CANCELED();

BOOL LOG_OPEN_MESSAGE( char * );

BOOL LOG_OPEN_TERMINATE();

BOOL LOG_OPEN_CONTROL( HWND hWnd , HWND hWnds , BOOL sepload , char *FileName , int option , int option2 , int option3 , int option4 , int option5 , int option6 , CALLBACK_LOG_OPEN_CONTROL call_api );

//============================================================================================================================
//============================================================================================================================

void Get_Log_File_Name( char *Buffer , char *catstr , int mode , char *logsubfolder , BOOL slotisdummy , int slot , int slot2 , char *slotstr0 , int ch , char *chstr0 , char *appstr );

//
void LogView_Open_Full_Error_Message( HWND );
void LogView_Open_Full_Method( HWND );
//
BOOL LogView_Open_Lot_History( HWND , int i );

BOOL LogView_Open_Method_History( HWND , int i );
BOOL LogView_Open_Method_History_Init( HWND hwndListView , int mode );
BOOL LogView_Open_Method_History_Data_Set( HWND hdlg , int winpos , BOOL sepload , int index , int infoinsert , int findwid , int fileorder );

BOOL LogView_Open_Lot_Log( HWND , int i , int extview );
BOOL LogView_Open_Lot_Log_S( HWND , int i , int extview , BOOL selmode ); // 2012.06.26
BOOL LogView_Open_Lot_Log_Ext( HWND , int , int i , int extview , BOOL selmode , char * ); // 2012.06.26

BOOL LogView_Open_Module_Log( HWND , int i , int extview );
BOOL LogView_Open_Module_Log_S( HWND , int i , int extview , BOOL selmode ); // 2012.06.26
BOOL LogView_Open_Module_Log_Ext( HWND , int , int i , int extview , BOOL selmode , int s , int , char * );
//
BOOL LogView_Open_LotModule_Log_Data_Set( HWND hdlg , int winpos , int mode , int index , BOOL sepload , int ME_ReadMode , int ME_Module , int ME_Slot , int ME_extview );

BOOL LogView_Open_Process_Log( HWND , int i , int extview );
//BOOL LogView_Open_Process_Log_Ext( HWND , int , int i , int extview , int s , char * , char * ); // 2016.12.22
BOOL LogView_Open_Process_Log_Ext( HWND , int , int i , int extview , int s , char * , char * , char * ); // 2016.12.22

BOOL LogView_Open_Process_Log_Get_FileName( HWND hWnd , int index , int slot , char *filename , char *getMdlStr );

BOOL LogView_Open_Recipe_Log( HWND , int i , int extview );
//BOOL LogView_Open_Recipe_Log_Ext( HWND , int , int i , int extview , int s , char * , char * , int ); // 2016.12.22
BOOL LogView_Open_Recipe_Log_Ext( HWND , int , int i , int extview , int s , char * , char * , int , char *getMdlStr ); // 2016.12.22

BOOL LogView_Open_LotPM_Log( HWND , int i , int extview ); // 2005.02.21
BOOL LogView_Open_LotPM_Log_Ext( HWND , int , int i , int extview , char * );

BOOL Make_Estimate_LogData_for_Min_Max_Avr( BOOL UserMode , int ReadMode , int Module , int Slot , int SchIndex , char *LotDirectory , char *Single_Slot , int WriteMode , BOOL Upload , int extview , BOOL sepload );

BOOL LogView_Open_Lot_History_Data_Set( HWND hdlg , int wpos , int mode , int wfrid , char *FileName_org , char *FileName , int index , BOOL sepload );
BOOL LogView_Open_Lot_User_Data_Set( HWND hdlg , int wpos , char *FileName_org , char *FileName , BOOL sepload ); // 2014.01.16
BOOL LogView_Open_Recipe_Log_Data_Set( HWND hdlg , int winpos , char * , char * , int , BOOL sepload );

BOOL LogView_Open_Process_Log_Data_Set( HWND hdlg , int winpos , char * , char * , BOOL , int , int , int , BOOL , BOOL );

void _UTIL_LV_SET_EXTEND_STYLE( HWND hWnd , BOOL userdraw , int extmode );

void Data_Read_Start( HWND hdlg );
void Data_Read_End( HWND hdlg );
				
#endif