#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutlanal.h>
#include <Kutlgui.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum	{ Manual  , Auto };
enum	{ ON	  , OFF	 };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		PORT1_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM		271
#define		PORT2_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM		571
#define		PORT3_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM		871
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int ALARM_START_POINT = 71;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "CM1.MAPWAFER_COUNT"			, _K_D_IO	,	CM1_MAPWAFER_COUNT	,	0	} , 
	{ "CM2.MAPWAFER_COUNT"			, _K_D_IO	,	CM2_MAPWAFER_COUNT	,	0	} , 
	{ "CM3.MAPWAFER_COUNT"			, _K_D_IO	,	CM3_MAPWAFER_COUNT	,	0	} , 
	
	{ "CM1.C01_Wafer"				, _K_D_IO	,	CM1_C01_Wafer		,	0	} ,
	{ "CM1.C02_Wafer"				, _K_D_IO	,	CM1_C02_Wafer		,	0	} ,
	{ "CM1.C03_Wafer"				, _K_D_IO	,	CM1_C03_Wafer		,	0	} ,
	{ "CM1.C04_Wafer"				, _K_D_IO	,	CM1_C04_Wafer		,	0	} ,
	{ "CM1.C05_Wafer"				, _K_D_IO	,	CM1_C05_Wafer		,	0	} ,
	{ "CM1.C06_Wafer"				, _K_D_IO	,	CM1_C06_Wafer		,	0	} ,
	{ "CM1.C07_Wafer"				, _K_D_IO	,	CM1_C07_Wafer		,	0	} ,
	{ "CM1.C08_Wafer"				, _K_D_IO	,	CM1_C08_Wafer		,	0	} ,
	{ "CM1.C09_Wafer"				, _K_D_IO	,	CM1_C09_Wafer		,	0	} ,
	{ "CM1.C10_Wafer"				, _K_D_IO	,	CM1_C10_Wafer		,	0	} ,
	{ "CM1.C11_Wafer"				, _K_D_IO	,	CM1_C11_Wafer		,	0	} ,
	{ "CM1.C12_Wafer"				, _K_D_IO	,	CM1_C12_Wafer		,	0	} ,
	{ "CM1.C13_Wafer"				, _K_D_IO	,	CM1_C13_Wafer		,	0	} ,
	{ "CM1.C14_Wafer"				, _K_D_IO	,	CM1_C14_Wafer		,	0	} ,
	{ "CM1.C15_Wafer"				, _K_D_IO	,	CM1_C15_Wafer		,	0	} ,
	{ "CM1.C16_Wafer"				, _K_D_IO	,	CM1_C16_Wafer		,	0	} ,
	{ "CM1.C17_Wafer"				, _K_D_IO	,	CM1_C17_Wafer		,	0	} ,
	{ "CM1.C18_Wafer"				, _K_D_IO	,	CM1_C18_Wafer		,	0	} ,
	{ "CM1.C19_Wafer"				, _K_D_IO	,	CM1_C19_Wafer		,	0	} ,
	{ "CM1.C20_Wafer"				, _K_D_IO	,	CM1_C20_Wafer		,	0	} ,
	{ "CM1.C21_Wafer"				, _K_D_IO	,	CM1_C21_Wafer		,	0	} ,
	{ "CM1.C22_Wafer"				, _K_D_IO	,	CM1_C22_Wafer		,	0	} ,
	{ "CM1.C23_Wafer"				, _K_D_IO	,	CM1_C23_Wafer		,	0	} ,
	{ "CM1.C24_Wafer"				, _K_D_IO	,	CM1_C24_Wafer		,	0	} ,
	{ "CM1.C25_Wafer"				, _K_D_IO	,	CM1_C25_Wafer		,	0	} ,
	
	{ "CM2.C01_Wafer"				, _K_D_IO	,	CM2_C01_Wafer		,	0	} ,
	{ "CM2.C02_Wafer"				, _K_D_IO	,	CM2_C02_Wafer		,	0	} ,
	{ "CM2.C03_Wafer"				, _K_D_IO	,	CM2_C03_Wafer		,	0	} ,
	{ "CM2.C04_Wafer"				, _K_D_IO	,	CM2_C04_Wafer		,	0	} ,
	{ "CM2.C05_Wafer"				, _K_D_IO	,	CM2_C05_Wafer		,	0	} ,
	{ "CM2.C06_Wafer"				, _K_D_IO	,	CM2_C06_Wafer		,	0	} ,
	{ "CM2.C07_Wafer"				, _K_D_IO	,	CM2_C07_Wafer		,	0	} ,
	{ "CM2.C08_Wafer"				, _K_D_IO	,	CM2_C08_Wafer		,	0	} ,
	{ "CM2.C09_Wafer"				, _K_D_IO	,	CM2_C09_Wafer		,	0	} ,
	{ "CM2.C10_Wafer"				, _K_D_IO	,	CM2_C10_Wafer		,	0	} ,
	{ "CM2.C11_Wafer"				, _K_D_IO	,	CM2_C11_Wafer		,	0	} ,
	{ "CM2.C12_Wafer"				, _K_D_IO	,	CM2_C12_Wafer		,	0	} ,
	{ "CM2.C13_Wafer"				, _K_D_IO	,	CM2_C13_Wafer		,	0	} ,
	{ "CM2.C14_Wafer"				, _K_D_IO	,	CM2_C14_Wafer		,	0	} ,
	{ "CM2.C15_Wafer"				, _K_D_IO	,	CM2_C15_Wafer		,	0	} ,
	{ "CM2.C16_Wafer"				, _K_D_IO	,	CM2_C16_Wafer		,	0	} ,
	{ "CM2.C17_Wafer"				, _K_D_IO	,	CM2_C17_Wafer		,	0	} ,
	{ "CM2.C18_Wafer"				, _K_D_IO	,	CM2_C18_Wafer		,	0	} ,
	{ "CM2.C19_Wafer"				, _K_D_IO	,	CM2_C19_Wafer		,	0	} ,
	{ "CM2.C20_Wafer"				, _K_D_IO	,	CM2_C20_Wafer		,	0	} ,
	{ "CM2.C21_Wafer"				, _K_D_IO	,	CM2_C21_Wafer		,	0	} ,
	{ "CM2.C22_Wafer"				, _K_D_IO	,	CM2_C22_Wafer		,	0	} ,
	{ "CM2.C23_Wafer"				, _K_D_IO	,	CM2_C23_Wafer		,	0	} ,
	{ "CM2.C24_Wafer"				, _K_D_IO	,	CM2_C24_Wafer		,	0	} ,
	{ "CM2.C25_Wafer"				, _K_D_IO	,	CM2_C25_Wafer		,	0	} ,
	
	{ "CM3.C01_Wafer"				, _K_D_IO	,	CM3_C01_Wafer		,	0	} ,
	{ "CM3.C02_Wafer"				, _K_D_IO	,	CM3_C02_Wafer		,	0	} ,
	{ "CM3.C03_Wafer"				, _K_D_IO	,	CM3_C03_Wafer		,	0	} ,
	{ "CM3.C04_Wafer"				, _K_D_IO	,	CM3_C04_Wafer		,	0	} ,
	{ "CM3.C05_Wafer"				, _K_D_IO	,	CM3_C05_Wafer		,	0	} ,
	{ "CM3.C06_Wafer"				, _K_D_IO	,	CM3_C06_Wafer		,	0	} ,
	{ "CM3.C07_Wafer"				, _K_D_IO	,	CM3_C07_Wafer		,	0	} ,
	{ "CM3.C08_Wafer"				, _K_D_IO	,	CM3_C08_Wafer		,	0	} ,
	{ "CM3.C09_Wafer"				, _K_D_IO	,	CM3_C09_Wafer		,	0	} ,
	{ "CM3.C10_Wafer"				, _K_D_IO	,	CM3_C10_Wafer		,	0	} ,
	{ "CM3.C11_Wafer"				, _K_D_IO	,	CM3_C11_Wafer		,	0	} ,
	{ "CM3.C12_Wafer"				, _K_D_IO	,	CM3_C12_Wafer		,	0	} ,
	{ "CM3.C13_Wafer"				, _K_D_IO	,	CM3_C13_Wafer		,	0	} ,
	{ "CM3.C14_Wafer"				, _K_D_IO	,	CM3_C14_Wafer		,	0	} ,
	{ "CM3.C15_Wafer"				, _K_D_IO	,	CM3_C15_Wafer		,	0	} ,
	{ "CM3.C16_Wafer"				, _K_D_IO	,	CM3_C16_Wafer		,	0	} ,
	{ "CM3.C17_Wafer"				, _K_D_IO	,	CM3_C17_Wafer		,	0	} ,
	{ "CM3.C18_Wafer"				, _K_D_IO	,	CM3_C18_Wafer		,	0	} ,
	{ "CM3.C19_Wafer"				, _K_D_IO	,	CM3_C19_Wafer		,	0	} ,
	{ "CM3.C20_Wafer"				, _K_D_IO	,	CM3_C20_Wafer		,	0	} ,
	{ "CM3.C21_Wafer"				, _K_D_IO	,	CM3_C21_Wafer		,	0	} ,
	{ "CM3.C22_Wafer"				, _K_D_IO	,	CM3_C22_Wafer		,	0	} ,
	{ "CM3.C23_Wafer"				, _K_D_IO	,	CM3_C23_Wafer		,	0	} ,
	{ "CM3.C24_Wafer"				, _K_D_IO	,	CM3_C24_Wafer		,	0	} ,
	{ "CM3.C25_Wafer"				, _K_D_IO	,	CM3_C25_Wafer		,	0	} ,

	{ "CTC.FA_STATUS_AGV_CM1"		, _K_D_IO	,	AGV_MODE_CM1        ,	0   } ,  
    { "CTC.FA_STATUS_AGV_CM2" 		, _K_D_IO	,	AGV_MODE_CM2        ,	0   } ,  
    { "CTC.FA_STATUS_AGV_CM3"		, _K_D_IO	,	AGV_MODE_CM3        ,	0   } ,

