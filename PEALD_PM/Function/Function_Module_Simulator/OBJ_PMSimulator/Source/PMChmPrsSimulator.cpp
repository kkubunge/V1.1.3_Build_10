// PMChmPrsSimulator.cpp: implementation of the CPMChmPrsSimulator class.
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
#include "PMChmPrsSimulator.h"

// #include "TextLogDll.h"
extern void _LOG(const char *pFormat, ...);

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	M651_ModeDM,		
	M651_Set_Position,	
	M651_Set_Pressure,	
	M651_Position,		
	M651_Pressure,		
	M651_Valve,
	ChmPrsAI,			
	PumpPrsAI,			
	ChmPrssDI1,			
	BarIsoVlvDO,		
	PgSwIsoVlvDO,		
	CVGIsoVlvDO,		
	FastRoughVlvDO,	
	RoughValveDI,			// 2006.03.03
	DPRunDI,		
	Fixed_IO_Area,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{ "M651_ModeDM",		_K_D_IO,0},
	{ "M651_Set_Position",	_K_A_IO,0},
	{ "M651_Set_Pressure",	_K_A_IO,0},
	{ "M651_Position",		_K_A_IO,0},
	{ "M651_Pressure",		_K_A_IO,0},
	{ "M651_Valve",			_K_D_IO,0},
	{ "ChmPrsAI",			_K_A_IO,0},
	{ "PumpPrsAI",			_K_A_IO,0},
	{ "ChmPrssDI1",			_K_D_IO,0},
	{ "BarIsoVlvDO",		_K_D_IO,0},
	{ "PgSwIsoVlvDO",		_K_D_IO,0},
	{ "CVGIsoVlvDO",		_K_D_IO,0},
	{ "FastRoughVlvDO",		_K_D_IO,0},	
	{ "RoughValveDI",		_K_D_IO,0},			// 2006.03.03
	{ "DPRunDI",			_K_D_IO,0},
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPMChmPrsSimulator::CPMChmPrsSimulator()
{
	m_dblChmbPress = 1;
	m_dblDPPress = 1;
	m_bChm_SimEnable = true;
	m_dbTargetPress = _TV_BASE_PRESS;;
}

CPMChmPrsSimulator::~CPMChmPrsSimulator()
{

}

