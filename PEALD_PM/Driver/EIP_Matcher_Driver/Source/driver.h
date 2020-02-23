//////////////////////////////////////////////////////////////////////////////////////
// LFGS MLS300 Driver
// Version : 1.01
// Modify : 2004.08.17

#ifndef DRIVER__H
#define DRIVER__H

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
extern "C" 
{
BOOL   Device_Attach(  char * , char * , int , int , int , int , int , int , int , int , int , int );
BOOL   Device_Detach(  char * , char * , int , int , int , int , int , int , int , int , int , int );
//
double Analog_In(   int , int , int , int * );
void   Analog_Out(  int , int , int , double , int * );
//
//
int    Digital_In(  int , int , int , int , int * );
void   Digital_Out( int , int , int , int , int , int * );

// not use.
void String_In( int ID1 , char *data , int *Result );
void String_Out( int ID1, char *data , int *Result );
double Analog_Name_In( char *Name , int *Result );
void Analog_Name_Out( char *Name , double data , int *Result );
int Digital_Name_In( char *Name , int *Result );
void Digital_Name_Out( char *Name , int data , int *Result );
void String_Name_In( char *Name , char *data , int *Result );
void String_Name_Out( char *Name , char *data , int *Result );
}
//------------------------------------------------------------------------------------
// Structures

typedef struct _Parameters {
	float stopVSWR;
	float startVSWR;
	int maxStepLoad;
	int maxStepTune;
	int targetPosLoad;
	int targetPosTune;
	int targetZR;
	int targetZX;
	int V_ThreadoldCount;
	int I_ThreadoldCount;
	int priorTime;
	int future;
} Parameters, *pParameters;

typedef struct _RF_DCOP {
	double dbLoadPostion;
	double dbTunePostion; 
	double dbZ_ScanR;
	double dbZ_ScanX;	
	double dbZ_ScanVoltage;
	double dbZ_ScanCurr; 	
	double dbZ_ScanPhase;
	double dbZ_ScanPower;
} RF_DCOP;

void CalcDCOPAverage();
void InitSummaryData();

typedef enum {
	Param_Ready, Param_Updaing
} ParameterUpdateStatus;


//------------------------------------------------------------------------------------
#endif