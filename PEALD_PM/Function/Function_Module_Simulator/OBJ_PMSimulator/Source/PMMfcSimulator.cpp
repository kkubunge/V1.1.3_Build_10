// PMMfcSimulator.cpp: implementation of the CPMMfcSimulator class.
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
#include "PMMfcSimulator.h"

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	// MFC Set Point IO's
	MFC01StPtAO,
	MFC02StPtAO,
	MFC03StPtAO,
	MFC04StPtAO,
	MFC05StPtAO,
	MFC06StPtAO,
	MFC07StPtAO,
	MFC08StPtAO,
	MFC09StPtAO,
	MFC10StPtAO,
	MFC11StPtAO,
	MFC12StPtAO,
	MFC13StPtAO,
	MFC14StPtAO,
	MFC15StPtAO,
	MFC16StPtAO,
	MFC17StPtAO,
	MFC18StPtAO,
	MFC19StPtAO,
	MFC20StPtAO,

	// MFC Flow ReadBack IO's		
	MFC01FlwAI,
	MFC02FlwAI,
	MFC03FlwAI,
	MFC04FlwAI,
	MFC05FlwAI,
	MFC06FlwAI,
	MFC07FlwAI,
	MFC08FlwAI,
	MFC09FlwAI,
	MFC10FlwAI,
	MFC11FlwAI,
	MFC12FlwAI,
	MFC13FlwAI,
	MFC14FlwAI,
	MFC15FlwAI,
	MFC16FlwAI,
	MFC17FlwAI,
	MFC18FlwAI,
	MFC19FlwAI,
	MFC20FlwAI,
	
	// MFC Valve IO's		
	MFC01PgDO,
	MFC02PgDO,
	MFC03PgDO,
	MFC04PgDO,
	MFC05PgDO,
	MFC06PgDO,
	MFC07PgDO,
	MFC08PgDO,
	MFC09PgDO,
	MFC10PgDO,
	MFC11PgDO,
	MFC12PgDO,
	MFC13PgDO,
	MFC14PgDO,
	MFC15PgDO,
	MFC16PgDO,
	MFC17PgDO,
	MFC18PgDO,
	MFC19PgDO,
	MFC20PgDO,

	// MFC Use NotUse IO's
	MFC01_UseDM,
	MFC02_UseDM,
	MFC03_UseDM,
	MFC04_UseDM,
	MFC05_UseDM,
	MFC06_UseDM,
	MFC07_UseDM,
	MFC08_UseDM,
	MFC09_UseDM,
	MFC10_UseDM,
	MFC11_UseDM,
	MFC12_UseDM,
	MFC13_UseDM,
	MFC14_UseDM,
	MFC15_UseDM,
	MFC16_UseDM,
	MFC17_UseDM,
	MFC18_UseDM,
	MFC19_UseDM,
	MFC20_UseDM,

	M651_Position,
	FastRoughVlvDO,

	// 2006.09.07
	MFC_Feeding_Valve,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	// MFC SetPoint IO's
	{ "MFC01StPtAO",	_K_A_IO,	0},
	{ "MFC02StPtAO",	_K_A_IO,	0},
	{ "MFC03StPtAO",	_K_A_IO,	0},
	{ "MFC04StPtAO",	_K_A_IO,	0},
	{ "MFC05StPtAO",	_K_A_IO,	0},
	{ "MFC06StPtAO",	_K_A_IO,	0},
	{ "MFC07StPtAO",	_K_A_IO,	0},
	{ "MFC08StPtAO",	_K_A_IO,	0},
	{ "MFC09StPtAO",	_K_A_IO,	0},
	{ "MFC10StPtAO",	_K_A_IO,	0},
	{ "MFC11StPtAO",	_K_A_IO,	0},
	{ "MFC12StPtAO",	_K_A_IO,	0},
	{ "MFC13StPtAO",	_K_A_IO,	0},
	{ "MFC14StPtAO",	_K_A_IO,	0},
	{ "MFC15StPtAO",	_K_A_IO,	0},
	{ "MFC16StPtAO",	_K_A_IO,	0},
	{ "MFC17StPtAO",	_K_A_IO,	0},
	{ "MFC18StPtAO",	_K_A_IO,	0},
	{ "MFC19StPtAO",	_K_A_IO,	0},
	{ "MFC20StPtAO",	_K_A_IO,	0},

	// MFC Flow Read Back IO's
	{ "MFC01FlwAI",		_K_A_IO,	0},
	{ "MFC02FlwAI",		_K_A_IO,	0},
	{ "MFC03FlwAI",		_K_A_IO,	0},
	{ "MFC04FlwAI",		_K_A_IO,	0},
	{ "MFC05FlwAI",		_K_A_IO,	0},
	{ "MFC06FlwAI",		_K_A_IO,	0},
	{ "MFC07FlwAI",		_K_A_IO,	0},
	{ "MFC08FlwAI",		_K_A_IO,	0},
	{ "MFC09FlwAI",		_K_A_IO,	0},
	{ "MFC10FlwAI",		_K_A_IO,	0},
	{ "MFC11FlwAI",		_K_A_IO,	0},
	{ "MFC12FlwAI",		_K_A_IO,	0},
	{ "MFC13FlwAI",		_K_A_IO,	0},
	{ "MFC14FlwAI",		_K_A_IO,	0},
	{ "MFC15FlwAI",		_K_A_IO,	0},
	{ "MFC16FlwAI",		_K_A_IO,	0},
	{ "MFC17FlwAI",		_K_A_IO,	0},
	{ "MFC18FlwAI",		_K_A_IO,	0},
	{ "MFC19FlwAI",		_K_A_IO,	0},
	{ "MFC20FlwAI",		_K_A_IO,	0},
	
	// MFC Valves IO's
	{ "MFC01PgDO",		_K_D_IO,	0},
	{ "MFC02PgDO",		_K_D_IO,	0},
	{ "MFC03PgDO",		_K_D_IO,	0},
	{ "MFC04PgDO",		_K_D_IO,	0},
	{ "MFC05PgDO",		_K_D_IO,	0},
	{ "MFC06PgDO",		_K_D_IO,	0},
	{ "MFC07PgDO",		_K_D_IO,	0},
	{ "MFC08PgDO",		_K_D_IO,	0},
	{ "MFC09PgDO",		_K_D_IO,	0},
	{ "MFC10PgDO",		_K_D_IO,	0},
	{ "MFC11PgDO",		_K_D_IO,	0},
	{ "MFC12PgDO",		_K_D_IO,	0},
	{ "MFC13PgDO",		_K_D_IO,	0},
	{ "MFC14PgDO",		_K_D_IO,	0},
	{ "MFC15PgDO",		_K_D_IO,	0},
	{ "MFC16PgDO",		_K_D_IO,	0},
	{ "MFC17PgDO",		_K_D_IO,	0},
	{ "MFC18PgDO",		_K_D_IO,	0},
	{ "MFC19PgDO",		_K_D_IO,	0},
	{ "MFC20PgDO",		_K_D_IO,	0},
	
	// MFC Use NotUse IO's
	{ "MFC01_UseDM",	_K_D_IO,	0},
	{ "MFC02_UseDM",	_K_D_IO,	0},
	{ "MFC03_UseDM",	_K_D_IO,	0},
	{ "MFC04_UseDM",	_K_D_IO,	0},
	{ "MFC05_UseDM",	_K_D_IO,	0},
	{ "MFC06_UseDM",	_K_D_IO,	0},
	{ "MFC07_UseDM",	_K_D_IO,	0},
	{ "MFC08_UseDM",	_K_D_IO,	0},
	{ "MFC09_UseDM",	_K_D_IO,	0},
	{ "MFC10_UseDM",	_K_D_IO,	0},
	{ "MFC11_UseDM",	_K_D_IO,	0},
	{ "MFC12_UseDM",	_K_D_IO,	0},
	{ "MFC13_UseDM",	_K_D_IO,	0},
	{ "MFC14_UseDM",	_K_D_IO,	0},
	{ "MFC15_UseDM",	_K_D_IO,	0},
	{ "MFC16_UseDM",	_K_D_IO,	0},
	{ "MFC17_UseDM",	_K_D_IO,	0},
	{ "MFC18_UseDM",	_K_D_IO,	0},
	{ "MFC19_UseDM",	_K_D_IO,	0},
	{ "MFC20_UseDM",	_K_D_IO,	0},

	{ "M651_Position",		_K_A_IO,0},
	{ "FastRoughVlvDO",		_K_D_IO,0},	

	// 2006.09.07
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	

	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	

	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	

	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	{ "NotDefined",		_K_D_IO,	0},	
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPMMfcSimulator::CPMMfcSimulator()
{
	int i;
	for(i=0; i<_MFC_MAX_NO; i++) m_pnMFCErr[i] = 0;
	m_pdbTargetPress = NULL;
	m_bMFC_SimEnable = true;
}

