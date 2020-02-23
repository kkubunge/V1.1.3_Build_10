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

#include "TextParser.h"

#include "InterlockMonitorTM.h"
#include "InterlockMonitorPM.h"
CInterlockMonitor *g_pInterlockMonitor = NULL;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE	= FALSE;

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	""
};


//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	if( NULL == g_pInterlockMonitor ) return SYS_ABORTED;

	if ( SIMULATION_MODE ) return g_pInterlockMonitor->Sim_Main();
	return g_pInterlockMonitor->Real_Main();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	char szParam[256];
	char szLog[80];
	CTextParser txtP;

	strcpy(szParam, PROGRAM_ARGUMENT_READ());
	txtP.SetText(szParam);
	txtP.GetWord();
	if(txtP.BeEqual("TM"))
	{
		g_pInterlockMonitor = new CInterlockMonitorTM;
		strcpy(szLog, "CInterlockMonitorTM Created");
	}
	else if(txtP.BeEqual("PM"))
	{
		g_pInterlockMonitor = new CInterlockMonitorPM;
		strcpy(szLog, "CInterlockMonitorPM Created");
	}
	else
	{
		g_pInterlockMonitor = new CInterlockMonitorTM;
		strcpy(szLog, "CInterlockMonitorTM Created");
	}

	if( NULL == g_pInterlockMonitor ) return;
	g_pInterlockMonitor->LOG("------------------------------------------------------------");
	g_pInterlockMonitor->LOG(szLog);
	g_pInterlockMonitor->Enter_Code();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;

	// Checking System Exit
	SYSTEM_KILL_WITH_MESSAGE();

	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;

	if( NULL == g_pInterlockMonitor ) return FALSE;
	return g_pInterlockMonitor->Initialize();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		if( NULL != g_pInterlockMonitor ) g_pInterlockMonitor->Deinitialize();
	}
}

} // End of [extern "C"]
//==========================================================================================
