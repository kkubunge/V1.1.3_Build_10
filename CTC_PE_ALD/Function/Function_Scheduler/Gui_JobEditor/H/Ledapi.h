#ifndef LEDAPI_H
#define LEDAPI_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* - - - - - - - - - - - -   D E F I N E S    - - - - - - - - - - - - - - */

#define CDB_ADDR void *

//void All2FileName( char * , const char * , int );

//void KPLT_Local_Time( long time_data , char *buffer );

//void KPLT_Local_Time2( long time_data , char *buffer );

//void KPLT_Local_Time_for_File( long time_data , char *buffer );

//void KPLT_Elapsed_Time( long time_data , char *buffer );

void Kor_JOB_Initial();

BOOL Kor_JOB_Open( char *filename );

BOOL Kor_JOB_Save( char *filename );
	
	
void Kor_Info_Initial();

BOOL Kor_Info_Open( char *filename );

BOOL Kor_Info_Save( char *filename );



int Get_Info_LOG_SAVE_MONTH();
void Set_Info_LOG_SAVE_MONTH( int data );

BOOL Get_Info_DUAL_LOT_MODE();
void Set_Info_DUAL_LOT_MODE( BOOL data );

BOOL Get_Info_DISPLAY_MODE();
void Set_Info_DISPLAY_MODE( BOOL data );

int  Get_Info_AUTO_DELETE_MODE();
void Set_Info_AUTO_DELETE_MODE( int data );

int  Get_Info_END_TIME_SHOW_MODE();
void Set_Info_END_TIME_SHOW_MODE( int data );

int  Get_Info_PRE_POST_SHOW_MODE();
void Set_Info_PRE_POST_SHOW_MODE( int data );

int  Get_Info_DELETE_COUNT(); // 2014.10.15
void Set_Info_DELETE_COUNT( int data ); // 2014.10.15

#endif