CPMMfcSimulator::~CPMMfcSimulator()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CPMMfcSimulator::Initialize(double *pdbTargetPress)
{
	m_pdbTargetPress = pdbTargetPress;
	InitIOList(g_pIOList);
	CreateThread();
	return TRUE;
}

void CPMMfcSimulator::SetMFCError(char *szParam)
{
	char szPrm1[40];
	char szPrm2[40];
	int nMFC = 0;
	int nErr = 0;

	STR_SEPERATE(szParam , szPrm1, szPrm2, 255 );
	nMFC = atoi(szPrm1);
	nErr = atoi(szPrm2);

	if(nMFC < 1 || nMFC > _MFC_MAX_NO)
	{
		printf("-----> Invalid MFC Number %d in %s\n", nMFC, APP_NAME);
		return;
	}

	printf("-----> Set MFC[%d] Error = %d in %s\n", nMFC, nErr, APP_NAME);
	m_pnMFCErr[nMFC-1] = nErr;
}

void CPMMfcSimulator::ClearMFCError()
{
	int i;
	for(i=0; i<_MFC_MAX_NO; i++)
	{
		m_pnMFCErr[i] = 0;
	}
	// printf("-----> Clear MFC Error in %s\n", APP_NAME);
}
// End by cskim

void CPMMfcSimulator::SetMfcFeedValve(int nMfcNo, int nValveIdx, const char *szFeedValveName)		// 2006.09.07
{
	int nMfcIdx = nMfcNo-1;
	int nIOPoint;

	nIOPoint = MFC_Feeding_Valve + (nMfcIdx * 3) + nValveIdx;
	strcpy(g_pIOList[nIOPoint].szIOName, szFeedValveName);
}

