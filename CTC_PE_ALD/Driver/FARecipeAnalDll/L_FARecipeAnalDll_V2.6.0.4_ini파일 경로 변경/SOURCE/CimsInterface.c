#include "CimsInterface.h"

//=========== For use Cims API Index

///////////////////////////////////////////////////////////////////
//20070508  added for dummy operation
typedef AlarmStyle (*TypeREM_ALARM_MANAGE_API)          (int);
///////////////////////////////////////////////////////////////////
typedef void      (*TypeREM_COMMON_PRINTF_API) ( BOOL , char * , char * , LPSTR , ... );
//
typedef int       (*TypeREM_FIND_FROM_STRING_API) ( int , char * );
typedef int       (*TypeREM_dWRITE_FUNCTION_API) ( int , char * );
typedef int       (*TypeREM_dREAD_FUNCTION_API) ( int );
typedef void      (*TypeREM_dRUN_FUNCTION_ABORT_API) ( int );
typedef void      (*TypeREM_dWRITE_FUNCTION_EVENT_API) ( int , char * );
typedef int       (*TypeREM_dREAD_FUNCTION_EVENT_API) ( int );

typedef void      (*TypeREM_dREAD_UPLOAD_MESSAGE_API) ( int , char * );
typedef BOOL      (*TypeREM_dWRITE_UPLOAD_MESSAGE_API) ( char * );

typedef void      (*TypeREM_dWRITE_DIGITAL_API) ( int , int , int * );
typedef int       (*TypeREM_dREAD_DIGITAL_API) ( int , int * );
typedef void      (*TypeREM_dWRITE_ANALOG_API) ( int , double , int * );
typedef double    (*TypeREM_dREAD_ANALOG_API) ( int , int * );
typedef void      (*TypeREM_dWRITE_STRING_API) ( int , char * , int * );
typedef void      (*TypeREM_dREAD_STRING_API) ( int , char * , int * );

