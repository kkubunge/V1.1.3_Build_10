#ifndef INTERFACE_H
#define INTERFACE_H

//-------------------------------------------------------------------------
void KGUI_STANDARD_Set_Screen_Handle( HWND hWnd );
void KGUI_STANDARD_Window_Close_Signal();
void KGUI_STANDARD_Window_Open_Signal();
void KGUI_STANDARD_Window_Kill_Signal();
void KGUI_STANDARD_Set_Function_Size( HWND hWnd );
void KGUI_STANDARD_Screen_Nav_Info( int no );
HWND KGUI_STANDARD_Get_Screen_Handle();
//------------------------------------------------------------------------------------------
int  Program_Main();
//------------------------------------------------------------------------------------------

#endif