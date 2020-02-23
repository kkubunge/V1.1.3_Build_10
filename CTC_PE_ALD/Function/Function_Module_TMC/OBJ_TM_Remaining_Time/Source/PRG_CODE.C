#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"

int g_LotPre_Use, g_CM1_LotPre_Use, g_CM2_LotPre_Use, g_CM3_LotPre_Use;
//int g_nStageHeater_UpperSeconds = 0;

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "CTC.ESTIMATION_TIME"         , _K_S_IO	,   CTC_ESTIMATION_TIME   , 0   } ,    
    { "CTC.ESTIMATION_TIME2"        , _K_S_IO	,   CTC_ESTIMATION_TIME2  , 0   } ,         
    { "CTC.ESTIMATION_TIME3"        , _K_S_IO	,   CTC_ESTIMATION_TIME3  , 0   } ,                 

    { "CTC.REMAINING_TIME"          , _K_S_IO	,   CTC_REMAINING_TIME    , 0   } ,
    { "CTC.REMAINING_TIME2"         , _K_S_IO	,   CTC_REMAINING_TIME2   , 0   } ,	
    { "CTC.REMAINING_TIME3"         , _K_S_IO	,   CTC_REMAINING_TIME3   , 0   } ,	

	{ "CTC.MAIN_CONTROL"			, _K_D_IO	,   CTC_MAIN_CONTROL	  , 0   } ,
	{ "CTC.MAIN_CONTROL2"			, _K_D_IO	,   CTC_MAIN_CONTROL2	  , 0   } ,
	{ "CTC.MAIN_CONTROL3"			, _K_D_IO	,   CTC_MAIN_CONTROL3	  , 0   } ,
	""
};

enum { Use, NotUse };
enum { PM1, PM2, PM3, PM4, PM5 };

char* Convert_Time(int nSec) { // Time Change to hh:mm:ss 
   char*  pszTime = "";  
   char*  pszHour = "";      
   char*  pszMinutes = "";         
   char*  pszSeconds = "";      

   int nSeconds = 0, nMinutes = 0, nHour = 0;  
      
   if(nSec<= 0) {
       sprintf(pszTime,"00:00:00");	  	     
   } 	   
   else { 
       nMinutes = nSec/ 60;            
       nSeconds = nSec% 60;              

       nHour = nMinutes / 60;   
       nMinutes = nMinutes % 60;    

       if(nHour < 10) {
		 sprintf(pszHour,"0%d", nHour);
	   } 
       else {
		 sprintf(pszHour,"%d", nHour);	   
	   } 

       if(nMinutes < 10) {
	     sprintf(pszMinutes,"0%d", nMinutes);
	   }     
       else {
		 sprintf(pszMinutes,"%d", nMinutes);	   	   
	   } 	    
   
       if(nSeconds < 10) {
	     sprintf(pszSeconds ,"0%d", nSeconds);
	   }               
       else {
	     sprintf(pszSeconds ,"%d", nSeconds);	   
	   } 	    

//     printf("Remaining Time integer : %d / %d / %d \n", nHour, nMinutes, nSeconds);    
//     printf("Remaining Time String : %s / %s / %s \n", pszHour, pszMinutes, pszSeconds);    
	   sprintf(pszTime,"%s:%s:%s", pszHour, pszMinutes, pszSeconds);	  	     
   } 
   return  pszTime; 
}

