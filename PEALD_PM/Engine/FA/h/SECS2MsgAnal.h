#ifndef SECS2MSGANAL__H
#define SECS2MSGANAL__H
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
__declspec(dllexport) BOOL MsgAnalDll_Initialize();
__declspec(dllexport) BOOL MsgAnalDll_DeInitialize();
//-----------------------------------------------------------------------------------
__declspec(dllexport) int			Get_Int_From_Byte( char *Data , unsigned int Size );
__declspec(dllexport) unsigned int	Get_UInt_From_Byte( char *Data , unsigned int Size );
__declspec(dllexport) float			Get_Float_From_Byte( char *Data );
__declspec(dllexport) double		Get_Double_From_Byte( char *Data );
//----------------------------------------------------------------------------------------
__declspec(dllexport) void Get_Byte_From_Int( int Data , char *retval );
__declspec(dllexport) void Get_Byte_From_UInt( unsigned int Data , char *retval );
__declspec(dllexport) void Get_Byte_From_Float( float Data , char *retval );
__declspec(dllexport) void Get_Byte_From_Double( double Data , char *retval );
//----------------------------------------------------------------------------------------
__declspec(dllexport) void Set_LengthByte( int CurrentPosition , int *NextPosition , int DataType , int Length , char *retval );
__declspec(dllexport) void Set_List( int CurrentPosition , int *NextPosition , int ListNumbers , char *retval );
__declspec(dllexport) void Set_Ascii( int CurrentPosition , int *NextPosition , char *Data , char *retval );
__declspec(dllexport) void Set_Binary( int CurrentPosition , int *NextPosition , int BinaryCount , char *Data , char *retval );
__declspec(dllexport) void Set_Boolean( int CurrentPosition , int *NextPosition , int BooleanCount , char *Data , char *retval );
__declspec(dllexport) void Set_Int( int CurrentPosition , int *NextPosition , int DataType , int IntCount , int *Data , char *retval );
__declspec(dllexport) void Set_Float( int CurrentPosition , int *NextPosition , int FloatCount , float *Data , char *retval );
__declspec(dllexport) void Set_Double( int CurrentPosition , int *NextPosition , int DoubleCount , double *Data , char *retval );
__declspec(dllexport) BOOL Set_Data( int CurrentPosition , int *NextPosition , int Format , char *Data , char *NewData );
//----------------------------------------------------------------------------------------
__declspec(dllexport) int  Get_DataType( char Data );
__declspec(dllexport) int  Get_LengthByte( char Data );
__declspec(dllexport) int  Get_DataLength( int CurrentPosition , int *NextPosition , char *Data );
__declspec(dllexport) BOOL Get_String( int CurrentPosition , int *NextPosition , char *Data , int retsize , char *retval , int Count , int Snumber , int Fnumber );
__declspec(dllexport) BOOL Get_Binary( int CurrentPosition , int *NextPosition , char *Data , int retsize , char *retval , int Count , int Snumber , int Fnumber );
__declspec(dllexport) BOOL Get_Boolean( int CurrentPosition , int *NextPosition , char *Data , int retsize , char *retval , int Count , int Snumber , int Fnumber );
__declspec(dllexport) BOOL Get_Int( int CurrentPosition , int *NextPosition , char *Data , int retsize , int *retval , int Count , int Snumber , int Fnumber );
__declspec(dllexport) BOOL Get_Float( int CurrentPosition , int *NextPosition , char *Data , int retsize , float *retval , int Count , int Snumber , int Fnumber );
__declspec(dllexport) BOOL Get_Double( int CurrentPosition , int *NextPosition , char *Data , int retsize , double *retval , int Count , int Snumber , int Fnumber );
__declspec(dllexport) int  Get_Data( int CurrentPosition , int *NextPosition , char *Data , int *GetData , int *GetType , int Count , int StreamNum , int FunctionNum );
__declspec(dllexport) int  Get_FullData( int CurrentPosition , int *NextPosition , char *Data , char *GetData , int *GetType , int Count , int StreamNum , int FunctionNum );
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
#endif
