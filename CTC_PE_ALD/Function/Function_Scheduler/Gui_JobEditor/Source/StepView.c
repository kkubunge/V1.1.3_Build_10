//===========================================================================================================================
#define _CRT_SECURE_NO_DEPRECATE
//===========================================================================================================================

#include <stdio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

#include <windows.h>
#include <wingdi.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>

#include <Kutlstr.h>
#include <Kutlanal.h>
#include <Kutltime.h>
#include <Kutlfile.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>

#include "appCTC.h"

#include "iodefine.h"
#include "lottype.h"
#include "ledapi.h"
#include "appinit.h"
#include "appprm.h"
#include "logview.h"
#include "Jobdata.h"
#include "appPlotting.h"

#include "cimseqnc.h"
#include "resource.h"

int  KGUI_FT_Set_Color_Data( int index , char *data );

void Kor_StepView_Initial( StepViewTemplate *STD );

int LogView_GetParam( HWND hList , int r );
int  ListView_Draw_Get_TextColor( int bcolor ); // 2016.10.10


extern char JOB_INFO_FILENAME[256];


extern int    Dat_Count_Total;
extern int    Dat_Count_Total_Step[MAX_STEP_COUNT];

extern BOOL   Dat_Set_StepUse[MAX_STEP_COUNT];

extern BOOL   Dat_Set_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_Sum_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_Sum2_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_First_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_Last_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_Min_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern double Dat_Max_Step[MAX_STEP_COUNT][MAX_X_TABLE];


extern int    Dat_Count_Sum_Step[MAX_STEP_COUNT][MAX_X_TABLE];
extern int    Dat_Count_MinMax_Step[MAX_STEP_COUNT][MAX_X_TABLE];

//
//================================
//
// 2016.08.18
//
//
// Log Data
//
int    Step_Log_Item_Index[MAX_X_TABLE];
char   *Step_Log_Item_Name[MAX_X_TABLE];
int    Step_Log_Step_Count;
int    Step_Log_Item_Count;
//
char   Step_Log_FileName[128] = { 0 , }; // 2018.04.24
char   Step_Log_StepName[MAX_STEP_COUNT][64]; // 2018.04.24
int    Step_Log_StepStart[MAX_STEP_COUNT]; // 2018.04.24
int    Step_Log_StepEnd[MAX_STEP_COUNT]; // 2018.04.24

//
// Inside Calculation Buffer
//
double iStepView_MarginMin[MAX_X_TABLE];
double iStepView_MarginMax[MAX_X_TABLE];
double iStepView_MarginTime[MAX_X_TABLE];
int    iStepView_MarginOK[MAX_X_TABLE];

//
// StepView File DataBase
//
int     StepView_Item_Count;
int     StepView_Step_Count;

int     StepView_TimeView_Option; // xx:xx:xx xx:xx:xx.x xx:xx:xx.xx xx:xx:xx.xxx xx xx.x xx.xx xx.xxx
int     StepView_DecimalPoint_Option; // x x.x x.xx x.xxx x.xxxx
char   *StepView_User_Title;

char   *StepView_Item_Name[MAX_X_TABLE];
//
double  StepView_Ratio[MAX_X_TABLE];
double  StepView_CTime[MAX_X_TABLE];
//
BOOL    StepView_HasTarget[MAX_STEP_COUNT][MAX_X_TABLE];
double  StepView_TargetData[MAX_STEP_COUNT][MAX_X_TABLE];

char   *StepView_TimeStable[MAX_STEP_COUNT][MAX_X_TABLE];
char   *StepView_TimeStep[MAX_STEP_COUNT][MAX_X_TABLE];

char   *StepView_UserData[MAX_STEP_COUNT][MAX_X_TABLE];

//
// Time Cal
//

int 	StepView_Count_Step[MAX_STEP_COUNT][MAX_X_TABLE];
double	StepView_Time_Step[MAX_STEP_COUNT][MAX_X_TABLE];

//

int 	StepView_Count_Step_AllItem[MAX_STEP_COUNT];
double	StepView_Time_Step_AllItem[MAX_STEP_COUNT];

char	*StepView_Run_StartTime;
char	*StepView_Run_EndTime;

char	*StepView_Pre_StartTime;
char	*StepView_Pre_EndTime;

char	*StepView_Step_StartTime;
char	*StepView_Step_EndTime;

char	*StepView_Post_StartTime;
char	*StepView_Post_EndTime;

char	*StepView_StartTime[MAX_STEP_COUNT];
char	*StepView_EndTime[MAX_STEP_COUNT];
int 	 StepView_Count[MAX_STEP_COUNT];
double	 StepView_Time[MAX_STEP_COUNT];

char	*StepView_SetTime[MAX_STEP_COUNT];
char	*StepView_Result[MAX_STEP_COUNT];
//
// ListView Display
//
#define MAX_LV_PRCS_ROW_COUNT	30000 
//
char    StepView_LV_Map[MAX_X_TABLE][MAX_LV_PRCS_ROW_COUNT];

int     StepView_LV_From_Post;

int     StepView_LV_GarbageCount;

//




double STEP_Get_Var_Value( double sum , double avr , int cnt ) {
	double var;
	if ( cnt <= 0 ) return 0;
	var = ( sum / (double) cnt ) - (avr*avr);
	if ( var <= 0 ) return 0;
	return sqrt(var);
}


void STEP_Get_AVR_VAR_Value( int cnt , double sum , double sum2 , double *avr , double *var ) {
	//
	if ( cnt > 0 ) {
		//
		*avr = ( sum / (double) cnt );
		//
		*var = STEP_Get_Var_Value( sum2 , *avr , cnt );
		//
	}
	else {
		*avr = 0;
		*var = 0;
	}
	//
}


void STEP_CHECK_INIT() {
	int i , j;
	//
	StepView_User_Title = NULL;
	//
	for ( i = 0 ; i < MAX_X_TABLE ; i++ ) {
		//
		Step_Log_Item_Name[i] = NULL;
		//
		StepView_Item_Name[i] = NULL;
		//
		for ( j = 0 ; j < MAX_STEP_COUNT ; j++ ) {
			//
			StepView_TimeStable[j][i] = NULL;
			StepView_TimeStep[j][i] = NULL;
			//
			StepView_UserData[j][i] = NULL;
			//
		}
		//
	}
	//
	StepView_Run_StartTime = NULL;
	StepView_Run_EndTime = NULL;
	StepView_Step_StartTime = NULL;
	StepView_Step_EndTime = NULL;
	//
	StepView_Pre_StartTime = NULL;
	StepView_Pre_EndTime = NULL;
	StepView_Post_StartTime = NULL;
	StepView_Post_EndTime = NULL;
	//
	for ( i = 0 ; i < MAX_STEP_COUNT ; i++ ) {
		//
		StepView_StartTime[i] = NULL;
		StepView_EndTime[i] = NULL;
		//
		StepView_SetTime[i] = NULL;
		StepView_Result[i] = NULL;
		//
	}
	//
}

void STEP_CHECK_DISPLAY() {
	//
	int i , j;
	double avr , var;
	FILE *fpt;
	//
	//
	return;
	//
	//
	fpt = fopen( "stepcheck.txt" , "w" );
	//
	fprintf( fpt , "StepView_Item_Count = %d , StepView_TimeView_Option = %d , StepView_LV_GarbageCount=%d , StepView_LV_From_Post=%d\n\n" , StepView_Item_Count , StepView_TimeView_Option , StepView_LV_GarbageCount , StepView_LV_From_Post );
	//
	//
	fprintf( fpt , "<ITEM>	[ID]	StepView_Item_Name	--Ratio---	---Time---	---Min----	---Max----	---MTim---	OK\n" );
	for ( i = 0 ; i < StepView_Item_Count ; i++ ) {
		fprintf( fpt , "      	[%2d]	%14s	%10.3f	%10.3f	%10.3f	%10.3f	%10.3f	%d\n" , i , STR_MEM_GET_STR( StepView_Item_Name[ i ] ) , StepView_Ratio[i] , StepView_CTime[i] , iStepView_MarginMin[i] , iStepView_MarginMax[i] , iStepView_MarginTime[i] , iStepView_MarginOK[i] );
	}
	//
	fprintf( fpt , "<TARGET>\n" );
	for ( i = 0 ; i < StepView_Item_Count ; i++ ) {
		fprintf( fpt , "		[%d]" , i );
		for ( j = 0 ; j < StepView_Step_Count ; j++ ) {
			fprintf( fpt , "	[%d,%10.3f]" , StepView_HasTarget[j][i] , StepView_TargetData[j][i] );
		}
		fprintf( fpt , "\n" );
	}

	fprintf( fpt , "<STEP_USER>	%s\n" , STR_MEM_GET_STR( StepView_User_Title ) );

	for ( i = 0 ; i < StepView_Step_Count ; i++ ) {
		fprintf( fpt , "<STEP%3d>	[ID]	---User---\n" , i );
		for ( j = 0 ; j < StepView_Item_Count ; j++ ) {
			fprintf( fpt , "         	[%2d]	%10s\n" , j , STR_MEM_GET_STR( StepView_UserData[i][j] ) );
		}
	}

	//
	//
	//

	fprintf( fpt , "<TIME>\n" );
	//
	fprintf( fpt , "	RUN     %20s	%20s\n" , STR_MEM_GET_STR( StepView_Run_StartTime ) , STR_MEM_GET_STR( StepView_Run_EndTime ) );
	fprintf( fpt , "	PRE     %20s	%20s\n" , STR_MEM_GET_STR( StepView_Pre_StartTime ) , STR_MEM_GET_STR( StepView_Pre_EndTime ) );
	fprintf( fpt , "	STEP    %20s	%20s\n" , STR_MEM_GET_STR( StepView_Step_StartTime ) , STR_MEM_GET_STR( StepView_Step_EndTime ) );
	fprintf( fpt , "	POST    %20s	%20s\n" , STR_MEM_GET_STR( StepView_Post_StartTime ) , STR_MEM_GET_STR( StepView_Post_EndTime ) );
	//
	for ( i = 0 ; i < Step_Log_Step_Count ; i++ ) {
		fprintf( fpt , "<STEP%3d>	[ID]	Count	---Time---\n" , i );
		for ( j = 0 ; j < StepView_Item_Count ; j++ ) {
			fprintf( fpt , "         	[%2d]	%5d	%10.3f\n" , j , StepView_Count_Step[i][j] , StepView_Time_Step[i][j] );
		}
	}

	fprintf( fpt , "<STEP>	[ID]	-----START_TIME-----	------END_TIME------	ALL_COUNT-	-ALL_TIME-	STEP_COUNT	STEP_TIME-\n" );
	for ( i = 0 ; i < Step_Log_Step_Count ; i++ ) {
		fprintf( fpt , "      	[%2d]	%20s	%20s	%10d	%10.3f	%10d	%10.3f\n" , i , STR_MEM_GET_STR( StepView_StartTime[i] ) , STR_MEM_GET_STR( StepView_EndTime[i] ) , StepView_Count[i] , StepView_Time[i] , StepView_Count_Step_AllItem[i] , StepView_Time_Step_AllItem[i] );
	}
	//

	fprintf( fpt , "\n===================================================================================\n" );

	fprintf( fpt , "<DATA1> [ITEM] [SET CNT AVR VAR First Last CNT2 Min Max]\n" );
	for ( i = 0 ; i < StepView_Item_Count ; i++ ) {
		fprintf( fpt , "        [%2d]" , i );
		for ( j = 0 ; j < Step_Log_Step_Count ; j++ ) {
			//
			STEP_Get_AVR_VAR_Value( Dat_Count_Sum_Step[j][i] , Dat_Sum_Step[j][i] , Dat_Sum2_Step[j][i] , &avr , &var );
			//
			fprintf( fpt , " [%2d,%5d%20.3f,%20.3f,%10.3f,%10.3f,%5d,%10.3f,%10.3f]" , Dat_Set_Step[j][i] , Dat_Count_Sum_Step[j][i] , avr , var , Dat_First_Step[j][i] , Dat_Last_Step[j][i] , Dat_Count_MinMax_Step[j][i] , Dat_Min_Step[j][i] , Dat_Max_Step[j][i] );
		}
		fprintf( fpt , "\n" );
	}

	fprintf( fpt , "<DATA2> [STEP]	USE	TotalCount TOTAL=[%d]\n" , Dat_Count_Total );
	for ( i = 0 ; i < Step_Log_Step_Count ; i++ ) {
		fprintf( fpt , "        [%4d] %3d	%10d\n" , i , Dat_Set_StepUse[i] , Dat_Count_Total_Step[i] );
	}

	fclose( fpt );

}



