// IOSyncMonitor.cpp: implementation of the CIOSyncMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <string.h>
//====================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//====================================================================

#include "IOSyncMonitor.h"

#define MAX_IO_LIST		300
static IO_LIST g_pIOList1[MAX_IO_LIST];
static IO_LIST g_pIOList2[MAX_IO_LIST];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOSyncMonitor::CIOSyncMonitor()
{
	int i;
	m_nIOSyncCount = 0;
	for(i=0; i<MAX_IO_LIST; i++)
	{
		g_pIOList1[i].nIOPoint = -1;
		g_pIOList1[i].nIOType = 0;
		g_pIOList1[i].szIOName[0] = 0;

		g_pIOList2[i].nIOPoint = -1;
		g_pIOList2[i].nIOType = 0;
		g_pIOList2[i].szIOName[0] = 0;
	}
}

CIOSyncMonitor::~CIOSyncMonitor()
{
	DestroyThread();
}

//////////////////////////////////////////////////////////////////////

void CIOSyncMonitor::SetIOSyncList(const char *szIOName1, const char *szIOName2, const char *szIOType)
{
	int nIOType;

	     if(szIOType[0] == 'A') nIOType = _K_A_IO;
	else if(szIOType[0] == 'S') nIOType = _K_S_IO;
	else						nIOType = _K_D_IO;

	if(m_nIOSyncCount >= MAX_IO_LIST)	return;

	strcpy(g_pIOList1[m_nIOSyncCount].szIOName, szIOName1);
	g_pIOList1[m_nIOSyncCount].nIOType = nIOType;

	strcpy(g_pIOList2[m_nIOSyncCount].szIOName, szIOName2);
	g_pIOList2[m_nIOSyncCount].nIOType = nIOType;
	
	m_nIOSyncCount++;
}

BOOL CIOSyncMonitor::Initialize()
{
	m_DynIO1.InitIOList(g_pIOList1);
	m_DynIO2.InitIOList(g_pIOList2);
	CreateThread();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void CIOSyncMonitor::DoMonitor()
{
	int i;
	BOOL bIOStatus;
	double adbValue1[MAX_IO_LIST];
	double adbValue2[MAX_IO_LIST];
	double dbValue1, dbValue2;

	for(i=0; i<m_nIOSyncCount; i++)
	{
		switch(g_pIOList1[i].nIOType)
		{
		case _K_D_IO:
			adbValue1[i] = m_DynIO1.dREAD_DIGITAL(i, &bIOStatus);
			adbValue2[i] = m_DynIO2.dREAD_DIGITAL(i, &bIOStatus);
			break;
		case _K_A_IO:
			adbValue1[i] = m_DynIO1.dREAD_ANALOG(i, &bIOStatus);
			adbValue2[i] = m_DynIO2.dREAD_ANALOG(i, &bIOStatus);
		}
	}

	while(WaitSec(1))
	{
		for(i=0; i<m_nIOSyncCount; i++)
		{
			switch(g_pIOList1[i].nIOType)
			{
			case _K_D_IO:
					dbValue1 = m_DynIO1.dREAD_DIGITAL(i, &bIOStatus);
					dbValue2 = m_DynIO2.dREAD_DIGITAL(i, &bIOStatus);
					if(dbValue1 != adbValue1[i])
					{
						adbValue2[i] = adbValue1[i] = dbValue1;
						m_DynIO1.dWRITE_DIGITAL(i, (int)adbValue1[i], &bIOStatus);
						m_DynIO2.dWRITE_DIGITAL(i, (int)adbValue2[i], &bIOStatus);
					}
					else if(dbValue2 != adbValue2[i])
					{
						adbValue2[i] = adbValue1[i] = dbValue2;
						m_DynIO1.dWRITE_DIGITAL(i, (int)adbValue1[i], &bIOStatus);
						m_DynIO2.dWRITE_DIGITAL(i, (int)adbValue2[i], &bIOStatus);
					}
					else if(dbValue1 != dbValue2)
					{
						adbValue2[i] = adbValue1[i] = dbValue1;
						m_DynIO1.dWRITE_DIGITAL(i, (int)adbValue1[i], &bIOStatus);
						m_DynIO2.dWRITE_DIGITAL(i, (int)adbValue2[i], &bIOStatus);
					}
					break;
			case _K_A_IO:
					dbValue1 = m_DynIO1.dREAD_ANALOG(i, &bIOStatus);
					dbValue2 = m_DynIO2.dREAD_ANALOG(i, &bIOStatus);
					if(dbValue1 != adbValue1[i])
					{
						adbValue2[i] = adbValue1[i] = dbValue1;
						m_DynIO1.dWRITE_ANALOG(i, adbValue1[i], &bIOStatus);
						m_DynIO2.dWRITE_ANALOG(i, adbValue2[i], &bIOStatus);
					}
					else if(dbValue2 != adbValue2[i])
					{
						adbValue2[i] = adbValue1[i] = dbValue2;
						m_DynIO1.dWRITE_ANALOG(i, adbValue1[i], &bIOStatus);
						m_DynIO2.dWRITE_ANALOG(i, adbValue2[i], &bIOStatus);
					}
					else if(dbValue1 != dbValue2)
					{
						adbValue2[i] = adbValue1[i] = dbValue1;
						m_DynIO1.dWRITE_ANALOG(i, adbValue1[i], &bIOStatus);
						m_DynIO2.dWRITE_ANALOG(i, adbValue2[i], &bIOStatus);
					}
					break;
			}
		}
	}
}