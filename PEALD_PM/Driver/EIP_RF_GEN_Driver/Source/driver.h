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
int    Digital_In(  int , int , int , int , int * );
void   Digital_Out( int , int , int , int , int , int * );
//
void String_In( int ID1 , char *data , int *Result );
void String_Out( int ID1, char *data , int *Result );

// not use.
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
	int freqMode;
	int ignitionMode;
	int startFreq;
	int minFreq;
	int maxFreq;
	int freqStepMax;
	int freqStepMin;
	int returningThreshold;
	int highTuneThreshold;
	int lowTuneThreshold;
	int gammaThresholdMode;
	int tuningTimeout;
	int tuningDelay;
	int tuningGainDelay;
	int maxTuningCount;
	int stepUpGain;
	int stepDownGain;
	int scanStepSize;
	int tuningStepTime;
	int ipAddr;
	int subnetMask;
	int gatewayAddr;
	int dhcpClient;
} Parameters, *pParameters;

typedef struct _RF_DCOP {
	double deliver;
	double reflect;
	double freq;
} RF_DCOP;

typedef enum {
	Param_Ready, Param_Updaing
} ParameterUpdateStatus;

void CalcDCOPAverage();
void InitSummaryData();
//------------------------------------------------------------------------------------

#endif