//////////////////////////////////////////////////////////////////////////////////////////
//20070508 added for dummy operation
TypeREM_ALARM_MANAGE_API            _USER_INDEX_ALARM_MANAGE_API          = FALSE;
//////////////////////////////////////////////////////////////////////////////////////////
//================================================================================================================
TypeREM_COMMON_PRINTF_API			_USER_INDEX_COMMON_PRINTF_API = FALSE;
//================================================================================================================
TypeREM_FIND_FROM_STRING_API		_USER_INDEX_FIND_FROM_STRING_API = FALSE;
TypeREM_dWRITE_FUNCTION_API			_USER_INDEX_dWRITE_FUNCTION_API = FALSE;
TypeREM_dREAD_FUNCTION_API			_USER_INDEX_dREAD_FUNCTION_API = FALSE;
TypeREM_dRUN_FUNCTION_ABORT_API		_USER_INDEX_dRUN_FUNCTION_ABORT_API = FALSE;
TypeREM_dWRITE_FUNCTION_EVENT_API	_USER_INDEX_dWRITE_FUNCTION_EVENT_API = FALSE;
TypeREM_dREAD_FUNCTION_EVENT_API	_USER_INDEX_dREAD_FUNCTION_EVENT_API = FALSE;
//================================================================================================================
TypeREM_dREAD_UPLOAD_MESSAGE_API	_USER_INDEX_dREAD_UPLOAD_MESSAGE_API = FALSE;
TypeREM_dWRITE_UPLOAD_MESSAGE_API	_USER_INDEX_dWRITE_UPLOAD_MESSAGE_API = FALSE;
//================================================================================================================
TypeREM_dWRITE_DIGITAL_API			_USER_INDEX_dWRITE_DIGITAL_API = FALSE;
TypeREM_dREAD_DIGITAL_API			_USER_INDEX_dREAD_DIGITAL_API = FALSE;
TypeREM_dWRITE_ANALOG_API			_USER_INDEX_dWRITE_ANALOG_API = FALSE;
TypeREM_dREAD_ANALOG_API			_USER_INDEX_dREAD_ANALOG_API = FALSE;
TypeREM_dWRITE_STRING_API			_USER_INDEX_dWRITE_STRING_API = FALSE;
TypeREM_dREAD_STRING_API			_USER_INDEX_dREAD_STRING_API = FALSE;
//================================================================================================================
//================================================================================================================
//================================================================================================================
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//20070508 added for dummy operation
//Not yet supported
__declspec(dllexport) void USER_API_SET_ALARM_MANAGE          (TypeREM_ALARM_MANAGE_API          a) {_USER_INDEX_ALARM_MANAGE_API          = a;	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) void USER_API_SET_COMMON_PRINTF( TypeREM_COMMON_PRINTF_API a ) {	_USER_INDEX_COMMON_PRINTF_API = a;	}
__declspec(dllexport) void USER_API_SET_FIND_FROM_STRING( TypeREM_FIND_FROM_STRING_API a ) {	_USER_INDEX_FIND_FROM_STRING_API = a;	}
__declspec(dllexport) void USER_API_SET_dWRITE_FUNCTION( TypeREM_dWRITE_FUNCTION_API a ) {	_USER_INDEX_dWRITE_FUNCTION_API = a;	}
__declspec(dllexport) void USER_API_SET_dREAD_FUNCTION( TypeREM_dREAD_FUNCTION_API a ) {	_USER_INDEX_dREAD_FUNCTION_API = a;	}
__declspec(dllexport) void USER_API_SET_dRUN_FUNCTION_ABORT( TypeREM_dRUN_FUNCTION_ABORT_API a ) {	_USER_INDEX_dRUN_FUNCTION_ABORT_API = a;	}
__declspec(dllexport) void USER_API_SET_dWRITE_FUNCTION_EVENT( TypeREM_dWRITE_FUNCTION_EVENT_API a ) {	_USER_INDEX_dWRITE_FUNCTION_EVENT_API = a;	}
__declspec(dllexport) void USER_API_SET_dREAD_FUNCTION_EVENT( TypeREM_dREAD_FUNCTION_EVENT_API a ) {	_USER_INDEX_dREAD_FUNCTION_EVENT_API = a;	}
__declspec(dllexport) void USER_API_SET_dREAD_UPLOAD_MESSAGE( TypeREM_dREAD_UPLOAD_MESSAGE_API a ) {	_USER_INDEX_dREAD_UPLOAD_MESSAGE_API = a;	}
__declspec(dllexport) void USER_API_SET_dWRITE_UPLOAD_MESSAGE( TypeREM_dWRITE_UPLOAD_MESSAGE_API a ) {	_USER_INDEX_dWRITE_UPLOAD_MESSAGE_API = a;	}
//
__declspec(dllexport) void USER_API_SET_dWRITE_DIGITAL( TypeREM_dWRITE_DIGITAL_API a ) {	_USER_INDEX_dWRITE_DIGITAL_API = a;	}
__declspec(dllexport) void USER_API_SET_dREAD_DIGITAL( TypeREM_dREAD_DIGITAL_API a ) {	_USER_INDEX_dREAD_DIGITAL_API = a;	}
__declspec(dllexport) void USER_API_SET_dWRITE_ANALOG( TypeREM_dWRITE_ANALOG_API a ) {	_USER_INDEX_dWRITE_ANALOG_API = a;	}
__declspec(dllexport) void USER_API_SET_dREAD_ANALOG( TypeREM_dREAD_ANALOG_API a ) {	_USER_INDEX_dREAD_ANALOG_API = a;	}
__declspec(dllexport) void USER_API_SET_dWRITE_STRING( TypeREM_dWRITE_STRING_API a ) {	_USER_INDEX_dWRITE_STRING_API = a;	}
__declspec(dllexport) void USER_API_SET_dREAD_STRING( TypeREM_dREAD_STRING_API a ) {	_USER_INDEX_dREAD_STRING_API = a;	}

//=========================================================================================
// for Inside DLL API
//=========================================================================================
///////////////////////////////////////////////////////////////////////////////////////////
//20070508 added for dummy operation especially alarm recovery
///////////////////////////////////////////////////////////////////////////////////////////
int _dll_ALARM_MANAGE    (int Alarm_ID) {
	if (!_USER_INDEX_ALARM_MANAGE_API)
		return -1;

	return (*_USER_INDEX_ALARM_MANAGE_API) (Alarm_ID);

}
///////////////////////////////////////////////////////////////////////////////////////////
int _dll_FIND_FROM_STRING( int IO_Type, char *Name ) {
	if ( !_USER_INDEX_FIND_FROM_STRING_API ) return -1;
	return ( *_USER_INDEX_FIND_FROM_STRING_API ) ( IO_Type , Name );
}

