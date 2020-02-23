#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "kutlanal.h"  //Alarm통합관련 API를 위해 추가.

#include "cimseqnc.h"
#include "iodefine.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum	{ OFF_0	, ON_1		}; 
enum	{ NOT_0	, HOME_1	};	
enum	{ FAST_1, SLOW_1, FAST_2, SLOW_2, FAST_ALL, SLOW_ALL	};
enum	{ MOTOR_1, MOTOR_2, MOTOR_ALL };
enum	{ CommErr_0, RunTMO_1, MotorErr_2, StandbyFail_3 };
enum	{ STOP_0,	RUN_1	};
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE	= FALSE;

//------------------------------------------------------------------------------------------
#define		ALARM_CHUCK_MOTOR_ERROR			2501
#define		ALARM_HOME_ALL_TIMEOUT			2502
#define		ALARM_MOVE_ALL_TIMEOUT			2503

#define		ALARM_HOME_1_TIMEOUT			2511
#define		ALARM_MOVE_1_TIMEOUT			2512
#define		ALARM_INVALID_1_POSITION		2513
#define		ALARM_HOMEPOSION_OUT1			2514
#define		ALARM_MOTOR1_MOVE_INTLK			2515

#define		ALARM_HOME_2_TIMEOUT			2521
#define		ALARM_MOVE_2_TIMEOUT			2522
#define		ALARM_INVALID_2_POSITION		2523
#define		ALARM_HOMEPOSION_OUT2			2524
#define		ALARM_MOTOR2_MOVE_INTLK			2525

//... 2015.03.20 Add Need Home Virtual IO Add for MC OFF Case
#define		ALARM_MOTOR_ALL_NEED_HOME		2530

//------------------------------------------------------------------------------------------
#define		HOME_POSITION					89
#define		VALID_POSITION					85	// 55 - 4
#define		S_POSITION						4	// New Define, For MAHA HPIII

//------------------------------------------------------------------------------------------
//20061108 KSM ADD : Alarm통합 관련 변수 추가.
int	ALARM_START_POINTER = 0;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_DRIVER_LOAD			,	0	},
	
	//... User Config Parameter
	{ "FC_MOTOR_CONTROL"		, _K_D_IO	,	FC_MOTOR_CONTROL		,	0	},
	{ "PRMA_ZMT_CtrlTmOut"		, _K_A_IO	,	PRMA_ZMT_CtrlTmOut		,	0	},	
	{ "PRMA_ZMT_ErrTol"			, _K_A_IO	,	PRMA_ZMT_ErrTol			,	0	},
	{ "PRMA_ZMT_HmHighLmt"		, _K_A_IO	,	PRMA_ZMT_HmHighLmt		,	0	},
	{ "PRMA_ZMT_HmLowLmt"		, _K_A_IO	,	PRMA_ZMT_HmLowLmt		,	0	},
	{ "PRMA_ZMT_Pulse_mm"		, _K_A_IO	,	PRMA_ZMT_Pulse_mm		,	0	},
	{ "PRMA_ZMT_Offset"			, _K_A_IO	,	PRMA_ZMT_Offset			,	0	},
	{ "PRMA_ZMT_Offset2"		, _K_A_IO	,	PRMA_ZMT_Offset2		,	0	},
	{ "PRMA_ZMT_OffsIgLow"		, _K_A_IO	,	PRMA_ZMT_OffsIgLow 		,	0	},
	{ "PRMA_ZMT_OffsIgHigh"		, _K_A_IO	,	PRMA_ZMT_OffsIgHigh 	,	0	},
	{ "PRMA_ZMT_NormSpeed"		, _K_A_IO	,	PRMA_ZMT_NormSpeed		,	0	},

	//... Function Virtual IO
	{ "ZMT_SetPosAM"			, _K_A_IO	,	ZMT_SetPosAM			,	0	},
	{ "ZMT_SetPos2AM"			, _K_A_IO	,	ZMT_SetPos2AM			,	0	},

	//... Motor IO
	{ "ROR1_CommStat"			, _K_D_IO	,	R_Motor_CommStat		,	0	},
	{ "ROR1_Motor_Stop"			, _K_D_IO	,	R_Motor_Stop			,	0	},
	{ "ROR1_Motor_Fast1_Home"	, _K_D_IO	,	R_Motor_Fast1_Home		,	0	},
	{ "ROR1_Motor_Move_Home"	, _K_D_IO	,   R_Motor_Move_Home		,	0	},
	{ "ROR1_Motor_Move_ABS"		, _K_D_IO	,	R_Motor_Move_ABS		,	0	},
	{ "ROR1_Motor_Status_DI"	, _K_D_IO	,	R_Motor_Status_DI		,	0	},

	{ "ROR1_Move_Pulse_Set"		, _K_A_IO	,	R_Move_Pulse_Set		,	0	},
	{ "ROR1_Motor1_Position"	, _K_A_IO	,	R_Motor1_Position		,	0	},
	{ "ROR1_Motor2_Position"	, _K_A_IO	,	R_Motor2_Position		,	0	},

	{ "ROR1_All_Move_Pulse_Set"	, _K_A_IO	,	R_All_Move_Pulse_Set	,	0	},
	{ "ROR1_Motor_Move_All"		, _K_D_IO	,	R_Motor_Move_All		,	0	},
	{ "ROR1_Pulse_Set_String"	, _K_S_IO	,	R_Pulse_Set_String		,	0	},
	{ "ROR1_Position8_Pulse"	, _K_A_IO	,	R_Position8_Pulse		,	0	},
	{ "ROR1_Position9_Pulse"	, _K_A_IO	,	R_Position9_Pulse		,	0	},
	
	{ "ROR1_Motor1_High_Speed"	, _K_A_IO	,	R_Motor1_High_Speed 	,	0	},
	{ "ROR1_Motor2_High_Speed"	, _K_A_IO	,	R_Motor2_High_Speed 	,	0	},

	//... DeviceNet IO
	{ "ZMT_HomeSnsDI"			, _K_D_IO	,	ZMT_HomeSnsDI			,	0	},
	{ "ZMT2_HomeSnsDI"			, _K_D_IO	,	ZMT2_HomeSnsDI			,	0	},

	//... Reference Virtual IO
	{ "PRO_RunStsDM"			, _K_D_IO	,	PRO_RunStsDM			,	0	},
	{ "PMC_EVENT_LOG_CLI"		, _K_F_IO	,	PMC_EVENT_LOG_CLI		,	0	} ,				// 2014.10.09

	//... 2015.02.28 Add Chuck Move Cycle for MAHA-DPT MFG TFT
	{ "M_MT_CycMvSelDM"			, _K_D_IO	,	M_MT_CycMvSelDM			,	0	},
	{ "M_MT_CycMvPosAM"			, _K_A_IO	,	M_MT_CycMvPosAM			,	0	},
	{ "M_MT_CycMvTgtCntAM"		, _K_A_IO	,	M_MT_CycMvTgtCntAM		,	0	},
	//
	{ "M_MT_CycMvCurCntAM"		, _K_A_IO	,	M_MT_CycMvCurCntAM		,	0	},
	{ "M_MT_CycMvUpTmAM"		, _K_A_IO	,	M_MT_CycMvUpTmAM		,	0	},
	{ "M_MT_CycMvDnTmAM"		, _K_A_IO	,	M_MT_CycMvDnTmAM		,	0	},
	//
	{ "M_MT_ParmSaveDM"			, _K_D_IO	,	M_MT_ParmSaveDM			,	0	},
	
	//... 2015.03.20 Add Need Home Virtual IO Add for MC OFF Case (1:On ... Need Home)
	{ "ROR1_NeedHomeDM"			, _K_D_IO	,	R_Motor_NeedHomeDM		,	0	},
	//... 2016.01.19
	{ "ALECHK_DynCntDM"			, _K_D_IO	,	ALECHK_DynCntDM			,	0	},
	{ "Chuck_UpPos_AM"			, _K_A_IO	,	CHUCK_UPPOS_AM			,	0	},
	{ "Chuck_DnPos_AM"			, _K_A_IO	,	CHUCK_DOWNPOS_AM		,	0	},
	{ "LeakChkStsDM"			, _K_D_IO	,	LeakChkStsDM			,	0	},
	""
};
//------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Global Function
// 2014.10.09
extern void _MLOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);	
	WRITE_FUNCTION_EVENT(PMC_EVENT_LOG_CLI, _szString);
}