void STEP_CHECK_CALDATA_RESET( BOOL PreMake , char *TimeString ) {
	int i , j;
	//
	for ( i = 0 ; i < MAX_X_TABLE ; i++ ) {
		//
		for ( j = 0 ; j < MAX_LV_PRCS_ROW_COUNT ; j++ ) {
			StepView_LV_Map[i][j] = 0;
		}
	}
	//
	StepView_LV_From_Post = -1;
	//
	if ( PreMake ) {
		STR_MEM_MAKE_COPY2( &(StepView_Pre_StartTime) , STR_MEM_GET_STR( StepView_Run_StartTime ) );
		STR_MEM_MAKE_COPY2( &(StepView_Pre_EndTime) , STR_MEM_GET_STR( TimeString ) );
		//
		STR_MEM_MAKE_COPY2( &(StepView_Step_StartTime) , NULL );
		STR_MEM_MAKE_COPY2( &(StepView_Step_EndTime) , NULL );
	}
	else {
		STR_MEM_MAKE_COPY2( &(StepView_Run_StartTime) , NULL );
		STR_MEM_MAKE_COPY2( &(StepView_Run_EndTime) , NULL );

		STR_MEM_MAKE_COPY2( &(StepView_Pre_StartTime) , NULL );
		STR_MEM_MAKE_COPY2( &(StepView_Pre_EndTime) , NULL );

		STR_MEM_MAKE_COPY2( &(StepView_Step_StartTime) , NULL );
		STR_MEM_MAKE_COPY2( &(StepView_Step_EndTime) , NULL );

		STR_MEM_MAKE_COPY2( &(StepView_Post_StartTime) , NULL );
		STR_MEM_MAKE_COPY2( &(StepView_Post_EndTime) , NULL );
	}
	//
	for ( i = 0 ; i < MAX_STEP_COUNT ; i++ ) {
		//
		STR_MEM_MAKE_COPY2( &(StepView_StartTime[i]) , NULL );
		STR_MEM_MAKE_COPY2( &(StepView_EndTime[i]) , NULL );
		//
		StepView_Count[i] = 0;
		StepView_Time[i] = 0;
		//
		StepView_Count_Step_AllItem[i] = 0;
		StepView_Time_Step_AllItem[i] = 0;
		//
		for ( j = 0 ; j < MAX_X_TABLE ; j++ ) {
			StepView_Count_Step[i][j] = 0;
			StepView_Time_Step[i][j] = 0;
		}
		//
	}
	//
}

void STEP_CHECK_FILE_START() {
	int i , j;
	//
	//
	//-----
	//
	for ( i = 0 ; i < MAX_X_TABLE ; i++ ) {
		Step_Log_Item_Index[i] = -1;
		STR_MEM_MAKE_COPY2( &(Step_Log_Item_Name[i]) , NULL );
	}
	Step_Log_Item_Count = 0;
	Step_Log_Step_Count = 0;
	//
	//-----
	//
	StepView_TimeView_Option = 0;
	StepView_DecimalPoint_Option = 0;
	//
	StepView_LV_GarbageCount = 0;
	//
	StepView_Item_Count = 0;
	StepView_Step_Count = 0;
	//
	for ( i = 0 ; i < MAX_X_TABLE ; i++ ) {
		//
		StepView_Ratio[i] = 0;
		StepView_CTime[i] = 0;
		//
		for ( j = 0 ; j < MAX_STEP_COUNT ; j++ ) {
			//
			StepView_HasTarget[j][i] = FALSE;
			StepView_TargetData[j][i] = 0;
			//
			STR_MEM_MAKE_COPY2( &(StepView_TimeStable[j][i]) , NULL );
			STR_MEM_MAKE_COPY2( &(StepView_TimeStep[j][i]) , NULL );
			//
			STR_MEM_MAKE_COPY2( &(StepView_UserData[j][i]) , NULL );
			//
		}
	}
	//
	STEP_CHECK_CALDATA_RESET( FALSE , NULL );
	//
}

int _STEP_CHECK_APPEND_ITEM( char *Name , BOOL append ) {
	int i;
	//
	for ( i = 0 ; i < StepView_Item_Count ; i++ ) {
		if ( STRCMP_L( STR_MEM_GET_STR( StepView_Item_Name[ i ] ) , Name ) ) break;
	}
	//
	if ( i == StepView_Item_Count ) {
		//
		if ( !append ) return -1;
		//
		if ( i >= MAX_X_TABLE ) return -1;
		//
		if ( !STR_MEM_MAKE_COPY2( &(StepView_Item_Name[ i ]) , Name ) ) return -2;
		//
		StepView_Item_Count++;
		//
	}
	//
	return i;
	//
}
//
BOOL STEP_CHECK_FILE_APPEND_TARGET( char *Name , char *dataString ) {
	int ItemIndex , z , z2 , s;
	char Buffer[256];
	//
	ItemIndex = _STEP_CHECK_APPEND_ITEM( Name , TRUE );
	//
	if ( ItemIndex < 0 ) return FALSE;
	//
	z = 0;
	s = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( dataString , 9 , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		StepView_HasTarget[s][ItemIndex] = TRUE;
		StepView_TargetData[s][ItemIndex] = atof( Buffer );
		//=========================================================================
		//
		s++;
		//
		if ( s >= MAX_STEP_COUNT ) break;
		//
		z = z2;
		//
	}
	//
	if ( s > StepView_Step_Count ) StepView_Step_Count = s;
	//
	return TRUE;
	//
}

BOOL STEP_CHECK_FILE_APPEND_TIMESTABLE( char *Name , char *dataString ) {
	int ItemIndex , z , z2 , s;
	char Buffer[256];
	//
	ItemIndex = _STEP_CHECK_APPEND_ITEM( Name , TRUE );
	//
	if ( ItemIndex < 0 ) return FALSE;
	//
	z = 0;
	s = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( dataString , 9 , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		Extract_Reset_String( Buffer );
		//
		STR_MEM_MAKE_COPY2( &(StepView_TimeStable[s][ItemIndex]) , Buffer );
		//=========================================================================
		//
		s++;
		//
		if ( s >= MAX_STEP_COUNT ) break;
		//
		z = z2;
		//
	}
	//
	return TRUE;
	//
}

BOOL STEP_CHECK_FILE_APPEND_TIMESTEP( char *Name , char *dataString ) {
	int ItemIndex , z , z2 , s;
	char Buffer[256];
	//
	ItemIndex = _STEP_CHECK_APPEND_ITEM( Name , TRUE );
	//
	if ( ItemIndex < 0 ) return FALSE;
	//
	z = 0;
	s = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( dataString , 9 , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		Extract_Reset_String( Buffer );
		//
		STR_MEM_MAKE_COPY2( &(StepView_TimeStep[s][ItemIndex]) , Buffer );
		//=========================================================================
		//
		s++;
		//
		if ( s >= MAX_STEP_COUNT ) break;
		//
		z = z2;
		//
	}
	//
	return TRUE;
	//
}

//
BOOL STEP_CHECK_FILE_APPEND_STABLE( char *Name , char *dataString ) {
	int i , ItemIndex , z , z2 , s;
	char Buffer[256];
	//
	if ( STRCMP_L( Name , "*" ) ) {
		ItemIndex = -1;
	}
	else {
		//
		ItemIndex = _STEP_CHECK_APPEND_ITEM( Name , TRUE );
		//
		if ( ItemIndex < 0 ) return FALSE;
		//
	}
	//
	z = 0;
	s = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( dataString , 9 , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		Extract_Reset_String( Buffer );
		//
		if      ( s == 0 ) {
			if ( ItemIndex < 0 ) {
				for ( i = 0 ; i < MAX_X_TABLE ; i++ ) {
					StepView_Ratio[i] = atof( Buffer );
					StepView_CTime[i] = 1;
				}
			}
			else {
				StepView_Ratio[ItemIndex] = atof( Buffer );
				StepView_CTime[ItemIndex] = 1;
			}
		}
		else if ( s == 1 ) {
			if ( ItemIndex < 0 ) {
				for ( i = 0 ; i < MAX_X_TABLE ; i++ ) {
					StepView_CTime[i] = atof( Buffer );
				}
			}
			else {
				StepView_CTime[ItemIndex] = atof( Buffer );
			}
		}
		//=========================================================================
		//
		s++;
		//
		if ( s >= 2 ) break;
		//
		z = z2;
		//
	}
	//
	return TRUE;
	//
}
//
BOOL STEP_CHECK_FILE_APPEND_STEP( char *Name , char *dataString ) {
	int mode , z , z2 , s;
	char Buffer[256];
	//
	if ( STRCMP_L( Name , "SETTIME" ) ) {
		mode = 0;
	}
	else if ( STRCMP_L( Name , "RESULT" ) ) {
		mode = 1;
	}
	else {
		return FALSE;
	}
	//
	z = 0;
	s = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( dataString , 9 , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		//
		Extract_Reset_String( Buffer );
		//
		if ( mode == 0 ) {
			STR_MEM_MAKE_COPY2( &(StepView_SetTime[s]) , Buffer );
		}
		else {
			STR_MEM_MAKE_COPY2( &(StepView_Result[s]) , Buffer );
		}
		//
		//=========================================================================
		//
		s++;
		//
		if ( s >= MAX_STEP_COUNT ) break;
		//
		z = z2;
		//
	}
	//
	return TRUE;
	//
}


BOOL STEP_CHECK_FILE_APPEND_OPTION( char *dataString ) {
	int z , z2 , s;
	char Buffer[256];
	//
	z = 0;
	s = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( dataString , 9 , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		Extract_Reset_String( Buffer );
		//
		if      ( s == 0 ) {
			STR_MEM_MAKE_COPY2( &(StepView_User_Title) , Buffer );
		}
		else if ( s == 1 ) {
			StepView_TimeView_Option = atoi( Buffer );
		}
		else if ( s == 2 ) {
			StepView_DecimalPoint_Option = atoi( Buffer );
		}
		//=========================================================================
		//
		s++;
		//
		if ( s >= 3 ) break;
		//
		z = z2;
		//
	}
	//
	return TRUE;
	//
}


BOOL STEP_CHECK_FILE_APPEND_USER( char *Name , char *dataString ) {
	int ItemIndex , z , z2 , s;
	char Buffer[256];
	//
	ItemIndex = _STEP_CHECK_APPEND_ITEM( Name , TRUE );
	//
	if ( ItemIndex < 0 ) return FALSE;
	//
	z = 0;
	s = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( dataString , 9 , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		Extract_Reset_String( Buffer );
		//
		STR_MEM_MAKE_COPY2( &(StepView_UserData[s][ItemIndex]) , Buffer );
		//=========================================================================
		//
		s++;
		//
		if ( s >= MAX_STEP_COUNT ) break;
		//
		z = z2;
		//
	}
	//
	if ( s > StepView_Step_Count ) StepView_Step_Count = s;
	//
	return TRUE;
	//
}


BOOL STEP_CHECK_LOGDATA_MAKE_NAMEINDEX( char *NameString ) {
	int ItemIndex , z , z2 , s;
	char Buffer[256];
	//
	z = 0;
	s = 0;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( NameString , 9 , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		Extract_Reset_String( Buffer );
		//
		ItemIndex = _STEP_CHECK_APPEND_ITEM( Buffer , FALSE );
		//
		STR_MEM_MAKE_COPY2( &(Step_Log_Item_Name[s]) , Buffer );
		Step_Log_Item_Index[s] = ItemIndex;
		Step_Log_Item_Count = s + 1;
		//
		//=========================================================================
		//
		s++;
		//
		if ( s >= MAX_X_TABLE ) break;
		//
		z = z2;
		//
	}
	//
	return TRUE;
	//
}
//


