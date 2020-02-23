#ifndef DRIVER__H
#define DRIVER__H

//------------------------------------------------------------------------------------
BOOL   Device_Attach(  char * , char * , int , int , int , int , int , int , int , int , int , int );
BOOL   Device_Detach(  char * , char * , int , int , int , int , int , int , int , int , int , int );
//
double Analog_In(   int , int , int , int * );
void   Analog_Out(  int , int , int , double , int * );
//
//
int    Digital_In(  int , int , int , int , int * );
void   Digital_Out( int , int , int , int , int , int * );
//
void String_In( int ID1 , char *data , int *Result );
void String_Out( int ID1 , char *data , int *Result );

// Unused
double Analog_Name_In( char *Name , int *Result );
void Analog_Name_Out( char *Name , double data , int *Result );
int Digital_Name_In( char *Name , int *Result ); 
void Digital_Name_Out( char *Name , int data , int *Result ); 
void String_Name_In( char *Name , char *data , int *Result ); 
void String_Name_Out( char *Name , char *data , int *Result );
//------------------------------------------------------------------------------------

#endif