int _dll_dWRITE_FUNCTION( int Function_Point , char *Func_Param ) {
	if ( !_USER_INDEX_dWRITE_FUNCTION_API ) return 0;
	return ( *_USER_INDEX_dWRITE_FUNCTION_API ) ( Function_Point , Func_Param );
}

int _dll_dREAD_FUNCTION( int Function_Point ) {
	if ( !_USER_INDEX_dREAD_FUNCTION_API ) return 2;
	return ( *_USER_INDEX_dREAD_FUNCTION_API ) ( Function_Point );
}

void _dll_dRUN_FUNCTION_ABORT( int Function_Point ) {
	if ( !_USER_INDEX_dRUN_FUNCTION_ABORT_API ) return;
	( *_USER_INDEX_dRUN_FUNCTION_ABORT_API ) ( Function_Point );
}

void _dll_dWRITE_FUNCTION_EVENT( int Function_Point , char *Event ) {
	if ( !_USER_INDEX_dWRITE_FUNCTION_EVENT_API ) return;
	( *_USER_INDEX_dWRITE_FUNCTION_EVENT_API ) ( Function_Point , Event );
}

int _dll_dREAD_FUNCTION_EVENT( int Function_Point ) {
	if ( !_USER_INDEX_dREAD_FUNCTION_EVENT_API ) return 2;
	return ( *_USER_INDEX_dREAD_FUNCTION_EVENT_API ) ( Function_Point );
}

void _dll_dREAD_UPLOAD_MESSAGE( int Function_Point , char *ReadString ) {
	if ( !_USER_INDEX_dREAD_UPLOAD_MESSAGE_API ) {
		strcpy( ReadString , "" );
		return;
	}
	( *_USER_INDEX_dREAD_UPLOAD_MESSAGE_API ) ( Function_Point , ReadString );
}

BOOL _dll_dWRITE_UPLOAD_MESSAGE( char *WriteString ) {
	if ( !_USER_INDEX_dWRITE_UPLOAD_MESSAGE_API ) return FALSE;
	return ( *_USER_INDEX_dWRITE_UPLOAD_MESSAGE_API ) ( WriteString );
}

//

void _dll_dWRITE_DIGITAL( int IO_Point , int Data , int *CommStatus ) {
	if ( !_USER_INDEX_dWRITE_DIGITAL_API ) {
		*CommStatus = FALSE;
		return;
	}
	( *_USER_INDEX_dWRITE_DIGITAL_API ) ( IO_Point , Data , CommStatus );
}

int _dll_dREAD_DIGITAL( int IO_Point , int *CommStatus ) {
	if ( !_USER_INDEX_dREAD_DIGITAL_API ) {
		*CommStatus = FALSE;
		return 0;
	}
	return ( *_USER_INDEX_dREAD_DIGITAL_API ) ( IO_Point , CommStatus );
}

void _dll_dWRITE_ANALOG( int IO_Point , double Data , int *CommStatus ) {
	if ( !_USER_INDEX_dWRITE_ANALOG_API ) {
		*CommStatus = FALSE;
		return;
	}
	( *_USER_INDEX_dWRITE_ANALOG_API ) ( IO_Point , Data , CommStatus );
}

double _dll_dREAD_ANALOG( int IO_Point , int *CommStatus ) {
	if ( !_USER_INDEX_dREAD_ANALOG_API ) {
		*CommStatus = FALSE;
		return 0;
	}
	return ( *_USER_INDEX_dREAD_ANALOG_API ) ( IO_Point , CommStatus );
}

void _dll_dWRITE_STRING( int IO_Point , char *WriteData , int *CommStatus ) {
	if ( !_USER_INDEX_dWRITE_STRING_API ) {
		*CommStatus = FALSE;
		return;
	}
	( *_USER_INDEX_dWRITE_STRING_API ) ( IO_Point , WriteData , CommStatus );
}

void _dll_dREAD_STRING( int IO_Point , char *ReadData , int *CommStatus ) {
	if ( !_USER_INDEX_dREAD_STRING_API ) {
		*CommStatus = FALSE;
		strcpy( ReadData , "" );
		return;
	}
	( *_USER_INDEX_dREAD_STRING_API ) ( IO_Point , ReadData , CommStatus );
}

