// InterlockMonitorTM.cpp: implementation of the CInterlockMonitorTM class.
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

#include "InterlockMonitorTM.h"

//--------------------------------------------------------------------
extern void _LOG(const char *pFormat, ...);

//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
// #include "DynamicIO.h"
static enum {
	CTC_AbortControl,
	CTC_MAIN_CONTROL,
	CTC_MAIN_CONTROL2,
	CTC_MAIN_CONTROL3,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	
	{"CTC.AbortControl",		_K_D_IO, 0},
	{"CTC.MAIN_CONTROL",		_K_D_IO, 0},
	{"CTC.MAIN_CONTROL2",		_K_D_IO, 0},
	{"CTC.MAIN_CONTROL3",		_K_D_IO, 0},
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInterlockMonitorTM::CInterlockMonitorTM()
{

}

CInterlockMonitorTM::~CInterlockMonitorTM()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CInterlockMonitorTM::Initialize()
{
	CInterlockMonitor::Initialize();
	m_DynIO.InitIOList(g_pIOList);

	return SYS_SUCCESS;
}

void CInterlockMonitorTM::Deinitialize()
{
	CInterlockMonitor::Deinitialize();
}

void CInterlockMonitorTM::ExecInterlock(int nIntType)
{
	int nIdx;

	if(nIntType <= 0) return;
	CInterlockMonitor::ExecInterlock(nIntType);
	nIdx = nIntType - 1;
	switch(m_IoCtrlList[nIdx].nIntrCtrl)
	{
	case INTCT_NO: break;
	case INTCT_ABORT_LOT: AbortLot(); break;
	}
}

void CInterlockMonitorTM::AbortLot()
{
	enum { ABC_IDLE, ABC_ABORT, ABC_ABORTING };
	enum {_IDLE = 0 , _RUNNING , _PAUSING , _PAUSED , _ABORTING , _DISABLE , _WAITING , _WAIT_HAND_OFF};
	BOOL bIOStatus;
	int nCM1_Status;
	int nCM2_Status;
	int nCM3_Status;

	nCM1_Status = m_DynIO.dREAD_DIGITAL(CTC_MAIN_CONTROL, &bIOStatus);
	nCM2_Status = m_DynIO.dREAD_DIGITAL(CTC_MAIN_CONTROL2, &bIOStatus);
	nCM3_Status = m_DynIO.dREAD_DIGITAL(CTC_MAIN_CONTROL3, &bIOStatus);

	if(_RUNNING == nCM1_Status || _PAUSING == nCM1_Status || _PAUSED == nCM1_Status || _WAITING == nCM1_Status
		|| _RUNNING == nCM2_Status || _PAUSING == nCM2_Status || _PAUSED == nCM2_Status || _WAITING == nCM2_Status
		|| _RUNNING == nCM3_Status || _PAUSING == nCM3_Status || _PAUSED == nCM3_Status || _WAITING == nCM3_Status
		)
	{
		_LOG("-----> AbortLot Func Called");
		m_DynIO.dWRITE_DIGITAL(CTC_AbortControl, ABC_ABORT, &bIOStatus);
	}
}
