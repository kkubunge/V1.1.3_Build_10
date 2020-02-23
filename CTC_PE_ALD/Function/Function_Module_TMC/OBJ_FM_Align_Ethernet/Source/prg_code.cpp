#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//==========================================================================================

#include "TextLogDll.h"

typedef enum {
	OC_ON,
	OC_OFF
}OCR_CTRL_ENUM;

#define ALARM_ALIGNER_SET_POST_POSITION  3305
BOOL SIMULATION_MODE = FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	{ "FM.Align_Post_Pos"		, _K_A_IO	,	POSITION			,	0	} ,
	//
	{ "PRMA_POSTPOS_BM1"		, _K_A_IO	,	PARM_POSITION_BM1	,	0	} ,
	{ "PRMA_POSTPOS_BM2"		, _K_A_IO	,	PARM_POSITION_BM2	,	0	} ,
	{ "PRMA_POSTPOS2_BM1"		, _K_A_IO	,	PARM_POSITION2_BM1	,	0	} ,
	{ "PRMA_POSTPOS2_BM2"		, _K_A_IO	,	PARM_POSITION2_BM2	,	0	} ,
	//
	{ "CTC_EVENT_LOG_SVR",		_K_F_IO,		CTC_EVENT_LOG_SVR,		0 } ,		// 2013.11.15
	""
};

//------------------------------------------------------------------------------------------
// 2005.12.20
#include "DynamicIO.h"
CDynamicIO g_DynIO;
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	CTC_AbortControl,
	TMSIM_ALIGNER,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"CTC.AbortControl",			_K_D_IO, 0},
	{"TMSIM_ALIGNER",				_K_F_IO, 0},
	""
};



//////////////////////////////////////////////////////////////////////
// Global Function
// 2013.11.15
extern void _MLOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);	
	WRITE_FUNCTION_EVENT(CTC_EVENT_LOG_SVR, _szString);
}

// 2013.11.15
void LogEvent(char *szModule, char *szRunStr, char *szStatus, char *szData)
{
	char szEventID[20];

	do {
		if(STRCMP_L(szRunStr , "Align")) strcpy(szEventID, "Align");
		else
		{
			_MLOG("-----> %s %s %s %s", szModule, szRunStr, szStatus, szData);
			break;
		}
		_MLOG("FNC %s %s %s %s", szModule, szEventID, szStatus, szData);
	} while(0);
}

//==========================================================================================
extern "C"
{
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status Program_Main() {
	Module_Status msRet = SYS_SUCCESS;
	int	   CommStatus ;
	char   RunStr[41] , TempStr[41] , TempStr2[41];
	char   szBM_ASideWfrYES[41], szBM_BSideWfrYES[41]; 
	char   *pParam;
	double dbPostPosBM1 , dbPostPosBM2;
	double dbPostPos2BM1 , dbPostPos2BM2;
	int nIsBM_ASideWfr, nIsBM_BSideWfr;
	
	char szLogData[80];								// 2013.11.15
	double dbNotchAngle = 0;						// 2013.11.15

	pParam = PROGRAM_PARAMETER_READ();
	STR_SEPERATE( pParam	, szBM_ASideWfrYES , TempStr	, 40 );//
	STR_SEPERATE( TempStr	, szBM_BSideWfrYES , TempStr2	, 40 );

	STR_SEPERATE( TempStr2	, RunStr , TempStr	, 40 );		// CM2
	STR_SEPERATE( TempStr	, RunStr , TempStr2	, 40 );		// 1
	STR_SEPERATE( TempStr2	, RunStr , TempStr	, 40 );		// BM1

	dbPostPosBM1 = READ_ANALOG(PARM_POSITION_BM1 , &CommStatus);
	dbPostPosBM2 = READ_ANALOG(PARM_POSITION_BM2 , &CommStatus);
	
	//2014.06.18
	dbPostPos2BM1 = READ_ANALOG(PARM_POSITION2_BM1 , &CommStatus);
	dbPostPos2BM2 = READ_ANALOG(PARM_POSITION2_BM2 , &CommStatus);

	nIsBM_ASideWfr = atoi(szBM_ASideWfrYES);
	nIsBM_BSideWfr = atoi(szBM_BSideWfrYES);
	//

	// 2013.11.15 
	if(STRCMP_L(RunStr, "BM1" ) ){
		//2014.06.18
		if(nIsBM_ASideWfr >0 ){               
			dbNotchAngle = dbPostPosBM1;
		}else if(nIsBM_BSideWfr > 0){
			dbNotchAngle = dbPostPos2BM1;
		}else{
			dbNotchAngle = 0;
		}
		//
	}
	else if ( STRCMP_L( RunStr , "BM2" ) ){
		//2014.06.18
		if(nIsBM_ASideWfr >0 ){
			dbNotchAngle = dbPostPosBM2;
		}else if(nIsBM_BSideWfr > 0){
			dbNotchAngle = dbPostPos2BM2;
		}else{
			dbNotchAngle = 0;
		}
		//
	}

	_LOG("-----> '%s' Started in FM Aligner Func", pParam);

	// 2013.11.15
	sprintf(szLogData, "('NotchAngle',%.2f)\t('Station','%s')", dbNotchAngle, RunStr);
	LogEvent("Aligner", "Align", "Start", szLogData);

	do {
		//never align here mgsong...			
		if ( STRCMP_L( RunStr , "BM1" ) ) {
			WRITE_ANALOG(POSITION , dbNotchAngle , &CommStatus );         //2014.06.25
			printf("^___^ Post Position Set BM1[%f]\n" , dbNotchAngle);

		}
		else if ( STRCMP_L( RunStr , "BM2" ) ) {
			WRITE_ANALOG(POSITION , dbNotchAngle , &CommStatus );		  //2014.06.25
			printf("^___^ Post Position Set BM2[%f]\n" , dbNotchAngle);
		}
		else if ( STRCMP_L( RunStr , "TRANSFER" ) )	{
			// Special Case for Manual Transfer Function Moving
			// Christmas Eve 2013 by hyuk
			WRITE_ANALOG(POSITION , dbPostPosBM1 , &CommStatus );
			printf("^___^ Post Position Set BM[%f]\n" , dbPostPosBM1);
		}


	} while(0);

	if(msRet != SYS_SUCCESS) _LOG("--------> FM Aligner Func Failed");
	LogEvent("Aligner", "Align", "End", "");	// 2013.11.15
	_LOG("-----> '%s' Finished in FM Aligner Func", pParam);
	return msRet;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus = 0;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;

	g_DynIO.InitIOList(g_pIOList, false);

	//delete post position set part

	return TRUE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		// Deinitialize
		_gCLOSE_LOG();
	}
}

} // End of [extern "C"]
//==========================================================================================