void LogEvent(char *szRunStr, char *szStatus, char *szData)
{
	char szEventID[20];

	do {
		if(STRCMP_L(szRunStr , "STAGE1_UP")) strcpy(szEventID, "Stage1Up");
		else if(STRCMP_L(szRunStr , "STAGE1_DOWN")) strcpy(szEventID, "Stage1Down");
		else if(STRCMP_L(szRunStr , "STAGE1_SETPOS")) strcpy(szEventID, "Stage1SetPosition");
		else if(STRCMP_L(szRunStr , "STAGE1_LOADPOS")) strcpy(szEventID, "Stage1LoadPosition");		//2013.11.20
		else if(STRCMP_L(szRunStr , "STAGE2_UP")) strcpy(szEventID, "Stage2Up");
		else if(STRCMP_L(szRunStr , "STAGE2_DOWN")) strcpy(szEventID, "Stage2Down");
		else if(STRCMP_L(szRunStr , "STAGE2_SETPOS")) strcpy(szEventID, "Stage2SetPosition");
		else if(STRCMP_L(szRunStr , "STAGE2_LOADPOS")) strcpy(szEventID, "Stage2LoadPosition");		//2013.11.20
		else
		{
			_MLOG("-----> %s %s %s", szRunStr, szStatus, szData);
			break;
		}
		_MLOG("FNC %s %s %s", szEventID, szStatus, szData);
	} while(0);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int READ_DIGITAL_TRUE(int io, int *CommStatus) {  
	int Data;
	Data = READ_DIGITAL(io, CommStatus);
	if(!(*CommStatus)) {
		_sleep(100);
		Data = READ_DIGITAL(io, CommStatus);
		if(!(*CommStatus)) {
			_sleep(100);
			Data = READ_DIGITAL(io, CommStatus);
			if(!(*CommStatus)) {
				_sleep(100);
				return(READ_DIGITAL(io, CommStatus));
			}
			else return Data;
		}
		else return Data;
	}
	else return Data;
}


//------------------------------------------------------------------------------------------
//-->ADD 2012/03/13 CMD Response Check by JTM
//------------------------------------------------------------------------------------------
void WRITE_ANALOG_TRUE(int io, double Value, int *CommStatus)
{ 
	WRITE_ANALOG(io, Value, CommStatus);

	if(!(*CommStatus))
	{
		_sleep(100);
		WRITE_ANALOG(io, Value, CommStatus);

		if(!(*CommStatus)) 
		{
			_sleep(100);
			WRITE_ANALOG(io, Value, CommStatus);

			if(!(*CommStatus))
			{
				_sleep(100);
				WRITE_ANALOG(io, Value, CommStatus);
			}		
		}	
	}	
}

void WRITE_DIGITAL_TRUE(int io, int Value, int *CommStatus)
{ 
	WRITE_DIGITAL(io, Value, CommStatus);
	
	if(!(*CommStatus))
	{
		_sleep(100);
		WRITE_DIGITAL(io, Value, CommStatus);
		
		if(!(*CommStatus)) 
		{
			_sleep(100);
			WRITE_DIGITAL(io, Value, CommStatus);
			
			if(!(*CommStatus))
			{
				_sleep(100);
				WRITE_DIGITAL(io, Value, CommStatus);
			}		
		}	
	}	
}

//------------------------------------------------------------------------------------------
void ALARM_MESSAGE_REMAPPING(int id, double  ErrValue, double Lowband, double Hiband, BOOL Setting) {
	char Buffer[256], Buffer2[256], Buffer3[256];

	if(Setting){
		sprintf(Buffer2, "(ErrValue = %.1f)", ErrValue);
		sprintf(Buffer3, "/(SetRange = %.1f ~ %.1f)", Lowband, Hiband);
	}else{
		sprintf(Buffer2, "(Err[%.0f]", ErrValue);
		sprintf(Buffer3, "/Range[%.0f~%.0f])", Lowband, Hiband);
	}


	ALARM_MESSAGE_GET(id, Buffer);
	strcat(Buffer2, Buffer3);
	strcat(Buffer, Buffer2);
	ALARM_MESSAGE_SET(id, Buffer);
}

void ALARM_MESSAGE_REMAPPING2(int id, int motor, double ErrValue, double OffSet, double Offset_Ignore_Low)
{
	char Buffer[256];
	memset(Buffer, 0, sizeof(Buffer));

	if(motor == MOTOR_1)
		sprintf(Buffer, "Invalid ChuckA Position Error -(ErrValue=%.2f)/(Valid Range:%d~%d, Exception:%.2f~%.2f)", ErrValue, HOME_POSITION, S_POSITION,  Offset_Ignore_Low -0.01, Offset_Ignore_Low + OffSet);
	else if(motor == MOTOR_2)
		sprintf(Buffer, "Invalid ChuckB Position Error -(ErrValue=%.2f)/(Valid Range:%d~%d, Exception:%.2f~%.2f)", ErrValue, HOME_POSITION, S_POSITION,  Offset_Ignore_Low -0.01, Offset_Ignore_Low + OffSet);
	
	ALARM_MESSAGE_SET(id, Buffer);
}

void ALARM_MESSAGE_REMAPPING_CHUCK_ERR(int id,  int ErrorType, char ErrorDesc) {
	char Buffer[256], Buffer2[256];
	
	if(ErrorType == CommErr_0)
		sprintf(Buffer2, " : Motor Status Reading Fail");
	else if(ErrorType == RunTMO_1)
		sprintf(Buffer2, " : Chuck Motor is Running(Timeout Over)");
	else if(ErrorType == MotorErr_2)
		sprintf(Buffer2, " : See The Detail Inform(Error Code : %0X)", ErrorDesc);
	else if(ErrorType == StandbyFail_3)
		sprintf(Buffer2, " : StandBy Check Fail");

	
	ALARM_MESSAGE_GET(id, Buffer);
	strcat(Buffer, Buffer2);
	ALARM_MESSAGE_SET(id, Buffer);
}

//////////////////////////////////////////////////////////////////////////
//... 2015.03.20 Add Need Home Virtual IO Add for MC OFF Case
BOOL NeedHome_AllMotor()
{
	BOOL	bRet = FALSE;
	int		nIOStatus;

	if(ON_1 == READ_DIGITAL(R_Motor_NeedHomeDM, &nIOStatus))
	{
		printf("[INFO] Chuck Motor Need HOME\n");
		bRet = TRUE;
	}

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status ChuckMotor_Standby_Check()
{
	char Motor_Status;
	int CommStatus;
	double	dbCtrlTimeout = 0;

	dbCtrlTimeout = READ_ANALOG(PRMA_ZMT_CtrlTmOut, &CommStatus);

	S_TIMER_READY();

	while(TRUE)
	{
		Motor_Status = READ_DIGITAL_TRUE(R_Motor_Status_DI, &CommStatus);
		if(CommStatus == FALSE)
		{
			ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
			ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
			return SYS_ABORTED;
		}
		
		if(Motor_Status == 0x00) //Idle
		{
			break;
		}
		else if(Motor_Status == 0x01) //Running
		{
			if(S_TIMER_CHECK(1, dbCtrlTimeout))
			{
				ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, RunTMO_1, 0);
				ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
				return SYS_ABORTED;
			}
		}
		else if(Motor_Status >= 0x02 && Motor_Status <= 0x0f)	// Motor Error
		{
			ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, MotorErr_2, Motor_Status);
			ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
			return SYS_ABORTED;
		}

		if(S_TIMER_CHECK(1, dbCtrlTimeout + 5))
		{
			ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, StandbyFail_3, 0);
			ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
			return SYS_ABORTED;
		}

		if(! WAIT_SECONDS(0.1))		return SYS_ABORTED;
	}

	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
