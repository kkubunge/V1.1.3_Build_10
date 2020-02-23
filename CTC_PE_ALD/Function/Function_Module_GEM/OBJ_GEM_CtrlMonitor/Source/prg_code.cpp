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
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "GemMonitor.h"
#include "TextLogDll.h"

//--------------------------------------------------------------------
#ifndef _DEBUG
// #define _DEBUG
#endif

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"			, _K_D_IO	,	IO_DRIVER_LOAD,			 0 } ,

	{ "PM1.PRO_ChamberStatus"		, _K_D_IO	,   PM1_PRO_CHAMBER_STATUS,  0 } , 
	{ "PM2.PRO_ChamberStatus"		, _K_D_IO	,   PM2_PRO_CHAMBER_STATUS,  0 } ,
	{ "PM3.PRO_ChamberStatus"		, _K_D_IO	,   PM3_PRO_CHAMBER_STATUS,  0 } , 
	
	{ "CTC.PM1_CommStatus"			, _K_D_IO	,  	PM1_CommStatus,			 0 } ,
	{ "CTC.PM2_CommStatus"			, _K_D_IO	,   PM2_CommStatus,			 0 } ,
	{ "CTC.PM3_CommStatus"			, _K_D_IO	,   PM3_CommStatus,			 0 } , 
	
	{ "e:SCHEDULER"					, _K_S_IO	,	SCHEDULER,				 0 } ,
	""
};

//------------------------------------------------------------------------------------------

CGemMonitor *g_pGemMonitor = NULL;

//==========================================================================================
extern "C"
{

Module_Status Program_Main()
{	
	// printf( "%s Entered\n", APP_NAME);
	do {

	} while(0);
	// printf( "%s Leaved\n", APP_NAME);
	return SYS_SUCCESS;	
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	//g_pGemMonitor->Initialize();
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	//char szParam[256];
	//strcpy(szParam, PROGRAM_ARGUMENT_READ());

	g_pGemMonitor = new CGemMonitor;

	printf("GEM-ControlMonitor Created\n");

	//-----------------------------------------------------	
	// Create thread after constructor proceed.
	if(NULL != g_pGemMonitor)
	{
		g_pGemMonitor->Initialize();
	}	

}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		if(g_pGemMonitor != NULL) delete g_pGemMonitor;
		printf("GEM-ControlMonitor Destroyed\n");
		_gCLOSE_LOG();
	}
}

} // End of [extern "C"]
//==========================================================================================