int RemainTime_set(char* pszEstimation_time)
{
	char szBuffer1[32], szBuffer2[32], szBuffer3[32], szCurrent_time[32];
	int nRemain_seconds = 0; 
	int nEstimation_seconds = 0; 
	int nNow_seconds = 0;

	memset( szBuffer1,			0,		sizeof(szBuffer1));
	memset( szBuffer2,			0,		sizeof(szBuffer2));
	memset( szBuffer3,			0,		sizeof(szBuffer3));
	memset( szCurrent_time,		0,		sizeof(szCurrent_time));

	if(strlen(pszEstimation_time) == 0) {
		nRemain_seconds = 0; 
	} 	
	else {    
		_strtime( szCurrent_time );
		
		STR_SEPERATE_CHAR( pszEstimation_time , ' ' , szBuffer1 , szBuffer2 , 255 );//estimation
		STR_SEPERATE_CHAR( szBuffer2 , ':' , szBuffer1 , szBuffer3 , 255 );         // hour
		nEstimation_seconds = atoi(szBuffer1) * 60 * 60;
		STR_SEPERATE_CHAR( szBuffer3 , ':' , szBuffer1 , szBuffer2 , 255 );         // minutes
		nEstimation_seconds = nEstimation_seconds + atoi(szBuffer1) * 60 ; 
		nEstimation_seconds = nEstimation_seconds + atoi(szBuffer2);                // seconds
		
		STR_SEPERATE_CHAR( szCurrent_time , ':' , szBuffer1 , szBuffer2 , 255 );    // hour
		nNow_seconds = atoi(szBuffer1) * 60 * 60;
		STR_SEPERATE_CHAR(szBuffer2 , ':' , szBuffer1 , szBuffer3 , 255 );          //  minutes
		nNow_seconds = nNow_seconds + atoi(szBuffer1) * 60;
		nNow_seconds = nNow_seconds + atoi(szBuffer3);
		
		nRemain_seconds = nEstimation_seconds -  nNow_seconds;  
	} 
	return nRemain_seconds;
}

