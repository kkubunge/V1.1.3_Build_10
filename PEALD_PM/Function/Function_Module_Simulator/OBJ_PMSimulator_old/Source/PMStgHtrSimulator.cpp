// StgHtrSimulator.cpp: implementation of the CPMStgHtrSimulator class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

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
#include "PMStgHtrSimulator.h"

// #include "TextLogDll.h"
extern void _LOG(const char *pFormat, ...);

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	SHT1_TmpSpAIO	,
	SHT1_TmpSpAI	,
	SHT1_TmpRdAI		,
	SHT1_TmpHgLmtAIO	,
	SHT1_TmpLwLmtAIO	,

	SHT2_TmpSpAIO	,
	SHT2_TmpSpAI	,
	SHT2_TmpRdAI		,
	SHT2_TmpHgLmtAIO	,
	SHT2_TmpLwLmtAIO	,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{ "SHT1_TmpSpAIO", _K_A_IO,		0	},
	{ "SHT1_TmpSpAI", _K_A_IO,		0	},
	{ "SHT1_TmpRdAI", _K_A_IO,		0	},
	{ "SHT1_TmpHgLmtAIO", _K_A_IO,	0	},
	{ "SHT1_TmpLwLmtAIO", _K_A_IO,	0	},

	{ "SHT2_TmpSpAIO", _K_A_IO,		0	},
	{ "SHT2_TmpSpAI", _K_A_IO,		0	},
	{ "SHT2_TmpRdAI", _K_A_IO,		0	},
	{ "SHT2_TmpHgLmtAIO", _K_A_IO,	0	},
	{ "SHT2_TmpLwLmtAIO", _K_A_IO,	0	},
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPMStgHtrSimulator::CPMStgHtrSimulator()
{
	m_fStgHtrRamp = 1.3f;
	m_fStgHtrError = 0;
	m_bSimEnable = true;
}

CPMStgHtrSimulator::~CPMStgHtrSimulator()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CPMStgHtrSimulator::Initialize()
{
	InitIOList(g_pIOList);
	CreateThread();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void CPMStgHtrSimulator::DoMonitor()
{
	char pszLocFtnName[] = "CPMStgHtrSimulator";
	char pszFtnName[256];
	BOOL bRet = FALSE;
	int nIOStatus = 0;
	double dbSetTemp;
	double dbReadTemp;
	double dbWriteTemp;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	// StageHeater 1
	dWRITE_ANALOG(SHT1_TmpHgLmtAIO, 80, &nIOStatus);
	dWRITE_ANALOG(SHT1_TmpLwLmtAIO, 20, &nIOStatus);
	
	dbSetTemp = dREAD_ANALOG(SHT1_TmpSpAIO, &nIOStatus);
	if(dbSetTemp == 0)
	{
		dWRITE_ANALOG(SHT1_TmpRdAI, 50, &nIOStatus);
		dWRITE_ANALOG(SHT1_TmpSpAIO, 50, &nIOStatus);
		dWRITE_ANALOG(SHT1_TmpSpAI, 50, &nIOStatus);
	}
	else
	{
		dWRITE_ANALOG(SHT1_TmpRdAI, dbSetTemp+1, &nIOStatus);
		dWRITE_ANALOG(SHT1_TmpSpAI, dbSetTemp, &nIOStatus);
	}
	
	// StageHeater 2
	dWRITE_ANALOG(SHT2_TmpHgLmtAIO, 80, &nIOStatus);
	dWRITE_ANALOG(SHT2_TmpLwLmtAIO, 20, &nIOStatus);
	
	dbSetTemp = dREAD_ANALOG(SHT2_TmpSpAIO, &nIOStatus);
	if(dbSetTemp == 0)
	{
		dWRITE_ANALOG(SHT2_TmpRdAI, 50, &nIOStatus);
		dWRITE_ANALOG(SHT2_TmpSpAIO, 50, &nIOStatus);
		dWRITE_ANALOG(SHT2_TmpSpAI, 50, &nIOStatus);
	}
	else
	{
		dWRITE_ANALOG(SHT2_TmpRdAI, dbSetTemp+1, &nIOStatus);
		dWRITE_ANALOG(SHT2_TmpSpAI, dbSetTemp, &nIOStatus);
	}

	do
	{
		// Simulation Loop
		while(WaitSec(_STG_HTR_SCAN_INTERVAL))
		{
			if(! m_bSimEnable) continue;


			// StageHeater 1
			dbSetTemp = dREAD_ANALOG(SHT1_TmpSpAIO, &nIOStatus);
			dbReadTemp = dREAD_ANALOG(SHT1_TmpRdAI, &nIOStatus) - m_fStgHtrError;

			if(dbReadTemp > dbSetTemp)
			{
				dbWriteTemp = dbReadTemp - m_fStgHtrRamp;
			}
			else if(dbReadTemp < dbSetTemp)
			{
				dbWriteTemp = dbReadTemp + m_fStgHtrRamp;
			}
			dbWriteTemp += m_fStgHtrError;
			dWRITE_ANALOG(SHT1_TmpRdAI, dbWriteTemp, &nIOStatus);
			dWRITE_ANALOG(SHT1_TmpSpAI, dbSetTemp, &nIOStatus);
			// end

			// StageHeater 2
			dbSetTemp = dREAD_ANALOG(SHT2_TmpSpAIO, &nIOStatus);
			dbReadTemp = dREAD_ANALOG(SHT2_TmpRdAI, &nIOStatus) - m_fStgHtrError;

			if(dbReadTemp > dbSetTemp)
			{
				dbWriteTemp = dbReadTemp - m_fStgHtrRamp;
			}
			else if(dbReadTemp < dbSetTemp)
			{
				dbWriteTemp = dbReadTemp + m_fStgHtrRamp;
			}
			dbWriteTemp += m_fStgHtrError;
			dWRITE_ANALOG(SHT2_TmpRdAI, dbWriteTemp, &nIOStatus);
			dWRITE_ANALOG(SHT2_TmpSpAI, dbSetTemp, &nIOStatus);
			// end

			
		}

		bRet = TRUE;
	}while(0);

	printf("Leaved %s\n", pszFtnName);	
}