//////////////////////////////////////////////////////////////////////////
void CPMMfcSimulator::DoMonitor()
{
	char pszLocFtnName[] = "CPMMfcSimulator";
	char pszFtnName[256];
	BOOL bRet = FALSE;
	int nIOStatus = 0;


	int	pnUsedMFCHandle[_MFC_MAX_NO];
	int	nUsedMFCNo	= 0;
	int	nRetryCount		= 5;
	int i	= 0;
	double fSetPoint	= 0;
	double fRead		= 0;
	int	nMFCControl			= SetPoint;
	double dbPress = _TV_BASE_PRESS;
	int nFastRoughValve;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		// Get All Used MFC Handles
		GetAllUsedMFCHandle( pnUsedMFCHandle, &nUsedMFCNo);

//		LOG(pszFtnName,_ELT_Console, "Total Used MFC No. = [%d]", nUsedMFCNo);

		// Simulation Loop
		while(WaitSec(1))
		{
			if(! m_bMFC_SimEnable) continue;
			dbPress = _TV_BASE_PRESS;
			for( i=0; i < nUsedMFCNo; i++)
			{
				nMFCControl	  = dREAD_DIGITAL( MFC01PgDO + pnUsedMFCHandle[i], &nIOStatus);
				if( SetPoint == nMFCControl ) 
				{
					if(IsFeedValveOpened(pnUsedMFCHandle[i]+1))
					{
						fSetPoint = dREAD_ANALOG( MFC01StPtAO	+ pnUsedMFCHandle[i], &nIOStatus);
						fRead	  = dREAD_ANALOG( MFC01FlwAI	+ pnUsedMFCHandle[i], &nIOStatus);					
						if( (fSetPoint - fRead) != 0)
						{
							fSetPoint += m_pnMFCErr[i];
							if(fSetPoint < 0) fSetPoint = 0;
							WaitSec(_MFC_SET_TIME);
							dWRITE_ANALOG(MFC01FlwAI + pnUsedMFCHandle[i], fSetPoint, &nIOStatus);
						}
						// 2006.01.18
						if(fRead > 10) dbPress += 0.112;
					}
					else
					{
						dWRITE_ANALOG(MFC01FlwAI + pnUsedMFCHandle[i], 0, &nIOStatus);
					}
				}
				else if( Open == nMFCControl ) // Open
				{					
					if( _MFC_MAX_FLOW != dREAD_ANALOG(MFC01FlwAI	+ pnUsedMFCHandle[i], &nIOStatus))
					{
//						LOG(pszFtnName,_ELT_Console, "Start MFC%02d Full Open", pnUsedMFCHandle[i]+1);
						WaitSec(_MFC_SET_TIME);
						dWRITE_ANALOG(MFC01FlwAI + pnUsedMFCHandle[i], _MFC_MAX_FLOW, &nIOStatus);
//						LOG(pszFtnName,_ELT_Console, "End MFC%02d Full Open", pnUsedMFCHandle[i]+1);
					}
					dbPress += 1.2134;
				}
				else // Close
				{						
					if( 0 != dREAD_ANALOG(MFC01FlwAI	+ pnUsedMFCHandle[i], &nIOStatus))
					{
//						LOG(pszFtnName,_ELT_Console, "Start MFC%02d Full Close", pnUsedMFCHandle[i]+1);
						WaitSec(_MFC_SET_TIME);
						dWRITE_ANALOG(MFC01FlwAI + pnUsedMFCHandle[i], 0, &nIOStatus);
//						LOG(pszFtnName,_ELT_Console, "End MFC%02d Full Close", pnUsedMFCHandle[i]+1);
					}
				}
				WaitSec(_MFC_SCAN_INTERVAL);
			}

			if(m_pdbTargetPress != NULL) *m_pdbTargetPress = dbPress;

			nFastRoughValve = dREAD_DIGITAL(FastRoughVlvDO, &nIOStatus);
			fRead = dREAD_ANALOG( M651_Position, &nIOStatus);
			if(fRead <= 0 || VClose == nFastRoughValve)
			{
				if(dbPress > _TV_BASE_PRESS)
				{
					if(m_pdbTargetPress != NULL) *m_pdbTargetPress = 761;
				}
			}
		}
		bRet = TRUE;
	}while(0);
	printf("Leaved %s\n", pszFtnName);	
}