Module_Status STOP(int Motor) {
	int CommStatus;
	
	// 0 : Fast_Stop Motor1		 1 : Slow_Stop Motor1  
	// 2 : Fast_Stop Motor2		 3 : Slow_Stop Motor2
	// 4 : Fast_Stop Motor All   5 : Slow_Stop Motor All 
	if(Motor == MOTOR_1) 		WRITE_DIGITAL(R_Motor_Stop, SLOW_1, &CommStatus);
	else if(Motor == MOTOR_2) 		WRITE_DIGITAL(R_Motor_Stop, SLOW_2, &CommStatus);
	else if(Motor == MOTOR_ALL)		WRITE_DIGITAL(R_Motor_Stop, SLOW_ALL, &CommStatus);
	else							return SYS_ABORTED;
		
	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
Module_Status RESET(int Motor) {
	int CommStatus, AlmResult;
	BOOL Motor1_Pos, Motor2_Pos;
	int SetIO, ReadIO, SetIO2, ReadIO2;	
	double	dbReadPos = 0;
	double	dbReadPosTol = 0;

	Motor1_Pos = Motor2_Pos = FALSE;

	dbReadPosTol = READ_ANALOG(PRMA_ZMT_ErrTol, &CommStatus);

	if		(Motor == MOTOR_1) {
		SetIO  = ZMT_SetPosAM;
		ReadIO = R_Motor1_Position;
	}
	else if(Motor == MOTOR_2) {
		SetIO  = ZMT_SetPos2AM;
		ReadIO = R_Motor2_Position;
	}
	else {
		SetIO   = ZMT_SetPosAM;
		SetIO2  = ZMT_SetPos2AM;
		ReadIO  = R_Motor1_Position;
		ReadIO2 = R_Motor2_Position;
	}

	while(TRUE) 
	{
		//... Set Pulse Zero
		WRITE_ANALOG_TRUE(R_Move_Pulse_Set, 0, &CommStatus);
		if(CommStatus == FALSE)
		{
			if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
			{
				ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
				ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
				return SYS_ABORTED;
			}
		}

		//... Set Move ABS
		WRITE_DIGITAL_TRUE(R_Motor_Move_ABS, Motor, &CommStatus);
		if(CommStatus == FALSE)
		{
			if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
			{
				ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
				ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
				return SYS_ABORTED;
			}
		}

		if(Motor == MOTOR_ALL)		WRITE_ANALOG(SetIO2, HOME_POSITION, &CommStatus);
		WRITE_ANALOG(SetIO, HOME_POSITION, &CommStatus);

		S_TIMER_READY();
		while(TRUE) 
		{
			if(Motor == MOTOR_ALL) 
			{
				dbReadPos = READ_ANALOG(ReadIO2, &CommStatus);
				if(fabs(dbReadPos) <= dbReadPosTol)		Motor2_Pos = TRUE;
			}
			else Motor2_Pos = TRUE;

			dbReadPos = READ_ANALOG(ReadIO, &CommStatus);
			if(fabs(dbReadPos) <= dbReadPosTol)			 Motor1_Pos = TRUE;

			if(Motor1_Pos && Motor2_Pos)	break;
			if(!WAIT_SECONDS(0.1))	return SYS_ABORTED;

			if(S_TIMER_CHECK(1, READ_ANALOG(PRMA_ZMT_CtrlTmOut, &CommStatus))) 
			{
				     if(Motor1_Pos == FALSE && Motor2_Pos == FALSE)		AlmResult = ALARM_MANAGE(ALARM_HOME_ALL_TIMEOUT);
				else if(Motor1_Pos == FALSE)							AlmResult = ALARM_MANAGE(ALARM_HOME_1_TIMEOUT);
				else if(Motor2_Pos == FALSE)							AlmResult = ALARM_MANAGE(ALARM_HOME_2_TIMEOUT);

				     if(AlmResult == ALM_IGNORE)		return SYS_SUCCESS;
				else if(AlmResult != ALM_RETRY)			return SYS_ABORTED;
				else									break;		//.. Retry
			}
		}

		Motor1_Pos = Motor2_Pos = FALSE;

		if(Motor == MOTOR_ALL) 
		{
			dbReadPos = READ_ANALOG(ReadIO2, &CommStatus);
			if(fabs(dbReadPos) <= dbReadPosTol)		Motor2_Pos = TRUE;
		}
		else Motor2_Pos = TRUE;

		dbReadPos = READ_ANALOG(ReadIO, &CommStatus);
		if(fabs(dbReadPos) <= dbReadPosTol)			Motor1_Pos = TRUE;

		if(Motor1_Pos && Motor2_Pos)		break;
	}
	
	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//... HOME 2 in PROCESS_CONTROL
Module_Status HOME(int Motor) {
	int CommStatus, AlmResult;
	double ReadBuffer1, ReadBuffer2, HighLim, LowLim;
	BOOL RetryFlag, Motor1_Pos, Motor2_Pos;
	int HomeIO, ReadIO, PulsIO, AlrmIO, AlrmIO2, Chuck1, Chuck2;
	int Motor_Pos_IO;

	RetryFlag = Motor1_Pos = Motor2_Pos = FALSE;

	//...
	if(ChuckMotor_Standby_Check() == SYS_ABORTED)	return SYS_ABORTED;

	HighLim	= READ_ANALOG(PRMA_ZMT_HmHighLmt, &CommStatus);
	LowLim	= READ_ANALOG(PRMA_ZMT_HmLowLmt, &CommStatus);

	if(Motor == MOTOR_1) 
	{
		HomeIO = ZMT_HomeSnsDI;
		ReadIO = R_Position8_Pulse;
		PulsIO = ZMT_SetPosAM;
		AlrmIO = ALARM_HOME_1_TIMEOUT;
		AlrmIO2 = ALARM_HOMEPOSION_OUT1;
		Motor_Pos_IO = R_Motor1_Position;
	}
	else if(Motor == MOTOR_2) 
	{
		HomeIO = ZMT2_HomeSnsDI;
		ReadIO = R_Position9_Pulse;
		PulsIO = ZMT_SetPos2AM;
		AlrmIO = ALARM_HOME_2_TIMEOUT;
		AlrmIO2 = ALARM_HOMEPOSION_OUT2;
		Motor_Pos_IO = R_Motor2_Position;
	}

	//... Check Already Homed
	if(Motor == MOTOR_ALL) 
	{
		ReadBuffer1 = READ_ANALOG(R_Position8_Pulse, &CommStatus);
		ReadBuffer2 = READ_ANALOG(R_Position9_Pulse, &CommStatus);
		
		if((READ_DIGITAL(ZMT_HomeSnsDI, &CommStatus) == HOME_1) &&
			(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim) && 
			(READ_DIGITAL(ZMT2_HomeSnsDI, &CommStatus) == HOME_1) &&
			(ReadBuffer2 <= HighLim && ReadBuffer2 >= LowLim)) 
		{
			return SYS_SUCCESS;
		}
	}
	else 
	{
		Chuck1 = READ_DIGITAL(HomeIO, &CommStatus);
		ReadBuffer1 = READ_ANALOG(ReadIO, &CommStatus);

		if((READ_DIGITAL(HomeIO, &CommStatus) == HOME_1) &&
			(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim)) 
		{
			return SYS_SUCCESS;
		}
	}

	//... Run Home
	WRITE_DIGITAL_TRUE(R_Motor_Move_Home, Motor, &CommStatus);
	if(CommStatus == FALSE)
	{
		if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
		{
			ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
			ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
			return SYS_ABORTED;
		}
	}

	if(Motor == MOTOR_ALL) 
	{
		WRITE_ANALOG(ZMT_SetPosAM, HOME_POSITION, &CommStatus);
		WRITE_ANALOG(ZMT_SetPos2AM, HOME_POSITION, &CommStatus);
	}
	else
	{
		WRITE_ANALOG(PulsIO, HOME_POSITION, &CommStatus);
	}

	//... Wait Home Complete
	while(TRUE) 
	{
		if(RetryFlag) 
		{
			RetryFlag = FALSE;
			WRITE_DIGITAL_TRUE(R_Motor_Fast1_Home, Motor, &CommStatus);
			if(CommStatus == FALSE)
			{
				if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
				{
					ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
					ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
					return SYS_ABORTED;
				}
			}

			Motor1_Pos = FALSE;
			Motor2_Pos = FALSE;
		}
		
		//--> ADD 2011/03/03 Chuck Motor Home 진행 간, 무한루프 문제 수정 by JTM
		S_TIMER_READY();

		if(Motor == MOTOR_ALL) 
		{
			while(TRUE) 
			{			
				if(Motor1_Pos == FALSE)
				{
					if(READ_DIGITAL(ZMT_HomeSnsDI, &CommStatus) == HOME_1)
					{
						Chuck1 = READ_DIGITAL(ZMT_HomeSnsDI, &CommStatus);
						ReadBuffer1 = READ_ANALOG(R_Position8_Pulse, &CommStatus);
						
						printf("Chuck Position STAT_A ==> [%.0f],[%d]\n", ReadBuffer1, Chuck1);

						if(!(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim)) {

							printf("Chuck Position Out STAT_A ==> [%.0f],[%d]\n", ReadBuffer1, Chuck1);
					
							ALARM_MESSAGE_REMAPPING(ALARM_HOMEPOSION_OUT1, ReadBuffer1, LowLim, HighLim, FALSE);
							ALARM_POST(ALARM_HOMEPOSION_OUT1);
							return SYS_ABORTED;
						}
						else if(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim)
						{						
							Motor1_Pos = TRUE;
						}
					}
				}

				if(Motor2_Pos == FALSE)
				{			
					if(READ_DIGITAL(ZMT2_HomeSnsDI, &CommStatus) == HOME_1)
					{
						Chuck2 = READ_DIGITAL(ZMT2_HomeSnsDI, &CommStatus);
						ReadBuffer2 = READ_ANALOG(R_Position9_Pulse, &CommStatus);
						
						printf("Chuck Position STAT_B ==> [%.0f],[%d]\n", ReadBuffer2, Chuck2);

						if(!(ReadBuffer2 <= HighLim && ReadBuffer2 >= LowLim)) {

							printf("Chuck Position Out STAT_B ==> [%.0f],[%d]\n", ReadBuffer2, Chuck2);

							ALARM_MESSAGE_REMAPPING(ALARM_HOMEPOSION_OUT2, ReadBuffer2, LowLim, HighLim, FALSE);
							ALARM_POST(ALARM_HOMEPOSION_OUT2);
							return SYS_ABORTED;
						}					
						else if(ReadBuffer2 <= HighLim && ReadBuffer2 >= LowLim)
						{						
							Motor2_Pos = TRUE;
						}
					}
				}

				if(Motor1_Pos && Motor2_Pos) break;

				if(!WAIT_SECONDS(0.1)) return SYS_ABORTED;
				if(S_TIMER_CHECK(1, READ_ANALOG(PRMA_ZMT_CtrlTmOut, &CommStatus)))
				{
					if(READ_DIGITAL(PRO_RunStsDM, &CommStatus) == STOP_0) 
					{	// Manual Action
						if(Motor1_Pos == FALSE && Motor2_Pos == FALSE)			AlmResult = ALARM_MANAGE(ALARM_HOME_ALL_TIMEOUT);
						else if(Motor1_Pos == FALSE)							AlmResult = ALARM_MANAGE(ALARM_HOME_1_TIMEOUT);
						else if(Motor2_Pos == FALSE)							AlmResult = ALARM_MANAGE(ALARM_HOME_2_TIMEOUT);

						if(AlmResult == ALM_IGNORE)								return SYS_SUCCESS;
						else if(AlmResult != ALM_RETRY)							return SYS_ABORTED;
						else 
						{
							WRITE_DIGITAL_TRUE(R_Motor_Fast1_Home, Motor, &CommStatus);
							if(CommStatus == FALSE)
							{
								if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
								{
									ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
									ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
									return SYS_ABORTED;
								}
							}
							break;
						}
					}
					else 
					{	// In Recipe
						if(Motor1_Pos == FALSE && Motor2_Pos == FALSE)			ALARM_POST(ALARM_HOME_ALL_TIMEOUT);
						else if(Motor1_Pos == FALSE)							ALARM_POST(ALARM_HOME_1_TIMEOUT);
						else if(Motor2_Pos == FALSE)							ALARM_POST(ALARM_HOME_2_TIMEOUT);

						return SYS_ABORTED;
					}
				}
			}

			if(ChuckMotor_Standby_Check() == SYS_ABORTED) return SYS_ABORTED;

			if((READ_DIGITAL(ZMT_HomeSnsDI, &CommStatus) == HOME_1) &&(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim) && 
				(READ_DIGITAL(ZMT2_HomeSnsDI, &CommStatus) == HOME_1) &&(ReadBuffer2 <= HighLim && ReadBuffer2 >= LowLim)) {
				 //2008.04.23 ===모터 오동작시 다음 동작에서 검출하는 방법(원점)===
				 //-->2012/03/13 CMD Response Check by JTM
	        	 //WRITE_DIGITAL(R_Motor_Fast1_Home, Motor, &CommStatus);
				 WRITE_DIGITAL_TRUE(R_Motor_Fast1_Home, Motor, &CommStatus);
				 if(CommStatus == FALSE)
				 {
					 if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
					 {		
						 ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
						 ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
						 return SYS_ABORTED;
					 }
				}
		         //2008.04.23 ===모터 오동작시 다음 동작에서 검출하는 방법===

				 break;
			}
			else RetryFlag = TRUE;

			//--> DEL 2011/03/03 Chuck Motor Home 진행 간, 무한루프 문제 수정 by JTM
			//if(!WAIT_SECONDS(0.5)) return SYS_ABORTED;
		} //---> End MOTOR_ALL Case
		else 
		{
			while(TRUE) 
			{			

				if(READ_DIGITAL(HomeIO, &CommStatus) == HOME_1)
				{
					Chuck1		= READ_DIGITAL(HomeIO, &CommStatus);
					ReadBuffer1 = READ_ANALOG(ReadIO, &CommStatus);
					
					if(!(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim)) 
					{
						ALARM_MESSAGE_REMAPPING(AlrmIO2, ReadBuffer1, LowLim, HighLim, FALSE);
						ALARM_POST(AlrmIO2);
						return SYS_ABORTED;
					}
					
					if(! WAIT_SECONDS(0.5))		return SYS_ABORTED;
				
					if((READ_DIGITAL(HomeIO, &CommStatus) == HOME_1) &&
						(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim)) 
					{
						break;
					}
				}
				if(!WAIT_SECONDS(0.1)) return SYS_ABORTED;

				if(S_TIMER_CHECK(1, READ_ANALOG(PRMA_ZMT_CtrlTmOut, &CommStatus))) 
				{
					if(READ_DIGITAL(PRO_RunStsDM, &CommStatus) == STOP_0)
					{	// Manual Action
						AlmResult = ALARM_MANAGE(AlrmIO);
						if(AlmResult == ALM_IGNORE) return SYS_SUCCESS;
						else if(AlmResult != ALM_RETRY) return SYS_ABORTED;
						else   {
							//[ 2008.12.11 Chuck Manual Home 명령 시 Home Sensor 찾아 가도록 수정(BY LJH)
							//-->2012/03/13 CMD Response Check by JTM
							//WRITE_DIGITAL(R_Motor_Fast1_Home, Motor, &CommStatus);
							WRITE_DIGITAL_TRUE(R_Motor_Fast1_Home, Motor, &CommStatus);
							if(CommStatus == FALSE)
							{
								if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
								{		
									ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
									ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
									return SYS_ABORTED;
								}
							}
							//]
							break;
						}
					}
					else { // In Recipe
						ALARM_POST(AlrmIO);
						return SYS_ABORTED;
					}
				}
			} //---> End of Inner while()

	    	Chuck1 = READ_DIGITAL(HomeIO, &CommStatus);	
			ReadBuffer1 = READ_ANALOG(ReadIO, &CommStatus);

			printf("Chuck Position Stat2_AB ==> [%.0f],[%d]\n", ReadBuffer1, Chuck1);
			//if(!WAIT_SECONDS(0.5)) return SYS_ABORTED;

			if(ChuckMotor_Standby_Check() == SYS_ABORTED)	return SYS_ABORTED;

			if((READ_DIGITAL(HomeIO, &CommStatus) == HOME_1) &&(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim)) 
			{
				WRITE_DIGITAL_TRUE(R_Motor_Fast1_Home, Motor, &CommStatus);
				if(CommStatus == FALSE)
				{
					if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
					{
						ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
						ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
						return SYS_ABORTED;
					}
				}
				break;
			}
			else RetryFlag = TRUE;
		} //---> End MOTOR_1/MOTOR_2 Case
	} //---> End of Outer while()

	if(ChuckMotor_Standby_Check() == SYS_ABORTED) return SYS_ABORTED;	
	
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//[ 2008.12.11 Chuck Manual Home 명령 시 Home Sensor 찾아 가도록 수정(BY LJH)
Module_Status HOME_SNS_MOVE(int Motor) {
	int CommStatus, AlmResult;
	double ReadBuffer1, ReadBuffer2, HighLim, LowLim;
	BOOL RetryFlag, Motor1_Pos, Motor2_Pos;
	int HomeIO, ReadIO, PulsIO, AlrmIO;
	char szLogData[80];  //2014.10.09

	RetryFlag = Motor1_Pos = Motor2_Pos = FALSE;

	if(ChuckMotor_Standby_Check() == SYS_ABORTED)	return SYS_ABORTED;

	HighLim	= + READ_ANALOG(PRMA_ZMT_ErrTol, &CommStatus);
	LowLim	= - READ_ANALOG(PRMA_ZMT_ErrTol, &CommStatus);

	if		(Motor == MOTOR_1) {
		HomeIO = ZMT_HomeSnsDI;
		ReadIO = R_Motor1_Position;
		PulsIO = ZMT_SetPosAM;
		AlrmIO = ALARM_HOME_1_TIMEOUT;
	}
	else if(Motor == MOTOR_2) {
		HomeIO = ZMT2_HomeSnsDI;
		ReadIO = R_Motor2_Position;
		PulsIO = ZMT_SetPos2AM;
		AlrmIO = ALARM_HOME_2_TIMEOUT;
	}

	if(Motor == MOTOR_ALL) {
		ReadBuffer1 = READ_ANALOG(R_Motor1_Position, &CommStatus);
		ReadBuffer2 = READ_ANALOG(R_Motor2_Position, &CommStatus);
		if((READ_DIGITAL(ZMT_HomeSnsDI, &CommStatus) == HOME_1) &&(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim) &&
			(READ_DIGITAL(ZMT2_HomeSnsDI, &CommStatus) == HOME_1) &&(ReadBuffer2 <= HighLim && ReadBuffer2 >= LowLim))
		{
			//... 2015.03.20 Add Need Home Virtual IO Add for MC OFF Case
			if(FALSE == NeedHome_AllMotor())
			{
				printf("     Chuck All is at HOME Position ==> No Moving \n");
				return SYS_SUCCESS;
			}
		}
	}
	else {
		ReadBuffer1 = READ_ANALOG(ReadIO, &CommStatus);
		if((READ_DIGITAL(HomeIO, &CommStatus) == HOME_1) &&(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim)) {
			printf("     Chuck %d+1 is at HOME Position ==> No Moving \n", Motor);
			return SYS_SUCCESS;
		}
	}

	//-->2012/03/13 CMD Response Check by JTM
	//WRITE_DIGITAL(R_Motor_Fast1_Home, Motor, &CommStatus);
	WRITE_DIGITAL_TRUE(R_Motor_Fast1_Home, Motor, &CommStatus);
	if(CommStatus == FALSE)
	{
		if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
		{	
			ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
			ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
			return SYS_ABORTED;
		}
	}

	if(Motor == MOTOR_ALL) {
		WRITE_ANALOG(ZMT_SetPosAM, HOME_POSITION, &CommStatus);
		WRITE_ANALOG(ZMT_SetPos2AM, HOME_POSITION, &CommStatus);
	}
	else
		WRITE_ANALOG(PulsIO		, HOME_POSITION, &CommStatus);

	//--> DEL 2011/03/03 Chuck Motor Home 진행 간, 무한루프 문제 수정 by JTM
	//--> ADD 2011/02/06 Chuck Moving Tack Time Display 기능 추가 by JTM
	//S_TIMER_READY();
	//--> END 2011/02/06 Chuck Moving Tack Time Display 기능 추가 by JTM
	//////////////////////////////////////////////////////////////////////////
	// 2014.10.09
	if(Motor == MOTOR_1) 
	{
		sprintf(szLogData, "('SetPosition',%.3f)", ReadBuffer1);
		LogEvent("STAGE1_LOADPOS", "Start", szLogData);	
	}
	else if(Motor == MOTOR_2)
	{
		sprintf(szLogData, "('SetPosition',%.3f)", ReadBuffer2);
		LogEvent("STAGE2_LOADPOS", "Start", szLogData);	
	}
	else if(Motor == MOTOR_ALL)
	{
		sprintf(szLogData, "('SetPosition',%.3f)", ReadBuffer1);
		LogEvent("STAGE1_LOADPOS", "Start", szLogData);	
		sprintf(szLogData, "('SetPosition',%.3f)", ReadBuffer2);
		LogEvent("STAGE2_LOADPOS", "Start", szLogData);	
	}
	//....
	//////////////////////////////////////////////////////////////////////////
	while(TRUE) 
	{
		if(RetryFlag) {
			//-->2012/03/13 CMD Response Check by JTM
			//WRITE_DIGITAL(R_Motor_Fast1_Home, Motor, &CommStatus);
			WRITE_DIGITAL_TRUE(R_Motor_Fast1_Home, Motor, &CommStatus);
			if(CommStatus == FALSE)
			{
				if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
				{					
					ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
					ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
					return SYS_ABORTED;
				}
			}
			RetryFlag = FALSE;

			//--> ADD 2011/03/03 Chuck Motor Home 진행 간, 무한루프 문제 수정 by JTM
			Motor1_Pos = FALSE;
			Motor2_Pos = FALSE;
			//--> END 2011/03/03 Chuck Motor Home 진행 간, 무한루프 문제 수정 by JTM
		}		

		//--> ADD 2011/03/03 Chuck Motor Home 진행 간, 무한루프 문제 수정 by JTM
		//--> DEL 2011/02/06 Chuck Moving Tack Time Display 기능 추가 by JTM
		S_TIMER_READY();

		if(Motor == MOTOR_ALL) 
		{
			while(TRUE) 
			{
				if(Motor1_Pos == FALSE)
				{
					ReadBuffer1 = READ_ANALOG(R_Motor1_Position, &CommStatus);
					
					if(READ_DIGITAL(ZMT_HomeSnsDI, &CommStatus) == HOME_1) 	
						if(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim)												
							Motor1_Pos = TRUE;					
				}

				if(Motor2_Pos == FALSE)
				{
					ReadBuffer2 = READ_ANALOG(R_Motor2_Position, &CommStatus);	
				
					if(READ_DIGITAL(ZMT2_HomeSnsDI, &CommStatus) == HOME_1) 
						if(ReadBuffer2 <= HighLim && ReadBuffer2 >= LowLim)													
							Motor2_Pos = TRUE;						
				}
				//-->END 2011/02/16 Chuck Moving Time 재현성 개선 by JTM

				if(Motor1_Pos && Motor2_Pos) break;

				if(!WAIT_SECONDS(0.1)) return SYS_ABORTED;
				if(S_TIMER_CHECK(1, READ_ANALOG(PRMA_ZMT_CtrlTmOut, &CommStatus))) 
				{
					if(READ_DIGITAL(PRO_RunStsDM, &CommStatus) == STOP_0)
					{	// Manul Action
						if		(Motor1_Pos == FALSE && Motor2_Pos == FALSE)
							AlmResult = ALARM_MANAGE(ALARM_HOME_ALL_TIMEOUT);
						else if(Motor1_Pos == FALSE)
							AlmResult = ALARM_MANAGE(ALARM_HOME_1_TIMEOUT);
						else if(Motor2_Pos == FALSE)
							AlmResult = ALARM_MANAGE(ALARM_HOME_2_TIMEOUT);

						if(AlmResult == ALM_IGNORE) return SYS_SUCCESS;
						else if(AlmResult != ALM_RETRY) return SYS_ABORTED;
						else   {
							//[ 2008.12.11 Chuck Manual Home 명령 시 Home Sensor 찾아 가도록 수정(BY LJH)
							//-->2012/03/13 CMD Response Check by JTM
							//WRITE_DIGITAL(R_Motor_Fast1_Home, Motor, &CommStatus);
							WRITE_DIGITAL_TRUE(R_Motor_Fast1_Home, Motor, &CommStatus);
							if(CommStatus == FALSE)
							{
								if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
								{					
									ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
									ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
									return SYS_ABORTED;
								}
							}
							//]
							break;
						}
					}
					else { // In Recipe
						if		(Motor1_Pos == FALSE && Motor2_Pos == FALSE)
							ALARM_POST(ALARM_HOME_ALL_TIMEOUT);
						else if(Motor1_Pos == FALSE)
							ALARM_POST(ALARM_HOME_1_TIMEOUT);
						else if(Motor2_Pos == FALSE)
							ALARM_POST(ALARM_HOME_2_TIMEOUT);

						return SYS_ABORTED;
					}
				}
			} //---> End of Inner while()

			ReadBuffer1 = READ_ANALOG(R_Motor1_Position, &CommStatus);
			ReadBuffer2 = READ_ANALOG(R_Motor2_Position, &CommStatus);

			//--> ADD 2011/03/03 Chuck Motor Home 진행 간, 무한루프 문제 수정 by JTM
			//if(!WAIT_SECONDS(0.5)) return SYS_ABORTED;
			
			//-->ADD 2012/03/19 Chuck Motor Status Check by JTM
			if(ChuckMotor_Standby_Check() == SYS_ABORTED) return SYS_ABORTED;	

			if((READ_DIGITAL(ZMT_HomeSnsDI, &CommStatus) == HOME_1) &&(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim) && 
				(READ_DIGITAL(ZMT2_HomeSnsDI, &CommStatus) == HOME_1) &&(ReadBuffer2 <= HighLim && ReadBuffer2 >= LowLim)) 
			{
				 break;
			}
			else RetryFlag = TRUE;

			//--> DEL 2011/03/03 Chuck Motor Home 진행 간, 무한루프 문제 수정 by JTM
			//if(!WAIT_SECONDS(0.5)) return SYS_ABORTED;
		} //---> End MOTOR_ALL Case
		else {
			while(TRUE) {
				ReadBuffer1 = READ_ANALOG(ReadIO, &CommStatus);
				if((READ_DIGITAL(HomeIO,&CommStatus) == HOME_1) &&(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim))
				{
					break;
				}
				//-->Modify 2011/02/16 Chuck Moving Time 재현성 개선 by JTM



				if(!WAIT_SECONDS(0.1)) return SYS_ABORTED;

				if(S_TIMER_CHECK(1, READ_ANALOG(PRMA_ZMT_CtrlTmOut, &CommStatus))) 
				{
					if(READ_DIGITAL(PRO_RunStsDM, &CommStatus) == STOP_0)
					{ // Manul Action
						AlmResult = ALARM_MANAGE(AlrmIO);
						if(AlmResult == ALM_IGNORE) return SYS_SUCCESS;
						else if(AlmResult != ALM_RETRY) return SYS_ABORTED;
						else   {
							//[ 2008.12.11 Chuck Manual Home 명령 시 Home Sensor 찾아 가도록 수정(BY LJH)
							//-->2012/03/13 CMD Response Check by JTM
							//WRITE_DIGITAL(R_Motor_Fast1_Home, Motor, &CommStatus);
							WRITE_DIGITAL_TRUE(R_Motor_Fast1_Home, Motor, &CommStatus);
							if(CommStatus == FALSE)
							{
								if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
								{					
									ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
									ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
									return SYS_ABORTED;
								}
							}
							//]
							break;
						}
					}
					else { // In Recipe
						ALARM_POST(AlrmIO);
						return SYS_ABORTED;
					}
				}
			} //---> End of Inner while()

			ReadBuffer1 = READ_ANALOG(ReadIO, &CommStatus);

			//--> DEL 2011/02/06 Chuck Moving Tack Time Display 기능 추가 by JTM
			//if(!WAIT_SECONDS(0.5)) return SYS_ABORTED;

			//-->ADD 2012/03/19 Chuck Motor Status Check by JTM
			if(ChuckMotor_Standby_Check() == SYS_ABORTED) return SYS_ABORTED;

			if((READ_DIGITAL(HomeIO, &CommStatus) == HOME_1) &&(ReadBuffer1 <= HighLim && ReadBuffer1 >= LowLim)) 
			{
				break;				
			}
			else RetryFlag = TRUE;

			//--> ADD 2011/02/06 Chuck Moving Tack Time Display 기능 추가 by JTM
			if(!WAIT_SECONDS(0.5)) return SYS_ABORTED;

		} //---> End MOTOR_1/MOTOR_2 Case
	} //---> End of Outer while()
	
	//////////////////////////////////////////////////////////////////////////
	// 2014.10.09
	if(Motor == MOTOR_1) 
	{
		sprintf(szLogData, "('SetPosition',%.3f)", ReadBuffer1);
		LogEvent("STAGE1_LOADPOS", "End", szLogData);	
	}
	else if(Motor == MOTOR_2)
	{
		sprintf(szLogData, "('SetPosition',%.3f)", ReadBuffer2);
		LogEvent("STAGE2_LOADPOS", "End", szLogData);	
	}
	else if(Motor == MOTOR_ALL)
	{
		sprintf(szLogData, "('SetPosition',%.3f)", ReadBuffer1);
		LogEvent("STAGE1_LOADPOS", "End", szLogData);	
		sprintf(szLogData, "('SetPosition',%.3f)", ReadBuffer2);
		LogEvent("STAGE2_LOADPOS", "End", szLogData);	
	}
	//....
	//////////////////////////////////////////////////////////////////////////

	//... 2015.03.20 Need Home Flag OFF in MOTOR_ALL Case
	if(MOTOR_ALL == Motor)
	{	//... Check All Motor Homed in first Home
		WRITE_DIGITAL(R_Motor_NeedHomeDM, OFF_0, &CommStatus);
	}
	
	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//... 2016.03.03 Check Chuck Motor Power Off 
BOOL IsMotorPowerOff(int nMotor)
{
	int nIOStatus;
	int i = 0;
	double dbSide1Encoder = 0;
	double dbSide2Encoder = 0;

	dbSide1Encoder = READ_ANALOG(R_Motor1_Position, &nIOStatus);
	dbSide2Encoder = READ_ANALOG(R_Motor2_Position, &nIOStatus);
	if(nMotor == MOTOR_1)
	{
		for(i = 0; i < 3; i++ )
		{
			//... Ignore in the decimal point six
 			if(fabs( dbSide1Encoder - 0 ) <= 1.0e-6 && READ_DIGITAL(ZMT_HomeSnsDI, &nIOStatus) != HOME_1) 
			{
				if( i == 2)
				{
					ALARM_POST(ALARM_MOTOR1_MOVE_INTLK);
					return FALSE;
				}
				_sleep(1000);
			}
			else { return TRUE; }
		}
	}
	else if(nMotor == MOTOR_2)
	{
		for(i = 0; i < 3; i++ )
		{
			if(fabs( dbSide2Encoder - 0 ) <= 1.0e-6 && READ_DIGITAL(ZMT2_HomeSnsDI, &nIOStatus) != HOME_1) 
			{
				if( i == 2)
				{
					ALARM_POST(ALARM_MOTOR2_MOVE_INTLK);
					return FALSE;
				}
				_sleep(1000);
			}
			else { return TRUE; }
		}
	}
	else   //... Chuck All
	{
		for(i = 0; i < 3; i++ )
		{
			if(fabs( dbSide1Encoder - 0 ) <= 1.0e-6 && READ_DIGITAL(ZMT_HomeSnsDI, &nIOStatus) != HOME_1) 
			{
				if( i == 2)
				{
					ALARM_POST(ALARM_MOTOR1_MOVE_INTLK);
					return FALSE;
				}
				_sleep(1000);
			}
			else { return TRUE; }
		}

		for(i = 0; i < 3; i++ )
		{
			if(fabs( dbSide2Encoder - 0 ) <= 1.0e-6 && READ_DIGITAL(ZMT2_HomeSnsDI, &nIOStatus) != HOME_1) 
			{
				if( i == 2)
				{
					ALARM_POST(ALARM_MOTOR2_MOVE_INTLK);
					return FALSE;
				}
				_sleep(1000);
			}
			else { return TRUE; }
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//... "RUN 2 %f %f", RECIPE[i].A_GAP_POSITION, RECIPE[i].B_GAP_POSITION
Module_Status GOMOVE(int Motor, double Data1, double Data2) {
	
	int CommStatus, AlmResult;
	BOOL Motor1_Pos, Motor2_Pos;
	double ReadBuffer1, ReadBuffer2, HighLim1, LowLim1, HighLim2, LowLim2, SetData1, SetData2;
	double ReadOFFSET1, ReadOFFSET2, AlarmData1, AlarmData2;
	int HomeIO, OffsIO, ReadIO, PulsIO, AlrmIO;
	char StrSet[256] = "";
	double Offset_Ignore_Low, Offset_Ignore_High; 

	Offset_Ignore_Low = READ_ANALOG(PRMA_ZMT_OffsIgLow, &CommStatus);
	Offset_Ignore_High = READ_ANALOG(PRMA_ZMT_OffsIgHigh, &CommStatus);

	Motor1_Pos = Motor2_Pos = FALSE;
	ReadOFFSET1 = ReadOFFSET2 = 0;

	//... 1. Check Home Position
	if(Data1 == HOME_POSITION || Data2 == HOME_POSITION) 
	{
		if(HOME(Motor) == SYS_ABORTED)
		{
			//... 2016.03.03 When Home CMD don't Complete, Need Home Flag Change  
			if(ON_1 != READ_DIGITAL(R_Motor_NeedHomeDM, &CommStatus))
			{
				WRITE_DIGITAL(R_Motor_NeedHomeDM, ON_1 ,&CommStatus);
			}
			return SYS_ABORTED;
		}
		else	
		{
			return SYS_SUCCESS;
		}
	}

	//... 2. Check Motor Side & Control
	if(Motor == MOTOR_1) 
	{
		HomeIO			= ZMT_HomeSnsDI;
		OffsIO 			= PRMA_ZMT_Offset;
		ReadIO			= R_Motor1_Position;
		PulsIO			= ZMT_SetPosAM;
		AlrmIO			= ALARM_MOVE_1_TIMEOUT;
	}
	else if(Motor == MOTOR_2) 
	{
		HomeIO			= ZMT2_HomeSnsDI;
		OffsIO 			= PRMA_ZMT_Offset2;
		ReadIO			= R_Motor2_Position;
		PulsIO			= ZMT_SetPos2AM;
		AlrmIO			= ALARM_MOVE_2_TIMEOUT;
	}

	//////////////////////////////////////////////////////////////////////////
	if(Motor == MOTOR_ALL) 
	{	
		//... Set Position
		ReadOFFSET1 = READ_ANALOG(PRMA_ZMT_Offset,&CommStatus);
		ReadOFFSET2 = READ_ANALOG(PRMA_ZMT_Offset2,&CommStatus);

		if(Data1 >= Offset_Ignore_Low && Data1 <= Offset_Ignore_High)	ReadOFFSET1 = 0;
		if(Data2 >= Offset_Ignore_Low && Data2 <= Offset_Ignore_High)	ReadOFFSET2 = 0;
		WRITE_ANALOG(ZMT_SetPosAM, Data1, &CommStatus);
		WRITE_ANALOG(ZMT_SetPos2AM, Data2, &CommStatus);
		
		AlarmData1 = VALID_POSITION -(Data1 - S_POSITION) + ReadOFFSET1;
		AlarmData2 = VALID_POSITION -(Data2 - S_POSITION) + ReadOFFSET2;
		
		if(AlarmData1 > VALID_POSITION || AlarmData1 < 0) {
			ALARM_POST(ALARM_INVALID_1_POSITION);
			return SYS_ABORTED;
		}
		if(AlarmData2 > VALID_POSITION || AlarmData2 < 0) {
			ALARM_POST(ALARM_INVALID_2_POSITION);
			return SYS_ABORTED;
		}

		if(ReadOFFSET1 != 0)
		{
			if(Data1 >= Offset_Ignore_Low + ReadOFFSET1 && Data1 < Offset_Ignore_Low)
			{				
				ALARM_MESSAGE_REMAPPING2(ALARM_INVALID_1_POSITION, MOTOR_1, Data1, ReadOFFSET1, Offset_Ignore_Low);
				ALARM_POST(ALARM_INVALID_1_POSITION);
				return SYS_ABORTED;
			}
		}
		if(ReadOFFSET2 != 0)
		{
			if(Data2 >= Offset_Ignore_Low + ReadOFFSET2 && Data2 < Offset_Ignore_Low)
			{
				ALARM_MESSAGE_REMAPPING2(ALARM_INVALID_2_POSITION, MOTOR_2, Data2, ReadOFFSET2, Offset_Ignore_Low);
				ALARM_POST(ALARM_INVALID_2_POSITION);
				return SYS_ABORTED;
			}
		}

		//... 2016.03.03 Check Invalid Motor Status
		if(!IsMotorPowerOff(Motor)) return SYS_ABORTED;			

		//... Calculate High/Low Limit
		SetData1 =(VALID_POSITION -(Data1 - S_POSITION) + ReadOFFSET1) * READ_ANALOG(PRMA_ZMT_Pulse_mm, &CommStatus); // mm
		SetData2 =(VALID_POSITION -(Data2 - S_POSITION) + ReadOFFSET2) * READ_ANALOG(PRMA_ZMT_Pulse_mm, &CommStatus); // mm

		HighLim1 =  SetData1 + READ_ANALOG(PRMA_ZMT_ErrTol, &CommStatus);
		LowLim1	 =  SetData1 - READ_ANALOG(PRMA_ZMT_ErrTol, &CommStatus);
		HighLim2 =  SetData2 + READ_ANALOG(PRMA_ZMT_ErrTol, &CommStatus);
		LowLim2	 =  SetData2 - READ_ANALOG(PRMA_ZMT_ErrTol, &CommStatus);

		//////////////////////////////////////////////////////////////////////////
		while(TRUE) 
		{
			if(ChuckMotor_Standby_Check() == SYS_ABORTED)		return SYS_ABORTED;

			sprintf(StrSet, "%.0f,%.0f", SetData1, SetData2);
			printf("Chuck Position Set ==> [%s]\n", StrSet);
			WRITE_STRING(R_Pulse_Set_String, StrSet, &CommStatus);
			WRITE_ANALOG_TRUE(R_All_Move_Pulse_Set, 0, &CommStatus);
			
			if(CommStatus == FALSE)
			{
				if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
				{					
					ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
					ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
					return SYS_ABORTED;
				}
			}

			WRITE_DIGITAL_TRUE(R_Motor_Move_All, ON_1, &CommStatus);
			if(CommStatus == FALSE)
			{
				if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
				{					
					ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
					ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
					return SYS_ABORTED;
				}
			}

			S_TIMER_READY();
			Motor1_Pos = FALSE;		
			Motor2_Pos = FALSE;		

			while(TRUE) 
			{
				if(Motor1_Pos == FALSE)
				{
					ReadBuffer1 = READ_ANALOG(R_Motor1_Position, &CommStatus);
					
					if(ReadBuffer1 <= HighLim1 &&  ReadBuffer1 >= LowLim1) 												
						Motor1_Pos = TRUE;						
				}

				if(Motor2_Pos == FALSE)
				{
					ReadBuffer2 = READ_ANALOG(R_Motor2_Position, &CommStatus);
					
					if(ReadBuffer2 <= HighLim2 &&  ReadBuffer2 >= LowLim2)											
						Motor2_Pos = TRUE;						
				}

				if(Motor1_Pos & Motor2_Pos) break;

				if(! WAIT_SECONDS(0.1))		return SYS_ABORTED;

				if(S_TIMER_CHECK(1, READ_ANALOG(PRMA_ZMT_CtrlTmOut, &CommStatus))) 
				{
					if(READ_DIGITAL(PRO_RunStsDM, &CommStatus) == STOP_0)
					{	// Manul Action
						if(Motor1_Pos == FALSE && Motor2_Pos == FALSE)
						{
							AlmResult = ALARM_MANAGE(ALARM_MOVE_ALL_TIMEOUT);
						}
						else if(Motor1_Pos == FALSE) 
						{
							ALARM_MESSAGE_REMAPPING(ALARM_MOVE_1_TIMEOUT, ReadBuffer1, LowLim1, HighLim1, TRUE);
							AlmResult = ALARM_MANAGE(ALARM_MOVE_1_TIMEOUT);
						}
						else if(Motor2_Pos == FALSE) 
						{
							ALARM_MESSAGE_REMAPPING(ALARM_MOVE_2_TIMEOUT, ReadBuffer1, LowLim2, HighLim2, TRUE);
							AlmResult = ALARM_MANAGE(ALARM_MOVE_2_TIMEOUT);
						}

						     if(AlmResult == ALM_IGNORE)			return SYS_SUCCESS;
						else if(AlmResult != ALM_RETRY)				return SYS_ABORTED;
						else										break;
					}
					else 
					{	// In Recipe
						if(Motor1_Pos == FALSE && Motor2_Pos == FALSE)		ALARM_POST(ALARM_MOVE_ALL_TIMEOUT);
						else if(Motor1_Pos == FALSE) 
						{
							ALARM_MESSAGE_REMAPPING(ALARM_MOVE_1_TIMEOUT, ReadBuffer1, LowLim1, HighLim1, TRUE);
							ALARM_POST(ALARM_MOVE_1_TIMEOUT);
						}
						else if(Motor2_Pos == FALSE) 
						{
							ALARM_MESSAGE_REMAPPING(ALARM_MOVE_2_TIMEOUT, ReadBuffer1, LowLim2, HighLim2, TRUE);
							ALARM_POST(ALARM_MOVE_2_TIMEOUT);
						}

						return SYS_ABORTED;
					}
				}
			}

			ReadBuffer1 = READ_ANALOG(R_Motor1_Position, &CommStatus);
			ReadBuffer2 = READ_ANALOG(R_Motor2_Position, &CommStatus);

			if((ReadBuffer1 <= HighLim1 &&  ReadBuffer1 >= LowLim1) &&
				(ReadBuffer2 <= HighLim2 &&  ReadBuffer2 >= LowLim2)) 
			{
				break;
			}

			if(! WAIT_SECONDS(0.5))			return SYS_ABORTED;
		}
	}
	else 
	{	//---> Start of MOTOR_1 / MOTOR_2 Case
		ReadOFFSET1 = READ_ANALOG(OffsIO, &CommStatus);
		if(Data1 >= Offset_Ignore_Low && Data1 <= Offset_Ignore_High)		
			ReadOFFSET1 = 0;	
		WRITE_ANALOG(PulsIO, Data1, &CommStatus);
		
		AlarmData1 = VALID_POSITION - (Data1 - S_POSITION) + ReadOFFSET1;

		if(AlarmData1 > VALID_POSITION || AlarmData1 < 0) 
		{
			if     (Motor == MOTOR_1)	ALARM_POST(ALARM_INVALID_1_POSITION);
			else if(Motor == MOTOR_2)	ALARM_POST(ALARM_INVALID_2_POSITION);
			
			return SYS_ABORTED;
		}

		//... 2016.03.03 Check Invalid Motor Status
		if     (Motor == MOTOR_1) { if(!IsMotorPowerOff(Motor)) return SYS_ABORTED; }	
		else if(Motor == MOTOR_2) { if(!IsMotorPowerOff(Motor)) return SYS_ABORTED; }	


		if(ReadOFFSET1 != 0)
		{
			if(Data1 >= Offset_Ignore_Low + ReadOFFSET1 && Data1 < Offset_Ignore_Low)
			{
				if(Motor == MOTOR_1)
				{
					ALARM_MESSAGE_REMAPPING2(ALARM_INVALID_1_POSITION, MOTOR_1, Data1, ReadOFFSET1, Offset_Ignore_Low);
					ALARM_POST(ALARM_INVALID_1_POSITION);
				}
				else if(Motor == MOTOR_2)
				{
					ALARM_MESSAGE_REMAPPING2(ALARM_INVALID_2_POSITION, MOTOR_2, Data1, ReadOFFSET1, Offset_Ignore_Low);
					ALARM_POST(ALARM_INVALID_2_POSITION);
				}
				return SYS_ABORTED;
			}
		}		
		//---> Modify 2007/10/01 by JJY MAHA-SP
		SetData1 =(VALID_POSITION -(Data1 - S_POSITION) + ReadOFFSET1) * READ_ANALOG(PRMA_ZMT_Pulse_mm, &CommStatus); // mm
		//------------> End(5 of 5) 2007/10/01

		HighLim1 =  SetData1 + READ_ANALOG(PRMA_ZMT_ErrTol, &CommStatus);
		LowLim1	 =  SetData1 - READ_ANALOG(PRMA_ZMT_ErrTol, &CommStatus);

		while(TRUE) {
			//-->Modify 2012/03/19 Chuck Motor Status Check by JTM(이하동일)
			//if(PrepCheckModule() == SYS_ABORTED) return SYS_ABORTED;
			if(ChuckMotor_Standby_Check() == SYS_ABORTED) return SYS_ABORTED;

			//-->2012/03/13 CMD Response Check by JTM
			//WRITE_ANALOG(R_Move_Pulse_Set,(double) SetData1, &CommStatus);
			//WRITE_DIGITAL(R_Motor_Move_ABS, Motor, &CommStatus);

			WRITE_ANALOG_TRUE(R_Move_Pulse_Set,(double) SetData1, &CommStatus);
			if(CommStatus == FALSE)
			{
				if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
				{					
					ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
					ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
					return SYS_ABORTED;
				}
			}
			WRITE_DIGITAL_TRUE(R_Motor_Move_ABS, Motor, &CommStatus);
			if(CommStatus == FALSE)
			{
				if(ALARM_STATUS(ALARM_CHUCK_MOTOR_ERROR) != ALM_PAUSED)
				{					
					ALARM_MESSAGE_REMAPPING_CHUCK_ERR(ALARM_CHUCK_MOTOR_ERROR, CommErr_0, 0);
					ALARM_POST(ALARM_CHUCK_MOTOR_ERROR);
					return SYS_ABORTED;
				}
			}


			//--> ADD 2011/02/06 Chuck Moving Tack Time Display 기능 추가 by JTM
			S_TIMER_READY();

			if(SIMULATION_MODE) {
				if(!WAIT_SECONDS(7)) return SYS_ABORTED;
				printf("Chamber %d [%f] \n", Motor+1, SetData1);
				WRITE_ANALOG(ReadIO, SetData1, &CommStatus);
				WRITE_DIGITAL(HomeIO, NOT_0, &CommStatus);
			}

			//--> DEL 2011/02/06 Chuck Moving Tack Time Display 기능 추가 by JTM
			//S_TIMER_READY();

			while(TRUE) {

				ReadBuffer1 = READ_ANALOG(ReadIO, &CommStatus);
				if(ReadBuffer1 <= HighLim1 &&  ReadBuffer1 >= LowLim1)
				{
					break;
				}
				//-->END 2011/02/16 Chuck Moving Time 재현성 개선 by JTM

				if(!WAIT_SECONDS(0.1)) return SYS_ABORTED;

				if(S_TIMER_CHECK(1, READ_ANALOG(PRMA_ZMT_CtrlTmOut, &CommStatus))) 
				{
					if(READ_DIGITAL(PRO_RunStsDM, &CommStatus) == STOP_0)
					{ // Manul Action
						ALARM_MESSAGE_REMAPPING(AlrmIO, ReadBuffer1, LowLim1, HighLim1, TRUE);
						AlmResult = ALARM_MANAGE(AlrmIO);

						if(AlmResult == ALM_IGNORE) return SYS_SUCCESS;
						else if(AlmResult != ALM_RETRY) return SYS_ABORTED;
						else                                break;
					}
					else { // In Recipe
						ALARM_MESSAGE_REMAPPING(AlrmIO, ReadBuffer1, LowLim1, HighLim1, TRUE);
						ALARM_POST(AlrmIO);
						return SYS_ABORTED;
					}
				}
			} //---> End of Inner while()

			ReadBuffer1 = READ_ANALOG(ReadIO, &CommStatus);

			//--> DEL 2011/02/06 Chuck Moving Tack Time Display 기능 추가 by JTM
			//if(!WAIT_SECONDS(0.5)) return SYS_ABORTED;
			if(ReadBuffer1 <= HighLim1 &&  ReadBuffer1 >= LowLim1)
			{
				break;
			}

			//--> ADD 2011/02/06 Chuck Moving Tack Time Display 기능 추가 by JTM
			if(!WAIT_SECONDS(0.5)) return SYS_ABORTED;

		} //---> End of Outer while()
	} //---> End MOTOR_1/MOTOR_2 Case

	//-->Modify 2012/03/19 Chuck Motor Status Check by JTM(이하동일)
	if(ChuckMotor_Standby_Check() == SYS_ABORTED) return SYS_ABORTED;

	return SYS_SUCCESS;
}
//... 2016.01.19
Module_Status DynLKCycle()
{
	Module_Status msRet = SYS_ABORTED;
	
	int		nIOStatus	= 0;
	int		nTgtCycCnt	= 0;
	int		nCurCycCnt	= 0;

	double dbUpPos		= 0;
	double dbDownnPos   = 0;
	
	do 
	{
		//... Check Move Cycle Count
		nTgtCycCnt = READ_DIGITAL(ALECHK_DynCntDM, &nIOStatus);
		dbUpPos	   = READ_ANALOG(CHUCK_UPPOS_AM, &nIOStatus);
		dbDownnPos = READ_ANALOG(CHUCK_DOWNPOS_AM, &nIOStatus);

		if(nTgtCycCnt < 1)	{	msRet = SYS_SUCCESS;	break;	}
		
		//... Run Init Home Move Before Cycle Move
		msRet = HOME_SNS_MOVE(MOTOR_ALL);
		if(SYS_SUCCESS != msRet)		break;
		
		//... Check Invalid Up Position
		if(dbUpPos < 4)		{	msRet = SYS_SUCCESS;	break;	}
		
		//... Run Cycle Move TEST		
		for(nCurCycCnt = 1; nCurCycCnt <= nTgtCycCnt; nCurCycCnt++)
		{
			//... Count Update
			WRITE_ANALOG(M_MT_CycMvCurCntAM, nCurCycCnt, &nIOStatus);
			
			//... Move Up Position
			msRet = GOMOVE(MOTOR_ALL, dbUpPos, dbUpPos);
			
			if(SYS_SUCCESS != msRet)	break;
			if(! WAIT_SECONDS(3))		break;		//... Aborted by User
			
			//... Move Home Position
			msRet = GOMOVE(MOTOR_ALL, dbDownnPos, dbDownnPos);
			
			if(SYS_SUCCESS != msRet)	break;
			if(! WAIT_SECONDS(3))		break;		//... Aborted by User

			//.. Leak Check Complete & Error
			if(/*Error*/ 13 == READ_DIGITAL(LeakChkStsDM,&nIOStatus)
				||/*Completed*/12 == READ_DIGITAL(LeakChkStsDM,&nIOStatus))
			{
				break;
			}
		}
	} while (0);

	msRet = HOME_SNS_MOVE(MOTOR_ALL);
	
	return	msRet;
}
//////////////////////////////////////////////////////////////////////////
//... 2015.02.28 Add Chuck Move Cycle for MAHA-DPT MFG TFT
Module_Status MotorMovingCycle(int Motor, double dbUpPos)
{
	Module_Status msRet = SYS_ABORTED;

	int		nIOStatus	= 0;
	int		nTgtCycCnt	= 0;
	int		nCurCycCnt	= 0;

	clock_t	StartTime, EndTime;
	double	dbElapsedTime = 0;

	do 
	{
		//... Check Move Cycle Count
		nTgtCycCnt = (int) READ_ANALOG(M_MT_CycMvTgtCntAM, &nIOStatus);
		if(nTgtCycCnt < 1)	{	msRet = SYS_SUCCESS;	break;	}

		//... Run Init Home Move Before Cycle Move
		msRet = HOME_SNS_MOVE(Motor);
		if(SYS_SUCCESS != msRet)		break;

		//... Check Invalid Up Position
		if(dbUpPos < 4)		{	msRet = SYS_SUCCESS;	break;	}

		//... Init Run Cycle Move TEST Information
		WRITE_ANALOG(M_MT_CycMvUpTmAM, 0, &nIOStatus);
		WRITE_ANALOG(M_MT_CycMvDnTmAM, 0, &nIOStatus);

		//... Run Cycle Move TEST		
		for(nCurCycCnt = 1; nCurCycCnt <= nTgtCycCnt; nCurCycCnt++)
		{
			//... Count Update
			WRITE_ANALOG(M_MT_CycMvCurCntAM, nCurCycCnt, &nIOStatus);

			//... Move Up Position
			StartTime = clock();
			msRet = GOMOVE(Motor, dbUpPos, dbUpPos);
			EndTime = clock();
			dbElapsedTime = (double)(EndTime - StartTime) / CLOCKS_PER_SEC;
			WRITE_ANALOG(M_MT_CycMvUpTmAM, dbElapsedTime, &nIOStatus);

			if(SYS_SUCCESS != msRet)	break;
			if(! WAIT_SECONDS(3))		break;		//... Aborted by User

			//... Move Home Position
			StartTime = clock();
			msRet = GOMOVE(Motor, HOME_POSITION, HOME_POSITION);
			EndTime = clock();
			dbElapsedTime = (double)(EndTime - StartTime) / CLOCKS_PER_SEC;
			WRITE_ANALOG(M_MT_CycMvDnTmAM, dbElapsedTime, &nIOStatus);
			
			if(SYS_SUCCESS != msRet)	break;
			if(! WAIT_SECONDS(3))		break;		//... Aborted by User
		}
	} while (0);

	//... Save Information Parameter

	return	msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	Module_Status msRet = SYS_ABORTED; 
	int	Dl;
	unsigned char *Ed;
	int CommStatus;
	char ParamStr[31], ParamStr2[31], ParamStr3[31], ChambStr[31], RunStr[31];
	double dbSpeed = 0;

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen(Ed);
	Ed[ Dl ] = 0x00;

	//////////////////////////////////////////////////////////////////////////
	//... Information
	printf("[INFO] Entered %s[%s]\n", PROGRAM_FUNCTION_READ(), Ed);

	do 
	{
		if(READ_DIGITAL(FC_MOTOR_CONTROL, &CommStatus) == OFF_0) 
		{
			printf("[INFO] %s Control OFF Mode\n", PROGRAM_FUNCTION_READ());
			msRet = SYS_SUCCESS;
			break;	//... Command Skip
		}
		
		if(Dl < 1) 
		{
			printf("[ABORT] %s Parameter Error\n", PROGRAM_FUNCTION_READ());
			break;
		}
		
		//.....
		STR_SEPERATE(Ed, RunStr, ParamStr, 30);
		STR_SEPERATE(ParamStr, ChambStr, ParamStr, 30);
		STR_SEPERATE(ParamStr, ParamStr, ParamStr2, 30);
		STR_SEPERATE(ParamStr2, ParamStr2, ParamStr3, 30);

		//... 2015.03.20 Add Need Home Virtual IO Add for MC OFF Case
		if(TRUE == NeedHome_AllMotor())
		{	//... Check Only Move Command
			if(FALSE == STRCMP_L(RunStr, "HOME") && FALSE == STRCMP_L(RunStr, "STOP")) 
			{
				ALARM_POST(ALARM_MOTOR_ALL_NEED_HOME);
				printf("[ABORT] %s by Need Home\n", PROGRAM_FUNCTION_READ());
				break;
			}
		}
		
		//.....
		if     (STRCMP_L(RunStr, "READY"))		msRet = ChuckMotor_Standby_Check();
		else if(STRCMP_L(RunStr, "RESET"))		msRet = RESET(atoi(ChambStr));
		else if(STRCMP_L(RunStr, "HOME"))
		{
			//... 2016.03.03 When Home CMD don't Complete, Need Home Flag Change  
			msRet = HOME_SNS_MOVE(atoi(ChambStr));
			if(msRet != SYS_SUCCESS)
			{
				if(ON_1 != READ_DIGITAL(R_Motor_NeedHomeDM, &CommStatus))
				{
					WRITE_DIGITAL(R_Motor_NeedHomeDM, ON_1 ,&CommStatus);
				}
			}
		}
		else if(STRCMP_L(RunStr, "STOP"))		msRet = STOP(atoi(ChambStr));
		else if(STRCMP_L(RunStr, "RUNP"))		msRet = GOMOVE(atoi(ChambStr), atof(ParamStr), atof(ParamStr));
		//... 2015.02.28 Add Chuck Move Cycle for MAHA-DPT MFG TFT
		else if(STRCMP_L(RunStr, "RUNCYCLE"))	msRet = MotorMovingCycle(atoi(ChambStr), atof(ParamStr));
		//
		//... 2016.01.19 Dynamic LeakCheck
		else if(STRCMP_L(RunStr, "DYN_CYCLE"))	msRet = DynLKCycle();
		//
		else if(STRCMP_L(RunStr, "RUN") || STRCMP_L(RunStr, "RUNF"))
		{	//... Used by PROCESS_CONTROL
			dbSpeed = READ_ANALOG(PRMA_ZMT_NormSpeed, &CommStatus);
			WRITE_ANALOG(R_Motor1_High_Speed, dbSpeed, &CommStatus);   
			WRITE_ANALOG(R_Motor2_High_Speed, dbSpeed, &CommStatus);
			
			//...
			msRet = GOMOVE(atoi(ChambStr), atof(ParamStr), atof(ParamStr2));		
		}
		else 
		{
			printf("[ABORT] %s Invalid Command\n", PROGRAM_FUNCTION_READ());
			break;
		}

		if(msRet == SYS_ABORTED)	WRITE_DIGITAL(R_Motor_Stop, FAST_ALL, &CommStatus);

	} while (0);

	printf("[INFO] Leaved %s[%d]\n", PROGRAM_FUNCTION_READ(), msRet);

	return msRet;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	int CommStatus;	

	if(READ_DIGITAL(IO_DRIVER_LOAD, &CommStatus) == OFF_0)	{ 
		SIMULATION_MODE = TRUE;
	}
	else SIMULATION_MODE = FALSE;
	return TRUE;
}

