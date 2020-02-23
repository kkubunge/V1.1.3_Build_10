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
#include "EventLogServer.h"							// 2013.06.03

#define FUNC_NAME		"EventLogServer"
CEventLogServer g_SeqInterface;						// 2013.06.03
// CSeqInterface g_SeqInterface("SeqInterf");

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding,	0	} ,

	{ "CTC.PM1_CommStatus",		_K_D_IO,	PM1_CommStatus,			0 },
	{ "CTC.PM2_CommStatus",		_K_D_IO,	PM2_CommStatus,			0 },
	{ "CTC.PM3_CommStatus",		_K_D_IO,	PM3_CommStatus,			0 },
	{ "CTC.PM4_CommStatus",		_K_D_IO,	PM4_CommStatus,			0 },
	{ "CTC.PM5_CommStatus",		_K_D_IO,	PM5_CommStatus,			0 },
	{ "CTC.MAIN_CONTROL",		_K_D_IO,	CTC_MAIN_CONTROL,		0 },
	{ "CTC.MAIN_CONTROL2",		_K_D_IO,	CTC_MAIN_CONTROL2,		0 },
	{ "CTC.MAIN_CONTROL3",		_K_D_IO,	CTC_MAIN_CONTROL3,		0 },
	{ "CTC.MAIN_CONTROL4",		_K_D_IO,	CTC_MAIN_CONTROL4,		0 },

	{ "CTC.RECIPE_FILE",		_K_S_IO,	CTC_RECIPE_FILE,		0 },
	{ "CTC.RECIPE_FILE2",		_K_S_IO,	CTC_RECIPE_FILE2,		0 },
	{ "CTC.RECIPE_FILE3",		_K_S_IO,	CTC_RECIPE_FILE3,		0 },
	{ "CTC.RECIPE_FILE4",		_K_S_IO,	CTC_RECIPE_FILE4,		0 },
	{ "CTC.MID_NAME",			_K_S_IO,	CTC_MID_NAME,			0 },
	{ "CTC.MID_NAME2",			_K_S_IO,	CTC_MID_NAME2,			0 },
	{ "CTC.MID_NAME3",			_K_S_IO,	CTC_MID_NAME3,			0 },
	{ "CTC.MID_NAME4",			_K_S_IO,	CTC_MID_NAME4,			0 },

	{ "CTC.JOB_NAME",			_K_S_IO,	CTC_JOB_NAME,			0 },
	{ "CTC.JOB_NAME2",			_K_S_IO,	CTC_JOB_NAME2,			0 },
	{ "CTC.JOB_NAME3",			_K_S_IO,	CTC_JOB_NAME3,			0 },
	{ "CTC.JOB_NAME4",			_K_S_IO,	CTC_JOB_NAME4,			0 },

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
