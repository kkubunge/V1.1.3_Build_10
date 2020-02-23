///////////////////////////////////////////////////////////////////////////////
// PM Thickness Fault Interlock
// Create : 2014.10.15

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <string.h>

//==========================================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//==========================================================================================

//////////////////////////////////////////////////////////////////////////
#define FUNC_NAME	"PM_ThickInterlock"
#define MAX_PM		3

//////////////////////////////////////////////////////////////////////////
#define ALARM_PM1_THICK_OVER_FAULT			3553
#define ALARM_PM2_THICK_OVER_FAULT			3653
#define ALARM_PM3_THICK_OVER_FAULT			3753

//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {

	//... Scheduler for PM Disable
	{ "SCHEDULER"			, _K_F_IO	,	F_SCHEDULER,			0	} ,

	//... PM Status
	{ "CTC.PM1_CommStatus"	, _K_D_IO	,	PM1_CommStatus,			0	} ,
	{ "CTC.PM2_CommStatus"	, _K_D_IO	,	PM2_CommStatus,			0	} ,
	{ "CTC.PM3_CommStatus"	, _K_D_IO	,	PM3_CommStatus,			0	} ,

	//... PM Slot Valve Status for Protect Slot Valve Opened Maint
	{ "TM.PM1.SlotVv1XI"	, _K_D_IO	,	PM1_SlotVv1XI,			0	} ,
	{ "TM.PM2.SlotVv1XI"	, _K_D_IO	,	PM2_SlotVv1XI,			0	} ,
	{ "TM.PM3.SlotVv1XI"	, _K_D_IO	,	PM3_SlotVv1XI,			0	} ,

	//... Preventive Thickness Enable Option
	{ "PRMD_PM1_DRYEn_04"	, _K_D_IO	,	PRMD_PM1_DRYEn_04,		0	} ,
	{ "PRMD_PM2_DRYEn_04"	, _K_D_IO	,	PRMD_PM2_DRYEn_04,		0	} ,
	{ "PRMD_PM3_DRYEn_04"	, _K_D_IO	,	PRMD_PM3_DRYEn_04,		0	} ,

	//... Preventive Thickness Fault Alarm
	{ "PRMA_PM1_DRYFThick"	, _K_A_IO	,	PRMA_PM1_DRYFThick,		0	} ,
	{ "PRMA_PM2_DRYFThick"	, _K_A_IO	,	PRMA_PM2_DRYFThick,		0	} ,
	{ "PRMA_PM3_DRYFThick"	, _K_A_IO	,	PRMA_PM3_DRYFThick,		0	} ,

	//... Preventive Thickness Current
	{ "PRMA_PM1_DRYCThick"	, _K_A_IO	,	PRMA_PM1_DRYCThick,		0	} ,
	{ "PRMA_PM2_DRYCThick"	, _K_A_IO	,	PRMA_PM2_DRYCThick,		0	} ,
	{ "PRMA_PM3_DRYCThick"	, _K_A_IO	,	PRMA_PM3_DRYCThick,		0	} ,

	""
};

//////////////////////////////////////////////////////////////////////////
//... Define Enum
enum { 
	PMCOMM_UNUSE,
	PMCOMM_LOCAL,
	PMCOMM_DISCONN,
	PMCOMM_STANDBY,
	PMCOMM_MAINT,
	PMCOMM_UNINIT,
	PMCOMM_ENABLEPM,
	PMCOMM_CTCINUSE,
	PMCOMM_CTCDIS,
	PMCOMM_CTCDIS2,
	PMCOMM_CTCENAPM,
	PMCOMM_DISHW,
	PMCOMM_DISHW2
};

enum { 
	enOPT_DISABLE,
	enOPT_ENABLE
};

enum { 
	enSLOTVV_UNKNOWN,
	enSLOTVV_OPEN,
	enSLOTVV_CLOSE,
	enSLOTVV_ERROR
};