BOOL CPMChmPrsSimulator::Initialize()
{
	InitIOList(g_pIOList, true, Fixed_IO_Area);
	CreateThread();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CPMChmPrsSimulator::DoMonitor()
{
	char pszLocFtnName[] = "CPMChmPrsSimulator";
	char pszFtnName[256];
	BOOL bRet = FALSE;
	int nIOStatus = 0;

	int		i			= 0;
	double	fSetPoint	= 0;
	double	fRead		= 0;
	double	fDiff		= 0;
	int		nTVControl	= TV_POSITION;

	int		nBaratronIsoValve	= VClose;
	int		nPressSenseIsoValve	= VClose;
	int		nCVGIsoValve		= VClose;
	int		nFastRoughValve		= VClose;
	int		nDPRunDI			= DP_OFF;
	int		nM651Commmand		= TV_HOLD;
	int		nM651Status			= TV_HOLD;
	double  dbNoise = 0;
	double  dbNoiseSign = 1;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		// Simulation Loop
		while(WaitSec(_TV_SCAN_INTERVAL))
		{
			if(! m_bChm_SimEnable) continue;

			// 2004.12.15 by cskim
			dbNoiseSign *= -1;
			if(m_dblChmbPress > 500) dbNoise = 1.537;
			else if(m_dblChmbPress > 100) dbNoise = 0.013;
			else dbNoise = 0.0005;

			// 2004.04.17 by cskim
			// if valve is opened. set result pressure.
			nBaratronIsoValve = dREAD_DIGITAL(BarIsoVlvDO, &nIOStatus);
			nPressSenseIsoValve = dREAD_DIGITAL(PgSwIsoVlvDO, &nIOStatus);
			nCVGIsoValve = dREAD_DIGITAL(CVGIsoVlvDO, &nIOStatus);
			
			nFastRoughValve = dREAD_DIGITAL(FastRoughVlvDO, &nIOStatus);
			// 2006.03.03
			if(nFastRoughValve == VOpen) dWRITE_DIGITAL(RoughValveDI, 1, &nIOStatus);
			else dWRITE_DIGITAL(RoughValveDI, 2, &nIOStatus);

			// Update Chamber Pressure
			dWRITE_ANALOG(ChmPrsAI, m_dblChmbPress + dbNoise * dbNoiseSign, &nIOStatus);
			// Update Baratron Gauge
			// 2004.04.17 by cskim
			if( VOpen == nBaratronIsoValve /*&& VOpen == nFastRoughValve */)
			{
				dWRITE_ANALOG(M651_Pressure, m_dblChmbPress, &nIOStatus);
				//LOG( pszFtnName, _ELT_Console, "BarIsolation & FastRough Valve Opened, Write M651_Pressure[%f]", fResultPressure);
			}
			// Update Pumping Pressure Convectron Guage
			if( VOpen == nCVGIsoValve )
			{
				dWRITE_ANALOG(PumpPrsAI, m_dblDPPress, &nIOStatus);
				//LOG( pszFtnName, _ELT_Console, "CVG Isolation Valve Opened, Write PumpPrsAI[%f]", fResultPressure);
			}
			// Update Pressure Isoation Valve
			if(  VOpen == nPressSenseIsoValve /* && VOpen == nFastRoughValve */)
			{							
				if( m_dblChmbPress >= 600 )
				{
					//LOG( pszFtnName, _ELT_Console, "Fast Rough & PressSenseIso Valve Opened, Write ChmPrssDI1[ATM]");
					dWRITE_DIGITAL(ChmPrssDI1, ATM, &nIOStatus);
				}
				else if( m_dblChmbPress <= 10 )
				{
					//LOG( pszFtnName, _ELT_Console, "Fast Rough & PressSenseIso Valve Opened, Write ChmPrssDI1[VTM]");
					dWRITE_DIGITAL(ChmPrssDI1, VACUUM, &nIOStatus);
				}
				else
				{
					//LOG( pszFtnName, _ELT_Console, "Fast Rough & PressSenseIso Valve Opened, Write ChmPrssDI1[Transition]");
					dWRITE_DIGITAL(ChmPrssDI1, Transition, &nIOStatus);			
				}
			}

			nM651Status	  = dREAD_DIGITAL( M651_Valve, &nIOStatus);
			if(nM651Status != nM651Commmand )
			{
				nM651Commmand = nM651Status;
				if( TV_OPEN == nM651Commmand )
				{
					dWRITE_ANALOG( M651_Set_Position, 100, &nIOStatus );
					WaitSec(_TV_SET_TIME_POS);
					dWRITE_ANALOG( M651_Position, 100, &nIOStatus);
				}
				else if(TV_CLOSE == nM651Commmand )
				{
					dWRITE_ANALOG( M651_Set_Position, 0, &nIOStatus );
					WaitSec(_TV_SET_TIME_POS);
					dWRITE_ANALOG( M651_Position, 0, &nIOStatus);
				}
			}

			nTVControl	  = dREAD_DIGITAL( M651_ModeDM, &nIOStatus);
			if( TV_POSITION == nTVControl) 
			{
				fSetPoint = dREAD_ANALOG( M651_Set_Position, &nIOStatus);
				fRead	  = dREAD_ANALOG( M651_Position, &nIOStatus);
				fDiff	  = fSetPoint - fRead;
				if( fDiff != 0)
				{
					dWRITE_DIGITAL( M651_Valve, TV_HOLD, &nIOStatus);
					nM651Status = nM651Commmand = TV_HOLD;
					WaitSec(_TV_SET_TIME_POS);
					if(m_bChm_SimEnable) dWRITE_ANALOG(M651_Position, fSetPoint, &nIOStatus);	
				}
				}
			else if( TV_PRESSURE == nTVControl && DP_ON == nDPRunDI
				&& (VOpen == nBaratronIsoValve && VOpen == nFastRoughValve)
				&& m_bChm_SimEnable)// Pressure Control
			{
				dWRITE_DIGITAL( M651_Valve, TV_HOLD, &nIOStatus);
				dWRITE_ANALOG(M651_Position, 47, &nIOStatus);	

				fSetPoint = dREAD_ANALOG( M651_Set_Pressure, &nIOStatus);
				fDiff	  = fabs(fSetPoint - fRead);

				if(fSetPoint > m_dblChmbPress)
				{
					m_dblChmbPress = fSetPoint;
				}
				if( fDiff != 0)
				{
//					LOG( pszFtnName, _ELT_Console, "PRESSURE: Start To Set Point, SetPoint[%f] != Read[%f]", fSetPoint, fRead);
//					WaitSec(_TV_SET_TIME_PRS*fDiff/10);
				}
			}

			// 2004.04.17 by cskim
			nDPRunDI = dREAD_DIGITAL( DPRunDI, &nIOStatus);
			fRead = dREAD_ANALOG( M651_Position, &nIOStatus);
			if( DP_ON == nDPRunDI )
			{
				if(fRead > 0 && VOpen == nFastRoughValve)
				{
					if(nTVControl != TV_PRESSURE)
					{
						if(fRead > 50) ControlPressure(0.7);
						else ControlPressure(0.95);
					}
					else ControlPressureByTVPres();
					m_dblDPPress = m_dblChmbPress;
				}
				else
				{
					m_dblDPPress = _TV_BASE_PRESS; 
					if(m_dbTargetPress > 760)
					{
						if(m_dblChmbPress < 100) m_dblChmbPress += 13.7;
						else if(m_dblChmbPress < 760) m_dblChmbPress += 29.3;
					}
				}
			}
			else
			{
				if(m_dblDPPress < 100) m_dblDPPress += 13.7;
				else if(m_dblDPPress < 760) m_dblDPPress += 54.3;
				if(fRead > 0 && VOpen == nFastRoughValve)
				{
					m_dblChmbPress = m_dblDPPress;
				}
				// continue;
			}		
		}
		bRet = TRUE;
	}while(0);
	printf("Leaved %s\n", pszFtnName);	
}