BOOL STEP_CHECK_LOGDATA_STEP_DATA( int StepIndex , char *TimeString , double timgap , char *DataString , int *stepinx , int LVPos ) {
	int i;
	double d;
	int ItemIndex , z , z2 , s , lp;
	char Buffer[256];
	BOOL StepX;
	BOOL CurOK; // 2017.01.25
	//
	if ( StepIndex < 0 ) return FALSE;
	if ( StepIndex >= MAX_STEP_COUNT ) return FALSE;
	//
	if ( timgap >= 1000 ) {
		printf( "ERROR STEP_CHECK_LOGDATA_STEP_DATA\n" );
		return FALSE;
	}
	//
	if ( Step_Log_Step_Count < ( StepIndex + 1 ) ) Step_Log_Step_Count = StepIndex + 1;
	//
	if ( STR_MEM_SIZE( StepView_Run_StartTime ) == 0 ) STR_MEM_MAKE_COPY2( &(StepView_Run_StartTime) , TimeString );
	if ( STR_MEM_SIZE( StepView_Step_StartTime ) == 0 ) STR_MEM_MAKE_COPY2( &(StepView_Step_StartTime) , TimeString );
	//
	if ( StepView_Count[StepIndex] == 0 ) {
		//
		STR_MEM_MAKE_COPY2( &(StepView_StartTime[StepIndex]) , TimeString );
		//
		for ( i = 0 ; i < StepView_Item_Count ; i++ ) {
			//
			if ( ( !StepView_HasTarget[StepIndex][i] ) || ( StepView_Ratio[i] <= 0 ) ) {
				//
				iStepView_MarginOK[i] = -1;
				//
			}
			else {
				//
				d = ( StepView_TargetData[StepIndex][i] * StepView_Ratio[i] ) / 100;
				//
				iStepView_MarginMin[i] = StepView_TargetData[StepIndex][i] - d;
				iStepView_MarginMax[i] = StepView_TargetData[StepIndex][i] + d;
				//
				iStepView_MarginTime[i] = 0;
				//
				iStepView_MarginOK[i] = -2;
			}
		}
	}
	//
	STR_MEM_MAKE_COPY2( &(StepView_EndTime[StepIndex]) , TimeString );
	STR_MEM_MAKE_COPY2( &(StepView_Step_EndTime) , TimeString );
	//
	for ( i = 0 ; i < MAX_X_TABLE ; i++ ) stepinx[i] = FALSE;
	//
	z = 0;
	s = 0;
	//
	StepX = FALSE;
	//
	while ( TRUE ) {
		//
		z2 = STR_SEPERATE_CHAR_WITH_POINTER( DataString , 9 , Buffer , z , 255 );
		//
		if ( z == z2 ) break;
		//
		ItemIndex = Step_Log_Item_Index[s];
		//
		if ( ItemIndex >= 0 ) {
			//
			CurOK = FALSE; // 2017.01.25
			//
			lp = 0; // READY
			//
			d = atof( Buffer );
			//
			if ( iStepView_MarginOK[ItemIndex] == -2 ) {
				//
				if ( ( d >= iStepView_MarginMin[ItemIndex] ) && ( d <= iStepView_MarginMax[ItemIndex] ) ) {
					//
					iStepView_MarginTime[ItemIndex] = iStepView_MarginTime[ItemIndex] + timgap;
					//
					if ( iStepView_MarginTime[ItemIndex] >= StepView_CTime[ItemIndex] ) {
						//
						lp = 2; // STEPTIME
						//
						iStepView_MarginOK[ItemIndex] = StepView_Count[StepIndex];
						//
						CurOK = TRUE; // 2017.01.25
						//
					}
					else {
						//
						lp = 1; // STABLE
						//
					}
					//
				}
				else {
					//
					iStepView_MarginTime[ItemIndex] = iStepView_MarginTime[ItemIndex] - timgap;
					//
					if ( iStepView_MarginTime[ItemIndex] < 0 ) iStepView_MarginTime[ItemIndex] = 0;
					//
					lp = 1; // STABLE
					//
				}
			}
			else if ( iStepView_MarginOK[ItemIndex] != -1 ) {
				//
				if ( ( d >= iStepView_MarginMin[ItemIndex] ) && ( d <= iStepView_MarginMax[ItemIndex] ) ) {
					lp = 2; // STEPTIME
				}
				else {
					lp = 3; // OVERDATA
				}
				//
			}
			//
			if ( LVPos < MAX_LV_PRCS_ROW_COUNT ) StepView_LV_Map[ItemIndex][LVPos] = lp;
			//
			if ( iStepView_MarginOK[ItemIndex] == -2 ) {
				//
				if ( STEPVIEW_DATA.SV_Opt_Item_Cal_ExceptStable == 0 ) stepinx[s] = TRUE; // 2016.10.11
				//
				StepX = TRUE;
				//
			}
			else {
				//
				if ( !CurOK ) { // 2017.01.25
					//
					StepView_Count_Step[StepIndex][ItemIndex]++;
					StepView_Time_Step[StepIndex][ItemIndex] = StepView_Time_Step[StepIndex][ItemIndex] + timgap;
					//
				}
				else { // 2017.01.31
					StepX = TRUE;
				}
				//
			}
			//
		}
		//
		//=========================================================================
		//
		s++;
		//
		if ( s >= MAX_X_TABLE ) break;
		//
		z = z2;
		//
	}
	//
	if ( !StepX ) {
		StepView_Count_Step_AllItem[StepIndex]++;
		StepView_Time_Step_AllItem[StepIndex] = StepView_Time_Step_AllItem[StepIndex] + timgap;
	}
	//
	StepView_Count[StepIndex]++;
	StepView_Time[StepIndex] = StepView_Time[StepIndex] + timgap;
	//
	return TRUE;
	//
}


void STEP_CHECK_LOGDATA_INC_LV_GARBAGE() {
	StepView_LV_GarbageCount++;
}


int STEP_CHECK_LOGDATA_GET_LV_STEP( int ItemIndex , int LVPos ) {
	int x;
	//
	x = LVPos - StepView_LV_GarbageCount;
	//
	if ( x < 0 ) return -1;
	if ( x >= MAX_LV_PRCS_ROW_COUNT ) return -1;
	//
	if ( StepView_LV_Map[ItemIndex][x] == 0 ) {
		//
		if ( StepView_LV_From_Post >= 0 ) {
			if ( x >= StepView_LV_From_Post ) return 0;
		}
		//
		return -1;
	}
	//
	if ( StepView_LV_Map[ItemIndex][x] == 1 ) return 0;
	if ( StepView_LV_Map[ItemIndex][x] == 2 ) return 1;
	if ( StepView_LV_Map[ItemIndex][x] == 3 ) return 2;
	//
	return -1;
}


void STEP_CHECK_LOGDATA_FIND_POST( BOOL first , char *TimeString , int lpos ) {
	//
	if ( first ) {
		STR_MEM_MAKE_COPY2( &(StepView_Post_StartTime) , TimeString );
		StepView_LV_From_Post = lpos;
	}
	//
	STR_MEM_MAKE_COPY2( &(StepView_Post_EndTime) , TimeString );
	//
	if ( STR_MEM_SIZE( StepView_Run_StartTime ) == 0 ) STR_MEM_MAKE_COPY2( &(StepView_Run_StartTime) , TimeString );
	//
}

void STEP_CHECK_LOGDATA_END( char *TimeString ) {
	//
	if ( STR_MEM_SIZE( StepView_Run_StartTime ) == 0 ) STR_MEM_MAKE_COPY2( &(StepView_Run_StartTime) , TimeString );
	STR_MEM_MAKE_COPY2( &(StepView_Run_EndTime) , TimeString );
	//
}



































//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

StepViewTemplate			STEPVIEW_EDIT; // 2016.10.11
StepViewTemplate			STEPVIEW_EDIT_INITDATA; // 2016.10.11
BOOL						STEPVIEW_EDIT_REINIT; // 2016.10.11

BOOL STEPVIEW_EDIT_Save( char *filename ) { // 2016.10.12
	HFILE hFile;
	//
	hFile = _lcreat( filename , 0 );
	if ( hFile == -1 ) return FALSE;
	_lwrite( hFile , "JOBSTEPVIEW1.0" , 14 );
	_lwrite( hFile , (char * ) &STEPVIEW_EDIT , sizeof( StepViewTemplate ) );
	_lclose( hFile );
	//
	return TRUE;
}
	
	
BOOL STEPVIEW_EDIT_Open( char *filename ) { // 2016.10.12
	char Buffer[20];
	HFILE hFile;
	//
	hFile = _lopen( filename , OF_READ );
	//
	if ( hFile == -1 ) return FALSE;
	//
	_lread( hFile , Buffer , 14 ); Buffer[14] = 0;
	//
	if ( strcmp( "JOBSTEPVIEW1.0" , Buffer ) == 0 ) {
		//
		_lread( hFile , &STEPVIEW_EDIT , sizeof( StepViewTemplate ) );
		_lclose( hFile );
		//
	}
	else {
		_lclose( hFile );
		return FALSE;
	}
	//
	return TRUE;
}

BOOL STEPVIEW_EDIT_INSERT1( HWND hWnd , int i , int prm , char *Data1 ) {
	//
	LV_ITEM lvi;
	//
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= i;
	lvi.iSubItem	= 0;
	//
	lvi.pszText		= Data1;
	//
	lvi.cchTextMax	= 256;
	lvi.iImage		= i;
	lvi.lParam		= prm;
	//
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	//
	return TRUE;
}

BOOL STEPVIEW_EDIT_INSERT2( HWND hWnd , int i , int prm , char *Data1 , char *Data2 ) {
	//
	LV_ITEM lvi;
	//
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= i;
	lvi.iSubItem	= 0;
	//
	lvi.pszText		= Data1;
	//
	lvi.cchTextMax	= 256;
	lvi.iImage		= i;
	lvi.lParam		= prm;
	//
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	//
	ListView_SetItemText( hWnd , i , 1 , Data2 );
	//
	return TRUE;
}

BOOL STEPVIEW_EDIT_INSERT( HWND hWnd , int i , int prm , char *Data1 , int Data2 , int Data3 , int Data4 , int Data5 ) {
	//
	char Buf2[64];
	char Buf3[64];
	char Buf4[64];
	char Buf5[64];
	//
	LV_ITEM lvi;
	//
	if ( Data2 < 0 ) strcpy( Buf2 , "" ); else sprintf( Buf2 , "%d" , Data2 );
	if ( Data3 < 0 ) strcpy( Buf3 , "" ); else sprintf( Buf3 , "%d" , Data3 );
	if ( Data4 < 0 ) strcpy( Buf4 , "" ); else sprintf( Buf4 , "%d" , Data4 );
	if ( Data5 < 0 ) strcpy( Buf5 , "" ); else sprintf( Buf5 , "%d" , Data5 );
	//
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= i;
	lvi.iSubItem	= 0;
	//
	lvi.pszText		= Data1;
	//
	lvi.cchTextMax	= 256;
	lvi.iImage		= i;
	lvi.lParam		= prm;
	//
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	//
	ListView_SetItemText( hWnd , i , 1 , Buf2 );
	ListView_SetItemText( hWnd , i , 2 , Buf3 );
	ListView_SetItemText( hWnd , i , 3 , Buf4 );
	ListView_SetItemText( hWnd , i , 4 , Buf5 );
	//
	return TRUE;
}

BOOL STEPVIEW_EDIT_INSERTC( HWND hWnd , int i , int prm ) {
	//
	LV_ITEM lvi;
	//
	lvi.mask  = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.state = lvi.stateMask = 0;
	lvi.iItem		= i;
	lvi.iSubItem	= 0;
	//
	lvi.pszText		= LPSTR_TEXTCALLBACK;
	//
	lvi.cchTextMax	= 256;
	lvi.iImage		= i;
	lvi.lParam		= prm;
	//
	if ( ListView_InsertItem( hWnd , &lvi ) == -1 ) return FALSE;
	//
	return TRUE;
}

int STEPVIEW_EDIT2_POS;
int STEPVIEW_EDIT2_PARAM;
int STEPVIEW_EDIT2_COLOR;
int STEPVIEW_EDIT2_COLUMN;
int STEPVIEW_EDIT2_ALIGN;
int STEPVIEW_EDIT2_VIEW;
int STEPVIEW_EDIT2_OPTION;


BOOL STEPVIEW_EDIT_INIT2( HWND hwndListView , int mode ) {
	LV_COLUMN   lvColumn;
	//
	ListView_DeleteAllItems( hwndListView );
	while ( TRUE ) if ( !ListView_DeleteColumn( hwndListView , 0 ) ) break;
	ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	//
	if ( mode == 0 ) {
		lvColumn.cx = 40;				lvColumn.pszText = "";			ListView_InsertColumn( hwndListView , 0 , &lvColumn );
		lvColumn.cx = 100;				lvColumn.pszText = "";			ListView_InsertColumn( hwndListView , 1 , &lvColumn );
	}
	else {
		lvColumn.cx = 140;				lvColumn.pszText = "";			ListView_InsertColumn( hwndListView , 0 , &lvColumn );
	}
	//
	return TRUE;
}

