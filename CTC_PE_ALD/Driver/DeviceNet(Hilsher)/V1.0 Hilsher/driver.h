#ifndef DRIVER__H
#define DRIVER__H

//-------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <process.h>
#include <windows.h>
#include <winuser.h>
#include <winbase.h>
#include <time.h>
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
BOOL   Device_Attach( char * , char * , int , int , int , int , int , int , int , int , int , int );
BOOL   Device_Detach( char * , char * , int , int , int , int , int , int , int , int , int , int );
//
double Analog_In(   int , int , int , int * );
void   Analog_Out(  int , int , int , double , int * );
//
//
int    Digital_In(  int , int , int , int , int * );
void   Digital_Out( int , int , int , int , int , int * );
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#endif /* DRIVER_H */
