#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlanal.h>

#include "resource.h"

#include "cimseqnc.h"
#include "iodefine.h"
#include "ctl_code.h"
#include "plt_code.h"
#include "dat_code.h"
//

//========================================================================================
//========================================================================================

CRITICAL_SECTION CR_READ_DATA; // 2016.09.06

void Program_Enter_Code() {
	//
	InitializeCriticalSection( &CR_READ_DATA ); // 2016.09.06
	//
}

void Event_Message_Received() {
}

