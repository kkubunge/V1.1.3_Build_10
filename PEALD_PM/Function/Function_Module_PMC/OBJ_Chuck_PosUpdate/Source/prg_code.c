#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

#include <Kutlstr.h>
#include <Kutltime.h>
#include "kutletc.h"
#include "kutlanal.h"

#include "cimseqnc.h"
#include "iodefine.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define	CHUCK_HOME_POSITION			89
#define	CHUCK_VALID_POSITION		85	// 70 - 4
#define	CHUCK_S_POSITION			4	// New Define, For MAHA HPIII

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum	{ OFF_0	, ON_1		}; 
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE	= FALSE;

//------------------------------------------------------------------------------------------
#define		ALARM_CHUCK_MOTOR_ERROR		2501


//------------------------------------------------------------------------------------------
int	ALARM_START_POINTER = 0;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_DRIVER_LOAD			,	0	},
	
	//... User Config Parameter
	{ "PRMA_ZMT_Pulse_mm"		, _K_A_IO	,	PRMA_ZMT_Pulse_mm		,	0	},
	{ "PRMA_ZMT_Offset"			, _K_A_IO	,	PRMA_ZMT_Offset			,	0	},
	{ "PRMA_ZMT_Offset2"		, _K_A_IO	,	PRMA_ZMT_Offset2		,	0	},
	{ "PRMA_ZMT_OffsIgLow"		, _K_A_IO	,	PRMA_ZMT_OffsIgLow 		,	0	},
	{ "PRMA_ZMT_OffsIgHigh"		, _K_A_IO	,	PRMA_ZMT_OffsIgHigh 	,	0	},

	//... Function Virtual IO
	{ "ZMT_ReadPosAM"			, _K_A_IO	,	ZMT_ReadPosAM			,	0	},
	{ "ZMT_ReadPos2AM"			, _K_A_IO	,	ZMT_ReadPos2AM			,	0	},

	//... Motor IO
	{ "ROR1_Motor1_Position"	, _K_A_IO	,	R_Motor1_Position		,	0	},
	{ "ROR1_Motor2_Position"	, _K_A_IO	,	R_Motor2_Position		,	0	},


	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
double READ_ANALOG_TRUE(int io, int *CommStatus)
{
	double Data;
	
	Data = READ_ANALOG(io, CommStatus);
	if(!(*CommStatus)) {
		_sleep(50);
		Data = READ_ANALOG(io, CommStatus);
		if(!(*CommStatus)) {
			_sleep(50);
			Data = READ_ANALOG(io, CommStatus);
			if(!(*CommStatus)) {
				_sleep(50);
				return(READ_ANALOG(io, CommStatus));
			}
			else return Data;
		}
		else return Data;
	}
	else return Data;

}

//////////////////////////////////////////////////////////////////////////
void ALARM_MESSAGE_REMAPPING_CHUCK_ERR(int id) {
	char Buffer[256], Buffer2[256];
	
	sprintf(Buffer2, " : Motor Position Reading Fail");

	ALARM_MESSAGE_GET(id, Buffer);
	strcat(Buffer, Buffer2);
	ALARM_MESSAGE_SET(id, Buffer);
}

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {

	int CommStatus;

	double	dbData = 0;
	double	dbMotor1Pos_Pulse = 0;
	double	dbMotor1Pos_mm = 0;
	double	dbMotor1Pos_ProcGap = 0;

	double	dbMotor2Pos_Pulse = 0;
	double	dbMotor2Pos_mm = 0;
	double	dbMotor2Pos_ProcGap = 0;

	double	dbOffsetIgnoreLow = 0;
	double	dbOffsetIgnoreHigh = 0;
	double	dbMotor1Offset = 0;
	double	dbMotor2Offset = 0;
	double	dbPulsePerMM = 0;
	
	_sleep(5000);

	while(TRUE) 
	{
		//////////////////////////////////////////////////////////////////////////
		//... Update User Config Offset Ignore Band & Offset
		dbOffsetIgnoreLow	= READ_ANALOG(PRMA_ZMT_OffsIgLow,	&CommStatus);
		dbOffsetIgnoreHigh	= READ_ANALOG(PRMA_ZMT_OffsIgHigh,	&CommStatus);
		dbMotor1Offset		= READ_ANALOG(PRMA_ZMT_Offset,		&CommStatus);
		dbMotor2Offset		= READ_ANALOG(PRMA_ZMT_Offset2,		&CommStatus);
		dbPulsePerMM		= READ_ANALOG(PRMA_ZMT_Pulse_mm,	&CommStatus);
		
		//////////////////////////////////////////////////////////////////////////
		//... Motor 1 Side
		//... Read Motor Position
		dbData = READ_ANALOG_TRUE(R_Motor1_Position, &CommStatus);
		if(CommStatus == FALSE)
		{
			if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
			{
				ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR);
				ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
			}
		}
		else	dbMotor1Pos_Pulse = dbData;

		//////////////////////////////////////////////////////////////////////////
		//... Convert pulse -> mm
		dbMotor1Pos_mm = dbMotor1Pos_Pulse / dbPulsePerMM;
		dbMotor1Pos_ProcGap = CHUCK_HOME_POSITION - dbMotor1Pos_mm;

		if(dbMotor1Pos_ProcGap >= dbOffsetIgnoreLow && 
			dbMotor1Pos_mm <= dbOffsetIgnoreHigh)
		{	// Offset Ignore Band
			dbMotor1Offset = 0;	
		}
		
		if(dbMotor1Pos_mm != 0)			dbMotor1Pos_ProcGap = dbMotor1Pos_ProcGap + dbMotor1Offset; 

		if(dbMotor1Pos_ProcGap == 0)	WRITE_ANALOG(ZMT_ReadPosAM, CHUCK_HOME_POSITION, &CommStatus);
 		else							WRITE_ANALOG(ZMT_ReadPosAM, dbMotor1Pos_ProcGap, &CommStatus);
		
		//////////////////////////////////////////////////////////////////////////
		//... Motor 2 Side
		//... Read Motor Position
		dbData = READ_ANALOG_TRUE(R_Motor2_Position, &CommStatus);
		if(CommStatus == FALSE)
		{
			if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
			{
				ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR);
				ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
			}
		}
		else	dbMotor2Pos_Pulse = dbData;

		//////////////////////////////////////////////////////////////////////////
		//... Convert pulse -> mm
		dbMotor2Pos_mm = dbMotor2Pos_Pulse / dbPulsePerMM;
		dbMotor2Pos_ProcGap = CHUCK_HOME_POSITION - dbMotor2Pos_mm;

		if(dbMotor2Pos_ProcGap >= dbOffsetIgnoreLow && 
			dbMotor2Pos_mm <= dbOffsetIgnoreHigh)
		{	// Offset Ignore Band
			dbMotor2Offset = 0;	
		}
		
		if(dbMotor2Pos_mm != 0)			dbMotor2Pos_ProcGap = dbMotor2Pos_ProcGap + dbMotor2Offset; 
		
		if(dbMotor2Pos_ProcGap == 0)	WRITE_ANALOG(ZMT_ReadPos2AM, CHUCK_HOME_POSITION, &CommStatus);
		else							WRITE_ANALOG(ZMT_ReadPos2AM, dbMotor2Pos_ProcGap, &CommStatus);

		_sleep(500);
	}

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}

//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	int CommStatus;

	if(READ_DIGITAL(IO_DRIVER_LOAD, &CommStatus) == OFF_0)	SIMULATION_MODE = TRUE;
	else													SIMULATION_MODE = FALSE;

	return TRUE;
}