//    { "CTC.OPERATOR_START"			, _K_D_IO	,	OPERATOR_START      ,	0   } ,  
//    { "CTC.OPERATOR_START2"			, _K_D_IO	,	OPERATOR_START2     ,	0   } ,  
//    { "CTC.OPERATOR_START3"			, _K_D_IO	,	OPERATOR_START3     ,	0   } , 

    { "PRM_FMCM1_WFRCHECK"			, _K_D_IO	,	PRM_FMCM1_WFRCHECK  ,	0   } ,  
    { "PRM_FMCM2_WFRCHECK"			, _K_D_IO	,	PRM_FMCM2_WFRCHECK  ,	0   } ,  
    { "PRM_FMCM3_WFRCHECK"			, _K_D_IO	,	PRM_FMCM3_WFRCHECK  ,	0   } , 

	{ "CTC.TARGET_WFR_COUNT"		, _K_D_IO	,	Wafer_Count_LPMA	,	0	} ,
	{ "CTC.TARGET_WFR_COUNT2"		, _K_D_IO	,	Wafer_Count_LPMB	,	0	} ,
	{ "CTC.TARGET_WFR_COUNT3"		, _K_D_IO	,	Wafer_Count_LPMC	,	0	} ,

	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Port1_Wafer_Processed_Count_Check() {

	int nCM1_WaferProcessedCount, nCM1_WaferMapCount, i, nCommStatus, nCM1_WaferTargetCount;
	char szAlmDec[256] = {0,};
	char szAlmPst[256] = {0,};
	
	if ( ON == READ_DIGITAL(PRM_FMCM1_WFRCHECK, &nCommStatus)) {
		nCM1_WaferProcessedCount = 0;
		nCM1_WaferTargetCount = READ_DIGITAL( Wafer_Count_LPMA, &nCommStatus);
		nCM1_WaferMapCount = READ_DIGITAL( CM1_MAPWAFER_COUNT, &nCommStatus );
		for( i=CM1_C01_Wafer; i<=CM1_C25_Wafer; i++ ) {
			if( 3 == READ_DIGITAL( i, &nCommStatus ) ) {
				nCM1_WaferProcessedCount = nCM1_WaferProcessedCount + 1;
			}
		}
		
		if( nCM1_WaferTargetCount != nCM1_WaferProcessedCount ) {
			sprintf(szAlmDec,"[Order Wfr %d][Processed Wfr %d]",nCM1_WaferTargetCount,nCM1_WaferProcessedCount);
			ALARM_MESSAGE_GET( PORT1_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM , szAlmPst );
			strcat( szAlmPst , szAlmDec );
			ALARM_MESSAGE_SET( PORT1_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM , szAlmPst );
			if ( READ_DIGITAL( AGV_MODE_CM1, &nCommStatus ) == Auto )
			{
				ALARM_POST(PORT1_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM);
				WRITE_DIGITAL( AGV_MODE_CM1, Manual, &nCommStatus );
				return SYS_SUCCESS;
			}
			else {
				ALARM_POST(PORT1_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM);
				return SYS_SUCCESS;
			}
		}
		else {
			return SYS_SUCCESS;
		}
	}
	else {
		return SYS_SUCCESS;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Port2_Wafer_Processed_Count_Check() {

	int nCM2_WaferProcessedCount, nCM2_WaferMapCount, i, nCommStatus, nCM2_WaferTargetCount;
	char szAlmDec[256] = {0,};
	char szAlmPst[256] = {0,};
	
	if ( ON == READ_DIGITAL(PRM_FMCM2_WFRCHECK, &nCommStatus)) {
		nCM2_WaferProcessedCount = 0;
		nCM2_WaferTargetCount = READ_DIGITAL( Wafer_Count_LPMB, &nCommStatus);
		nCM2_WaferMapCount = READ_DIGITAL( CM2_MAPWAFER_COUNT, &nCommStatus );
		for( i=CM2_C01_Wafer; i<=CM2_C25_Wafer; i++ ) {
			if( 3 == READ_DIGITAL( i, &nCommStatus ) ) {
				nCM2_WaferProcessedCount = nCM2_WaferProcessedCount + 1;
			}
		}		
		if( nCM2_WaferTargetCount != nCM2_WaferProcessedCount ) {
			sprintf(szAlmDec,"[Order Wfr %d][Processed Wfr %d]",nCM2_WaferTargetCount,nCM2_WaferProcessedCount);
			ALARM_MESSAGE_GET( PORT2_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM , szAlmPst );
			strcat( szAlmPst , szAlmDec );
			ALARM_MESSAGE_SET( PORT2_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM , szAlmPst );
			if ( READ_DIGITAL( AGV_MODE_CM2, &nCommStatus ) == Auto )
			{
				ALARM_POST(PORT2_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM);
				WRITE_DIGITAL( AGV_MODE_CM2, Manual, &nCommStatus );
				return SYS_SUCCESS;
			}
			else {
				ALARM_POST(PORT2_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM);
				return SYS_SUCCESS;
			}
		}
		else {
			return SYS_SUCCESS;
		}
	}
	else {
		return SYS_SUCCESS;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Port3_Wafer_Processed_Count_Check() {

	int nCM3_WaferProcessedCount, nCM3_WaferMapCount, i, nCommStatus, nCM3_WaferTargetCount;
	char szAlmDec[256] = {0,};
	char szAlmPst[256] = {0,};
	
	if ( ON == READ_DIGITAL(PRM_FMCM3_WFRCHECK, &nCommStatus)) {
		nCM3_WaferProcessedCount = 0;
		nCM3_WaferTargetCount = READ_DIGITAL( Wafer_Count_LPMC, &nCommStatus);
		nCM3_WaferMapCount = READ_DIGITAL( CM3_MAPWAFER_COUNT, &nCommStatus );
		for( i=CM3_C01_Wafer; i<=CM3_C25_Wafer; i++ ) {
			if( 3 == READ_DIGITAL( i, &nCommStatus ) ) {
				nCM3_WaferProcessedCount = nCM3_WaferProcessedCount + 1;
			}
		}
		
		if( nCM3_WaferTargetCount != nCM3_WaferProcessedCount ) {
			sprintf(szAlmDec,"[Order Wfr %d][Processed Wfr %d]",nCM3_WaferTargetCount,nCM3_WaferProcessedCount);
			ALARM_MESSAGE_GET( PORT3_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM , szAlmPst );
			strcat( szAlmPst , szAlmDec );
			ALARM_MESSAGE_SET( PORT3_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM , szAlmPst );
			if ( READ_DIGITAL( AGV_MODE_CM3, &nCommStatus ) == Auto )
			{
				ALARM_POST(PORT3_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM);
				WRITE_DIGITAL( AGV_MODE_CM3, Manual, &nCommStatus );
				return SYS_SUCCESS;
			}
			else {
				ALARM_POST(PORT3_WAFER_PROCESSED_MAP_COUNT_DIFFERENT_ALARM);
				return SYS_SUCCESS;
			}
		}
		else {
			return SYS_SUCCESS;
		}
	}
	else {
		return SYS_SUCCESS;
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {

	char szBuffer[128], szEventID[128], szPortID[128];

	memset(szBuffer,		0,		sizeof(szBuffer));
	memset(szEventID,		0,		sizeof(szEventID));
	memset(szPortID,		0,		sizeof(szPortID));

	STR_SEPERATE_CHAR( PROGRAM_PARAMETER_READ(), ' ', szEventID, szBuffer, 127 );
	STR_SEPERATE_CHAR( szBuffer, '|', szPortID, szBuffer, 127 );

	if( STRCMP_L("PORT1", szPortID) || STRCMP_L("CM1", szPortID)) {
		return Port1_Wafer_Processed_Count_Check();
	}
	else if( STRCMP_L("PORT2", szPortID) || STRCMP_L("CM2", szPortID)) {
		return Port2_Wafer_Processed_Count_Check();
	}
	else if( STRCMP_L("PORT3", szPortID) || STRCMP_L("CM3", szPortID)) {
		return Port3_Wafer_Processed_Count_Check();
	}
	else {
		return SYS_SUCCESS ;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
