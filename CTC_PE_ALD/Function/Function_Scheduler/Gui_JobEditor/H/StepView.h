#ifndef STEPVIEW_H
#define STEPVIEW_H

void STEP_Get_AVR_VAR_Value( int cnt , double sum , double sum2 , double *avr , double *var );

void STEP_CHECK_INIT();

void STEP_CHECK_DISPLAY();

void STEP_CHECK_CALDATA_RESET( BOOL , char *ts );

void STEP_CHECK_FILE_START();

BOOL STEP_CHECK_FILE_APPEND_TARGET( char *Name , char *dataString );

BOOL STEP_CHECK_FILE_APPEND_TIMESTABLE( char *Name , char *dataString );

BOOL STEP_CHECK_FILE_APPEND_TIMESTEP( char *Name , char *dataString );

BOOL STEP_CHECK_FILE_APPEND_STABLE( char *Name , char *dataString );

BOOL STEP_CHECK_FILE_APPEND_OPTION( char *dataString );

BOOL STEP_CHECK_FILE_APPEND_STEP( char *Name , char *dataString );

BOOL STEP_CHECK_FILE_APPEND_USER( char *Name , char *dataString );

BOOL STEP_CHECK_LOGDATA_MAKE_NAMEINDEX( char *NameString );

BOOL STEP_CHECK_LOGDATA_STEP_DATA( int StepIndex , char *TimeString , double timgap , char *DataString , int *stepinx , int LVPos );

void STEP_CHECK_LOGDATA_INC_LV_GARBAGE();

int  STEP_CHECK_LOGDATA_GET_LV_STEP( int ItemIndex , int LVPos );

void STEP_CHECK_LOGDATA_FIND_POST( BOOL first , char *TimeString , int lp );

void STEP_CHECK_LOGDATA_END( char *TimeString );

void STEPVIEW_REVIEW( HWND hWnd , int id );

BOOL STEP_VIEW_RUN_EDITING();

BOOL STEP_VIEW_EDITING( HWND hdlg );

#endif