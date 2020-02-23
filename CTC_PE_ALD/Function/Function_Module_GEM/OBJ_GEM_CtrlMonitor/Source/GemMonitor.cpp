// TmcMonitor.cpp: implementation of the CTmcMonitor class.
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
#include "GemMonitor.h"
#include "IPS_Log.h"

//--------------------------------------------------------------------
// #include "TextLogDll.h"
extern void _LOG(const char *pFormat, ...);

//--------------------------------------------------------------------
char	pszFtnName[256] = "";
#define APP_NAME	"GEM_Control_Monitor"
//--------------------------------------------------------------------
// Enumeration  Definition
enum { Enable	, Dry_Reserved	, Dry_Running , Dry_Aborted	}; 
enum { Unuse, Local, DisConn, Standby, Maint, Uninit, EnablePM, CTCinUSE, CTCDis, CTCDis2, CTCEnaPM, CTCDisHW, CTCDIsHW2}; 

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	""
};

//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGemMonitor::CGemMonitor()
{

}

CGemMonitor::~CGemMonitor()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CGemMonitor::Initialize()
{
	BOOL bRet = TRUE;
	//if(! InitIOList(g_pIOList))	printf("-----> InitIOList Failed in %s\n", APP_NAME);
	int i;
	for(i=0; i<=2; i++) {
		CreateThread(i);
		_LOG("PM%d Dry Clean Thread Created..", i+1);
	}
	return bRet;
}


//--------------------------------------------------------------------
// Initialize Function

//////////////////////////////////////////////////////////////////////
// Thread Function

void CGemMonitor::DoMonitor(int nThreadNo)
{
	#ifdef _DEBUG
	printf("-----> Thread[%d] Created in CGemMonitor\n", nThreadNo);
	#endif
	switch(nThreadNo)
	{
	case 0: PM1_DryCleaning_Status_Check_Thread(nThreadNo); break;
	case 1: PM2_DryCleaning_Status_Check_Thread(nThreadNo); break;
	case 2: PM3_DryCleaning_Status_Check_Thread(nThreadNo); break;
	}
}

//--------------------------------------------------------------------
void CGemMonitor::PM1_DryCleaning_Status_Check_Thread(int nThreadNo)
{
	int nCommStatus;
	int nPM1_DryCleaning_Status;
	int nStartFlag, nEndFlag, nAbortFlag, nReserveFlag;
	
	nStartFlag   = 0;
	nEndFlag     = 1;
	nAbortFlag   = 0;
	nReserveFlag = 0;
	
	while(WAIT_SEC(1))
	{
		if ( Maint != READ_DIGITAL(PM1_CommStatus, &nCommStatus))
		{
			nPM1_DryCleaning_Status = READ_DIGITAL(PM1_PRO_CHAMBER_STATUS, &nCommStatus);

			//_LOG("PM1 : %d, %d, %d, %d", nPM1_DryCleaning_Status, nStartFlag, nEndFlag, nAbortFlag);

			if (( Dry_Running == nPM1_DryCleaning_Status) && nStartFlag == 0)
			{
				nStartFlag   = 1;
				nEndFlag     = 0;
				nAbortFlag   = 0;
				nReserveFlag = 0;

				WRITE_STRING( SCHEDULER, "EVENT_ON PM1_DryCleaning_Start", &nCommStatus);
				_LOG("PM1_DryCleaning_Start");
			}
			
			if (( Enable == nPM1_DryCleaning_Status || Dry_Reserved == nPM1_DryCleaning_Status) && nEndFlag == 0)
			{

				nStartFlag   = 0;
				nEndFlag     = 1;
				nAbortFlag   = 0;
				nReserveFlag = 0;

				WRITE_STRING( SCHEDULER, "EVENT_ON PM1_DryCleaning_Completed", &nCommStatus);
				_LOG("PM1_DryCleaning_Completed");
			}
			
			if (( Dry_Aborted == nPM1_DryCleaning_Status) && nAbortFlag == 0)
			{
				nStartFlag   = 0;
				nEndFlag     = 0;
				nAbortFlag   = 1;
				nReserveFlag = 0;

				WRITE_STRING( SCHEDULER, "EVENT_ON PM1_DryCleaning_Aborted", &nCommStatus);
				_LOG("PM1_DryCleaning_Aborted");
			}

			/*
			if (( Dry_Reserved == nPM1_DryCleaning_Status) && nReserveFlag == 0)
			{
				nStartFlag   = 0;
				nEndFlag     = 0;
				nAbortFlag   = 0;
				nReserveFlag = 1;			

				_LOG("PM1_DryCleaning_Reserved");
			}
			*/
		}
	}
	_endthread();
}

