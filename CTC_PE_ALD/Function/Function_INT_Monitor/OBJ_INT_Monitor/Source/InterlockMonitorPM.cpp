// InterlockMonitorPM.cpp: implementation of the CInterlockMonitorPM class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//====================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//====================================================================

#include "InterlockMonitorPM.h"

//--------------------------------------------------------------------
extern void _LOG(const char *pFormat, ...);

//--------------------------------------------------------------------
// 2005.12.20
// #include "DynamicIO.h"
// Dynamic IO Enum List Definition
static enum {
	PRO_RunStsDM,
	SW_GbIntlckDM,
	SW_ModuleFailDM,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	
	{"PRO_RunStsDM",		_K_D_IO, 0},
	{"SW_GbIntlckDM",		_K_D_IO, 0},
	{"SW_ModuleFailDM",		_K_D_IO, 0},
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInterlockMonitorPM::CInterlockMonitorPM()
{

}

CInterlockMonitorPM::~CInterlockMonitorPM()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

