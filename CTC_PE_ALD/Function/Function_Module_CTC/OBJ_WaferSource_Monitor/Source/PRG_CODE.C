#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"
#include "IPS_Log.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {

	{ "CTC.PM1_Wafer_Source"					, _K_D_IO	,	CTC_PM1_Wafer_Source                ,	0	} ,
	{ "CTC.PM1_Wafer_Source2"					, _K_D_IO	,	CTC_PM1_Wafer_Source2               ,	0	} ,
	{ "PM1.Wafer_Source"						, _K_D_IO	,	PM1_Wafer_Source                    ,	0	} ,
	{ "PM1.Wafer_Source2"						, _K_D_IO	,	PM1_Wafer_Source2                   ,	0	} ,
	{ "CTC.PM1_Wafer_Status"					, _K_D_IO	,	CTC_PM1_Wafer_Status                ,	0	} ,
	{ "CTC.PM1_Wafer_Status2"					, _K_D_IO	,	CTC_PM1_Wafer_Status2               ,	0	} ,
	{ "PM1.Wafer_Status"						, _K_D_IO	,	PM1_Wafer_Status                    ,	0	} ,
	{ "PM1.Wafer_Status2"						, _K_D_IO	,	PM1_Wafer_Status2                   ,	0	} ,


	{ "CTC.PM2_Wafer_Source"					, _K_D_IO	,	CTC_PM2_Wafer_Source                ,	0	} ,
	{ "CTC.PM2_Wafer_Source2"					, _K_D_IO	,	CTC_PM2_Wafer_Source2               ,	0	} ,
	{ "PM2.Wafer_Source"						, _K_D_IO	,	PM2_Wafer_Source                    ,	0	} ,
	{ "PM2.Wafer_Source2"						, _K_D_IO	,	PM2_Wafer_Source2                   ,	0	} ,

	{ "CTC.PM2_Wafer_Status"					, _K_D_IO	,	CTC_PM2_Wafer_Status                ,	0	} ,
	{ "CTC.PM2_Wafer_Status2"					, _K_D_IO	,	CTC_PM2_Wafer_Status2               ,	0	} ,
	{ "PM2.Wafer_Status"						, _K_D_IO	,	PM2_Wafer_Status                    ,	0	} ,
	{ "PM2.Wafer_Status2"						, _K_D_IO	,	PM2_Wafer_Status2                   ,	0	} ,


	{ "CTC.PM3_Wafer_Source"					, _K_D_IO	,	CTC_PM3_Wafer_Source                ,	0	} ,
	{ "CTC.PM3_Wafer_Source2"					, _K_D_IO	,	CTC_PM3_Wafer_Source2               ,	0	} ,
	{ "PM3.Wafer_Source"						, _K_D_IO	,	PM3_Wafer_Source                    ,	0	} ,
	{ "PM3.Wafer_Source2"						, _K_D_IO	,	PM3_Wafer_Source2                   ,	0	} ,

	{ "CTC.PM3_Wafer_Status"					, _K_D_IO	,	CTC_PM3_Wafer_Status                ,	0	} ,
	{ "CTC.PM3_Wafer_Status2"					, _K_D_IO	,	CTC_PM3_Wafer_Status2               ,	0	} ,
	{ "PM3.Wafer_Status"						, _K_D_IO	,	PM3_Wafer_Status                    ,	0	} ,
	{ "PM3.Wafer_Status2"						, _K_D_IO	,	PM3_Wafer_Status2                   ,	0	} ,


	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void PM1_Wafer_Source_Check_Thread()
{
	int nCommStatus;

	int nWaferSourceID, nWaferSourceID2;
	int nWaferStatus, nWaferStatus2;
	
	while(1)
	{
		nWaferSourceID  = READ_DIGITAL(CTC_PM1_Wafer_Source ,  &nCommStatus);
		nWaferSourceID2 = READ_DIGITAL(CTC_PM1_Wafer_Source2 , &nCommStatus);
		nWaferStatus    = READ_DIGITAL(CTC_PM1_Wafer_Status  , &nCommStatus);
		nWaferStatus2   = READ_DIGITAL(CTC_PM1_Wafer_Status2 , &nCommStatus);

		//WAIT_SECONDS(0.1);
		//... 2017.03.07
		Sleep(100);

		WRITE_DIGITAL(PM1_Wafer_Source,  nWaferSourceID,  &nCommStatus);
		WRITE_DIGITAL(PM1_Wafer_Source2, nWaferSourceID2, &nCommStatus);
		WRITE_DIGITAL(PM1_Wafer_Status,  nWaferStatus,  &nCommStatus);
		WRITE_DIGITAL(PM1_Wafer_Status2, nWaferStatus2, &nCommStatus);
	}
	_endthread();
}

void PM2_Wafer_Source_Check_Thread()
{
	int nCommStatus;

	int nWaferSourceID, nWaferSourceID2;
	int nWaferStatus, nWaferStatus2;
	
	while(1)
	{
		nWaferSourceID  = READ_DIGITAL(CTC_PM2_Wafer_Source,  &nCommStatus);
		nWaferSourceID2 = READ_DIGITAL(CTC_PM2_Wafer_Source2, &nCommStatus);
		nWaferStatus    = READ_DIGITAL(CTC_PM2_Wafer_Status  , &nCommStatus);
		nWaferStatus2   = READ_DIGITAL(CTC_PM2_Wafer_Status2 , &nCommStatus);

		//WAIT_SECONDS(0.1);
		//... 2017.03.07
		Sleep(100);

		WRITE_DIGITAL(PM2_Wafer_Source,  nWaferSourceID,  &nCommStatus);
		WRITE_DIGITAL(PM2_Wafer_Source2, nWaferSourceID2, &nCommStatus);
		WRITE_DIGITAL(PM2_Wafer_Status,  nWaferStatus,  &nCommStatus);
		WRITE_DIGITAL(PM2_Wafer_Status2, nWaferStatus2, &nCommStatus);
	}
	_endthread();
}

void PM3_Wafer_Source_Check_Thread()
{
	int nCommStatus;

	int nWaferSourceID, nWaferSourceID2;
	int nWaferStatus, nWaferStatus2;
	
	while(1)
	{
		nWaferSourceID  = READ_DIGITAL(CTC_PM3_Wafer_Source,  &nCommStatus);
		nWaferSourceID2 = READ_DIGITAL(CTC_PM3_Wafer_Source2, &nCommStatus);
		nWaferStatus    = READ_DIGITAL(CTC_PM3_Wafer_Status  , &nCommStatus);
		nWaferStatus2   = READ_DIGITAL(CTC_PM3_Wafer_Status2 , &nCommStatus);

		//WAIT_SECONDS(0.1);
		//... 2017.03.07
		Sleep(100);

		WRITE_DIGITAL(PM3_Wafer_Source,  nWaferSourceID,  &nCommStatus);
		WRITE_DIGITAL(PM3_Wafer_Source2, nWaferSourceID2, &nCommStatus);
		WRITE_DIGITAL(PM3_Wafer_Status,  nWaferStatus,  &nCommStatus);
		WRITE_DIGITAL(PM3_Wafer_Status2, nWaferStatus2, &nCommStatus);
	}
	_endthread();
}



//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	_beginthread((void*)PM1_Wafer_Source_Check_Thread, 0, NULL);

	_beginthread((void*)PM2_Wafer_Source_Check_Thread, 0, NULL);

	_beginthread((void*)PM3_Wafer_Source_Check_Thread, 0, NULL);

	return SYS_SUCCESS;
}
