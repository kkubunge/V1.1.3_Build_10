// PrevMaintMonitor.cpp: implementation of the CPrevMaintMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PrevMaintMonitor.h"
#include "CPreventiveMaintenance_Client.h"

extern int g_nPMNumber;
// #define _DEBUG
//--------------------------------------------------------------------
#include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	PM_CommStatus,
	PRVD_DRYCWfrCnt,
	PRVA_DRYCSrcFeed,
	PRVA_DRYCThick,

	CTC_PRMD_DRYCWfrCnt,
	CTC_PRMA_DRYCSrcFeed,
	CTC_PRMA_DRYCThick,

	//2014.06.20
	PRVD_WETCWfrCnt,
	CTC_WETCWfrCnt,	

	PRO_PostRunDM,
	PRO_PostRunDM1,
	PRO_PostRunDM2,
	PRO_PostRunDM3,
	PRO_PostRunDM4,
	PRO_PostRunDM5,
	PRO_PostRunDM6,
	PRO_PostRunDM7,
	PRO_PostRunDM8,
	PRO_PostRunDM9,
	PRMD_PM_Leaf_AuEn_02,
	PRMD_PM_Leaf_AuEn_03,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"CTC.PM#_CommStatus",			_K_D_IO, 0},
	{"PM#.PRVD_DRYCWfrCnt",			_K_D_IO, 0},
	{"PM#.PRVA_DRYCSrcFeed",		_K_A_IO, 0},
	{"PM#.PRVA_DRYCThick",			_K_A_IO, 0},

	{"PRMD_PM#_DRYCWfrCnt",			_K_D_IO, 0},
	{"PRMD_PM#_DRYCSrcFeed",		_K_A_IO, 0},
	{"PRMA_PM#_DRYCThick",			_K_A_IO, 0},
	//2014.06.20
	{"PM#.PRVD_WETCWfrCnt",			_K_D_IO, 0},
	{"PRMD_PM#_WETCWfrCnt",			_K_D_IO, 0},	

	{"PM#.PRO_PostRunDM",			_K_D_IO, 0},
	{"PM#.PRO_PostRunDM1",			_K_D_IO, 0},
	{"PM#.PRO_PostRunDM2",			_K_D_IO, 0},
	{"PM#.PRO_PostRunDM3",			_K_D_IO, 0},
	{"PM#.PRO_PostRunDM4",			_K_D_IO, 0},
	{"PM#.PRO_PostRunDM5",			_K_D_IO, 0},
	{"PM#.PRO_PostRunDM6",			_K_D_IO, 0},
	{"PM#.PRO_PostRunDM7",			_K_D_IO, 0},
	{"PM#.PRO_PostRunDM8",			_K_D_IO, 0},
	{"PM#.PRO_PostRunDM9",			_K_D_IO, 0},
	{"PRMD_PM#_Leaf_AuEn_02",		_K_D_IO, 0},
	{"PRMD_PM#_Leaf_AuEn_03",		_K_D_IO, 0},
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrevMaintMonitor::CPrevMaintMonitor()
{
	m_nPMNumber = 0;
	InitializeCriticalSection(&m_csLock);
}

