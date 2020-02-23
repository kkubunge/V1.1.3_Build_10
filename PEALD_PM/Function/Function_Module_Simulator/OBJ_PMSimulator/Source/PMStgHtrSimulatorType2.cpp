// StgHtrSimulatorType2.cpp: implementation of the CPMStgHtrSimulatorType2 class.
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

#include "TextParser.h"
#include "Definition.h"
#include "PMStgHtrSimulatorType2.h"

// #include "TextLogDll.h"
extern void _LOG(const char *pFormat, ...);

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	SHT_TmpSpAIO	,
	SHT_TmpRdAI		,
	SHT_TmpHgLmtAIO	,
	SHT_TmpLwLmtAIO	,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{ "HTA01_TmpSpAIO", _K_A_IO,	0	},
	{ "HTA01_TmpRdAI", _K_A_IO,		0	},
	{ "HTA01_TmpHgLmtAIO", _K_A_IO,	0	},
	{ "HTA01_TmpLwLmtAIO", _K_A_IO,	0	},
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPMStgHtrSimulatorType2::CPMStgHtrSimulatorType2()
{

}

CPMStgHtrSimulatorType2::~CPMStgHtrSimulatorType2()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CPMStgHtrSimulatorType2::Initialize()
{
	InitIOList(g_pIOList);
	CreateThread();
	return TRUE;
}