BOOL STEPVIEW_EDIT_SET2( HWND hwndListView , int mode , int prm , int data ) {
	//
	char Buffer[64];
	int i , c , fc;
	//
	if ( mode == 0 ) { // color
		//
		fc = -1;
		c = 0;
		//
		for ( i = COLOR_Black ; i <= COLOR_BackGray ; i++ ) {
			//--------------------------------------------------------------------------------------
			if ( i == data ) fc = c;
			//--------------------------------------------------------------------------------------
			KGUI_FT_Set_Color_Data( i , Buffer );
			//
			STEPVIEW_EDIT_INSERT2( hwndListView , c , i , "" , Buffer );
			//
			//--------------------------------------------------------------------------------------
			c++;
			//--------------------------------------------------------------------------------------
		}
		//
		for ( i = 0 ; i < KGUI_PALETTE_Get_SIZE() ; i++ ) {
			//--------------------------------------------------------------------------------------
			if ( i == data ) fc = c;
			//--------------------------------------------------------------------------------------
			KGUI_FT_Set_Color_Data( i , Buffer );
			//
			STEPVIEW_EDIT_INSERT2( hwndListView , c , i , "" , Buffer );
			//
			//--------------------------------------------------------------------------------------
			c++;
			//--------------------------------------------------------------------------------------
		}
		//
		for ( i = 0 ; i < KGUI_PALETTE_Get_SIZE_F500() ; i++ ) { // 2014.11.19
			//--------------------------------------------------------------------------------------
			if ( (i+500) == data ) fc = c;
			//--------------------------------------------------------------------------------------
			KGUI_FT_Set_Color_Data( i+500 , Buffer );
			//
			STEPVIEW_EDIT_INSERT2( hwndListView , c , i+500 , "" , Buffer );
			//
			//--------------------------------------------------------------------------------------
			c++;
			//--------------------------------------------------------------------------------------
		}
		//
		if ( fc != -1 ) ListView_SetItemState( hwndListView , fc , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
		//
	}
	else if ( mode == 1 ) { // column
		//
		fc = -1;
		//
		if ( prm == 7 ) { // Row Size
			//
			for ( i = 0 ; i < 20 ; i++ ) {
				//
				sprintf( Buffer , "%d" , i + 1 );
				//
				STEPVIEW_EDIT_INSERT1( hwndListView , i , i , Buffer );
				//
				if ( i == data ) fc = i;
				//
			}
		}
		else {
			for ( i = 0 ; i < 200 ; i++ ) {
				//
				sprintf( Buffer , "%d" , i + 5 );
				//
				STEPVIEW_EDIT_INSERT1( hwndListView , i , i + 5 , Buffer );
				//
				if ( ( i + 5 ) == data ) fc = i;
				//
			}
		}
		//
		if ( fc != -1 ) ListView_SetItemState( hwndListView , fc , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
		//
	}
	else if ( mode == 2 ) { // Align
		//
		STEPVIEW_EDIT_INSERT1( hwndListView , 0 , 0 , "Center" );
		STEPVIEW_EDIT_INSERT1( hwndListView , 1 , 1 , "Left" );
		STEPVIEW_EDIT_INSERT1( hwndListView , 2 , 2 , "Right" );
		//
		ListView_SetItemState( hwndListView , data , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
		//
	}
	else if ( mode == 3 ) { // view
		//
		STEPVIEW_EDIT_INSERT1( hwndListView , 0 , 0 , "ON" );
		STEPVIEW_EDIT_INSERT1( hwndListView , 1 , 1 , "OFF" );
		//
		ListView_SetItemState( hwndListView , data , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
		//
	}
	else if ( mode == 4 ) { // option
		if      ( prm == 51 ) { // char Size
			//
			STEPVIEW_EDIT_INSERT1( hwndListView , 0 , 0 , "14" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 1 , 1 , "16" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 2 , 2 , "18" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 3 , 3 , "20" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 4 , 4 , "22" );
			//
			ListView_SetItemState( hwndListView , data , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
			//
		}
		else if ( prm == 52 ) { // BackMode
			//
			STEPVIEW_EDIT_INSERT1( hwndListView , 0 , 0 , "Column" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 1 , 1 , "Row" );
			//
			ListView_SetItemState( hwndListView , data , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
			//
		}
		else if ( prm == 53 ) { // Cal_ExceptStable
			//
			STEPVIEW_EDIT_INSERT1( hwndListView , 0 , 0 , "No" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 1 , 1 , "Yes" );
			//
			ListView_SetItemState( hwndListView , data , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
			//
		}
		else if ( prm == 54 ) { // Time_Calculation
			//
			STEPVIEW_EDIT_INSERT1( hwndListView , 0 , 0 , "LOG/Cal1" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 1 , 1 , "LOG/Cal2" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 2 , 2 , "LOG" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 3 , 3 , "Cal1" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 4 , 4 , "Cal2" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 5 , 5 , "(x)" );
			//
			ListView_SetItemState( hwndListView , data , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
			//
		}
		else if ( prm == 55 ) { // Time_Display(Run) Def h:m:s.ms h:m:s
			//
			STEPVIEW_EDIT_INSERT1( hwndListView , 0 , 0 , "Default" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 1 , 1 , "HH:MM:SS.x" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 2 , 2 , "HH:MM:SS" );
			//
			ListView_SetItemState( hwndListView , data , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
			//
		}
		else if ( prm == 56 ) { // Time_Display_Cal Def xx:xx:xx xx:xx:xx.x xx:xx:xx.xx xx:xx:xx.xxx xx xx.x xx.xx xx.xxx
			//
			STEPVIEW_EDIT_INSERT1( hwndListView , 0 , 0 , "Default" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 1 , 1 , "HH:MM:SS" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 2 , 2 , "HH:MM:SS.x" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 3 , 3 , "HH:MM:SS.xx" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 4 , 4 , "HH:MM:SS.xxx" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 5 , 5 , "SEC" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 6 , 6 , "SEC.x" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 7 , 7 , "SEC.xx" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 8 , 8 , "SEC.xxx" );
			//
			ListView_SetItemState( hwndListView , data , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
			//
		}
		else if ( prm == 57 ) { // Time_Display_Unknown Def 0(Zero)
			//
			STEPVIEW_EDIT_INSERT1( hwndListView , 0 , 0 , "Default" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 1 , 1 , "0(Zero)" );
			//
			ListView_SetItemState( hwndListView , data , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
			//
		}
		else if ( prm == 58 ) { // DecimalPoint Def x.x x.xx x.xxx x.xxxx x
			//
			STEPVIEW_EDIT_INSERT1( hwndListView , 0 , 0 , "Default" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 1 , 1 , "x.x" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 2 , 2 , "x.xx" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 3 , 3 , "x.xxx" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 4 , 4 , "x.xxxx" );
			STEPVIEW_EDIT_INSERT1( hwndListView , 5 , 5 , "x" );
			//
			ListView_SetItemState( hwndListView , data , LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED );
			//
		}
	}
	//
	return TRUE;
	//
}


LRESULT STEPVIEW_EDIT_Notify2( HWND hWnd , int mode , LPARAM lParam , BOOL *chg ) {
	int ItemData , prm;
	LPNMHDR  lpnmh = (LPNMHDR) lParam;

	*chg = FALSE;

	switch( lpnmh->code ) {
		case LVN_GETDISPINFO: 
				return 0;

		case NM_DBLCLK :
			return 0;

		case LVN_COLUMNCLICK:
			return 0;

		case LVN_ITEMCHANGED :
			//
			ItemData = ListView_GetNextItem( hWnd , -1 , LVNI_SELECTED );
			//
			if ( ItemData < 0 ) return 0;
			//
			prm = LogView_GetParam( hWnd , ItemData );
			//
			if      ( mode == 0 ) {
				STEPVIEW_EDIT2_COLOR = prm;
				*chg = TRUE;
			}
			else if ( mode == 1 ) {
				STEPVIEW_EDIT2_COLUMN = prm;
				*chg = TRUE;
			}
			else if ( mode == 2 ) {
				STEPVIEW_EDIT2_ALIGN = prm;
				*chg = TRUE;
			}
			else if ( mode == 3 ) {
				STEPVIEW_EDIT2_VIEW = prm;
				*chg = TRUE;
			}
			else if ( mode == 4 ) {
				STEPVIEW_EDIT2_OPTION = prm;
				*chg = TRUE;
			}
			return 0;

		case LVN_KEYDOWN :
			return 0;

		case LVN_ODCACHEHINT: {
			LPNMLVCACHEHINT   lpCacheHint = (LPNMLVCACHEHINT) lParam;
		}
		return 0;

		case LVN_ODFINDITEM: {
			LPNMLVFINDITEM lpFindItem = (LPNMLVFINDITEM)lParam;
		}
		return 0;
	}
	return 0;
}
//
void STEPVIEW_EDIT_WM_DRAWITEM2( HWND hwnd , int mode , LPARAM lParam , int prm , char *buffer , int buffercount ) {
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , b , a , cprm;
	int align = 1;
	int color_text , color_back , color_line;
	BOOL sel;
	LVCOLUMN lvcol;
	//
	if ( dis->itemState & ODS_SELECTED ) { // select
		//
		sel = TRUE;
		//
//		color_text = COLOR_White;		color_back = COLOR_DarkBlue;
		color_text = COLOR_White;		color_back = 1000+RGB(51,153,255);
		color_line = COLOR_Blank;
	}
	else {
		//
		sel = FALSE;
		//
		color_text = COLOR_Black;				color_back = COLOR_White;
		color_line = COLOR_Blank;
		//
	}
	//
	cprm = LogView_GetParam( hwnd , dis->itemID );
	//
	i = 0;
	z = 0;
	while( TRUE ) {
		//
		lvcol.mask = LVCF_WIDTH;
		lvcol.cx = 0;
		if ( !ListView_GetColumn( hwnd , z , &lvcol ) ) break;
		j = lvcol.cx;
		//
		ListView_GetItemText( hwnd , dis->itemID , z , buffer , buffercount );
		//
		a = align;
		b = color_back;
		c = color_text;
		//
		if ( mode == 0 ) {
			if ( z == 0 ) {
				if ( cprm != COLOR_Blank ) {
					b = cprm;
					c = cprm;
				}
			}
		}
		//
		KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , "" );
		//
		i = i + j;
		z++;
	}
}
//



BOOL APIENTRY Console_Gui_For_StepView2_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) { // 2016.10.11
	LPDRAWITEMSTRUCT dis;
	LPNMHDR  lpnmh;
	char Temp_Buffer[512];
	BOOL schg;

	switch ( msg ) {

		case WM_INITDIALOG:
			//
			//------------------------------------
			//
			if ( STEPVIEW_EDIT2_COLOR < 0 ) {
				KWIN_Item_Disable( hdlg , IDC_LIST_LOGVIEW );
			}
			else {
				STEPVIEW_EDIT_INIT2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 );
				STEPVIEW_EDIT_SET2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , STEPVIEW_EDIT2_PARAM , STEPVIEW_EDIT2_COLOR );
			}
			//
			//------------------------------------
			//
			if ( STEPVIEW_EDIT2_COLUMN < 0 ) {
				KWIN_Item_Disable( hdlg , IDC_LIST_LOGVIEW2 );
			}
			else {
				STEPVIEW_EDIT_INIT2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW2 ) , 1 );
				STEPVIEW_EDIT_SET2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW2 ) , 1 , STEPVIEW_EDIT2_PARAM , STEPVIEW_EDIT2_COLUMN );
			}
			//
			//------------------------------------
			//
			if ( STEPVIEW_EDIT2_ALIGN < 0 ) {
				KWIN_Item_Disable( hdlg , IDC_LIST_LOGVIEW3 );
			}
			else {
				STEPVIEW_EDIT_INIT2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW3 ) , 2 );
				STEPVIEW_EDIT_SET2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW3 ) , 2 , STEPVIEW_EDIT2_PARAM , STEPVIEW_EDIT2_ALIGN );
			}
			//
			//------------------------------------
			//
			if ( STEPVIEW_EDIT2_VIEW < 0 ) {
				KWIN_Item_Disable( hdlg , IDC_LIST_LOGVIEW4 );
			}
			else {
				STEPVIEW_EDIT_INIT2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW4 ) , 3 );
				STEPVIEW_EDIT_SET2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW4 ) , 3 , STEPVIEW_EDIT2_PARAM , STEPVIEW_EDIT2_VIEW );
			}
			//
			//------------------------------------
			//
			if ( STEPVIEW_EDIT2_OPTION < 0 ) {
				KWIN_Item_Disable( hdlg , IDC_LIST_LOGVIEW5 );
			}
			else {
				STEPVIEW_EDIT_INIT2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW5 ) , 4 );
				STEPVIEW_EDIT_SET2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW5 ) , 4 , STEPVIEW_EDIT2_PARAM , STEPVIEW_EDIT2_OPTION );
			}
			//
			//------------------------------------
			//
			KWIN_Item_Disable( hdlg , IDOK );
			//
			return TRUE;
			
		case WM_NOTIFY:
			//
			lpnmh = (LPNMHDR) lParam;
			//
			if      ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) ) {
				//
				STEPVIEW_EDIT_Notify2( lpnmh->hwndFrom , 0 , lParam , &schg );
				//
				if ( schg ) KWIN_Item_Enable( hdlg , IDOK );
				//
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_LOGVIEW2 ) ) {
				//
				STEPVIEW_EDIT_Notify2( lpnmh->hwndFrom , 1 , lParam , &schg );
				//
				if ( schg ) KWIN_Item_Enable( hdlg , IDOK );
				//
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_LOGVIEW3 ) ) {
				//
				STEPVIEW_EDIT_Notify2( lpnmh->hwndFrom , 2 , lParam , &schg );
				//
				if ( schg ) KWIN_Item_Enable( hdlg , IDOK );
				//
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_LOGVIEW4 ) ) {
				//
				STEPVIEW_EDIT_Notify2( lpnmh->hwndFrom , 3 , lParam , &schg );
				//
				if ( schg ) KWIN_Item_Enable( hdlg , IDOK );
				//
			}
			else if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_LOGVIEW5 ) ) {
				//
				STEPVIEW_EDIT_Notify2( lpnmh->hwndFrom , 4 , lParam , &schg );
				//
				if ( schg ) KWIN_Item_Enable( hdlg , IDOK );
				//
			}
			return TRUE;


		case WM_DRAWITEM:
			//
			dis = (LPDRAWITEMSTRUCT) lParam;
			//
			if      ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) ) {
				STEPVIEW_EDIT_WM_DRAWITEM2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , 0 , lParam , STEPVIEW_EDIT2_PARAM , Temp_Buffer , 511 );
			}
			else if ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_LOGVIEW2 ) ) {
				STEPVIEW_EDIT_WM_DRAWITEM2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW2 ) , 1 , lParam , STEPVIEW_EDIT2_PARAM , Temp_Buffer , 511 );
			}
			else if ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_LOGVIEW3 ) ) {
				STEPVIEW_EDIT_WM_DRAWITEM2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW3 ) , 2 , lParam , STEPVIEW_EDIT2_PARAM , Temp_Buffer , 511 );
			}
			else if ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_LOGVIEW4 ) ) {
				STEPVIEW_EDIT_WM_DRAWITEM2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW4 ) , 3 , lParam , STEPVIEW_EDIT2_PARAM , Temp_Buffer , 511 );
			}
			else if ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_LOGVIEW5 ) ) {
				STEPVIEW_EDIT_WM_DRAWITEM2( GetDlgItem( hdlg , IDC_LIST_LOGVIEW5 ) , 4 , lParam , STEPVIEW_EDIT2_PARAM , Temp_Buffer , 511 );
			}
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
				case IDOK :
					EndDialog( hdlg , 1 );
					return TRUE;

				case IDCANCEL :
					EndDialog( hdlg , 0 );
					return TRUE;
			}
			return TRUE;

	}
	return FALSE;
}



BOOL STEP_VIEW_EDITING2( HWND hdlg ) { // 2016.10.11
	//
	if ( GoModalDialogBoxParam( GETHINST( hdlg ) , MAKEINTRESOURCE( IDD_JOB_STEPVIEW2 ) , hdlg , Console_Gui_For_StepView2_Proc , (LPARAM) NULL ) ) {
		return TRUE;
	}
	//
	return FALSE;
	//
}


