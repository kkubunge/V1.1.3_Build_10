// PMDryPumpSimulator.cpp: implementation of the CPMDryPumpSimulator class.
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
#include "PMDryPumpSimulator.h"

// #include "TextLogDll.h"
extern void _LOG(const char *pFormat, ...);

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	DPDO,	
	DPRunDI,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	// Dry Pump Simulation IO Channels
	{ "DPDO",				_K_D_IO,		0},
	{ "DPRunDI",			_K_D_IO,		0},
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPMDryPumpSimulator::CPMDryPumpSimulator()
{

}

CPMDryPumpSimulator::~CPMDryPumpSimulator()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CPMDryPumpSimulator::Initialize()
{
	InitIOList(g_pIOList);
	CreateThread();
	return TRUE;
}

void CPMDryPumpSimulator::DoMonitor()
{
	char pszLocFtnName[] = "CPMDryPumpSimulator";
	char pszFtnName[256];
	BOOL bRet = FALSE;
	int nIOStatus = 0;

	int nDPCommand	= DP_Start;
	int nDPStatus	= DP_OFF;
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		// Simulation Loop
		while(WaitSec(_DP_SCAN_INTERVAL))
		{
			nDPCommand	= dREAD_DIGITAL( DPDO, &nIOStatus);
			nDPStatus	= dREAD_DIGITAL( DPRunDI, &nIOStatus);
			if( DP_Start == nDPCommand && DP_OFF == nDPStatus )
			{
				_LOG( "DP On Start");
				WaitSec(_DP_SET_TIME);
				dWRITE_DIGITAL(DPRunDI, DP_ON, &nIOStatus);
				_LOG( "DP On Completed");
			}
			else if( DP_Stop == nDPCommand && DP_ON == nDPStatus )
			{
				_LOG( "DP Stop Start");
				WaitSec(_DP_SET_TIME);
				dWRITE_DIGITAL(DPRunDI, DP_OFF, &nIOStatus);
				_LOG( "DP Stop Completed");
			}
		}
		bRet = TRUE;
	}while(0);
	printf("Leaved %s\n", pszFtnName);	
}