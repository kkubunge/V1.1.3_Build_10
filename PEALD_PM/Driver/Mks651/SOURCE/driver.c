#include "cimrs232.h"
#include "driver.h"

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
char SendTermStr[12];
char RecvTermStr[12];
int  TimeOut = 100;

BOOL CommStatus = FALSE;
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//
//
BOOL Device_Attach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
	SendTermStr[0] = 13;
	SendTermStr[1] = 10;
	SendTermStr[2] = 0;
	RecvTermStr[0] = 13;
	RecvTermStr[1] = 10;
	RecvTermStr[2] = 0;
	if ( ID10 < 100 ) TimeOut = 100;
	else              TimeOut = ID10;
	return( RS232_Connect_Port( ID1 , ID2 , ID3 , ID4 , ID5 , ID6 , ID7 , ID8 ) );
}
//
//
//
//
//
//
BOOL Device_Detach( char *Name , char *File , int ID1 , int ID2 , int ID3 , int ID4 , int ID5 , int ID6 , int ID7 , int ID8 , int ID9 , int ID10 ) {
	RS232_Disconnect_Port();
	return TRUE;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
BOOL Check_Enable_Data( char *Buffer , char *Buffer2 , int Count ) {
	int i;
	for ( i = 0 ; i < Count ; i++ ) {
		if ( Buffer[i] != Buffer2[i] ) return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------------------------------------------
BOOL Rs232_IO_Send( char *Cmnds ) {
	RS232_Write_String( Cmnds , strlen( Cmnds ) );
	return( RS232_Write_String( SendTermStr , strlen( SendTermStr ) ) );
}
//-----------------------------------------------------------------------------------
BOOL mks651io( char *Command , char *Response , int rmode ) {
	int Result;
	int ReadCnt;

	if ( rmode ) RS232_Clear_Buffer();

	Result = Rs232_IO_Send( Command );

	if ( rmode ) Result = RS232_Read_String( Response , RecvTermStr , 255 , TimeOut , &ReadCnt );

	if ( !Result ) CommStatus = FALSE;
	else           CommStatus = TRUE;
	return Result;
}
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
int Digital_In( int ID1 , int ID2 , int ID3 , int ID4 , int *Result ) {
	char  cmnd[256] , resp[256] , chks[32];
	int   i , ClearCnt = 2;
	int	  iii = 0;

	*Result = TRUE;
	// Prepare Part
	switch ( ID1 ) {
		case   0 :
					return CommStatus;
					break;

		case   1 :	// Read Remote Local
					sprintf( cmnd , "R37" );
					sprintf( chks , "M" );
					ClearCnt = 1;
                    break;

		case   5 :	// Read Valve Status
					sprintf( cmnd , "R7" );
					sprintf( chks , "M" );
					ClearCnt = 1;
                    break;

		case   6 :	// Read Setpoint Status
					sprintf( cmnd , "R7" );
					sprintf( chks , "M" );
					ClearCnt = 1;
                    break;

		case  11 :	// Read Control Mode
					sprintf( cmnd , "R51" );
					sprintf( chks , "V" );
					ClearCnt = 1;
                    break;

		case  12 :	// Read Calibration Valve
					sprintf( cmnd , "R23" );
					sprintf( chks , "J" );
					ClearCnt = 1;
                    break;

		case  13 :	// Read Sensor Voltage Range
					sprintf( cmnd , "R35" );
					sprintf( chks , "G" );
					ClearCnt = 1;
                    break;

		case  14 :	// Read Sensor Range
					sprintf( cmnd , "R33" );
					sprintf( chks , "E" );
					ClearCnt = 1;
                    break;

		case  15 :	// Read Pressure Unit
					sprintf( cmnd , "R34" );
					sprintf( chks , "F" );
					ClearCnt = 1;
                    break;

		case  16 :	// Read Sensor Type Abs / Diff
					sprintf( cmnd , "R36" );
					sprintf( chks , "U" );
					ClearCnt = 1;
                    break;

		case  17 :	// Read Direct Reverse
					sprintf( cmnd , "R32" );
					sprintf( chks , "N" );
					ClearCnt = 1;
                    break;

		case  18 :	// Read Position Indicator Range
					sprintf( cmnd , "R31" );
					sprintf( chks , "B" );
					ClearCnt = 1;
                    break;

		case  19 :	// Read Kt
					sprintf( cmnd , "R40" );
					sprintf( chks , "K" );
					ClearCnt = 1;
                    break;

		case  21 :	// Read Active Setpoint type
					sprintf( cmnd , "R%d" , ID2 + 25 );
					sprintf( chks , "T%d" , ID2 );
					//ClearCnt = 3;
                    break;

		case  22 :	// Read Analog Voltage Type
					sprintf( cmnd , "R24" );
					sprintf( chks , "A" );
					ClearCnt = 1;
                    break;

		default  :
					*Result = FALSE;
					printf( "MKS651 D - [%d-%d-%d-%d] - Unknown parameter\n" , ID1 , ID2 , ID3 , ID4 );
					return 0;
	}
	// Communication Part
	if ( !mks651io( cmnd , resp , TRUE ) ) {
		*Result = FALSE;
		printf( "MKS651 D - [%d-%d-%d-%d] - Receive Error\n" , ID1 , ID2 , ID3 , ID4 );
		return 0;
	}
	else {
		if ( Check_Enable_Data( resp , chks , strlen(chks) ) ) {
			for ( i = 0 ; i < ClearCnt ; i++ ) resp[i] = ' ';
			iii = atoi( resp );
		}
		else {
			*Result = FALSE;
			printf( "MKS651 D - [%d-%d-%d-%d-%s] - Invalid Error\n" , ID1 , ID2 , ID3 , ID4 , resp );
			return 0;
		}
	}
	// Conversion Part
	switch ( ID1 ) {
		case  1 :
					return ( iii / 100 );
					break;
		case  5 :
					iii = iii / 10;
					if      ( iii % 10 == 2 ) iii = 1;
					else if ( iii % 10 == 4 ) iii = 2;
					else                      iii = 0;
					break;
		case  6 :
					return ( iii / 100 );
					break;
	}
	return iii;
}
//-------------------------------------------------------
//-------------------------------------------------------
//-------------------------------------------------------

double Analog_In( int ID1 , int ID2 , int ID3 , int *Result ) {
	char  cmnd[256] , resp[256] , chks[32];
	int   i , ClearCnt = 2;
	double fff = 0;

	*Result = TRUE;
	// Prepare Part
	switch ( ID1 ) {
		case  1 :	// Read Pressure
					sprintf( cmnd , "R5" );
					sprintf( chks , "P" );
					ClearCnt = 1;
                    break;

		case  2 :	// Read Position
					sprintf( cmnd , "R6" );
					sprintf( chks , "V" );
					ClearCnt = 1;
                    break;

		case  11 :	// Read Setpoint
					if ( ID2 == 5 ) sprintf( cmnd , "R10" );
					else            sprintf( cmnd , "R%d" , ID2 );
					sprintf( chks , "S%d" , ID2 );
                    break;
		case  21 :	// Read Lead
					sprintf( cmnd , "R%d" , ID2 + 40 );
					sprintf( chks , "X%d" , ID2 );
                    break;

		case  22 :	// Read Gain
					sprintf( cmnd , "R%d" , ID2 + 45 );
					sprintf( chks , "M%d" , ID2 );
                    break;

		case  23 :	// Read Soft Start
					sprintf( cmnd , "R%d" , ID2 + 14 );
					sprintf( chks , "I%d" , ID2 );
                    break;

		case  24 :	// Read Fan Status
					sprintf( cmnd , "R0");
					sprintf( chks , "S0");
                    break;

		default  :
					*Result = FALSE;
					printf( "MKS651 A - [%d-%d-%d] - Unknown parameter\n" , ID1 , ID2 , ID3 );
					return 0;
	}
	// Communication Part
	if ( !mks651io( cmnd , resp , TRUE ) ) {
		*Result = FALSE;
		printf( "MKS651 A - [%d-%d-%d] - Receive Error\n" , ID1 , ID2 , ID3 );
		return 0;
	}
	else {
		if ( Check_Enable_Data( resp , chks , strlen(chks) ) ) {
			for ( i = 0 ; i < ClearCnt ; i++ ) resp[i] = ' ';
			fff = atof( resp );
			if(ID1 == 1 || ID1 == 11) fff /= 10;		// 2007.07.01
		}
		else {
			*Result = FALSE;
			printf( "MKS651 A - [%d-%d-%d-%s] - Invalid Error\n" , ID1 , ID2 , ID3 , resp );
			return 0;
		}
	}
	return fff;
}

//-------------------------------------------------------
//-------------------------------------------------------
//-------------------------------------------------------

void Digital_Out( int ID1 , int ID2 , int ID3 , int ID4 , int data , int *Result ) {
	char cmnd[256];

	switch ( ID1 ) {
		case   5 :	if      ( data == 0 ) sprintf( cmnd , "H" );
					else if ( data == 1 ) sprintf( cmnd , "O" );
					else if ( data == 2 ) sprintf( cmnd , "C" );
					break;
	
		case   6 :	// Active Setpoint
					sprintf( cmnd , "D%d" , data );
					break;

		case   11 :	// Control Mode
					sprintf( cmnd , "V%d" , data );
					break;
		
		case   12 :	// Calibrate Valve
					sprintf( cmnd , "J%d" , data );
					break;
		

		case   13 :	// Sensor Full Scale Voltage
					sprintf( cmnd , "G%d" , data );
					break;

		case   14 :	// Sensor Range
					sprintf( cmnd , "E%d" , data );
					break;

		case   15 :	// Sensor Unit
					sprintf( cmnd , "F%d" , data );
					break;

		case   16 :	// Sensor Type
					sprintf( cmnd , "U%d" , data );
					break;

		case   17 :	// Direct / Reverse
					sprintf( cmnd , "N%d" , data );
					break;

		case   18 :	// Valve Position Range
					sprintf( cmnd , "B%d" , data );
					break;

		case   19 :	// Disable Battery Backup / Open Fail / Close Fail
					sprintf( cmnd , "K%d" , data );
					break;

		case   20 :	// Zero / Special Zero / Remove zero
					sprintf( cmnd , "Z%d" , data + 1 );
					break;

		case   21 :	// Control Type
					sprintf( cmnd , "T%d%d" , ID2 , data );
					break;

		case   22 :	// Analog Voltage Type
					sprintf( cmnd , "A%d" , data );
					break;

		default :	*Result = FALSE;
					return;
					break;
	}
	*Result = Rs232_IO_Send( cmnd );
}

//-------------------------------------------------------
//-------------------------------------------------------
//-------------------------------------------------------

void Analog_Out( int ID1 , int ID2 , int ID3 , double data , int *Result ) {
	char cmnd[256];

	switch ( ID1 ) {
		case  11 :	// New Setpoint
					sprintf( cmnd , "S%d%lf" , ID2 , data );
					break;

		case  21 :	// New Lead
					sprintf( cmnd , "X%d%lf" , ID2 , data );
					break;

		case  22 :	// New Gain
					sprintf( cmnd , "M%d%lf" , ID2 , data );
					break;

		case  23 :	// New SoftStart Rate
					sprintf( cmnd , "I%d%f" , ID2 , data );
					break;

		case  41 :	// New Setpoint Pressure
					data *= 10;					// 2005.12.05
					sprintf( cmnd , "S%d%lf" , ID2 , data );
					*Result = Rs232_IO_Send( cmnd );
					sprintf( cmnd , "T%d1" , ID2 );
					*Result = Rs232_IO_Send( cmnd );
					sprintf( cmnd , "D%d" , ID2 );
					break;

		case  42 :	// New Setpoint Position
					sprintf( cmnd , "S%d%lf" , ID2 , data );
					*Result = Rs232_IO_Send( cmnd );
					sprintf( cmnd , "T%d0" , ID2 );
					*Result = Rs232_IO_Send( cmnd );
					sprintf( cmnd , "D%d" , ID2 );
					break;

		default :	*Result = FALSE;
					return;
					break;
	}
	*Result = Rs232_IO_Send( cmnd );
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void String_In( int ID1 , char *data , int *Result ) {
	*Result = TRUE;
}
//
void String_Out( int ID1, char *data , int *Result ) {
	*Result = TRUE;
}
//
//-------------------------------------------------------------------------------------
//
double Analog_Name_In( char *Name , int *Result ) {
	*Result = TRUE;
	return 0;
}
//
void Analog_Name_Out( char *Name , double data , int *Result ) {
	*Result = TRUE;
}
//
int Digital_Name_In( char *Name , int *Result ) {
	*Result = TRUE;
	return 0;
}
//
void Digital_Name_Out( char *Name , int data , int *Result ) {
	*Result = TRUE;
}
//
//
void String_Name_In( char *Name , char *data , int *Result ) {
	*Result = TRUE;
}

void String_Name_Out( char *Name , char *data , int *Result ) {
	*Result = TRUE;
}
//