BOOL CPMMfcSimulator::GetAllUsedMFCHandle(int * pnUsedMFCHandle, int * pnSize)
{
	char pszLocFtnName[] = "GetAllUsedMFCHandle";
	char pszFtnName[256];
	BOOL bRet = FALSE;	
	int nIOStatus = 0;

	int i = 0;
	*pnSize = 0;
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	// printf("Entered %s\n", pszFtnName);
	do
	{		
		for( i = 0; i < _MFC_MAX_NO; i++)
		{
			if( Use == dREAD_DIGITAL(MFC01_UseDM+i, &nIOStatus))
			{				
				pnUsedMFCHandle[*pnSize] = i;
				*pnSize = *pnSize + 1;
				// LOG(pszFtnName,_ELT_Console, "Used MFC No:%d, MFC ID:%d", *pnSize, i+1);
			}
		}
		if( 0 == *pnSize )	bRet = FALSE;
		else				bRet = TRUE;
	}while(0);
	// printf("Leaved %s\n", pszFtnName);
	return bRet;
}

bool CPMMfcSimulator::IsFeedValveOpened(int nMfcNo)		// 2006.09.07
{
	bool bRet = true;
	int nMfcIdx = nMfcNo-1;
	int nIOPoint;
	int i;
	BOOL bIOStatus;
	enum { VLV_CLOSE, VLV_OPEN };
	
	nIOPoint = MFC_Feeding_Valve + (nMfcIdx * 3);
	for(i=0; i<3; i++)
	{
		if(IOValid(nIOPoint+i))
		{
			if(dREAD_DIGITAL(nIOPoint+i, &bIOStatus) == VLV_CLOSE) bRet = false;
		}
	}
	return bRet;
}
