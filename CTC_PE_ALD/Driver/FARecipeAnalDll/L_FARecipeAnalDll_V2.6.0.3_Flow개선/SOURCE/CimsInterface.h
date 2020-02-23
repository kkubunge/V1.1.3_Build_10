#ifndef CIMSINTERFACE__H
#define CIMSINTERFACE__H

#include <windows.h>

//=====================================================================================================
// For use Cims API inside DLL
//=====================================================================================================

typedef enum { _K_D_IO , _K_A_IO , _K_S_IO , _K_F_IO } IO_Status;
typedef enum {ALM_CLEAR	,ALM_RETRY	,ALM_WAIT	,ALM_IGNORE	,ALM_DISABLE,ALM_ENABLE	,ALM_STOP,ALM_ABORT	,ALM_PAUSED} AlarmStyle;

typedef struct {
	char			*IO_Name;
	int				IO_Mode;
	int				IO_Enum;
	int				IO_Match_Point;
} IO_Name_String_Map;

/////////////////////////////////////////////////////////////////////////
//20070508 added for dummy operation
////////////////////////////////////////////////////////////////////////
AlarmStyle  _dll_ALARM_MANAGE         (int Alarm_ID);
////////////////////////////////////////////////////////////////////////
int    _dll_FIND_FROM_STRING( int IO_Type, char *Name );
//
int    _dll_dWRITE_FUNCTION( int Function_Point , char *Func_Param );
int    _dll_dREAD_FUNCTION( int Function_Point );
void   _dll_dRUN_FUNCTION_ABORT( int Function_Point );
void   _dll_dWRITE_FUNCTION_EVENT( int Function_Point , char *Event );
int    _dll_dREAD_FUNCTION_EVENT( int Function_Point );
void   _dll_dREAD_UPLOAD_MESSAGE( int Function_Point , char *ReadString );
BOOL   _dll_dWRITE_UPLOAD_MESSAGE( char *WriteString );
//
void   _dll_dWRITE_DIGITAL( int IO_Point , int Data , int *CommStatus );
int    _dll_dREAD_DIGITAL( int IO_Point , int *CommStatus );
void   _dll_dWRITE_ANALOG( int IO_Point , double Data , int *CommStatus );
double _dll_dREAD_ANALOG( int IO_Point , int *CommStatus );
void   _dll_dWRITE_STRING( int IO_Point , char *WriteData , int *CommStatus );
void   _dll_dREAD_STRING( int IO_Point , char *ReadData , int *CommStatus );


#endif
