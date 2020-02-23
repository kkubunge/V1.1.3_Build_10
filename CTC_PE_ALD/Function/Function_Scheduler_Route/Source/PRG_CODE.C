#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
#include <KutlGui.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "Scheduler"				, _K_F_IO	,	SCHEDULER	,	0	} ,
	{ "START_CONTROL_OPTION"	, _K_D_IO	,	STARTOPTION	,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int Option , CommStatus;
	char Buffer[256];
	Option = READ_DIGITAL( STARTOPTION , &CommStatus );
	if		( Option == 0 ) sprintf( Buffer , "SET_CONFIG CTC|START_OPTION|1" ); // concurrent
	else if ( Option == 1 ) sprintf( Buffer , "SET_CONFIG CTC|START_OPTION|2" ); // continue
	else sprintf( Buffer , "SET_CONFIG CTC|START_OPTION|3" ); // sequential
	WRITE_FUNCTION_EVENT( SCHEDULER , Buffer );
	_sleep(100);
	WRITE_FUNCTION_EVENT( SCHEDULER , PROGRAM_PARAMETER_READ() );
	_sleep(100);

	// Always Continue
	//WRITE_DIGITAL( STARTOPTION , 1 , &CommStatus ); // 1 = continue

	// Set By User Option //////////////////////////////////////////////
	if(Option < 0 || Option > 2)
		WRITE_DIGITAL( STARTOPTION , 1 , &CommStatus ); // 1 = continue
	else
		WRITE_DIGITAL( STARTOPTION , Option , &CommStatus );
	// Set By User Option //////////////////////////////////////////////

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