//===================================================================================================================================================================
//===================================================================================================================================================================
//===================================================================================================================================================================
//===================================================================================================================================================================
//===================================================================================================================================================================
//===================================================================================================================================================================
//===================================================================================================================================================================
//===================================================================================================================================================================
//===================================================================================================================================================================



BOOL STEPVIEW_EDIT_INIT( HWND hwndListView ) {
	LV_COLUMN   lvColumn;
	//
	//	Group Name Color ColumnSize View Option
	//
	ListView_DeleteAllItems( hwndListView );
	while ( TRUE ) if ( !ListView_DeleteColumn( hwndListView , 0 ) ) break;
	ListView_SetExtendedListViewStyleEx( hwndListView , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES , LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	//
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	//
	lvColumn.cx = 140;				lvColumn.pszText = "Item";			ListView_InsertColumn( hwndListView , 0 , &lvColumn );
	lvColumn.cx = 60;				lvColumn.pszText = "Color";			ListView_InsertColumn( hwndListView , 1 , &lvColumn );
	lvColumn.cx = 90;				lvColumn.pszText = "ColumnSize";	ListView_InsertColumn( hwndListView , 2 , &lvColumn );
	lvColumn.cx = 60;				lvColumn.pszText = "Align";			ListView_InsertColumn( hwndListView , 3 , &lvColumn );
	lvColumn.cx = 50;				lvColumn.pszText = "View";			ListView_InsertColumn( hwndListView , 4 , &lvColumn );
	lvColumn.cx = 100;				lvColumn.pszText = "Option";		ListView_InsertColumn( hwndListView , 5 , &lvColumn );
	//
	return TRUE;
}

BOOL STEPVIEW_EDIT_SET( HWND hwndListView ) {
	//
	int rc;
	//
	//	GROUP						0
	//
	//	COLOR						1
	//	COLOR	COLUMN				2
	//	COLOR				VIEW	3
	//			COLUMN				4
	//			COLUMN		VIEW	5
	//	COLOR	COLUMN		VIEW	6
	//	COLOR	COLUMN				7 (COLUMN->ROW(0 is 1)
	//
	//					AL			10
	//	COLOR			AL			11
	//	COLOR	COLUMN	AL			12
	//	COLOR			AL	VIEW	13
	//			COLUMN	AL			14
	//			COLUMN	AL	VIEW	15
	//	COLOR	COLUMN	AL	VIEW	16
	//	COLOR	COLUMN	AL			17 (COLUMN->ROW(0 is 1)

	//	BLANK						99

	rc = 0;
	//
	//================================
	//
	STEPVIEW_EDIT_INSERT1( hwndListView , rc ,  0 , "[SUMMARY]" );	rc++;
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 51 );	rc++;				//  1	1.Char Size
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  1 );	rc++;				//  2	2.BackGround
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  1 );	rc++;				//  3	   Line
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 12 );	rc++;				//  4	3.Job Name..
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 12 );	rc++;				//  5	   Time Total
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 12 );	rc++;				//  6	4.PM Info
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 12 );	rc++;				//  7	   Step Info
	STEPVIEW_EDIT_INSERT1( hwndListView , rc , 99 , ""  );	rc++;
	//
	//================================
	//
	STEPVIEW_EDIT_INSERT1( hwndListView , rc ,  0 , "[STEP]" );	rc++;
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 51 );	rc++;				//  10	1.Char Size
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  1 );	rc++;				//  11	2.BackGround
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  1 );	rc++;				//  12	   Line
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 11 );	rc++;				//  13	3.TimeTitle
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 11 );	rc++;				//  14	   TimeData
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  1 );	rc++;				//  15	4.StepTitle
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  1 );	rc++;				//  16	   StepData
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  3 );	rc++;				//  17	   StepPrePost
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 14 );	rc++;				//  18	5.Step
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 15 );	rc++;				//	19	   StartTime
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 15 );	rc++;				//	20     EndTime
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 15 );	rc++;				//	21     ActualTime
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 15 );	rc++;				//	22     StableTime
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 15 );	rc++;				//	23     StepTime
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 15 );	rc++;				//	24     SetTime
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 15 );	rc++;				//	25     Result
	STEPVIEW_EDIT_INSERT1( hwndListView , rc , 99 , "" );	rc++;
	//
	//================================
	//
	STEPVIEW_EDIT_INSERT1( hwndListView , rc ,  0 , "[ITEM]" );	rc++;
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  1 );	rc++;				//  28	1.BackGround
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  1 );	rc++;				//  29	   Line
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  1 );	rc++;				//  30	2.Title

	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 52 );	rc++;				//  31	3.BackMode
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 12 );	rc++;				//  32	4.No
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 12 );	rc++;				//  33	   Name

	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  34	   Count
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  35	   Target
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  36	   Start
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  37	   End
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  38	   Min
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  39	   Max
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  40	   Avr
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  41	   StdDev
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  42	   StableTime
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  43	   StepTime
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 16 );	rc++;				//  44	   Result

	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  7 );	rc++;				//  45	   Row(1)
	STEPVIEW_EDIT_INSERTC( hwndListView , rc ,  7 );	rc++;				//  46	   Row(2)

	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 53 );	rc++;				//  47	5.Cal_ExceptStable
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 54 );	rc++;				//  48	6.Time_Calculation
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 55 );	rc++;				//  49	7.Time_Display(Run)
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 56 );	rc++;				//  50	8.Time_Display(Cal)
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 57 );	rc++;				//  51	9.Time_Display(Unknown)
	STEPVIEW_EDIT_INSERTC( hwndListView , rc , 58 );	rc++;				//  52	10.Decimal_Point
	//
	return TRUE;
	//
}


