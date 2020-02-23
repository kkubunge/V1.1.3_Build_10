#ifndef RECIPECODE_H
#define RECIPECODE_H

//------------------------------------------------------------------------------------------
void RECIPE_CONTROL_Get_Selected_FILENAME( HWND hdlg , char *Buffer , int Count );
void RECIPE_CONTROL_Set_Selected_FILENAME( HWND hdlg , char *Buffer );
void RECIPE_CONTROL_Set_Select_ANOTHERFILE( HWND hdlg , BOOL data );
//------------------------------------------------------------------------------------------
void RECIPE_CONTROL_Set_MAIN_DIRECTORY( char *data );
char *RECIPE_CONTROL_Get_MAIN_DIRECTORY();
//------------------------------------------------------------------------------------------
void RECIPE_CONTROL_Set_GROUP_DIRECTORY( char *data );
char *RECIPE_CONTROL_Get_GROUP_DIRECTORY();
//------------------------------------------------------------------------------------------
void RECIPE_CONTROL_Set_FILE_DIRECTORY( char *data );
char *RECIPE_CONTROL_Get_FILE_DIRECTORY();
//------------------------------------------------------------------------------------------
void RECIPE_CONTROL_Set_FILE_CHANGE( BOOL data );
BOOL RECIPE_CONTROL_Get_FILE_CHANGE();

void RECIPE_CONTROL_POINTER_INIT( HWND hWnd );

BOOL RECIPE_CONTROL_BUTTON_HANDLING( HWND hdlg , long wParam , BOOL *Change , BOOL *Redraw );

void RECIPE_CONTROL_ALL_DATA_DISPLAY( HWND hdlg );
void DATA_PUT_TO_FILE( BOOL All ) ;

void RECIPE_CONTROL_INIT_PROC( HWND hWnd );
BOOL RECIPE_CONTROL_FILE_NEW_PROC( HWND hWnd , char *Buffer );
BOOL RECIPE_CONTROL_FILE_SAVE_PROC( HWND hWnd , char *Buffer, int dType );
BOOL RECIPE_CONTROL_FILE_OPEN_PROC( HWND hWnd , char *Buffer );

// -----------------------------------------------------------------------------------------
void SampleLogSave(char *buffer, char *m_stOption);
void SampleLogDell(char *m_szModule);
void RcpEditLog(char *RcpNameBuf);

#endif