void CM1_RemainingTime_Control() {
    int /*i,*/ nCommStatus = 0,/* nMaxGapTemp, nMaxGapTempPM,*/ nMainControl, nMainControl2, nMainControl3;//, nTempStable_Time;
	double dbRamp_Rate = 0, dbPM1_RampRateRAIO = 0, dbPM2_RampRateRAIO = 0, dbPM3_RampRateRAIO = 0;//, dbPM4_RampRateRAIO = 0, dbPM5_RampRateRAIO = 0;      
	double dbPM1_RampRateFAIO = 0, dbPM2_RampRateFAIO = 0, dbPM3_RampRateFAIO = 0;//, dbPM4_RampRateFAIO = 0, dbPM5_RampRateFAIO = 0;      
    char szEstimation_time[32];
	char* pRemain_time = ""; 
//	double dbPM_STH_Gap[5];
	int nRemaining_seconds=0, nRemaining_seconds2=0, nRemaining_seconds3=0;

	memset( szEstimation_time,		0,		sizeof(szEstimation_time));

	Sleep(1000);

	nMainControl2 = READ_DIGITAL( CTC_MAIN_CONTROL2 ,&nCommStatus );
	nMainControl3 = READ_DIGITAL( CTC_MAIN_CONTROL3 ,&nCommStatus );
	do{
		Sleep(1000);

		if( 1==nMainControl2 || 6==nMainControl2 ) {
			READ_STRING(CTC_ESTIMATION_TIME2, szEstimation_time , &nCommStatus );
			nRemaining_seconds2 = RemainTime_set(szEstimation_time);
		}
		else nRemaining_seconds2 = 0;
			
		if( 1==nMainControl3 || 6==nMainControl3 ) {
			READ_STRING(CTC_ESTIMATION_TIME3, szEstimation_time , &nCommStatus );
			nRemaining_seconds3 = RemainTime_set(szEstimation_time);
		}
		else nRemaining_seconds3 = 0;
		
		READ_STRING(CTC_ESTIMATION_TIME, szEstimation_time , &nCommStatus );
		nRemaining_seconds = RemainTime_set(szEstimation_time);
		nRemaining_seconds = nRemaining_seconds + nRemaining_seconds2 + nRemaining_seconds3;
		pRemain_time = Convert_Time(nRemaining_seconds);
		WRITE_STRING (CTC_REMAINING_TIME, pRemain_time , &nCommStatus);
		nMainControl = READ_DIGITAL( CTC_MAIN_CONTROL ,&nCommStatus );
	}while( 1==nMainControl || 6==nMainControl );
	WRITE_STRING (CTC_REMAINING_TIME, "00:00:00" , &nCommStatus);
	_endthread();
}
void CM2_RemainingTime_Control()
{
    int /*i,*/ nCommStatus = 0, /* nMaxGapTemp, nMaxGapTempPM,*/ nMainControl, nMainControl2, nMainControl3;//, nTempStable_Time;
	double dbRamp_Rate = 0, dbPM1_RampRateRAIO = 0, dbPM2_RampRateRAIO = 0, dbPM3_RampRateRAIO = 0;//, dbPM4_RampRateRAIO = 0, dbPM5_RampRateRAIO = 0;      
	double dbPM1_RampRateFAIO = 0, dbPM2_RampRateFAIO = 0, dbPM3_RampRateFAIO = 0;//, dbPM4_RampRateFAIO = 0, dbPM5_RampRateFAIO = 0;      
    char szEstimation_time[32];
	char* pRemain_time = ""; 
//	double dbPM_STH_Gap[5];
	int nRemaining_seconds=0, nRemaining_seconds2=0, nRemaining_seconds3=0;

	memset( szEstimation_time,		0,		sizeof(szEstimation_time));

	Sleep(1000);

	nMainControl = READ_DIGITAL( CTC_MAIN_CONTROL ,&nCommStatus );
	nMainControl3 = READ_DIGITAL( CTC_MAIN_CONTROL3 ,&nCommStatus );
	do{
		Sleep(1000);

		if( 1==nMainControl || 6==nMainControl ) {
			READ_STRING(CTC_ESTIMATION_TIME, szEstimation_time , &nCommStatus );
			nRemaining_seconds = RemainTime_set(szEstimation_time);
		}
		else nRemaining_seconds = 0;

		if( 1==nMainControl3 || 6==nMainControl3 ) {
			READ_STRING(CTC_ESTIMATION_TIME3, szEstimation_time , &nCommStatus );
			nRemaining_seconds3 = RemainTime_set(szEstimation_time);
		}
		else nRemaining_seconds3 = 0;

		READ_STRING(CTC_ESTIMATION_TIME2, szEstimation_time , &nCommStatus );
		nRemaining_seconds2 = RemainTime_set(szEstimation_time);
		nRemaining_seconds2 = nRemaining_seconds + nRemaining_seconds2 + nRemaining_seconds3;
		pRemain_time = Convert_Time(nRemaining_seconds2);
		WRITE_STRING (CTC_REMAINING_TIME2, pRemain_time , &nCommStatus);
//		}
		
		nMainControl2 = READ_DIGITAL( CTC_MAIN_CONTROL2 ,&nCommStatus );
//		printf("==============> nMainControl2 = [%d]\n", nMainControl2);		
	}while ( (1 == nMainControl2) || (6 == nMainControl2) );
	WRITE_STRING (CTC_REMAINING_TIME2, "00:00:00" , &nCommStatus);
	_endthread();
}     
void CM3_RemainingTime_Control()
{
    int /*i,*/ nCommStatus = 0,/* nMaxGapTemp, nMaxGapTempPM,*/ nMainControl, nMainControl2, nMainControl3;//, nTempStable_Time;
	double dbRamp_Rate = 0, dbPM1_RampRateRAIO = 0, dbPM2_RampRateRAIO = 0, dbPM3_RampRateRAIO = 0;//, dbPM4_RampRateRAIO = 0, dbPM5_RampRateRAIO = 0;      
	double dbPM1_RampRateFAIO = 0, dbPM2_RampRateFAIO = 0, dbPM3_RampRateFAIO = 0;//, dbPM4_RampRateFAIO = 0, dbPM5_RampRateFAIO = 0;      
    char szEstimation_time[32] = "";
	char* pRemain_time = ""; 
//	double dbPM_STH_Gap[5];
	int nRemaining_seconds=0, nRemaining_seconds2=0, nRemaining_seconds3=0;

	memset( szEstimation_time,		0,		sizeof(szEstimation_time));

	Sleep(1000);

	nMainControl = READ_DIGITAL( CTC_MAIN_CONTROL ,&nCommStatus );
	nMainControl2 = READ_DIGITAL( CTC_MAIN_CONTROL2 ,&nCommStatus );
	do{
		Sleep(1000);

		if( 1==nMainControl || 6==nMainControl ) {
			READ_STRING(CTC_ESTIMATION_TIME, szEstimation_time , &nCommStatus );
			nRemaining_seconds = RemainTime_set(szEstimation_time);
		}
		else nRemaining_seconds = 0;

		if( 1==nMainControl2 || 6==nMainControl2 ) {
			READ_STRING(CTC_ESTIMATION_TIME2, szEstimation_time , &nCommStatus );
			nRemaining_seconds2 = RemainTime_set(szEstimation_time);
		}
		else nRemaining_seconds2 = 0;

		READ_STRING(CTC_ESTIMATION_TIME3, szEstimation_time , &nCommStatus );
		nRemaining_seconds3 = RemainTime_set(szEstimation_time);
		nRemaining_seconds3 = nRemaining_seconds + nRemaining_seconds2 + nRemaining_seconds3;
		pRemain_time = Convert_Time(nRemaining_seconds3);
		WRITE_STRING (CTC_REMAINING_TIME3, pRemain_time , &nCommStatus);
//		}
		
		nMainControl3 = READ_DIGITAL( CTC_MAIN_CONTROL3 ,&nCommStatus );
		
	}while( 1==nMainControl3 || 6==nMainControl3 );
	WRITE_STRING (CTC_REMAINING_TIME3, "00:00:00" , &nCommStatus);
	_endthread();
}     