//////////////////////////////////////////////////////////////////////////
void StartMonitor()
{
	int		nIOStatus;
	int		i = 0;
	int		nPM_CommStatus	= PMCOMM_UNUSE;
	int		nEnableOpt		= enOPT_DISABLE;
	int		nAlarmID		= ALARM_PM1_THICK_OVER_FAULT;
	int		nPMSlotVvSts	= enSLOTVV_UNKNOWN;
	
	char	szEvent[256] = "";

	double	dbThickCurrent	= 0;
	double	dbThickLimit	= 0;

	//...
	while(1)
	{
		Sleep(2000);

		//////////////////////////////////////////////////////////////////////////
		for(i = 0; i < MAX_PM; i++)
		{
			Sleep(1000);

			//... Read PM Status
			nPM_CommStatus = READ_DIGITAL(PM1_CommStatus + i, &nIOStatus);

			//... Check Skip Case
			if( PMCOMM_UNUSE   == nPM_CommStatus || PMCOMM_LOCAL  == nPM_CommStatus ||
				PMCOMM_DISCONN == nPM_CommStatus || PMCOMM_UNINIT == nPM_CommStatus)
			{
				continue;		// Check Skip
			}

			//... Check Preventive Option
			nEnableOpt = READ_DIGITAL(PRMD_PM1_DRYEn_04 + i, &nIOStatus);
			if(enOPT_DISABLE == nEnableOpt)			continue;		// Check Skip

			//... Check Preventive Thickness Limit
			dbThickLimit = READ_ANALOG(PRMA_PM1_DRYFThick + i, &nIOStatus);
			if(dbThickLimit <= 0 )				continue;		// Check Skip

			//... Check Preventive Thickness Current
			dbThickCurrent = READ_ANALOG(PRMA_PM1_DRYCThick + i, &nIOStatus);
			if(dbThickCurrent >= dbThickLimit)
			{	//... Case Limit Over

				//... Fixed Alarm Define
				if     (i == 0)		nAlarmID = ALARM_PM1_THICK_OVER_FAULT;
				else if(i == 1)		nAlarmID = ALARM_PM2_THICK_OVER_FAULT;
				else if(i == 2)		nAlarmID = ALARM_PM3_THICK_OVER_FAULT;
				else				nAlarmID = ALARM_PM1_THICK_OVER_FAULT;

				//... Alarm Post (Clear Type)
				if(ALM_PAUSED != ALARM_STATUS(nAlarmID))	ALARM_POST(nAlarmID);

				
				//... PM forced Change Disable
				if( PMCOMM_STANDBY  == nPM_CommStatus || PMCOMM_ENABLEPM == nPM_CommStatus ||
					PMCOMM_CTCINUSE == nPM_CommStatus || PMCOMM_CTCENAPM == nPM_CommStatus)
				{
					//... Change PM Disable Mode in PM Slot Valve Opened, Slot Valve Not Closed Bug.
					nPMSlotVvSts = READ_DIGITAL(PM1_SlotVv1XI + i, &nIOStatus);
					if(enSLOTVV_CLOSE == nPMSlotVvSts)
					{
						memset(szEvent, 0, strlen(szEvent));
						sprintf(szEvent, "SET_MODULE_INFO_LOCAL PM%d|DISABLE", i+1);
						WRITE_FUNCTION_EVENT(F_SCHEDULER, szEvent);
						
						//..
						printf("[INFO] PM%d Thickness Over Fault Alarm Opt[%d] Cur[%.0f] Lmt[%.0f] Com[%d]\n", 
							i+1, nEnableOpt, dbThickCurrent, dbThickLimit, nPM_CommStatus);
						//...
						Sleep(2000);	// Mode Change Interval
					}
				}
			}
		}
	}
}

//==========================================================================================
extern "C"
{

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;

	printf("Entered %s\n", FUNC_NAME);

	StartMonitor();

	printf("Leave %s\n", FUNC_NAME);

	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}

} // End of [extern "C"]
//==========================================================================================


