#ifndef DAT_CODE_H
#define DAT_CODE_H

//------------------------------------------------------------------------------------------
BOOL DATA_LOGGING_STATUS();
char *DATA_LOGGING_FILENAME();
//------------------------------------------------------------------------------------------
BOOL DISP_GO_READ_MODE_GET();
void DISP_GO_READ_MODE_SET( BOOL data );
void DISP_GO_READ_MODE_INIT();
void DISP_WAIT_REFORM_COMPLETED(); // 2005.06.15
//------------------------------------------------------------------------------------------
void Console_Datalog_Control( HWND );
void Console_Datalog_Time( HWND );
BOOL Console_Datalog_Read( HWND , BOOL Clear );
void Console_Datalog_Save_Mode( HWND hdlg );
//------------------------------------------------------------------------------------------
BOOL FindPosFromDataBuffer( int p , int VPos , int *Res );
//------------------------------------------------------------------------
void Datalog_Data_Polling( HWND h );
void Plotting_Data_Polling( HWND h , BOOL disp , BOOL Poll );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Plotting_Data_Set_For_Auto_Scale( int trg );
BOOL Plotting_Data_Set_For_Zoom_Scale( int scr , int x1 , int x2 , int y1 , int y2 ); // 2002.04.11
BOOL Plotting_Zoom_Recover( int scr ); // 2002.04.11

#endif