LRESULT STEPVIEW_EDIT_Notify( HWND hWnd , LPARAM lParam , BOOL *chg ) {
	//
	int ItemData;
	TCHAR szString[ 256 ];
	//
	LPNMHDR  lpnmh = (LPNMHDR) lParam;
	LV_DISPINFO *lpdi = (LV_DISPINFO *)lParam;

	*chg = FALSE;

	switch( lpnmh->code ) {
		//
		case LVN_GETDISPINFO:
			//
			strcpy( szString , "" );
			//
			switch( lpdi->item.iItem ) {
			//
			//================================
			//
			case 1 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  1.Char Size" );	break;
				case 5 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_CharSize );	break;
				}
				//
				break;

			case 2 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  2.BackGround" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_BackColor );	break;
				}
				//
				break;

			case 3 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Line" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_LineColor );	break;
				}
				//
				break;

			case 4 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  3.Job Name.." );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_BackColor_Job );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_NameJob );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_Align_NameJob );	break;
				}
				//
				break;

			case 5 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Time Total" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_BackColor_TimeJob );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_DataJob );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_Align_DataJob );	break;
				}
				//
				break;

			case 6 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  4.PM Info" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_BackColor_Prcs );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_NamePrcs );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_Align_DataPrcs );	break;
				}
				//
				break;

			case 7 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Step Info" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_BackColor_TimePrcs );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_DataPrcs );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Summary_Align_DataPrcs );	break;
				}
				//
				break;
			//
			//================================
			//
			case 10 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  1.Char Size" );	break;
				case 5 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_StepItem_CharSize );	break;
				}
				//
				break;

			case 11 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  2.BackGround" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_BackColor );	break;
				}
				//
				break;

			case 12 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Line" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_LineColor );	break;
				}
				//
				break;

			case 13 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  3.TimeTitle" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_BackColor_TimeTitle );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_Align_TimeTitle );	break;
				}
				//
				break;

			case 14 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     TimeData" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_BackColor_TimeData );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_Align_TimeData );	break;
				}
				//
				break;

			case 15 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  4.StepTitle" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_BackColor_StepTitle );	break;
				}
				//
				break;

			case 16 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     StepData" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_BackColor_StepData );	break;
				}
				//
				break;

			case 17 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     StepPrePost" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_BackColor_StepPrePost );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_View_HidePrePost );	break;
				}
				//
				break;

			case 18 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  5.Step" );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_Step );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_Align_Step );	break;
				}
				//
				break;

			case 19 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     StartTime" );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_StartTime );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_Align_StartTime );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_View_HideStartTime );	break;
				}
				//
				break;

			case 20 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     EndTime" );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_EndTime );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_Align_EndTime );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_View_HideEndTime );	break;
				}
				//
				break;

			case 21 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     ActualTime" );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_ActualTime );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_Align_ActualTime );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_View_HideActualTime );	break;
				}
				//
				break;

			case 22 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     StableTime" );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_StableTime );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_Align_StableTime );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_View_HideStableTime );	break;
				}
				//
				break;

			case 23 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     StepTime" );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_StepTime );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_Align_StepTime );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_View_HideStepTime );	break;
				}
				//
				break;

			case 24 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     SetTime" );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_SetTime );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_Align_SetTime );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_View_HideSetTime );	break;
				}
				//
				break;

			case 25 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Result" );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_Result );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_Align_Result );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Step_View_HideResult );	break;
				}
				//
				break;
			//
			//================================
			//
			case 28 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  1.BackGround" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor );	break;
				}
				//
				break;

			case 29 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Line" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_LineColor );	break;
				}
				//
				break;

			case 30 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  2.Title" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Title );	break;
				}
				//
				break;

			case 31 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  3.BackMode" );	break;
				case 5 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColorMode );	break;
				}
				//
				break;

			case 32 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  4.No" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_No );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_No );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_No );	break;
				}
				//
				break;

			case 33 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Name" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Name );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Name );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_Name );	break;
				}
				//
				break;

			case 34 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Count" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Count );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Count );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_Count );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideCount );	break;
				}
				//
				break;

			case 35 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Target" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Target );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Target );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_Target );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideTarget );	break;
				}
				//
				break;

			case 36 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Start" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Start );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Start );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_Start );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideStart );	break;
				}
				//
				break;

			case 37 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     End" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_End );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_End );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_End );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideEnd );	break;
				}
				//
				break;

			case 38 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Min" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Min );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Min );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_Min );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideMin );	break;
				}
				//
				break;

			case 39 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Max" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Max );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Max );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_Max );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideMax );	break;
				}
				//
				break;

			case 40 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Avr" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Avr );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Avr );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_Avr );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideAvr );	break;
				}
				//
				break;

			case 41 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     StdDev" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_StdDev );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_StdDev );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_StdDev );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideStdDev );	break;
				}
				//
				break;

			case 42 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     StableTime" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_StableTime );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_StblTime );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_StblTime );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideStblTime );	break;
				}
				//
				break;

			case 43 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     StepTime" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_StepTime );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_StepTime );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_StepTime );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideStepTime );	break;
				}
				//
				break;

			case 44 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Result" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Result );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Result );	break;
				case 3 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Align_Result );	break;
				case 4 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_View_HideResult );	break;
				}
				//
				break;

			case 45 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Row(1)" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Row1 );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_RowSize_Row1 + 1 );	break;
				}
				//
				break;

			case 46 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "     Row(2)" );	break;
				case 1 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_BackColor_Row2 );	break;
				case 2 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_RowSize_Row2 + 1 );	break;
				}
				//
				break;

			case 47 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  5.Cal_ExceptStable" );	break;
				case 5 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Cal_ExceptStable );	break;
				}
				//
				break;

			case 48 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  6.Time_Calculation" );	break;
				case 5 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_Item_Time_Calculation );	break;
				}
				//
				break;

			case 49 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  7.Time_Display(Run)" );	break;
				case 5 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_TimeDisplay_Run );	break;
				}
				//
				break;

			case 50 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  8.Time_Display(Cal)" );	break;
				case 5 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_TimeDisplay_Cal );	break;
				}
				//
				break;

			case 51 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  9.Time_Display(Unknown)" );	break;
				case 5 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_TimeDisplay_Unknown );	break;
				}
				//
				break;

			case 52 :
				//
				switch( lpdi->item.iSubItem ) {
				case 0 :	strcpy( szString , "  10.Decimal_Point" );	break;
				case 5 :	sprintf( szString , "%d" , STEPVIEW_EDIT.SV_Opt_DecimalPoint );	break;
				}
				//
				break;
			}
			//
			lstrcpy( lpdi->item.pszText , szString );
			//
			return 0;

		case NM_DBLCLK :
			//
			ItemData = ListView_GetNextItem( hWnd , -1 , LVNI_SELECTED );
			//
			if ( ItemData < 0 ) return 0;
			//
			//================================
			//
			STEPVIEW_EDIT2_COLOR	= -1;
			STEPVIEW_EDIT2_COLUMN	= -1;
			STEPVIEW_EDIT2_ALIGN	= -1;
			STEPVIEW_EDIT2_VIEW		= -1;
			STEPVIEW_EDIT2_OPTION	= -1;
			//
			switch( ItemData ) {

			case 1 :	// 1.Char Size
				STEPVIEW_EDIT2_OPTION = STEPVIEW_EDIT.SV_Opt_Summary_CharSize;
				break;
			case 2 :	// 2.BackGround
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Summary_BackColor;
				break;
			case 3 :	//   Line
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Summary_LineColor;
				break;
			case 4 :	// 3.Job Name..
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Summary_BackColor_Job;		STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_NameJob;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Summary_Align_NameJob;
				break;
			case 5 :	// 4.Time Total
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Summary_BackColor_TimeJob;	STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_DataJob;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Summary_Align_DataJob;
				break;
			case 6 :	// 5.PM Info
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Summary_BackColor_Prcs;		STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_NamePrcs;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Summary_Align_NamePrcs;
				break;
			case 7 :	// 6.Step Info
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Summary_BackColor_TimePrcs;	STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_DataPrcs;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Summary_Align_DataPrcs;
				break;

			case 10 :	// 1.Char Size
				STEPVIEW_EDIT2_OPTION = STEPVIEW_EDIT.SV_Opt_StepItem_CharSize;
				break;
			case 11 :	// 2.BackGround
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Step_BackColor;
				break;
			case 12 :	//   Line
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Step_LineColor;
				break;
			case 13 :	// 3.TimeTitle
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Step_BackColor_TimeTitle;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Step_Align_TimeTitle;
				break;
			case 14 :	// 4.TimeData
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Step_BackColor_TimeData;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Step_Align_TimeData;
				break;
			case 15 :	// 5.StepTitle
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Step_BackColor_StepTitle;
				break;
			case 16 :	// 6.StepData
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Step_BackColor_StepData;
				break;
			case 17 :	// 7.StepPrePost
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Step_BackColor_StepPrePost;		STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Step_View_HidePrePost;
				break;
			case 18 :	// 8.Step
				STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_Step;																						STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Step_Align_Step;
				break;
			case 19 :	// 9.StartTime
				STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_StartTime;			STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Step_View_HideStartTime;	STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Step_Align_StartTime;
				break;
			case 20 :	// 10.EndTime
				STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_EndTime;			STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Step_View_HideEndTime;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Step_Align_EndTime;
				break;
			case 21 :	// 11.ActualTime
				STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_ActualTime;		STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Step_View_HideActualTime;	STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Step_Align_ActualTime;
				break;
			case 22 :	// 12.StableTime
				STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_StableTime;		STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Step_View_HideStableTime;	STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Step_Align_StableTime;
				break;
			case 23 :	// 13.StepTime
				STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_StepTime;			STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Step_View_HideStepTime;	STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Step_Align_StepTime;
				break;
			case 24 :	// 14.SetTime
				STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_SetTime;			STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Step_View_HideSetTime;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Step_Align_SetTime;
				break;
			case 25 :	// 15.Result
				STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_Result;			STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Step_View_HideResult;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Step_Align_Result;
				break;

			case 28 :	// 1.BackGround
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor;
				break;
			case 29 :	//   Line
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_LineColor;
				break;
			case 30 :	// 2.Title
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Title;
				break;
			case 31 :	// 3.BackMode
				STEPVIEW_EDIT2_OPTION = STEPVIEW_EDIT.SV_Opt_Item_ColorMode;
				break;
			case 32 :	// 3.No
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_No;			STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_No;				STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_No;	
				break;
			case 33 :	// 4.Name
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Name;		STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Name;				STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_Name;	
				break;
			case 34 :	// 5.Count
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Count;		STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Count;				STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideCount;			STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_Count;	
				break;
			case 35 :	// 6.Target
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Target;		STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Target;			STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideTarget;			STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_Target;
				break;
			case 36 :	// 7.Start
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Start;		STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Start;				STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideStart;			STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_Start;
				break;
			case 37 :	// 8.End
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_End;			STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_End;				STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideEnd;				STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_End;
				break;
			case 38 :	// 9.Min
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Min;			STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Min;				STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideMin;				STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_Min;
				break;
			case 39 :	// 10.Max
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Max;			STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Max;				STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideMax;				STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_Max;
				break;
			case 40 :	// 11.Avr
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Avr;			STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Avr;				STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideAvr;				STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_Avr;
				break;
			case 41 :	// 12.StdDev
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_StdDev;		STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_StdDev;			STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideStdDev;			STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_StdDev;
				break;
			case 42 :	// 13.StableTime
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_StableTime;	STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_StblTime;			STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideStblTime;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_StblTime;
				break;
			case 43 :	// 14.StepTime
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_StepTime;	STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_StepTime;			STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideStepTime;		STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_StepTime;
				break;
			case 44 :	// 15.Result
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Result;		STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Result;			STEPVIEW_EDIT2_VIEW   = STEPVIEW_EDIT.SV_Opt_Item_View_HideResult;			STEPVIEW_EDIT2_ALIGN   = STEPVIEW_EDIT.SV_Opt_Item_Align_Result;
				break;
			case 45 :	// Row(1)
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Row1;		STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_RowSize_Row1;
				break;
			case 46 :	// Row(2)
				STEPVIEW_EDIT2_COLOR = STEPVIEW_EDIT.SV_Opt_Item_BackColor_Row2;		STEPVIEW_EDIT2_COLUMN = STEPVIEW_EDIT.SV_Opt_Item_RowSize_Row2;
				break;

			case 47 :	// 16.Cal_ExceptStable
				STEPVIEW_EDIT2_OPTION = STEPVIEW_EDIT.SV_Opt_Item_Cal_ExceptStable;
				break;
			case 48 :	// 17.Time_Calculation
				STEPVIEW_EDIT2_OPTION = STEPVIEW_EDIT.SV_Opt_Item_Time_Calculation;
				break;

			case 49 :	// 18.Time_Display
				STEPVIEW_EDIT2_OPTION = STEPVIEW_EDIT.SV_Opt_TimeDisplay_Run;
				break;
			case 50 :	// 19.Time_Display
				STEPVIEW_EDIT2_OPTION = STEPVIEW_EDIT.SV_Opt_TimeDisplay_Cal;
				break;
			case 51 :	// 20.Time_Display
				STEPVIEW_EDIT2_OPTION = STEPVIEW_EDIT.SV_Opt_TimeDisplay_Unknown;
				break;
			case 52 :	// 21.Decimal_Point
				STEPVIEW_EDIT2_OPTION = STEPVIEW_EDIT.SV_Opt_DecimalPoint;
				break;

			default :
				return 0;
			}
			//
			STEPVIEW_EDIT2_POS    = ItemData;
			//
			STEPVIEW_EDIT2_PARAM  = LogView_GetParam( hWnd , ItemData );
			//
			//================================
			//
			if ( !STEP_VIEW_EDITING2( hWnd ) ) return 0;
			//
			//================================
			//
			switch( STEPVIEW_EDIT2_POS ) {

			case 1 :	// 1.Char Size
				STEPVIEW_EDIT.SV_Opt_Summary_CharSize = STEPVIEW_EDIT2_OPTION;
				break;
			case 2 :	// 2.BackGround
				STEPVIEW_EDIT.SV_Opt_Summary_BackColor = STEPVIEW_EDIT2_COLOR;
				break;
			case 3 :	//   Line
				STEPVIEW_EDIT.SV_Opt_Summary_LineColor = STEPVIEW_EDIT2_COLOR;
				break;
			case 4 :	// 3.Job Name..
				STEPVIEW_EDIT.SV_Opt_Summary_BackColor_Job = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_NameJob = STEPVIEW_EDIT2_COLUMN;	STEPVIEW_EDIT.SV_Opt_Summary_Align_NameJob = STEPVIEW_EDIT2_ALIGN;
				break;
			case 5 :	// 4.Time Total
				STEPVIEW_EDIT.SV_Opt_Summary_BackColor_TimeJob = STEPVIEW_EDIT2_COLOR;	STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_DataJob = STEPVIEW_EDIT2_COLUMN;	STEPVIEW_EDIT.SV_Opt_Summary_Align_DataJob = STEPVIEW_EDIT2_ALIGN;
				break;
			case 6 :	// 5.PM Info
				STEPVIEW_EDIT.SV_Opt_Summary_BackColor_Prcs = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_NamePrcs = STEPVIEW_EDIT2_COLUMN;	STEPVIEW_EDIT.SV_Opt_Summary_Align_NamePrcs = STEPVIEW_EDIT2_ALIGN;
				break;
			case 7 :	// 6.Step Info
				STEPVIEW_EDIT.SV_Opt_Summary_BackColor_TimePrcs = STEPVIEW_EDIT2_COLOR;	STEPVIEW_EDIT.SV_Opt_Summary_ColumnSize_DataPrcs = STEPVIEW_EDIT2_COLUMN;	STEPVIEW_EDIT.SV_Opt_Summary_Align_DataPrcs = STEPVIEW_EDIT2_ALIGN;
				break;

			case 10 :	// 1.Char Size
				STEPVIEW_EDIT.SV_Opt_StepItem_CharSize = STEPVIEW_EDIT2_OPTION;
				break;
			case 11 :	// 2.BackGround
				STEPVIEW_EDIT.SV_Opt_Step_BackColor = STEPVIEW_EDIT2_COLOR;				
				break;
			case 12 :	//   Line
				STEPVIEW_EDIT.SV_Opt_Step_LineColor = STEPVIEW_EDIT2_COLOR;				
				break;
			case 13 :	// 3.TimeTitle
				STEPVIEW_EDIT.SV_Opt_Step_BackColor_TimeTitle = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Step_Align_TimeTitle = STEPVIEW_EDIT2_ALIGN;
				break;
			case 14 :	// 4.TimeData
				STEPVIEW_EDIT.SV_Opt_Step_BackColor_TimeData = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Step_Align_TimeData = STEPVIEW_EDIT2_ALIGN;
				break;
			case 15 :	// 5.StepTitle
				STEPVIEW_EDIT.SV_Opt_Step_BackColor_StepTitle = STEPVIEW_EDIT2_COLOR;
				break;
			case 16 :	// 6.StepData
				STEPVIEW_EDIT.SV_Opt_Step_BackColor_StepData = STEPVIEW_EDIT2_COLOR;
				break;

			case 17 :	// 7.StepPrePost
				STEPVIEW_EDIT.SV_Opt_Step_BackColor_StepPrePost = STEPVIEW_EDIT2_COLOR;	STEPVIEW_EDIT.SV_Opt_Step_View_HidePrePost = STEPVIEW_EDIT2_VIEW;
				break;
			case 18 :	// 8.Step
				STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_Step = STEPVIEW_EDIT2_COLUMN;																					STEPVIEW_EDIT.SV_Opt_Step_Align_Step = STEPVIEW_EDIT2_ALIGN;
				break;

			case 19 :	// 9.StartTime
				STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_StartTime = STEPVIEW_EDIT2_COLUMN;			STEPVIEW_EDIT.SV_Opt_Step_View_HideStartTime = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Step_Align_StartTime = STEPVIEW_EDIT2_ALIGN;
				break;
			case 20 :	// 10.EndTime
				STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_EndTime = STEPVIEW_EDIT2_COLUMN;			STEPVIEW_EDIT.SV_Opt_Step_View_HideEndTime = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Step_Align_EndTime = STEPVIEW_EDIT2_ALIGN;
				break;
			case 21 :	// 11.ActualTime
				STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_ActualTime = STEPVIEW_EDIT2_COLUMN;		STEPVIEW_EDIT.SV_Opt_Step_View_HideActualTime = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Step_Align_ActualTime = STEPVIEW_EDIT2_ALIGN;
				break;
			case 22 :	// 12.StableTime
				STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_StableTime = STEPVIEW_EDIT2_COLUMN;		STEPVIEW_EDIT.SV_Opt_Step_View_HideStableTime = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Step_Align_StableTime = STEPVIEW_EDIT2_ALIGN;
				break;
			case 23 :	// 13.StepTime
				STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_StepTime = STEPVIEW_EDIT2_COLUMN;			STEPVIEW_EDIT.SV_Opt_Step_View_HideStepTime = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Step_Align_StepTime = STEPVIEW_EDIT2_ALIGN;
				break;
			case 24 :	// 14.SetTime
				STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_SetTime = STEPVIEW_EDIT2_COLUMN;			STEPVIEW_EDIT.SV_Opt_Step_View_HideSetTime = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Step_Align_SetTime = STEPVIEW_EDIT2_ALIGN;
				break;
			case 25 :	// 15.Result
				STEPVIEW_EDIT.SV_Opt_Step_ColumnSize_Result = STEPVIEW_EDIT2_COLUMN;			STEPVIEW_EDIT.SV_Opt_Step_View_HideResult = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Step_Align_Result = STEPVIEW_EDIT2_ALIGN;
				break;

			case 28 :	// 1.BackGround
				STEPVIEW_EDIT.SV_Opt_Item_BackColor = STEPVIEW_EDIT2_COLOR;
				break;
			case 29 :	//   Line
				STEPVIEW_EDIT.SV_Opt_Item_LineColor = STEPVIEW_EDIT2_COLOR;
				break;
			case 30 :	// 2.Title
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Title = STEPVIEW_EDIT2_COLOR;
				break;
			case 31 :	// 3.BackMode
				STEPVIEW_EDIT.SV_Opt_Item_ColorMode = STEPVIEW_EDIT2_OPTION;
				break;

			case 32 :	// 3.No
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_No = STEPVIEW_EDIT2_COLOR;			STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_No = STEPVIEW_EDIT2_COLUMN;	STEPVIEW_EDIT.SV_Opt_Item_Align_No = STEPVIEW_EDIT2_ALIGN;
				break;
			case 33 :	// 4.Name
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Name = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Name = STEPVIEW_EDIT2_COLUMN;	STEPVIEW_EDIT.SV_Opt_Item_Align_Name = STEPVIEW_EDIT2_ALIGN;
				break;

			case 34 :	// 5.Count
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Count = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Count = STEPVIEW_EDIT2_COLUMN;				STEPVIEW_EDIT.SV_Opt_Item_View_HideCount = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_Count = STEPVIEW_EDIT2_ALIGN;
				break;
			case 35 :	// 6.Target
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Target = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Target = STEPVIEW_EDIT2_COLUMN;			STEPVIEW_EDIT.SV_Opt_Item_View_HideTarget = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_Target = STEPVIEW_EDIT2_ALIGN;
				break;
			case 36 :	// 7.Start
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Start = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Start = STEPVIEW_EDIT2_COLUMN;				STEPVIEW_EDIT.SV_Opt_Item_View_HideStart = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_Start = STEPVIEW_EDIT2_ALIGN;
				break;
			case 37 :	// 8.End
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_End = STEPVIEW_EDIT2_COLOR;			STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_End = STEPVIEW_EDIT2_COLUMN;				STEPVIEW_EDIT.SV_Opt_Item_View_HideEnd = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_End = STEPVIEW_EDIT2_ALIGN;
				break;
			case 38 :	// 9.Min
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Min = STEPVIEW_EDIT2_COLOR;			STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Min = STEPVIEW_EDIT2_COLUMN;				STEPVIEW_EDIT.SV_Opt_Item_View_HideMin = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_Min = STEPVIEW_EDIT2_ALIGN;
				break;
			case 39 :	// 10.Max
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Max = STEPVIEW_EDIT2_COLOR;			STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Max = STEPVIEW_EDIT2_COLUMN;				STEPVIEW_EDIT.SV_Opt_Item_View_HideMax = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_Max = STEPVIEW_EDIT2_ALIGN;
				break;
			case 40 :	// 11.Avr
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Avr = STEPVIEW_EDIT2_COLOR;			STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Avr = STEPVIEW_EDIT2_COLUMN;				STEPVIEW_EDIT.SV_Opt_Item_View_HideAvr = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_Avr = STEPVIEW_EDIT2_ALIGN;
				break;
			case 41 :	// 12.StdDev
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_StdDev = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_StdDev = STEPVIEW_EDIT2_COLUMN;			STEPVIEW_EDIT.SV_Opt_Item_View_HideStdDev = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_StdDev = STEPVIEW_EDIT2_ALIGN;
				break;
			case 42 :	// 13.StableTime
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_StableTime = STEPVIEW_EDIT2_COLOR;	STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_StblTime = STEPVIEW_EDIT2_COLUMN;			STEPVIEW_EDIT.SV_Opt_Item_View_HideStblTime = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_StblTime = STEPVIEW_EDIT2_ALIGN;
				break;
			case 43 :	// 14.StepTime
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_StepTime = STEPVIEW_EDIT2_COLOR;	STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_StepTime = STEPVIEW_EDIT2_COLUMN;			STEPVIEW_EDIT.SV_Opt_Item_View_HideStepTime = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_StepTime = STEPVIEW_EDIT2_ALIGN;
				break;
			case 44 :	// 15.Result
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Result = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Item_ColumnSize_Result = STEPVIEW_EDIT2_COLUMN;			STEPVIEW_EDIT.SV_Opt_Item_View_HideResult = STEPVIEW_EDIT2_VIEW;	STEPVIEW_EDIT.SV_Opt_Item_Align_Result = STEPVIEW_EDIT2_ALIGN;
				break;

			case 45 :	// Row(1)
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Row1 = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Item_RowSize_Row1 = STEPVIEW_EDIT2_COLUMN;
				break;
			case 46 :	// Row(2)
				STEPVIEW_EDIT.SV_Opt_Item_BackColor_Row2 = STEPVIEW_EDIT2_COLOR;		STEPVIEW_EDIT.SV_Opt_Item_RowSize_Row2 = STEPVIEW_EDIT2_COLUMN;
				break;

			case 47 :	// 16.Cal_ExceptStable
				STEPVIEW_EDIT.SV_Opt_Item_Cal_ExceptStable = STEPVIEW_EDIT2_OPTION;
				break;
			case 48 :	// 17.Time_Calculation
				STEPVIEW_EDIT.SV_Opt_Item_Time_Calculation = STEPVIEW_EDIT2_OPTION;
				break;

			case 49 :	// 18.Time_Display
				STEPVIEW_EDIT.SV_Opt_TimeDisplay_Run = STEPVIEW_EDIT2_OPTION;
				break;
			case 50 :	// 19.Time_Display
				STEPVIEW_EDIT.SV_Opt_TimeDisplay_Cal = STEPVIEW_EDIT2_OPTION;
				break;
			case 51 :	// 20.Time_Display
				STEPVIEW_EDIT.SV_Opt_TimeDisplay_Unknown = STEPVIEW_EDIT2_OPTION;
				break;
			case 52 :	// 21.Decimal_Point
				STEPVIEW_EDIT.SV_Opt_DecimalPoint = STEPVIEW_EDIT2_OPTION;
				break;

			default :
				return 0;
			}
			//
			*chg = TRUE;
			//
			//================================
			//
			return 0;

		case LVN_COLUMNCLICK:
			return 0;

		case LVN_ITEMCHANGED :
			return 0;

		case LVN_KEYDOWN :
			return 0;

		case LVN_ODCACHEHINT: {
			LPNMLVCACHEHINT   lpCacheHint = (LPNMLVCACHEHINT) lParam;
		}
		return 0;

		case LVN_ODFINDITEM: {
			LPNMLVFINDITEM lpFindItem = (LPNMLVFINDITEM)lParam;
		}
		return 0;
	}
	return 0;
}
//
void STEPVIEW_EDIT_WM_DRAWITEM( HWND hwnd , LPARAM lParam , int prm , char *buffer , int buffercount ) {
	LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT) lParam;
	int i , j , z , c , b , a , Data;
	int align = 1;
	int color_text , color_back , color_line;
	BOOL sel;
	LVCOLUMN lvcol;
	//
	if ( dis->itemState & ODS_SELECTED ) { // select
		//
		sel = TRUE;
		//
//		color_text = COLOR_White;		color_back = COLOR_DarkBlue;
		color_text = COLOR_White;		color_back = 1000+RGB(51,153,255);
		color_line = COLOR_Blank;
	}
	else {
		//
		sel = FALSE;
		//
		color_text = COLOR_Black;				color_back = COLOR_White;
		color_line = COLOR_Blank;
		//
	}
	//
	i = 0;
	z = 0;
	while( TRUE ) {
		//
		lvcol.mask = LVCF_WIDTH;
		lvcol.cx = 0;
		if ( !ListView_GetColumn( hwnd , z , &lvcol ) ) break;
		j = lvcol.cx;
		//
		ListView_GetItemText( hwnd , dis->itemID , z , buffer , buffercount );
		//
		a = align;
		b = color_back;
		c = color_text;
		//
		if ( sel ) {
			//
			if ( prm == 0 ) {
				b = COLOR_DarkGray;
				c = COLOR_White;
			}
			else if ( prm == 99 ) {
				b = COLOR_White;
				c = COLOR_White;
			}
			else {
				//
				switch( z ) {
				case 1 : // Color
					//
					if ( buffer[0] != 0 ) {
						//
						Data = atoi( buffer );
						//
						if ( Data == COLOR_Blank ) {
							strcpy( buffer , "Blank" );
						}
						else if ( Data == COLOR_White ) {
							strcpy( buffer , "White" );
						}
						else {
							b = Data;
							c = Data;
						}
					}
					//
					break;
					//
				case 2 : // columnSize
					break;

				case 3 : // Align
					//
					if ( buffer[0] != 0 ) {
						//
						Data = atoi( buffer );
						//
						if      ( Data == 1 ) {
							strcpy( buffer , "Left" );
						}
						else if ( Data == 2 ) {
							strcpy( buffer , "Right" );
						}
						else {
							strcpy( buffer , "Center" );
						}
						//
					}
					break;
					//
				case 4 : // View
					//
					if ( buffer[0] != 0 ) {
						//
						Data = atoi( buffer );
						//
						if ( Data == 0 ) {
							strcpy( buffer , "On" );
						}
						else {
							strcpy( buffer , "   Off" );
						}
						//
					}
					break;
					//
				case 5 : // option
					//
					switch( prm ) {
					case 51 : // char
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , " 16" );
							}
							else if ( Data == 2 ) {
								strcpy( buffer , "  18" );
							}
							else if ( Data == 3 ) {
								strcpy( buffer , "   20" );
							}
							else if ( Data == 4 ) {
								strcpy( buffer , "    22" );
							}
							else {
								strcpy( buffer , "14" );
							}
							//
						}
						//
						break;
					case 52 : // BackMode
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if ( Data == 0 ) {
								strcpy( buffer , "Column" );
							}
							else {
								strcpy( buffer , "Row" );
							}
							//
						}
						//
						break;
					case 53 : // Cal_ExceptStable
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if ( Data == 0 ) {
								strcpy( buffer , "No" );
							}
							else {
								strcpy( buffer , "   Yes" );
							}
							//
						}
						//
						break;

					case 54 : // Time_Calculation
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , " LOG/Cal2" );
							}
							else if ( Data == 2 ) {
								strcpy( buffer , "  LOG" );
							}
							else if ( Data == 3 ) {
								strcpy( buffer , "   Cal1" );
							}
							else if ( Data == 4 ) {
								strcpy( buffer , "    Cal2" );
							}
							else if ( Data == 5 ) {
								strcpy( buffer , "     (x)" );
							}
							else {
								strcpy( buffer , "LOG/Cal1" );
							}
							//
						}
						//
						break;

					case 55 : // Time_Display(Run) Def h:m:s.ms h:m:s
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , "HH:MM:SS.x" );
							}
							else if ( Data == 2 ) {
								strcpy( buffer , "HH:MM:SS" );
							}
							else {
								strcpy( buffer , "Default" );
							}
							//
						}
						//
						break;

					case 56 : // Time_Display Def xx:xx:xx xx:xx:xx.x xx:xx:xx.xx xx:xx:xx.xxx xx xx.x xx.xx xx.xxx
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , "HH:MM:SS" );
							}
							else if ( Data == 2 ) {
								strcpy( buffer , "HH:MM:SS.x" );
							}
							else if ( Data == 3 ) {
								strcpy( buffer , "HH:MM:SS.xx" );
							}
							else if ( Data == 4 ) {
								strcpy( buffer , "HH:MM:SS.xxx" );
							}
							else if ( Data == 5 ) {
								strcpy( buffer , "SEC" );
							}
							else if ( Data == 6 ) {
								strcpy( buffer , "SEC.x" );
							}
							else if ( Data == 7 ) {
								strcpy( buffer , "SEC.xx" );
							}
							else if ( Data == 8 ) {
								strcpy( buffer , "SEC.xxx" );
							}
							else {
								strcpy( buffer , "Default" );
							}
							//
						}
						//
						break;

					case 57 : // Time_Display Def 0(Zero)
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , "0(Zero)" );
							}
							else {
								strcpy( buffer , "Default" );
							}
							//
						}
						//
						break;

					case 58 : // Decimal_Point Def x.x x.xx x.xxx x.xxxx x
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , "x.x" );
							}
							else if ( Data == 2 ) {
								strcpy( buffer , "x.xx" );
							}
							else if ( Data == 3 ) {
								strcpy( buffer , "x.xxx" );
							}
							else if ( Data == 4 ) {
								strcpy( buffer , "x.xxxx" );
							}
							else if ( Data == 5 ) {
								strcpy( buffer , "x" );
							}
							else {
								strcpy( buffer , "Default" );
							}
							//
						}
						//
						break;
					}
				}
			}
		}
		else {
			//
			if ( prm == 0 ) {
				b = COLOR_DarkGray;
				c = COLOR_White;
			}
			else if ( prm == 99 ) {
				b = COLOR_White;
				c = COLOR_White;
			}
			else {
				//
				switch( z ) {
				case 1 : // Color
					//
					if ( buffer[0] != 0 ) {
						//
						Data = atoi( buffer );
						//
						if ( Data == COLOR_Blank ) {
							strcpy( buffer , "Blank" );
						}
						else if ( Data == COLOR_White ) {
							strcpy( buffer , "White" );
						}
						else {
							b = Data;
							c = Data;
						}
					}
					//
					break;
					//
				case 2 : // columnSize
					break;

				case 3 : // Align
					//
					if ( buffer[0] != 0 ) {
						//
						Data = atoi( buffer );
						//
						if      ( Data == 1 ) {
							strcpy( buffer , "Left" );
						}
						else if ( Data == 2 ) {
							strcpy( buffer , "Right" );
						}
						else {
							strcpy( buffer , "Center" );
						}
						//
					}
					break;
					//
				case 4 : // View
					//
					if ( buffer[0] != 0 ) {
						//
						Data = atoi( buffer );
						//
						if ( Data == 0 ) {
							strcpy( buffer , "On" );
						}
						else {
							b = COLOR_RightRed;
							strcpy( buffer , "   Off" );
						}
						//
					}
					break;
					//
				case 5 : // option
					//
					switch( prm ) {
					case 51 : // char
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , " 16" );
							}
							else if ( Data == 2 ) {
								strcpy( buffer , "  18" );
							}
							else if ( Data == 3 ) {
								strcpy( buffer , "   20" );
							}
							else if ( Data == 4 ) {
								strcpy( buffer , "    22" );
							}
							else {
								strcpy( buffer , "14" );
							}
							//
						}
						//
						break;
					case 52 : // BackMode
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if ( Data == 0 ) {
								strcpy( buffer , "Column" );
							}
							else {
								strcpy( buffer , "Row" );
							}
							//
						}
						//
						break;
					case 53 : // Cal_ExceptStable
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if ( Data == 0 ) {
								strcpy( buffer , "No" );
							}
							else {
								strcpy( buffer , "   Yes" );
							}
							//
						}
						//
						break;

					case 54 : // Time_Calculation
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , " LOG/Cal2" );
							}
							else if ( Data == 2 ) {
								strcpy( buffer , "  LOG" );
							}
							else if ( Data == 3 ) {
								strcpy( buffer , "   Cal1" );
							}
							else if ( Data == 4 ) {
								strcpy( buffer , "    Cal2" );
							}
							else if ( Data == 5 ) {
								strcpy( buffer , "     (x)" );
							}
							else {
								strcpy( buffer , "LOG/Cal1" );
							}
							//
						}
						//
						break;

					case 55 : // Time_Display(Run) Def h:m:s.ms h:m:s
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , "HH:MM:SS.x" );
							}
							else if ( Data == 2 ) {
								strcpy( buffer , "HH:MM:SS" );
							}
							else {
								strcpy( buffer , "Default" );
							}
							//
						}
						//
						break;

					case 56 : // Time_Display Def xx:xx:xx xx:xx:xx.x xx:xx:xx.xx xx:xx:xx.xxx xx xx.x xx.xx xx.xxx
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , "HH:MM:SS" );
							}
							else if ( Data == 2 ) {
								strcpy( buffer , "HH:MM:SS.x" );
							}
							else if ( Data == 3 ) {
								strcpy( buffer , "HH:MM:SS.xx" );
							}
							else if ( Data == 4 ) {
								strcpy( buffer , "HH:MM:SS.xxx" );
							}
							else if ( Data == 5 ) {
								strcpy( buffer , "SEC" );
							}
							else if ( Data == 6 ) {
								strcpy( buffer , "SEC.x" );
							}
							else if ( Data == 7 ) {
								strcpy( buffer , "SEC.xx" );
							}
							else if ( Data == 8 ) {
								strcpy( buffer , "SEC.xxx" );
							}
							else {
								strcpy( buffer , "Default" );
							}
							//
						}
						//
						break;

					case 57 : // Time_Display Def 0(Zero)
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , "0(Zero)" );
							}
							else {
								strcpy( buffer , "Default" );
							}
							//
						}
						//
						break;

					case 58 : // Decimal_Point Def x.x x.xx x.xxx x.xxxx x
						//
						if ( buffer[0] != 0 ) {
							//
							Data = atoi( buffer );
							//
							if      ( Data == 1 ) {
								strcpy( buffer , "x.x" );
							}
							else if ( Data == 2 ) {
								strcpy( buffer , "x.xx" );
							}
							else if ( Data == 3 ) {
								strcpy( buffer , "x.xxx" );
							}
							else if ( Data == 4 ) {
								strcpy( buffer , "x.xxxx" );
							}
							else if ( Data == 5 ) {
								strcpy( buffer , "x" );
							}
							else {
								strcpy( buffer , "Default" );
							}
							//
						}
						//
						break;
					}
				}
			}
		}
		//
		KGUI_DRAW_StrBoxLV( dis->hDC , dis->rcItem.left + i , dis->rcItem.top , buffer , dis->rcItem.left + i + j , dis->rcItem.bottom , 0 , a , color_line , color_line , c , b , "" );
		//
		i = i + j;
		z++;
	}
}
//

