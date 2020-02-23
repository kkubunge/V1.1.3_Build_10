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

#include "TmcMonitor.h"
#include "TextLogDll.h"

//------------------------------------------------------------------------------------------
// 2006.03.21
#include "PersistIO.h"
CPersistIO *g_pPersistIO = NULL;

//--------------------------------------------------------------------
#ifndef _DEBUG
// #define _DEBUG
#endif

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding",	_K_D_IO,	IO_DRIVER_LOAD,		0 } ,
	{ "Scheduler",			_K_F_IO,	SCHEDULER,			0 } ,
	{"PREVENTIVE_PM1",		_K_F_IO,	PREVENTIVE_PM1,		0 } ,
	{"PREVENTIVE_PM2",		_K_F_IO,	PREVENTIVE_PM2,		0 } ,
	{"PREVENTIVE_PM3",		_K_F_IO,	PREVENTIVE_PM3,		0 } ,
	{"PREVENTIVE_PM4",		_K_F_IO,	PREVENTIVE_PM4,		0 } ,
	{"PREVENTIVE_PM5",		_K_F_IO,	PREVENTIVE_PM5,		0 } ,

	""
};

//------------------------------------------------------------------------------------------

CTmcMonitor *g_pTmcMonitor = NULL;

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
	g_pTmcMonitor->Initialize();
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {

	g_pTmcMonitor = new CTmcMonitor;					// Default
	printf("TMC-ControlMonitor[Type1] Created\n");

	SYSTEM_KILL_WITH_MESSAGE();
	
	g_pPersistIO = new CPersistIO();
	if(g_pPersistIO != NULL)
	{
		#ifdef _DEBUG
		printf("-----> PersistIO Is Created\n");
		#endif
	}
	else printf("-----> PersistIO Creation is failed !\n");
	
	//------------------------------------------------------
	// Create thread after constructor proceed.
	if(NULL != g_pTmcMonitor)		g_pTmcMonitor->Initialize();
	if(NULL != g_pPersistIO)		g_pPersistIO->Initialize();
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		//------------------------------------------------------
		// 2006.03.21
		// Deinitialize
		 g_pPersistIO->Deinitialize();
		 delete g_pPersistIO;
		#ifdef _DEBUG
		printf("-----> PersistIO Deinit completed\n");
		#endif
		//------------------------------------------------------

		if(g_pTmcMonitor != NULL) delete g_pTmcMonitor;
		printf("TMC-ControlMonitor Destroyed\n");
		_gCLOSE_LOG();
	}
}

} // End of [extern "C"]
//==========================================================================================
