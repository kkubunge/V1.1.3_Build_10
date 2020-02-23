
#include "stdafx.h"
#include "TextParser.h"
#include "TextLogDll.h"
#include "DummyMonitor.h"

CDummyMonitor* g_pDummyMonitor = NULL;

#define   DUMMY_WARNING_COUNT_ALARM     40
#define   DUMMY_ALARM_COUNT_ALARM       70
#define	  NO_VALID_DUMMY_WAFER_ALARM	99
//------------------------------------------------------------------------------------------
// Initialize Function
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "e:SCHEDULER"				, _K_S_IO  ,	SCHEDULER		        	,	0	} ,
	{ "CTC.MAIN_CONTROL"		, _K_D_IO  ,	MAIN_CONTROL				,	0	} ,
	{ "CTC.MAIN_CONTROL2"		, _K_D_IO  ,	MAIN_CONTROL2				,	0	} ,
	{ "CTC.MAIN_CONTROL3"		, _K_D_IO  ,	MAIN_CONTROL3				,	0	} , //2007.12.06 CM3 no dummy chamber anymore mgsong...
  //{ "PRM_FMCM3_USE_MODE"		, _K_D_IO  ,	DUMMY_USE_MODE				,	0	} ,
  //{ "LPMC.F_ProperlyPlaceDI"	, _K_D_IO  ,	DUMMY_CASSETTE_STATUS		,	0	} ,

	{ "CTC.DummyPrcs_1_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count1	,	0	} ,
	{ "CTC.DummyPrcs_2_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count2	,	0	} ,
	{ "CTC.DummyPrcs_3_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count3	,	0	} ,
	{ "CTC.DummyPrcs_4_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count4	,	0	} ,
	{ "CTC.DummyPrcs_5_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count5	,	0	} ,
	{ "CTC.DummyPrcs_6_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count6	,	0	} ,
	{ "CTC.DummyPrcs_7_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count7	,	0	} ,
	{ "CTC.DummyPrcs_8_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count8	,	0	} ,
	{ "CTC.DummyPrcs_9_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count9	,	0	} ,
	{ "CTC.DummyPrcs_10_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count10	,	0	} ,
	{ "CTC.DummyPrcs_11_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count11	,	0	} ,
	{ "CTC.DummyPrcs_12_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count12	,	0	} ,
	{ "CTC.DummyPrcs_13_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count13	,	0	} ,
	{ "CTC.DummyPrcs_14_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count14	,	0	} ,
	{ "CTC.DummyPrcs_15_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count15	,	0	} ,
	{ "CTC.DummyPrcs_16_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count16	,	0	} ,
	{ "CTC.DummyPrcs_17_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count17	,	0	} ,
	{ "CTC.DummyPrcs_18_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count18	,	0	} ,
	{ "CTC.DummyPrcs_19_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count19	,	0	} ,
	{ "CTC.DummyPrcs_20_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count20	,	0	} ,
	{ "CTC.DummyPrcs_21_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count21	,	0	} ,
	{ "CTC.DummyPrcs_22_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count22	,	0	} ,
	{ "CTC.DummyPrcs_23_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count23	,	0	} ,
	{ "CTC.DummyPrcs_24_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count24	,	0	} ,
	{ "CTC.DummyPrcs_25_Count"	, _K_D_IO  ,	Current_Dummy_Prcs_Count25	,	0	} ,

	{ "CTC.DumPrcs_01_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count1	,	0	} ,
	{ "CTC.DumPrcs_02_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count2	,	0	} ,
	{ "CTC.DumPrcs_03_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count3	,	0	} ,
	{ "CTC.DumPrcs_04_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count4	,	0	} ,
	{ "CTC.DumPrcs_05_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count5	,	0	} ,
	{ "CTC.DumPrcs_06_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count6	,	0	} ,
	{ "CTC.DumPrcs_07_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count7	,	0	} ,
	{ "CTC.DumPrcs_08_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count8	,	0	} ,
	{ "CTC.DumPrcs_09_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count9	,	0	} ,
	{ "CTC.DumPrcs_10_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count10	,	0	} ,
	{ "CTC.DumPrcs_11_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count11	,	0	} ,
	{ "CTC.DumPrcs_12_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count12	,	0	} ,
	{ "CTC.DumPrcs_13_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count13	,	0	} ,
	{ "CTC.DumPrcs_14_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count14	,	0	} ,
	{ "CTC.DumPrcs_15_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count15	,	0	} ,
	{ "CTC.DumPrcs_16_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count16	,	0	} ,
	{ "CTC.DumPrcs_17_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count17	,	0	} ,
	{ "CTC.DumPrcs_18_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count18	,	0	} ,
	{ "CTC.DumPrcs_19_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count19	,	0	} ,
	{ "CTC.DumPrcs_20_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count20	,	0	} ,
	{ "CTC.DumPrcs_21_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count21	,	0	} ,
	{ "CTC.DumPrcs_22_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count22	,	0	} ,
	{ "CTC.DumPrcs_23_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count23	,	0	} ,
	{ "CTC.DumPrcs_24_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count24	,	0	} ,
	{ "CTC.DumPrcs_25_WCnt"  	, _K_D_IO  ,	Dummy_Used_Warning_Count25	,	0	} ,

	{ "CTC.DumPrcs_01_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count1  	,	0	} ,
	{ "CTC.DumPrcs_02_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count2  	,	0	} ,
	{ "CTC.DumPrcs_03_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count3  	,	0	} ,
	{ "CTC.DumPrcs_04_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count4  	,	0	} ,
	{ "CTC.DumPrcs_05_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count5  	,	0	} ,
	{ "CTC.DumPrcs_06_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count6  	,	0	} ,
	{ "CTC.DumPrcs_07_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count7  	,	0	} ,
	{ "CTC.DumPrcs_08_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count8  	,	0	} ,
	{ "CTC.DumPrcs_09_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count9  	,	0	} ,
	{ "CTC.DumPrcs_10_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count10  	,	0	} ,
	{ "CTC.DumPrcs_11_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count11  	,	0	} ,
	{ "CTC.DumPrcs_12_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count12  	,	0	} ,
	{ "CTC.DumPrcs_13_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count13  	,	0	} ,
	{ "CTC.DumPrcs_14_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count14  	,	0	} ,
	{ "CTC.DumPrcs_15_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count15  	,	0	} ,
	{ "CTC.DumPrcs_16_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count16  	,	0	} ,
	{ "CTC.DumPrcs_17_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count17  	,	0	} ,
	{ "CTC.DumPrcs_18_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count18  	,	0	} ,
	{ "CTC.DumPrcs_19_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count19  	,	0	} ,
	{ "CTC.DumPrcs_20_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count20  	,	0	} ,
	{ "CTC.DumPrcs_21_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count21  	,	0	} ,
	{ "CTC.DumPrcs_22_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count22  	,	0	} ,
	{ "CTC.DumPrcs_23_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count23  	,	0	} ,
	{ "CTC.DumPrcs_24_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count24  	,	0	} ,
	{ "CTC.DumPrcs_25_ACnt"  	, _K_D_IO  ,	Dummy_Used_Alarm_Count25  	,	0	} ,

	{ "CTC.DummyPrcs_1_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode1        	,	0	} ,
	{ "CTC.DummyPrcs_2_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode2        	,	0	} ,
	{ "CTC.DummyPrcs_3_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode3        	,	0	} ,
	{ "CTC.DummyPrcs_4_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode4        	,	0	} ,
	{ "CTC.DummyPrcs_5_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode5        	,	0	} ,
	{ "CTC.DummyPrcs_6_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode6        	,	0	} ,
	{ "CTC.DummyPrcs_7_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode7        	,	0	} ,
	{ "CTC.DummyPrcs_8_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode8        	,	0	} ,
	{ "CTC.DummyPrcs_9_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode9        	,	0	} ,
	{ "CTC.DummyPrcs_10_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode10        	,	0	} ,
	{ "CTC.DummyPrcs_11_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode11        	,	0	} ,
	{ "CTC.DummyPrcs_12_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode12        	,	0	} ,
	{ "CTC.DummyPrcs_13_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode13        	,	0	} ,
	{ "CTC.DummyPrcs_14_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode14        	,	0	} ,
	{ "CTC.DummyPrcs_15_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode15        	,	0	} ,
	{ "CTC.DummyPrcs_16_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode16        	,	0	} ,
	{ "CTC.DummyPrcs_17_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode17        	,	0	} ,
	{ "CTC.DummyPrcs_18_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode18        	,	0	} ,
	{ "CTC.DummyPrcs_19_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode19        	,	0	} ,
	{ "CTC.DummyPrcs_20_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode20        	,	0	} ,
	{ "CTC.DummyPrcs_21_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode21        	,	0	} ,
	{ "CTC.DummyPrcs_22_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode22        	,	0	} ,
	{ "CTC.DummyPrcs_23_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode23        	,	0	} ,
	{ "CTC.DummyPrcs_24_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode24        	,	0	} ,
	{ "CTC.DummyPrcs_25_Mode"  	, _K_D_IO  ,	Dummy_Used_Mode25        	,	0	} ,
		""
};


extern "C" void Screen_Nav_Info( int no )
{
	//Class destruction here
	if (no == -1) {
		if (NULL != g_pDummyMonitor)
			delete g_pDummyMonitor;
	}
}

extern "C" BOOL Program_Init_Code()
{
	//Class construct & Initialize here
	g_pDummyMonitor = new CDummyMonitor();
	if (NULL != g_pDummyMonitor) {
		if (FALSE == g_pDummyMonitor->InitializeMonitor(DUMMY_ALARM_COUNT_ALARM , DUMMY_WARNING_COUNT_ALARM))
			printf ("Dummy Monitor Initialize Fail...\n");
	} else {
		printf ("Dummy Monitor Create Fail...\n");
	}	
	
	return TRUE;
}

extern "C" void Event_Message_Received()
{
	char* msgEvent;
	char  szSlot[32] , szMode[32] , TempStr[32];
	int   nSlot , nMode;
	
	msgEvent = PROGRAM_EVENT_READ();
	STR_SEPERATE(msgEvent , szSlot  , TempStr , 32);
	STR_SEPERATE(TempStr  , szMode  , TempStr , 32);

	nSlot = atoi(szSlot);
	nMode = atoi(szMode);

	printf ("Dummy Mode Change Event Slot %d Mode %d Received \n" , nSlot , nMode);
	if (NULL != g_pDummyMonitor)
		if (FALSE == g_pDummyMonitor->ChangeSlotDummyMode(nSlot , nMode))
			printf("Dummy Management ChangeSlotDummyMode Fail... \n");
}

extern "C" void Program_Enter_Code()
{
	//When Function Loaded by engine
	
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main()
{
	int AlmResult;
	Module_Status  enFuncResult = SYS_SUCCESS;
	char szParam[60];
	char szTemp [60];
	char szSlot [60];
	char szEtc  [60];
	
	STR_SEPERATE( PROGRAM_PARAMETER_READ() , szParam , szTemp , 60 );
	STR_SEPERATE( szTemp , szSlot , szEtc , 60 );
	printf("Dummy Management for %s Started \n" , szParam);
	
	if (STRCMP_L(szParam , "ALARM_RECOVERY")) {
		AlmResult = ALARM_MANAGE(NO_VALID_DUMMY_WAFER_ALARM);
		if      (AlmResult == ALM_RETRY) return SYS_SUCCESS;
		else if (AlmResult != ALM_RETRY) return SYS_ABORTED;
	} else if (STRCMP_L(szParam , "DUMMY_REMOVE")) {
		if (NULL != g_pDummyMonitor)
			g_pDummyMonitor->RemoveDummy();
	} else if (STRCMP_L(szParam , "RESET_COUNT")) {
		if (NULL != g_pDummyMonitor)
			g_pDummyMonitor->ResetCount(szSlot);
	}
	
	return SYS_SUCCESS;
}