void STEPVIEW_REVIEW( HWND hWnd , int id ) {
	//
	int xs , ys;
	HWND hdlg;
	RECT rect;
	BOOL vis;
	//
	hdlg = GetDlgItem( hWnd ,id );
	//
	if ( IsWindow( hdlg ) ) {
		//
		vis = IsWindowVisible( hdlg );
		//
		GetWindowRect( hdlg , &rect );
		//
		xs = rect.right - rect.left;
		ys = rect.bottom - rect.top;
		//
		ScreenToClient( hWnd , (LPPOINT)&rect );
		//
		DestroyWindow( hdlg );
		//
		//----------------------
		//
		hdlg = CreateWindowEx( WS_EX_STATICEDGE , WC_LISTVIEW , "" , WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_AUTOARRANGE | LVS_OWNERDRAWFIXED | LVS_NOSORTHEADER | WS_BORDER | WS_TABSTOP , rect.left , rect.top , xs , ys , hWnd , (HMENU) id , GetModuleHandle( NULL ) , NULL );
		//
		ShowWindow( hdlg , vis ? SW_SHOW : SW_HIDE );
		//
		if ( vis ) InvalidateRect( hdlg , NULL , TRUE );
		//
	}
	//
}


HWND STEP_VIEW_HWND_EDITING = NULL;
HWND STEP_VIEW_MAINHWND_EDITING = NULL;