//------------------------------------------------------------------------------------------
Module_Status Program_Main() {

	int nCommStatus;
	char *pszEd;
	char szBuffer1[256] , szBuffer2[256], szEvent[256], szModuleName[256];
	memset(szEvent,			0,  sizeof(szEvent));
	memset(szBuffer1,		0,  sizeof(szBuffer1));
	memset(szBuffer2,		0,	sizeof(szBuffer2));
    memset(szModuleName,	0,	sizeof(szModuleName));     

	pszEd = PROGRAM_PARAMETER_READ();

	STR_SEPERATE( pszEd , szEvent , szBuffer1	, 255 );
	STR_SEPERATE_CHAR( szBuffer1, '|', szModuleName, szBuffer2, 255 );	

	if( STRCMP_L( szEvent, "TIME_START" ))
	{
		if( STRCMP_L( szModuleName, "PORT1" ))
		{
			_beginthread( (void*)CM1_RemainingTime_Control, 0 , NULL );
		}
	    else if( STRCMP_L( szModuleName, "PORT2" ))
		{
			_beginthread( (void*)CM2_RemainingTime_Control, 0 , NULL );
		}
	    else if( STRCMP_L( szModuleName, "PORT3" ))
		{
			_beginthread( (void*)CM3_RemainingTime_Control, 0 , NULL );
		}
		else
			printf("===> Error : szModuleName is not available.\n");
	}
	else if( STRCMP_L( szEvent, "TIME_FIRSTRUN" ))
	{
		if( STRCMP_L( szModuleName, "PORT1" ))
		{
			g_CM1_LotPre_Use = NotUse;
		}
	    else if( STRCMP_L( szModuleName, "PORT2" ))
		{
			g_CM2_LotPre_Use = NotUse;
		}
	    else if( STRCMP_L( szModuleName, "PORT3" ))
		{
			g_CM3_LotPre_Use = NotUse;
		}
		else
			printf("===> Error : szModuleName is not available.\n");
	}
	else if( (STRCMP_L( szEvent, "TIME_COMPLETE" )) || (STRCMP_L( szEvent, "ABORTED_F" )) || (STRCMP_L( szEvent, "END_F" )) )
	{
		if( STRCMP_L( szModuleName, "PORT1" ))
		{
			WRITE_STRING (CTC_REMAINING_TIME, "00:00:00" , &nCommStatus);
		}
	    else if( STRCMP_L( szModuleName, "PORT2" ))
		{
			WRITE_STRING (CTC_REMAINING_TIME2, "00:00:00" , &nCommStatus);
		}
	    else if( STRCMP_L( szModuleName, "PORT3" ))
		{
			WRITE_STRING (CTC_REMAINING_TIME3, "00:00:00" , &nCommStatus);
		}
		else
			printf("===> Error : szModuleName is not available.\n");
	}
	else
		printf("===> Error : Event Name is not available.\n");

	return SYS_SUCCESS;
}
BOOL Program_Init_Code() {
	int nCommStatus;
	WRITE_STRING (CTC_REMAINING_TIME, "00:00:00" , &nCommStatus);
	WRITE_STRING (CTC_REMAINING_TIME2, "00:00:00" , &nCommStatus);
	WRITE_STRING (CTC_REMAINING_TIME3, "00:00:00" , &nCommStatus);
	return TRUE;
}
