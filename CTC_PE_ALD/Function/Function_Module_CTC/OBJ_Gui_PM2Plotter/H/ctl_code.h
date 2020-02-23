#ifndef CTL_CODE_H
#define CTL_CODE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Console_Time_Display( int mode );
void Get_Long_Time_To_String( long time_data , char *buffer );
long Get_Time_Data_From_String( char *b0 );
long Elapsed_Time( char *buffer );
void Elapsed_Time_with_Datalog( int mode , char *buffer );
//------------------------------------------------------------------------------------------
char *DATALOG_DIRECTORY_NAME();
//------------------------------------------------------------------------------------------
void ConGui_Display_HeadTime( HDC hDC , int Mode );
void ConGui_Display( HDC hDC );
void ConGui_Display_Title( HDC hDC );
void ConGui_Display_Data( HDC hDC );
void ConGui_Datalog_Display( HDC hDC , int );
//------------------------------------------------------------------------------------------
void Console_Gui_Move( HWND hdlg , int Move );
void ConGui_Display_Plot_Item_Clear( HDC );
void ConGui_Display_Plot_Item( HDC hDC , int mode );
//------------------------------------------------------------------------------------------
BOOL Console_Gui_Left_Button( HWND hdlg , POINT ptcursor );
BOOL Console_Gui_Right_Button( HWND hdlg , POINT ptcursor , BOOL LeftLike );
BOOL Read_Information_File( char *FileName , BOOL );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