BOOL APIENTRY Console_Gui_For_StepView_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam ) { // 2016.10.11
	LPDRAWITEMSTRUCT dis;
	LPNMHDR  lpnmh;
	char Temp_Buffer[512];
	BOOL schg;
	//
	static BOOL stv;

	switch ( msg ) {

		case WM_INITDIALOG:
			//
			stv = STEPVIEW_SAVE;
			//
			STEPVIEW_EDIT_REINIT = FALSE;
			memcpy( &STEPVIEW_EDIT_INITDATA , &STEPVIEW_DATA , sizeof( StepViewTemplate ) );
			//
			memcpy( &STEPVIEW_EDIT , &STEPVIEW_DATA , sizeof( StepViewTemplate ) );
			//
			STEPVIEW_EDIT_INIT( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
			//
			STEPVIEW_EDIT_SET( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) );
			//
			KWIN_Item_Disable( hdlg , IDOK );
			//
			if ( ( STEPVIEW_EDIT.SV_Opt_Button_Char1[0] == 0 ) && ( STEPVIEW_EDIT.SV_Opt_Button_Char2[0] == 0 ) ) {
				sprintf( Temp_Buffer , "" );
			}
			else {
				sprintf( Temp_Buffer , "%s|%s" , STEPVIEW_EDIT.SV_Opt_Button_Char1 , STEPVIEW_EDIT.SV_Opt_Button_Char2 );
			}
			//
			KWIN_Item_String_Display( hdlg , IDC_CTRL_ADD , Temp_Buffer );
			//
			ShowWindow( hdlg , SW_SHOW );
			//
			return TRUE;
			
		case WM_NOTIFY:
			//
			lpnmh = (LPNMHDR) lParam;
			//
			if ( lpnmh->hwndFrom == GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) ) {
				//
				STEPVIEW_EDIT_Notify( lpnmh->hwndFrom , lParam , &schg );
				//
				if ( schg ) {
					//
					stv = TRUE;
					//
					KWIN_Item_Enable( hdlg , IDOK );
					//
					InvalidateRect( hdlg , NULL , FALSE );
					//
				}
				//
			}
			return TRUE;


		case WM_DRAWITEM:
			//
			dis = (LPDRAWITEMSTRUCT) lParam;
			//
			if ( dis->hwndItem == GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) ) {
				//
				STEPVIEW_EDIT_WM_DRAWITEM( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , lParam , LogView_GetParam( GetDlgItem( hdlg , IDC_LIST_LOGVIEW ) , dis->itemID ) , Temp_Buffer , 511 );
				//
			}
			return TRUE;

		case WM_COMMAND :
			switch( wParam ) {
				//
				case IDC_CTRL_ADD : // Button String
					//
					if ( ( STEPVIEW_EDIT.SV_Opt_Button_Char1[0] == 0 ) && ( STEPVIEW_EDIT.SV_Opt_Button_Char2[0] == 0 ) ) {
						sprintf( Temp_Buffer , "" );
					}
					else {
						sprintf( Temp_Buffer , "%s|%s" , STEPVIEW_EDIT.SV_Opt_Button_Char1 , STEPVIEW_EDIT.SV_Opt_Button_Char2 );
					}
					//
					if ( MODAL_STRING_BOX1( hdlg , "Edit" , "Button String?" , Temp_Buffer ) ) {
						//
						STR_SEPERATE_CHAR( Temp_Buffer , '|' , STEPVIEW_EDIT.SV_Opt_Button_Char1 , STEPVIEW_EDIT.SV_Opt_Button_Char2 , 63 );
						//
						if ( ( STEPVIEW_EDIT.SV_Opt_Button_Char1[0] == 0 ) && ( STEPVIEW_EDIT.SV_Opt_Button_Char2[0] == 0 ) ) {
							sprintf( Temp_Buffer , "" );
						}
						else {
							sprintf( Temp_Buffer , "%s|%s" , STEPVIEW_EDIT.SV_Opt_Button_Char1 , STEPVIEW_EDIT.SV_Opt_Button_Char2 );
						}
						//
						KWIN_Item_String_Display( hdlg , IDC_CTRL_ADD , Temp_Buffer );
						//
						stv = TRUE;
						//
						KWIN_Item_Enable( hdlg , IDOK );
						//
					}
					//
					return TRUE;

				case IDRETRY : // Set with Default
					//
					if ( IDYES == MODAL_WINDOW_MESSAGE_BOX( hdlg , "Really Set with Default???" , "Check" , MB_ICONQUESTION | MB_YESNO ) ) {
						//
						stv = FALSE;
						//
						Kor_StepView_Initial( &STEPVIEW_EDIT );
						//
						KWIN_Item_Enable( hdlg , IDOK );
						//
						InvalidateRect( hdlg , NULL , FALSE );
						//
					}
					//
					return TRUE;

				case IDYES : // Read File
					//
					if ( !MODAL_FILE_OPEN_BOX( hdlg , "" , "Select Open File...." , "*.*" , "subdir|global" , Temp_Buffer ) ) return TRUE;
					//
					if ( STEPVIEW_EDIT_Open( Temp_Buffer ) ) {
						//
						stv = TRUE;
						//
						KWIN_Item_Enable( hdlg , IDOK );
						//
						InvalidateRect( hdlg , NULL , FALSE );
						//
					}
					else {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Invalid StepView File for Open" , "Error" , MB_ICONHAND );
					}
					//
					return TRUE;

				case IDNO : // Save File
					//
					if ( !MODAL_FILE_SAVE_BOX( hdlg , "" , "Select Save File...." , "*.*" , "subdir|global" , Temp_Buffer ) ) return TRUE;
					//
					if ( !STEPVIEW_EDIT_Save( Temp_Buffer ) ) {
						//
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Invalid StepView File for Save" , "Error" , MB_ICONHAND );
						//
					}
					//
					return TRUE;

				case IDABORT : // Adapt
					//
					STEPVIEW_EDIT_REINIT = TRUE;
					//
					memcpy( &STEPVIEW_DATA , &STEPVIEW_EDIT , sizeof( StepViewTemplate ) );
					//
					SendMessage( STEP_VIEW_MAINHWND_EDITING , WM_COMMAND , WM_PAINT , (LPARAM) NULL );
					//
					return TRUE;

				case IDOK :
					//
					if ( Get_Prm_READONLY_MODE() || SYSTEM_LOGSKIP_STATUS() ) {
						MODAL_WINDOW_MESSAGE_BOX( hdlg , "Edit Impossible(Read Only Mode)" , "Edit Impossible" , MB_ICONHAND );
						return TRUE;
					}
					//
					STEPVIEW_SAVE = stv;
					//
					memcpy( &STEPVIEW_DATA , &STEPVIEW_EDIT , sizeof( StepViewTemplate ) );
					//
					Kor_Info_Save( JOB_INFO_FILENAME );
					//
					SendMessage( STEP_VIEW_MAINHWND_EDITING , WM_COMMAND , WM_PAINT , (LPARAM) NULL );
					//
					STEP_VIEW_HWND_EDITING = NULL;
					//
					DestroyWindow( hdlg );
					//
					return TRUE;

				case IDCANCEL :
					//
					if ( STEPVIEW_EDIT_REINIT ) {
						//
						memcpy( &STEPVIEW_DATA , &STEPVIEW_EDIT_INITDATA , sizeof( StepViewTemplate ) );
						//
						SendMessage( STEP_VIEW_MAINHWND_EDITING , WM_COMMAND , WM_PAINT , (LPARAM) NULL );
						//
					}
					//
					STEP_VIEW_HWND_EDITING = NULL;
					//
					DestroyWindow( hdlg );
					//
					return TRUE;
			}
			return TRUE;

	}
	return FALSE;
}









BOOL STEP_VIEW_RUN_EDITING() {
	return ( STEP_VIEW_HWND_EDITING != NULL );
}



BOOL STEP_VIEW_EDITING( HWND hdlg ) { // 2016.10.11
	//
	if ( STEP_VIEW_HWND_EDITING != NULL ) return FALSE;
	//
	STEP_VIEW_MAINHWND_EDITING = hdlg;
	//
	STEP_VIEW_HWND_EDITING = CreateDialog( GETHINST( hdlg ) , MAKEINTRESOURCE( IDD_JOB_STEPVIEW ) , hdlg , (DLGPROC) Console_Gui_For_StepView_Proc );
	//
	if ( STEP_VIEW_HWND_EDITING == NULL ) return FALSE;
	//
	return TRUE;
	//
}
