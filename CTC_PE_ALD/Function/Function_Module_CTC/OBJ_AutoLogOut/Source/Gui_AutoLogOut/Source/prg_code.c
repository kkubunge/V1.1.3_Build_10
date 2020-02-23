#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <Winbase.h>

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

extern HHOOK        hKHook;
extern HHOOK        hMHook;

enum	{ OFF_0	 ,	ON_1 } ;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "UserLevelControl"	, _K_D_IO	,	UserLevelControl		,	0	} ,
	{ "CurrentUserName"	    , _K_S_IO	,	CurrentUserName			,	0	} ,
	{ "CurrentGroupName"	, _K_S_IO	,	CurrentGroupName        ,	0	} ,
	{ "SCREEN_USER_CONTROL"	, _K_F_IO	,	UserControl				,	0	} ,
	{ "PRMA_AutoLogOut_TO"	, _K_D_IO	,	PRMA_AutoLogOut_TO		,	0	} ,
	""
};

// extern HHOOK	ghKeyHook;
extern __declspec(dllimport) int StopHook();


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status Program_Main() {

	return SYS_SUCCESS ;
}