void CGemMonitor::PM2_DryCleaning_Status_Check_Thread(int nThreadNo)
{
	int nCommStatus;
	int nPM2_DryCleaning_Status;
	int nStartFlag, nEndFlag, nAbortFlag, nReserveFlag;
	
	nStartFlag   = 0;
	nEndFlag     = 1;
	nAbortFlag   = 0;
	nReserveFlag = 0;
	
	while(WAIT_SEC(1))
	{
		if ( Maint != READ_DIGITAL(PM2_CommStatus, &nCommStatus))
		{
			nPM2_DryCleaning_Status = READ_DIGITAL(PM2_PRO_CHAMBER_STATUS, &nCommStatus);
			
			//_LOG("PM2 : %d, %d, %d, %d", nPM2_DryCleaning_Status, nStartFlag, nEndFlag, nAbortFlag);
			
			if (( Dry_Running == nPM2_DryCleaning_Status) && nStartFlag == 0)
			{
				nStartFlag   = 1;
				nEndFlag     = 0;
				nAbortFlag   = 0;
				nReserveFlag = 0;
				
				WRITE_STRING( SCHEDULER, "EVENT_ON PM2_DryCleaning_Start", &nCommStatus);
				_LOG("PM2_DryCleaning_Start");
			}
			
			if (( Enable == nPM2_DryCleaning_Status || Dry_Reserved == nPM2_DryCleaning_Status) && nEndFlag == 0)
			{
				
				nStartFlag   = 0;
				nEndFlag     = 1;
				nAbortFlag   = 0;
				nReserveFlag = 0;
				
				WRITE_STRING( SCHEDULER, "EVENT_ON PM2_DryCleaning_Completed", &nCommStatus);
				_LOG("PM2_DryCleaning_Completed");
			}
			
			if (( Dry_Aborted == nPM2_DryCleaning_Status) && nAbortFlag == 0)
			{
				nStartFlag   = 0;
				nEndFlag     = 0;
				nAbortFlag   = 1;
				nReserveFlag = 0;
				
				WRITE_STRING( SCHEDULER, "EVENT_ON PM2_DryCleaning_Aborted", &nCommStatus);
				_LOG("PM2_DryCleaning_Aborted");
			}
			/*
			if (( Dry_Reserved == nPM2_DryCleaning_Status) && nReserveFlag == 0)
			{
				nStartFlag   = 0;
				nEndFlag     = 0;
				nAbortFlag   = 0;
				nReserveFlag = 1;			
				
				_LOG("PM2_DryCleaning_Reserved");
			}*/
		}
	}
	_endthread();
}

void CGemMonitor::PM3_DryCleaning_Status_Check_Thread(int nThreadNo)
{
	int nCommStatus;
	int nPM3_DryCleaning_Status;
	int nStartFlag, nEndFlag, nAbortFlag, nReserveFlag;
	
	nStartFlag   = 0;
	nEndFlag     = 1;
	nAbortFlag   = 0;
	nReserveFlag = 0;
	
	while(WAIT_SEC(1))
	{
		if ( Maint != READ_DIGITAL(PM3_CommStatus, &nCommStatus))
		{
			nPM3_DryCleaning_Status = READ_DIGITAL(PM3_PRO_CHAMBER_STATUS, &nCommStatus);
			
			//_LOG("PM3 : %d, %d, %d, %d", nPM3_DryCleaning_Status, nStartFlag, nEndFlag, nAbortFlag);
			
			if (( Dry_Running == nPM3_DryCleaning_Status) && nStartFlag == 0)
			{
				nStartFlag   = 1;
				nEndFlag     = 0;
				nAbortFlag   = 0;
				nReserveFlag = 0;
				
				WRITE_STRING( SCHEDULER, "EVENT_ON PM3_DryCleaning_Start", &nCommStatus);
				_LOG("PM3_DryCleaning_Start");
			}
			
			if (( Enable == nPM3_DryCleaning_Status || Dry_Reserved == nPM3_DryCleaning_Status) && nEndFlag == 0)
			{
				
				nStartFlag   = 0;
				nEndFlag     = 1;
				nAbortFlag   = 0;
				nReserveFlag = 0;
				
				WRITE_STRING( SCHEDULER, "EVENT_ON PM3_DryCleaning_Completed", &nCommStatus);
				_LOG("PM3_DryCleaning_Completed");
			}
			
			if (( Dry_Aborted == nPM3_DryCleaning_Status) && nAbortFlag == 0)
			{
				nStartFlag   = 0;
				nEndFlag     = 0;
				nAbortFlag   = 1;
				nReserveFlag = 0;
				
				WRITE_STRING( SCHEDULER, "EVENT_ON PM3_DryCleaning_Aborted", &nCommStatus);
				_LOG("PM3_DryCleaning_Aborted");
			}
			/*
			if (( Dry_Reserved == nPM3_DryCleaning_Status) && nReserveFlag == 0)
			{
				nStartFlag   = 0;
				nEndFlag     = 0;
				nAbortFlag   = 0;
				nReserveFlag = 1;			
				
				_LOG("PM3_DryCleaning_Reserved");
			}*/
		}
	}
	_endthread();
}