// 2004.04.17 by cskim
void CPMChmPrsSimulator::ControlPressure(double dblRate)
{
	if(m_dblChmbPress > m_dbTargetPress)
	{
		if(m_dblChmbPress < 100) dblRate *= 0.33;
		m_dblChmbPress *= dblRate;
	}

	if(m_dblChmbPress <= m_dbTargetPress)
	{
		m_dblChmbPress = m_dbTargetPress;
	}
}

// 2004.04.17 by cskim
void CPMChmPrsSimulator::ControlPressureByTVPres()
{
	static int nFlag = 0;
	double dblTVPos;
	int nIOStatus;

	dblTVPos = dREAD_ANALOG( M651_Position, &nIOStatus);
	if(nFlag == 0)
	{
		m_dblChmbPress *= 0.95;
		dblTVPos += 1;
	}
	else
	{
		m_dblChmbPress *= 0.98;
		dblTVPos -= 1;
	}
	dWRITE_ANALOG(M651_Position, dblTVPos, &nIOStatus);
	nFlag ^= 0x01;
}

void CPMChmPrsSimulator::ControlPressureByPos(double fPosition_SetPoint, double fTargetPress)
{
	char pszLocFtnName[] = "ControlPressureByPos";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;

	int		i				= 0;
	double	fCurPressure	= 0;
	int		nTotalStep		= 0;
	double	fStepPressure	= 0;

	int		nBaratronIsoValve	= VClose;
	int		nPressSenseIsoValve = VClose;
	int		nCVGIsoValve		= VClose;
	int		nFastRoughValve		= VClose;
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		fCurPressure = dREAD_ANALOG( ChmPrsAI, &nIOStatus);		
		//LOG(pszFtnName,_ELT_Console, "Current Pressure: %f", fCurPressure);

		nTotalStep = (int)(_TV_POS_STEP_GAIN - fPosition_SetPoint);
		//LOG(pszFtnName, _ELT_Console, "Total Position Step: %d", nTotalStep);

		fStepPressure = (fCurPressure - fTargetPress)/nTotalStep;
		//LOG(pszFtnName, _ELT_Console, "Target Pressure: %f, Step Pressure: %f", fTargetPress, fStepPressure);
		for( i=0; i < nTotalStep; i++)
		{
			fCurPressure = fCurPressure - fStepPressure;
			//LOG(pszFtnName, _ELT_Console, "Calculated Current Presure: %f", fCurPressure);

			// Check Valve & Update
			nBaratronIsoValve = dREAD_DIGITAL(BarIsoVlvDO, &nIOStatus);
			nPressSenseIsoValve = dREAD_DIGITAL(PgSwIsoVlvDO, &nIOStatus);
			nCVGIsoValve = dREAD_DIGITAL(CVGIsoVlvDO, &nIOStatus);
			nFastRoughValve = dREAD_DIGITAL(FastRoughVlvDO, &nIOStatus);

			// Update Chamber Pressure
			if( VOpen == nFastRoughValve)
			{
				dWRITE_ANALOG(ChmPrsAI, fCurPressure, &nIOStatus);
				//LOG( pszFtnName, _ELT_Console, "Fast Rough Valve Opened, Write ChmPrsAI[%f]", fCurPressure);
			}
			//else
			//	LOG( pszFtnName, _ELT_Console, "Fast Rough Valve Closed");

			// Update Baratron Gauge
			if( VOpen == nBaratronIsoValve && VOpen == nFastRoughValve)
			{
				dWRITE_ANALOG(M651_Pressure, fCurPressure, &nIOStatus);
			}
			//else
			//	LOG( pszFtnName, _ELT_Console, "Baratron Isolation Valve Closed");


			// Update Convectron Guage
			if( VOpen == nCVGIsoValve )
			{
				dWRITE_ANALOG(PumpPrsAI, fCurPressure, &nIOStatus);
			//	LOG( pszFtnName, _ELT_Console, "CVG Isolation Valve Opened, PumpPrsAI[%f]", fCurPressure);
			}
			//else
			//	LOG( pszFtnName, _ELT_Console, "CVG Isolation Valve Closed");

			// Update Pressure Isoation Valve
			if(  VOpen == nPressSenseIsoValve && VOpen == nFastRoughValve)
			{
			//	LOG( pszFtnName, _ELT_Console, "VOpen == nPressSenseIsoValve && VOpen == nFastRoughValve");
				if( fCurPressure >= 600)
					dWRITE_DIGITAL(ChmPrssDI1, ATM, &nIOStatus);
				else if( fCurPressure <= 10)
					dWRITE_DIGITAL(ChmPrssDI1, VACUUM, &nIOStatus);
				else
					dWRITE_DIGITAL(ChmPrssDI1, Transition, &nIOStatus);
			}
			//else
			//	LOG( pszFtnName, _ELT_Console, "VClose == nPressSenseIsoValve || VClose == nFastRoughValve");
			WaitSec(_TV_SET_TIME_POS);
		}
		m_dblChmbPress = fCurPressure;
		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leaved %s\n", pszFtnName);	
}