CPrevMaintMonitor::~CPrevMaintMonitor()
{
	DeleteCriticalSection(&m_csLock);
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CPrevMaintMonitor::Initialize(int nPMNumber)
{
	m_nPMNumber = nPMNumber;
	InitIOList(g_pIOList, false, 0, nPMNumber);
	CreateThread();
}

void CPrevMaintMonitor::DoMonitor()
{
	int nPM_CommStatus = 0;
	int nValue;
	bool bUpdatePM = false;
	BOOL bIOStatus;
	// BOOL bPostComptMon = FALSE;
	// extern CPreventiveMaintenance_Client* g_pCPreventiveMaintenance_Client;

	/*
	// For Preventive Count Auto Reset
	if(IOValid(PRO_PostCmptDM))
	{
		#ifdef _DEBUG
		printf("-----> PM%d DryCleaning Run Monitoring Enable\n", m_nPMNumber);
		#endif
		bPostComptMon = TRUE;
	}
	*/

	WaitSec(5);
	while(WaitSec(3))
	{
		nValue = dREAD_DIGITAL(PM_CommStatus, &bIOStatus);
		switch(nValue)
		{
		case 2:	// Disconnect
			if(nValue == nPM_CommStatus) break;
			nPM_CommStatus = nValue;
			bUpdatePM = false;
			break;
		default:
			if(nValue == nPM_CommStatus) break;
			nPM_CommStatus = nValue;
			if(bUpdatePM) break;
			UpdatePM();
			bUpdatePM = true;
			break;
		}

		/*
		// For Preventive Count Auto Reset
		if(bPostComptMon)
		{
			if(dREAD_DIGITAL(PRO_PostCmptDM, &bIOStatus) == 1)
			{
				#ifdef _DEBUG
				printf("-----> PM%d Dry Cleaning Complete and Prev Reseted\n", m_nPMNumber);
				#endif
				dWRITE_DIGITAL(PRO_PostCmptDM, 0, &bIOStatus);
				g_pCPreventiveMaintenance_Client->MsgParser("RESET 00 02 01");
				if(IOValid(PRO_PostRunDM)) dWRITE_DIGITAL(PRO_PostRunDM, 0, &bIOStatus);
			}
		}
		*/
	}
}

void CPrevMaintMonitor::UpdatePM()
{
	EnterCriticalSection(&m_csLock);

	double dbValue;
	int nValue;
	BOOL bIOStatus;

	nValue = dREAD_DIGITAL(CTC_PRMD_DRYCWfrCnt, &bIOStatus);
	dWRITE_DIGITAL(PRVD_DRYCWfrCnt, nValue, &bIOStatus);

	dbValue = dREAD_ANALOG(CTC_PRMA_DRYCSrcFeed, &bIOStatus);
	dWRITE_ANALOG(PRVA_DRYCSrcFeed, dbValue, &bIOStatus);

	dbValue = dREAD_ANALOG(CTC_PRMA_DRYCThick, &bIOStatus);
	dWRITE_ANALOG(PRVA_DRYCThick, dbValue, &bIOStatus);

	//2014.06.20
	nValue = dREAD_DIGITAL(CTC_WETCWfrCnt, &bIOStatus);
	dWRITE_DIGITAL(PRVD_WETCWfrCnt, nValue, &bIOStatus);	

	LeaveCriticalSection(&m_csLock);
}

//// It was added for online dry cleaning Leak Check, Dry Post 2007.02.1 //////////////////////
void CPrevMaintMonitor::ReservePostRun(int nPRState,int nType)
{
/*
INVALID_TYPE								 = 0	
WET_CLEANING_COMPOSITE_TYPE					 = 1 
DRY_CLEANING_COMPOSITE_TYPE					 = 2
SOURCE_EXCHANGE_COMPOSITE_TYPE				 = 3
STAGE_HEATER_EXCHANGE_COMPOSITE_TYPE		 = 4	
RF_GENERATOR_COMPOSITE_TYPE					 = 5
DRY_PUMP_EXCHANGE_COMPOSITE_TYPE			 = 6
TRAP_EXCHANGE_COMPOSITE_TYPE				 = 7
MAINTENANCE_PERIOD_A_EXCHANGE_COMPOSITE_TYPE = 8	
MAINTENANCE_PERIOD_B_EXCHANGE_COMPOSITE_TYPE = 9	
MAINTENANCE_PERIOD_C_EXCHANGE_COMPOSITE_TYPE = 10
=================================================================================================================================
|   0   |   1   |   2   |      3       |      4       |      5       |       6      |      7       |       8      |       9      |
|Drycl,LeakCheck,DryPost|    Purge1    |    Purge2    |    Purge3    |      ROR     |    PChk1     |     PChk2    |     PChk3    |
|PRO_PostRunDM          |PRO_PostRunDM3|PRO_PostRunDM4|PRO_PostRunDM4|PRO_PostRunDM4|PRO_PostRunDM4|PRO_PostRunDM4|PRO_PostRunDM9|
=================================================================================================================================
*/

	BOOL bIOStatus;
	if(nType == 2)
	{
		if(IOValid(PRO_PostRunDM))  dWRITE_DIGITAL(PRO_PostRunDM, nPRState, &bIOStatus); // For dry cleaning

		/*
		//// It was added for online dry cleaning Leak Check, Dry Post 2007.01.11 //////////////////////////////
		if(IOValid(PRMD_PM_Leaf_AuEn_02)) // For dry cleaning -> Leak Check
		{
			
			if(dREAD_DIGITAL(PRMD_PM_Leaf_AuEn_02, &bIOStatus) == 1)
			{
				if(IOValid(PRO_PostRunDM1))  dWRITE_DIGITAL(PRO_PostRunDM1, nPRState, &bIOStatus);
			}
			else
			{
				if(IOValid(PRO_PostRunDM1))  dWRITE_DIGITAL(PRO_PostRunDM1, 0 , &bIOStatus);
			}
			
		}

		if(IOValid(PRMD_PM_Leaf_AuEn_03)) // For dry cleaning -> Leak Check -> Dry Post
		{
			if(dREAD_DIGITAL(PRMD_PM_Leaf_AuEn_03, &bIOStatus) == 1)
			{
				if(IOValid(PRO_PostRunDM2))  dWRITE_DIGITAL(PRO_PostRunDM2, nPRState, &bIOStatus);
			}
			else
			{
				if(IOValid(PRO_PostRunDM2))  dWRITE_DIGITAL(PRO_PostRunDM2, 0 , &bIOStatus);
			}
		}
		//// It was added for online dry cleaning Leak Check, Dry Post 2007.01.11 //////////////////////////////
		*/
	}
	else if(nType == 8) // Maintenance Period A
	{
		if(IOValid(PRO_PostRunDM3)) dWRITE_DIGITAL(PRO_PostRunDM3, nPRState, &bIOStatus); // For purging
	}
	else
	{ ; }

}

void CPrevMaintMonitor::ResetPostRun(int nType)
{
	BOOL bIOStatus;

	if(nType == 2)
	{
		if(IOValid(PRO_PostRunDM))  dWRITE_DIGITAL(PRO_PostRunDM, 0 , &bIOStatus); // For dry cleaning
		switch (g_nPMNumber) {

			case 1:
				WRITE_DIGITAL(PM1_ChamberStatus, 0 , &bIOStatus);
				break;
			case 2:
				WRITE_DIGITAL(PM2_ChamberStatus, 0 , &bIOStatus);
				break;
			case 3:
				WRITE_DIGITAL(PM3_ChamberStatus, 0 , &bIOStatus);
				break;
			default:
				break;
		}		
	}
	else if(nType == 8) // Maintenance Period A
	{
		if(IOValid(PRO_PostRunDM3)) dWRITE_DIGITAL(PRO_PostRunDM3, 0 , &bIOStatus); // For purging
	}
	else
	{ ; }

}

BOOL CPrevMaintMonitor::IsPostRunReserved(int nType)
{
	BOOL bRet = FALSE;
	BOOL bIOStatus = 0;

	if(nType == 2)
	{
		if(IOValid(PRO_PostRunDM)) 
		{
			if(dREAD_DIGITAL(PRO_PostRunDM, &bIOStatus) > 0) bRet = TRUE;
			else bRet = FALSE;
		}
	}
	else if(nType == 8) // Maintenance Period A
	{
		if(IOValid(PRO_PostRunDM3)) 
		{
			if(dREAD_DIGITAL(PRO_PostRunDM3, &bIOStatus) > 0) bRet = TRUE;
			else bRet = FALSE;
		}
	}
	else
	{ ; }

	return bRet;
}
//// It was added for online dry cleaning Leak Check, Dry Post 2007.02.1 //////////////////////