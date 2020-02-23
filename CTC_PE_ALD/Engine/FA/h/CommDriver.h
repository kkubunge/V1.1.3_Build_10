#ifndef COMMDRIVER__H
#define COMMDRIVER__H
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
__declspec(dllexport) double Get_Time5_Value();
__declspec(dllexport) BOOL   Set_Time5_Value( double Time );
//--------------------------------------------------------------------------------
__declspec(dllexport) double Get_Time8_Value();
__declspec(dllexport) BOOL   Set_Time8_Value( double Time );
//--------------------------------------------------------------------------------
__declspec(dllexport) BOOL Get_Connect_Exit();
__declspec(dllexport) void Set_Connect_Exit( BOOL Sts );
//--------------------------------------------------------------------------------
__declspec(dllexport) BOOL Get_Connect_Status();
__declspec(dllexport) void Set_Connect_Status( BOOL Sts );
//--------------------------------------------------------------------------------
__declspec(dllexport) BOOL Get_Connect_Reset();
__declspec(dllexport) void Set_Connect_Reset( BOOL Sts );
//--------------------------------------------------------------------------------
__declspec(dllexport) int  Get_Comm_Method();
__declspec(dllexport) BOOL Set_Comm_Method( int Method );
//--------------------------------------------------------------------------------
__declspec(dllexport) int  Get_Comm_Try();
__declspec(dllexport) BOOL Set_Comm_Try( int Try );
//--------------------------------------------------------------------------------
__declspec(dllexport) void Get_Comm_IP( char *IPAddr );
__declspec(dllexport) BOOL Set_Comm_IP( char *IPAddr );
//--------------------------------------------------------------------------------
__declspec(dllexport) int  Get_Comm_Port();
__declspec(dllexport) BOOL Set_Comm_Port( int Port );
//--------------------------------------------------------------------------------
__declspec(dllexport) int  Get_Comm_Enable();
__declspec(dllexport) BOOL Set_Comm_Enable( int Enable );
//--------------------------------------------------------------------------------
__declspec(dllexport) int  Get_Comm_Net_Monitor();
__declspec(dllexport) BOOL Set_Comm_Net_Monitor( int Enable );
//--------------------------------------------------------------------------------
__declspec(dllexport) int  Get_Comm_IO_Loading();
__declspec(dllexport) BOOL Set_Comm_IO_Loading( int Enable );
//----------------------------------------------------------------------------------------
__declspec(dllexport) BOOL Get_Disconnect_from_Host();
__declspec(dllexport) void Set_Disconnect_from_Host( int Status );
//----------------------------------------------------------------------------------------
__declspec(dllexport) int  Get_DataLog_Enable();
__declspec(dllexport) BOOL Set_DataLog_Enable( int Enable );
//----------------------------------------------------------------------------------------
__declspec(dllexport) void Get_DataLog_Path( char *Path );
__declspec(dllexport) BOOL Set_DataLog_Path( char *Path );
//----------------------------------------------------------------------------------------
__declspec(dllexport) double Get_LogDel_Day();
__declspec(dllexport) BOOL Set_LogDel_Day( double Day );
//----------------------------------------------------------------------------------------
// ymc 2008.11.1 for PSK UMC
__declspec(dllexport) int  Get_DebugLog_Enable();
__declspec(dllexport) void Set_DebugLog_Enable( int Enable );
//----------------------------------------------------------------------------------------
// ymc 2008.11.1 for PSK UMC
__declspec(dllexport) int  Get_MsgCheck_Before_Send();
__declspec(dllexport) void Set_MsgCheck_Before_Send( int Check );
//----------------------------------------------------------------------------------------
__declspec(dllexport) void Set_SystemLog_Skip( BOOL Skip );
//--------------------------------------------------------------------------------
__declspec(dllexport) BOOL Net_Write_String( char *Data , int Length );
//----------------------------------------------------------------------------------------
__declspec(dllexport) BOOL Set_Queue_Initial( int Queue );
__declspec(dllexport) BOOL Gem_Comm_Open();
//----------------------------------------------------------------------------------------
__declspec(dllexport) BOOL Have_GemMessage( int *Length );
__declspec(dllexport) BOOL Get_GemMessage( char *Data );
//----------------------------------------------------------------------------------------
__declspec(dllexport) BOOL Set_GemLength( int Length );
__declspec(dllexport) BOOL Set_GemMessage( int Length , char *Data );
__declspec(dllexport) BOOL Usable_SendQueue();
//----------------------------------------------------------------------------------------
__declspec(dllexport) int  Get_PreMode_Reconnect();
__declspec(dllexport) void Set_PreMode_Reconnect( int Mode );
//----------------------------------------------------------------------------------------
// ymc 2005.9.9 for CCS
__declspec(dllexport) BOOL Get_Select_Status();
__declspec(dllexport) void Set_Select_Status( BOOL Status );
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
#endif
