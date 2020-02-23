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

#include "SeqInterface.h"
#include "EventLogClient.h"							// 2013.06.03

#define FUNC_NAME		"EventLogClient"
CEventLogClient g_SeqInterface;						// 2013.06.03
// CSeqInterface g_SeqInterface("SeqInterf");

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding,	0	} ,
	{ "MARS_PMNameSM"			, _K_S_IO	,	MARS_PMNameSM,		0	} ,
	""
};

//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void Event_Message_Received() {
	// printf("-----> Event Msg Received\n");
	g_SeqInterface.Event_Message_Received();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	if ( g_SeqInterface.m_bSimulMode ) return g_SeqInterface.Sim_Main();
	else return g_SeqInterface.Main();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	g_SeqInterface.Enter_Code();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	g_SeqInterface.m_bSimulMode = TRUE;
	else														g_SeqInterface.m_bSimulMode = FALSE;
	return g_SeqInterface.CSeqInterface::Initialize(FUNC_NAME);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		g_SeqInterface.Deinitialize();
	}
}

} // End of [extern "C"]
//==========================================================================================
