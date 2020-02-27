#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

//==========================================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//==========================================================================================

#include "TextLogDll.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE	= FALSE;
BOOL	ALARM_HAPPEN;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		ALARM_COMM_ERROR		0
#define		ALARM_FATAL				1
#define		ALARM_NOT_HOME			2
#define		ALARM_MOTION			3
#define		ALARM_OTHER				4
#define		ALARM_CHECK_WAFER		5

#define		ALARM_ROBOT_ERROR		6
#define		ALARM_AWS_ERROR			7
#define		ALARM_ROBOT_RUNNING		8

#define		ALARM_HOME_WAFER		10
#define		ALARM_PICK_WAFER		11
#define		ALARM_PLACE_WAFER		12
#define		ALARM_ROTATE_WAFER		13
#define		ALARM_EXTEND_WAFER		14
#define		ALARM_RETRACT_WAFER		15
#define		ALARM_UP_WAFER			16
#define		ALARM_DOWN_WAFER		17

#define		ALARM_TM_ARM_WAFER_SYNCH_MISS	6520
#define		ALARM_RB_PICK_WFR_ERR_ANI		6521
#define		ALARM_RB_PICK_WFR_ERR_ANI2		6522
#define		ALARM_RB_PLACE_WFR_ERR_ANI		6523
#define		ALARM_RB_PLACE_WFR_ERR_ANI2		6524

#define		ALARM_PMA_1_CHUCK_NOTHOME	6530
#define		ALARM_PMA_2_CHUCK_NOTHOME	6531
#define		ALARM_PMB_1_CHUCK_NOTHOME	6532
#define		ALARM_PMB_2_CHUCK_NOTHOME	6533
#define		ALARM_PMC_1_CHUCK_NOTHOME	6534
#define		ALARM_PMC_2_CHUCK_NOTHOME	6535

#define		ALARM_SLIDE_PMA_1			6540
#define		ALARM_SLIDE_PMA_2			6541
#define		ALARM_SLIDE_PMB_1			6542
#define		ALARM_SLIDE_PMB_2			6543
#define		ALARM_SLIDE_PMC_1			6544
#define		ALARM_SLIDE_PMC_2			6545
#define		ALARM_SLIDE_BM_1			6546
#define		ALARM_SLIDE_BM_2			6547

#define		ALARM_SLITVV_PMA_1			6550
#define		ALARM_SLITVV_PMA_2			6551
#define		ALARM_SLITVV_PMB_1			6552
#define		ALARM_SLITVV_PMB_2			6553
#define		ALARM_SLITVV_PMC_1			6554
#define		ALARM_SLITVV_PMC_2			6555
#define		ALARM_SLITVV_BM1_1			6556
#define		ALARM_SLITVV_BM1_2			6557
#define		ALARM_SLITVV_BM2_1			6558
#define		ALARM_SLITVV_BM2_2			6559

// 2020.02.26 Wafer Sync
#define		ALARM_WAFER_PICK_SYNC_FAIL	6560
#define		ALARM_WAFER_PLACE_SYNC_FAIL	6561

#define		ALARM_ROBOT_COMMUNITION	7300
//------------------------------------------------------------------------------------------
#define		ARM_A		0
#define		ARM_B		1
//------------------------------------------------------------------------------------------
#define		ROBOT_STATUS_DOWN			1
#define		ROBOT_STATUS_UP				2
#define		ROBOT_STATUS_RETRACT		1
#define		ROBOT_STATUS_EXTEND			2
//2008.01.09 by mgsong
#define     OPERATION_ERROR_COUNT		15
#define     MOTION_ERROR_COUNT			12
#define     MOTOR_ERROR_COUNT			49
//------------------------------------------------------------------------------------------
int ALARM_INDEX_POINTER = 6501;
//------------------------------------------------------------------------------------------
enum { ROBOT_SYNCH_READY , ROBOT_SYNCH_ROTATE_START , ROBOT_SYNCH_EXTEND_START , ROBOT_SYNCH_UPDATE , ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RESET_REVERSE , ROBOT_SYNCH_RESET_ONLY , ROBOT_SYNCH_SUCCESS };
//------------------------------------------------------------------------------------------
enum { NO_STATION = -1, PMA , PMB , PMC , BM1_1 , BM1_2 , BM2_1 , BM2_2 };
enum { READY	, RUNNING	};
enum { UV_IDLE , UV_BUSY , UV_COMPLETE , UV_FAIL } ;
enum { Absent_0 , Present_1 } ;
enum { NOTHOME	, YESHOME	};
enum { YESSLIDE	, NOTSLIDE	};
enum { eLK_START, eLK_END	};  //... 2016.01.19
//2014.10.04
enum { SRC_A, SRC_B, SRC_C, SRC_X, SRC_Y, SRC_D };
//2008.01.09 added by mgsong
enum {OPERATION_ERROR = 0 , MOTION_ERROR , MOTOR_ERROR};

//... 2017.03.07
enum { All_Enable,  SEna_VDis,  SDis_VEna,  All_Disable };

enum {noCheck, check};

// added 2003.06.17 hjeon - for wafer counter
BOOL gbDestCM = FALSE;
BOOL Old_AWS_Control;

char *ROBOT_ACTION_COMPLETE		= "Action%s was Completed. will run next action.\nCheck Wafer Status.";
char *ROBOT_ACTION_NOT_COMPLETE	= "Action was failed. will Retry%s.\nCheck Wafer Status.";

//2014.08.06
char *EVENT_STRING_PICK_TO_PM1		= "EVENT_ON VTM_ROBOT_PICK_TO_PM1";
char *EVENT_STRING_PICK_TO_PM2		= "EVENT_ON VTM_ROBOT_PICK_TO_PM2";
char *EVENT_STRING_PICK_TO_PM3		= "EVENT_ON VTM_ROBOT_PICK_TO_PM3";
char *EVENT_STRING_PLACE_TO_PM1		= "EVENT_ON VTM_ROBOT_PLACE_TO_PM1";
char *EVENT_STRING_PLACE_TO_PM2		= "EVENT_ON VTM_ROBOT_PLACE_TO_PM2";
char *EVENT_STRING_PLACE_TO_PM3		= "EVENT_ON VTM_ROBOT_PLACE_TO_PM3";
char *EVENT_STRING_PICK_TO_LL1		= "EVENT_ON VTM_ROBOT_PICK_TO_LL1";
char *EVENT_STRING_PICK_TO_LL2		= "EVENT_ON VTM_ROBOT_PICK_TO_LL2";
char *EVENT_STRING_PLACE_TO_LL1		= "EVENT_ON VTM_ROBOT_PLACE_TO_LL1";
char *EVENT_STRING_PLACE_TO_LL2		= "EVENT_ON VTM_ROBOT_PLACE_TO_LL2";
//
//------------------------------------------------------------------------------------------
//2008.01.08
//6 axis station number mgsong
//MAHA ULVAC Station Number hyuk
//								  PMA   PMB   PMC   LL1_1 LL1_2  LL2_1  LL2_2
int	MODULE_CONTROL_STATION[7] = {  1  ,  2  ,  3  ,  4  ,   5  ,   6  ,  7 };

//2008.01.09 Alarm Number Change with 6 axis robot
//2009.05.26 Alarm Added for AWC + AWC Calibration
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	//============================================================================
	{ "e:SCHEDULER"				, _K_S_IO	,	SCHEDULER			,	0	} ,			//2014.08.06
	//============================================================================
	{ "FC_TM_ROBOT_CONTROL"		, _K_D_IO	,	CONTROL				,	0	} ,

	{ "ULVAC_CommStatus"		, _K_D_IO	,	MTR7_Comm_Status	,	0	} ,
//	{ "MTR7.Error_Clear"		, _K_D_IO	,	MTR7_T_Error_Clear	,	0	} ,  //
	{ "ULVAC_Run_Status"		, _K_D_IO	,	MTR7_Run_Status		,	0	} ,
	{ "TM.VacRobotBusy"			, _K_D_IO	,	TM_RB_Run_Status	,	0	} ,

	{ "ULVAC_HI_Pick_UP"		, _K_D_IO	,	MTR7_Get_Robot		,	0	} ,
	{ "ULVAC_HI_Pick_A"			, _K_D_IO	,	MTR7_Get_RobotL		,	0	} ,
	{ "ULVAC_HI_Pick_B"			, _K_D_IO	,	MTR7_Get_RobotR		,	0	} ,
	{ "ULVAC_HI_Place_UP"		, _K_D_IO	,	MTR7_Put_Robot		,	0	} ,
	{ "ULVAC_HI_Place_A"		, _K_D_IO	,	MTR7_Put_RobotL		,	0	} ,
	{ "ULVAC_HI_Place_B"		, _K_D_IO	,	MTR7_Put_RobotR		,	0	} ,

	{ "ULVAC_Rotate_UP"			, _K_D_IO	,	MTR7_Rot_Robot		,	0	} ,
	{ "ULVAC_Rotate_Dn_UP"		, _K_D_IO	,	MTR7_RotG_Robot		,	0	} ,
	{ "ULVAC_Rotate_Up_UP"		, _K_D_IO	,	MTR7_RotP_Robot		,	0	} ,

	{ "ULVAC_Ext_Robot"			, _K_D_IO	,	MTR7_Ext_Robot		,	0	} ,
	{ "ULVAC_Ret_Robot"			, _K_D_IO	,	MTR7_Ret_Robot		,	0	} ,
//	{ "MTR7.Up_Robot"			, _K_D_IO	,	MTR7_Up_Robot		,	0	} ,
//	{ "MTR7.Down_Robot"			, _K_D_IO	,	MTR7_Down_Robot		,	0	} ,

	{ "ULVAC_HI_Pick_LO"		, _K_D_IO	,	MTR7_Get_RobotB		,	0	} ,
	{ "ULVAC_HI_Pick_C"			, _K_D_IO	,	MTR7_Get_RobotBL	,	0	} ,
	{ "ULVAC_HI_Pick_D"			, _K_D_IO	,	MTR7_Get_RobotBR	,	0	} ,
	{ "ULVAC_HI_Place_LO"		, _K_D_IO	,	MTR7_Put_RobotB		,	0	} ,
	{ "ULVAC_HI_Place_C"		, _K_D_IO	,	MTR7_Put_RobotBL	,	0	} ,
	{ "ULVAC_HI_Place_D"		, _K_D_IO	,	MTR7_Put_RobotBR	,	0	} ,

	{ "ULVAC_Rotate_LO"			, _K_D_IO	,	MTR7_Rot_RobotB		,	0	} ,
	{ "ULVAC_Rotate_Dn_LO"		, _K_D_IO	,	MTR7_RotG_RobotB	,	0	} ,
	{ "ULVAC_Rotate_Up_LO"		, _K_D_IO	,	MTR7_RotP_RobotB	,	0	} ,

//	{ "ULVAC_Ext_RobotB"		, _K_D_IO	,	MTR7_Ext_RobotB		,	0	} ,
//	{ "ULVAC_Ret_RobotB"		, _K_D_IO	,	MTR7_Ret_RobotB		,	0	} ,
//	{ "MTR7.Up_RobotB"			, _K_D_IO	,	MTR7_Up_RobotB		,	0	} ,
//	{ "MTR7.Down_RobotB"		, _K_D_IO	,	MTR7_Down_RobotB	,	0	} ,

	{ "ULVAC_Home"				, _K_D_IO	,	MTR7_HOME			,	0	} ,
	{ "ULVAC_Home2"				, _K_D_IO	,	MTR7_HOME2			,	0	} ,
	{ "ULVAC_Stop"				, _K_D_IO	,	MTR7_STOP			,	0	} ,
	{ "ULVAC_Err_Code"			, _K_D_IO	,	MTR7_ErrorCode		,	0	} ,
	{ "ULVAC_Error"				, _K_D_IO	,	MTR7_Error			,	0	} ,

	{ "TM.VAC_Robot_Error_0"	, _K_D_IO	,	TM_RB_ErrorBit0 	,	0	} ,
	{ "TM.VAC_Robot_Error_1"	, _K_D_IO	,	TM_RB_ErrorBit1		,	0	} ,
	{ "TM.VAC_Robot_Error_2"	, _K_D_IO	,	TM_RB_ErrorBit2		,	0	} ,
	{ "TM.VAC_Robot_Error_3"	, _K_D_IO	,	TM_RB_ErrorBit3		,	0	} ,
	{ "TM.VAC_Robot_Error_4"	, _K_D_IO	,	TM_RB_ErrorBit4		,	0	} ,

	{ "TM.RB_SYNCH"				, _K_D_IO	,	ROBOT_SYNCH			,	0	} ,
	{ "TM.RB_RET"				, _K_D_IO	,	ROBOT_RET			,	0	} ,
	{ "TM.RB_STATION"			, _K_D_IO	,	ROBOT_STATION		,	0	} ,
	{ "PRMD_TM_RB_MOVE_TMO"		, _K_D_IO	,	MOVE_WAFER_TIMEOUT	,	0	} ,

	{ "CTC.Rb_Rotation"			, _K_A_IO	,	CTC_Rb_Rotation 	,	0	} ,
	{ "CTC.Rb_Movement"			, _K_A_IO	,	CTC_Rb_Movement 	,	0	} ,
	{ "CTC.Rb_Extension"		, _K_A_IO	,	CTC_Rb_Extension 	,	0	} ,
	{ "CTC.Rb_Extension2"		, _K_A_IO	,	CTC_Rb_Extension2 	,	0	} ,

	{ "FUNC_WFR_COUNTER"		, _K_F_IO	,	FUNC_WFR_COUNTER	,	0	} ,

    { "CTC.TA_Wafer_Status"		, _K_D_IO	,	TM_A_Status			,	0	} ,
	{ "CTC.TA_Wafer_Status2"	, _K_D_IO	,	TM_A_Status2		,	0	} ,
    { "CTC.TB_Wafer_Status"		, _K_D_IO	,	TM_B_Status			,	0	} ,
	{ "CTC.TB_Wafer_Status2"	, _K_D_IO	,	TM_B_Status2		,	0	} ,
	{ "CTC.BM2_Wafer_Status"	, _K_D_IO	,	BM2_Status1			,	0	} ,
	{ "CTC.BM2_Wafer_Status2"	, _K_D_IO	,	BM2_Status2			,	0	} ,
	{ "CTC.BM2_Wafer_Status3"	, _K_D_IO	,	BM2_Status3			,	0	} ,
	{ "CTC.BM2_Wafer_Status4"	, _K_D_IO	,	BM2_Status4			,	0	} ,
	{ "TM.ATMVACSensorXI"	    , _K_D_IO	,	TM_Sensor			,  	0	} ,
	{ "TM.CGChmPrsAI"	        , _K_A_IO	,	TM_Pressure			,	0	} ,
	{ "BM2.ATMVACSensorXI"	    , _K_D_IO	,	BM2_Sensor			,	0	} ,
	{ "BM2.CGChmPrsAI"	        , _K_A_IO	,	BM2_Pressure		,	0	} ,
	{ "TMC_ISOGATE_CONTROL"	    , _K_F_IO	,	TMC_ISOGATE_CONTROL	,	0	} ,

	{ "ULVAC_Load_Status_A"		, _K_D_IO   ,	ULVAC_Load_Status_A	,	0	} ,
	{ "ULVAC_Load_Status_B"		, _K_D_IO   ,	ULVAC_Load_Status_B	,	0	} ,
	{ "ULVAC_Load_Status_C"		, _K_D_IO   ,	ULVAC_Load_Status_C	,	0	} ,
	{ "ULVAC_Load_Status_D"		, _K_D_IO   ,	ULVAC_Load_Status_D	,	0	} ,

	{ "OBJ_TM_AWS"				, _K_F_IO	,	OBJ_TM_AWS				,	0	} ,
	{ "ULVAC_AWS_LIMIT_PM_HI"	, _K_A_IO	,	ULVAC_AWS_LIMIT_PM_HI	,	0	} ,
	{ "ULVAC_AWS_LIMIT_BM_HI"	, _K_A_IO	,	ULVAC_AWS_LIMIT_BM_HI	,	0	} ,
	{ "ULVAC_AWS_LIMIT_PM_LOW"	, _K_A_IO	,	ULVAC_AWS_LIMIT_PM_LOW	,	0	} ,
	{ "ULVAC_AWS_LIMIT_BM_LOW"	, _K_A_IO	,	ULVAC_AWS_LIMIT_BM_LOW	,	0	} ,
	{ "ULVAC_AWS_USE_V"			, _K_D_IO	,	ULVAC_AWS_USE_V			,	0	} ,

	{ "ULVAC_AWS_INIT_A"		, _K_D_IO	,	ULVAC_AWS_INIT_A		,	0	} ,
	{ "ULVAC_AWS_INIT_B"		, _K_D_IO	,	ULVAC_AWS_INIT_B		,	0	} ,
	{ "ULVAC_AWS_INIT_C"		, _K_D_IO	,	ULVAC_AWS_INIT_C		,	0	} ,
	{ "ULVAC_AWS_INIT_D"		, _K_D_IO	,	ULVAC_AWS_INIT_D		,	0	} ,
	{ "ULVAC_AWS_INIT_UP"		, _K_D_IO	,	ULVAC_AWS_INIT_UP		,	0	} ,
	{ "ULVAC_AWS_INIT_LO"		, _K_D_IO	,	ULVAC_AWS_INIT_LO		,	0	} ,

	{ "PM1.ZMT_HomeSnsDI"		, _K_D_IO	,	PMA_1_CHUCK_HOME_STAT	,	0	} ,
	{ "PM1.ZMT2_HomeSnsDI"		, _K_D_IO	,	PMA_2_CHUCK_HOME_STAT	,	0	} ,
	{ "PM2.ZMT_HomeSnsDI"		, _K_D_IO	,	PMB_1_CHUCK_HOME_STAT	,	0	} ,
	{ "PM2.ZMT2_HomeSnsDI"		, _K_D_IO	,	PMB_2_CHUCK_HOME_STAT	,	0	} ,
	{ "PM3.ZMT_HomeSnsDI"		, _K_D_IO	,	PMC_1_CHUCK_HOME_STAT	,	0	} ,
	{ "PM3.ZMT2_HomeSnsDI"		, _K_D_IO	,	PMC_2_CHUCK_HOME_STAT	,	0	} ,

	{ "PM1.SERV_MAIN"			, _K_F_IO	,	PMA_SERV_MAIN			,	0	} ,
	{ "PM2.SERV_MAIN"			, _K_F_IO	,	PMB_SERV_MAIN			,	0	} ,
	{ "PM3.SERV_MAIN"			, _K_F_IO	,	PMC_SERV_MAIN			,	0	} ,

	{ "TM.PM1.WaferSlide1DI"	, _K_D_IO	,	TM_Slide_PMA_1			,	0	} ,
	{ "TM.PM1.WaferSlide2DI"	, _K_D_IO	,	TM_Slide_PMA_2			,	0	} ,
	{ "TM.PM2.WaferSlide1DI"	, _K_D_IO	,	TM_Slide_PMB_1			,	0	} ,
	{ "TM.PM2.WaferSlide2DI"	, _K_D_IO	,	TM_Slide_PMB_2			,	0	} ,
	{ "TM.PM3.WaferSlide1DI"	, _K_D_IO	,	TM_Slide_PMC_1			,	0	} ,
	{ "TM.PM3.WaferSlide2DI"	, _K_D_IO	,	TM_Slide_PMC_2			,	0	} ,
	{ "BM.WaferSlideR1DI"		, _K_D_IO	,	TM_Slide_BM_1			,	0	} ,
	{ "BM.WaferSlideL1DI"		, _K_D_IO	,	TM_Slide_BM_2			,	0	} ,

	{ "TM.PM1.SlotVv1XI"		, _K_D_IO	,	TM_SlitVv_PMA_1			,	0	} ,
	{ "TM.PM1.SlotVv2XI"		, _K_D_IO	,	TM_SlitVv_PMA_2			,	0	} ,
	{ "TM.PM2.SlotVv1XI"		, _K_D_IO	,	TM_SlitVv_PMB_1			,	0	} ,
	{ "TM.PM2.SlotVv2XI"		, _K_D_IO	,	TM_SlitVv_PMB_2			,	0	} ,
	{ "TM.PM3.SlotVv1XI"		, _K_D_IO	,	TM_SlitVv_PMC_1			,	0	} ,
	{ "TM.PM3.SlotVv2XI"		, _K_D_IO	,	TM_SlitVv_PMC_2			,	0	} ,
	{ "TM.BM1.SlotVvRXI"		, _K_D_IO	,	TM_SlitVv_BM1_1			,	0	} ,
	{ "TM.BM1.SlotVvLXI"		, _K_D_IO	,	TM_SlitVv_BM1_2			,	0	} ,
	{ "TM.BM2.SlotVvRXI"		, _K_D_IO	,	TM_SlitVv_BM2_1			,	0	} ,
	{ "TM.BM2.SlotVvLXI"		, _K_D_IO	,	TM_SlitVv_BM2_2			,	0	} ,

	{ "CTC_EVENT_LOG_SVR"		, _K_F_IO	,	CTC_EVENT_LOG_SVR	,	0	} ,		// 2013.11.15 for MARS
	// 2014.10.04
    { "CTC.TA_Wafer_Source"		, _K_D_IO	,	TM_A_Source			,	0	} ,
	{ "CTC.TA_Wafer_Source2"	, _K_D_IO	,	TM_A_Source2		,	0	} ,
    { "CTC.TB_Wafer_Source"		, _K_D_IO	,	TM_B_Source			,	0	} ,
	{ "CTC.TB_Wafer_Source2"	, _K_D_IO	,	TM_B_Source2		,	0	} ,

	{ "CTC.JOB_NAME"			, _K_S_IO	,	JOB_NAME			,	0	} ,
	{ "CTC.JOB_NAME2"			, _K_S_IO	,	JOB_NAME2			,	0	} ,
	{ "CTC.JOB_NAME3"			, _K_S_IO	,	JOB_NAME3			,	0	} ,

	{ "CTC.MID_NAME"			, _K_S_IO	,	MID_NAME			,	0	} ,
	{ "CTC.MID_NAME2"			, _K_S_IO	,	MID_NAME2			,	0	} ,
	{ "CTC.MID_NAME3"			, _K_S_IO	,	MID_NAME3			,	0	} ,

    { "TM_RB_LOTID1"			, _K_S_IO	,	TM_RB_LOTID1		,	0	} ,
	{ "TM_RB_LOTID2"			, _K_S_IO	,	TM_RB_LOTID2		,	0	} ,
    { "TM_RB_MIDID1"			, _K_S_IO	,	TM_RB_MIDID1		,	0	} ,
	{ "TM_RB_MIDID2"			, _K_S_IO	,	TM_RB_MIDID2		,	0	} ,
    { "TM_RB_WAFERID1"			, _K_D_IO	,	TM_RB_WAFERID1		,	0	} ,
	{ "TM_RB_WAFERID2"			, _K_D_IO	,	TM_RB_WAFERID2		,	0	} ,
	//... 2016.01.19
	{ "PRMD_TM_ROTATE_CNT"		, _K_D_IO	,	PRMD_TM_ROTATE_CNT	,	0	} ,
	//...
	//... 2017.03.07
	{ "INTLKS_CONTROL"			, _K_D_IO	,	INTLKS_CONTROL		,	0	} ,

	// ULVAC AXIS별 부하율 Reading ( 2013/02/05 , LJH )
	{"ULVAC_LOAD_AXIS_CTR", _K_D_IO, ULVAC_LOAD_AXIS_CTR, 0},
	{"ULVAC_LOAD_READ", _K_S_IO, ULVAC_LOAD_READ, 0},
	{"ULVAC_LOAD_Ti", _K_D_IO, ULVAC_LOAD_Ti, 0},
	{"ULVAC_LOAD_A", _K_D_IO, ULVAC_LOAD_A, 0},
	{"ULVAC_LOAD_D", _K_D_IO, ULVAC_LOAD_D, 0},
	{"ULVAC_LOAD_To", _K_D_IO, ULVAC_LOAD_To, 0},
	{"ULVAC_LOAD_B", _K_D_IO, ULVAC_LOAD_B, 0},
	{"ULVAC_LOAD_C", _K_D_IO, ULVAC_LOAD_C, 0},
	{"ULVAC_LOAD_Z", _K_D_IO, ULVAC_LOAD_Z, 0},

	// ROBOT RUN STATUS : ACTION 시 IDle , Busy , Complete , Fail 상태.
	{"ULVAC_LOAD_ALM_RANGE", _K_D_IO, ULVAC_LOAD_ALM_RANGE, 0},

	// ULVAC AXIS별 Count, TIme Reading ( 2013/10/29, JBM )
	{"ULVAC_MOVE_CNT_READ", _K_S_IO, ULVAC_MOVE_CNT_READ, 0},
	{"ULVAC_MOVE_CNT_N", _K_D_IO, ULVAC_MOVE_CNT_N, 0},
	{"ULVAC_MOVE_CNT_R", _K_D_IO, ULVAC_MOVE_CNT_R, 0},
	{"ULVAC_MOVE_CNT_Z", _K_D_IO, ULVAC_MOVE_CNT_Z, 0},
	{"ULVAC_MOVE_TIME", _K_D_IO, ULVAC_MOVE_TIME, 0},
	{"PRMD_TM_ROBOT_Load", _K_D_IO, PRMD_TM_ROBOT_Load, 0},

	//2020.02.26 Wafer Sync
	{ "CTC.PM1_Wafer_Status"		,_K_D_IO	,	CTC_PM1_Wafer_Status	,	0	} ,
	{ "CTC.PM1_Wafer_Status2"		,_K_D_IO	,	CTC_PM1_Wafer_Status2	,	0	} ,
	{ "CTC.PM2_Wafer_Status"		,_K_D_IO	,	CTC_PM2_Wafer_Status	,	0	} ,
	{ "CTC.PM2_Wafer_Status2"		,_K_D_IO	,	CTC_PM2_Wafer_Status2	,	0	} ,
	{ "CTC.PM3_Wafer_Status"		,_K_D_IO	,	CTC_PM3_Wafer_Status	,	0	} ,
	{ "CTC.PM3_Wafer_Status2"		,_K_D_IO	,	CTC_PM3_Wafer_Status2	,	0	} ,
	{ "PM1.WaferDI"					,_K_D_IO	,	PM1_WaferDI				,	0	} ,
	{ "PM1.Wafer2DI"				,_K_D_IO	,	PM1_Wafer2DI			,	0	} ,
	{ "PM2.WaferDI"					,_K_D_IO	,	PM2_WaferDI				,	0	} ,
	{ "PM2.Wafer2DI"				,_K_D_IO	,	PM2_Wafer2DI			,	0	} ,
	{ "PM3.WaferDI"					,_K_D_IO	,	PM3_WaferDI				,	0	} ,
	{ "PM3.Wafer2DI"				,_K_D_IO	,	PM3_Wafer2DI			,	0	} ,

	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2005.08.19 by cskim, Wafer Sync
enum { RB_IDLE, RB_READY, RB_DOING_STEP1, RB_DOING_STEP2, RB_DONE };
int g_nRobotStatus = RB_IDLE;

//------------------------------------------------------------------------------------------
// 2005.12.20
#include "DynamicIO.h"
CDynamicIO g_DynIO;
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	CTC_AbortControl,
	TMSIM_VAC_ROBOT,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"CTC.AbortControl",				_K_D_IO, 0},
	{"TMSIM_VAC_ROBOT",					_K_F_IO, 0},
	""
};
BOOL CheckAbortControl()
{
	BOOL bRet = FALSE;
	BOOL bIOStatus;
	enum { ABC_IDLE, ABC_ABORT, ABC_ABORTING };
	if(g_DynIO.IOValid(CTC_AbortControl))
	{
		if(g_DynIO.dREAD_DIGITAL(CTC_AbortControl, &bIOStatus) != ABC_IDLE) bRet = TRUE;
	}
	return bRet;
}
// 2020.02.26 Wafer Sync
void WaferImageSync(char *RunStr, int EStation)
{
	int nCommStatus=0;
	int nCommStatus2=0;
	int nErrorCnt=0;
	int nCTC_Wafer_StatusIO, nCTC_Wafer_StatusValue;
	int nCTC_Wafer_StatusIO2, nCTC_Wafer_StatusValue2;
	int nPM_Wafer_StatusIO, nPM_Wafer_StatusValue;
	int nPM_Wafer_StatusIO2, nPM_Wafer_StatusValue2;
	int nCheckFlag = FALSE, nCheckFlag2 = FALSE;

	if(	STRCMP_L(RunStr, "PICK") || STRCMP_L(RunStr, "PLACE"))
	{
		switch(EStation)
		{
		case PMA:
			nCTC_Wafer_StatusIO = CTC_PM1_Wafer_Status;
			nPM_Wafer_StatusIO = PM1_Wafer2DI;

			nCTC_Wafer_StatusIO2 = CTC_PM1_Wafer_Status2;
			nPM_Wafer_StatusIO2 = PM1_WaferDI;
			break;

		case PMB:
			nCTC_Wafer_StatusIO = CTC_PM2_Wafer_Status;
			nPM_Wafer_StatusIO = PM2_Wafer2DI;

			nCTC_Wafer_StatusIO2 = CTC_PM2_Wafer_Status2;
			nPM_Wafer_StatusIO2 = PM2_WaferDI;
			break;

		case PMC:
			nCTC_Wafer_StatusIO = CTC_PM3_Wafer_Status;
			nPM_Wafer_StatusIO = PM3_Wafer2DI;

			nCTC_Wafer_StatusIO2 = CTC_PM3_Wafer_Status2;
			nPM_Wafer_StatusIO2 = PM3_WaferDI;
			break;

		default:
			nCTC_Wafer_StatusIO = -1;
			nPM_Wafer_StatusIO = -1;
			nCTC_Wafer_StatusIO2 = -1;
			nPM_Wafer_StatusIO2 = -1;
			break;
		}

		if(nCTC_Wafer_StatusIO != -1 && nPM_Wafer_StatusIO != -1 && nCTC_Wafer_StatusIO2 != -1 && nPM_Wafer_StatusIO2 != -1)
		{
			while(nErrorCnt < 10)
			{
				//CTC PM Wafer Status Read
				nCTC_Wafer_StatusValue = READ_DIGITAL(nCTC_Wafer_StatusIO, &nCommStatus);
				nCTC_Wafer_StatusValue2 = READ_DIGITAL(nCTC_Wafer_StatusIO2, &nCommStatus2);
				if((nCommStatus != TRUE) || (nCommStatus2 != TRUE))
				{
					nErrorCnt++;
					_LOG("----------> CTC Wafer Image Read Fail Count : %d", nErrorCnt);
					continue;
				}


				//PM Wafer Status Write
				if(nCTC_Wafer_StatusValue > 0)	WRITE_DIGITAL(nPM_Wafer_StatusIO, 1, &nCommStatus);
				else							WRITE_DIGITAL(nPM_Wafer_StatusIO, 0, &nCommStatus);

				if(nCTC_Wafer_StatusValue2 > 0)	WRITE_DIGITAL(nPM_Wafer_StatusIO2, 1, &nCommStatus2);
				else							WRITE_DIGITAL(nPM_Wafer_StatusIO2, 0, &nCommStatus2);

				//PM Wafer Status Read
				nPM_Wafer_StatusValue = READ_DIGITAL(nPM_Wafer_StatusIO, &nCommStatus);
				nPM_Wafer_StatusValue2 = READ_DIGITAL(nPM_Wafer_StatusIO2, &nCommStatus2);
				if((nCommStatus != TRUE) || (nCommStatus2 != TRUE))
				{
					nErrorCnt++;
					_LOG("----------> PM Wafer Image Read Fail Count : %d", nErrorCnt);
					continue;
				}

				//CTC <-> PM Side01 Wafer Status Sync Check
				if(nCTC_Wafer_StatusValue > 0)
				{
					if(nPM_Wafer_StatusValue != 1)
					{
						nErrorCnt++;
						_LOG("----------> Wafer Image Present Sync Fail Count : %d", nErrorCnt);

						continue;
					}
					else
					{
						_LOG("----------> Wafer Image Present Sync Check Ok");
					}
				}
				else
				{
					if(nPM_Wafer_StatusValue != 0)
					{
						nErrorCnt++;
						_LOG("----------> Wafer Image Absent Sync Fail Count : %d", nErrorCnt);

						continue;
					}
					else
					{
						_LOG("----------> Wafer Image Absent Sync Check Ok");
					}
				}

				//CTC <-> PM Side02 Wafer Status Sync Check
				if(nCTC_Wafer_StatusValue2 > 0)
				{
					if(nPM_Wafer_StatusValue2 != 1)
					{
						nErrorCnt++;
						_LOG("----------> Wafer2 Image Present Sync Fail Count : %d", nErrorCnt);

						continue;
					}
					else
					{
						_LOG("----------> Wafer2 Image Present Sync Check Ok");
						break;
					}
				}
				else
				{
					if(nPM_Wafer_StatusValue2 != 0)
					{
						nErrorCnt++;
						_LOG("----------> Wafer2 Image Absent Sync Fail Count : %d", nErrorCnt);

						continue;
					}
					else
					{
						_LOG("----------> Wafer2 Image Absent Sync Check Ok");
						break;
					}
				}
				Sleep(500);
			}
		}
	}

	if(nErrorCnt >= 10)
	{
		if		(STRCMP_L(RunStr, "PICK"))	ALARM_POST(ALARM_WAFER_PICK_SYNC_FAIL);
		else if	(STRCMP_L(RunStr, "PLACE"))	ALARM_POST(ALARM_WAFER_PLACE_SYNC_FAIL);
	}
}
//////////////////////////////////////////////////////////////////////
// Global Function
// 2013.11.15
extern void _MLOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);
	WRITE_FUNCTION_EVENT(CTC_EVENT_LOG_SVR, _szString);
}

// 2013.11.15
void LogEvent(char *szModule, char *szRunStr, char *szStatus, char *szDest, int nSlot, int nArmIdx,
				char *szData, BOOL bDualArm)
{
	char szEventID[20];
	char szDeviceID[20];
	int offset = 0;

	if ( szDest[ 1 ] == '_' ) offset = 2;
	strcpy(szDeviceID, szDest+offset);

	do {
		if(STRCMP_L(szRunStr , "PICK")) strcpy(szEventID, "Get");				// 2013.07.18
		else if(STRCMP_L(szRunStr , "PLACE")) strcpy(szEventID, "Put");			// 2013.07.18
		else if(STRCMP_L(szRunStr , "ROTATE")) strcpy(szEventID, "Rotate");
		else break;

		if(! bDualArm) _MLOG("XFR %s %s %s %s %d %c %s", szModule, szEventID, szStatus, szDeviceID, nSlot, 'A'+nArmIdx, szData);
		else _MLOG("XFR %s %s %s %s [2,%d,%d] %c %s", szModule, szEventID, szStatus, szDeviceID, nSlot, nSlot+1, 'A'+nArmIdx, szData);	// 2013.07.29
	} while(0);
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int READ_DIGITAL_TRUE( int io , int *CommStatus ) {
	int Data;
	Data = READ_DIGITAL( io , CommStatus );
	if ( !(*CommStatus) ) {
		_sleep(250);
		Data = READ_DIGITAL( io , CommStatus );
		if ( !(*CommStatus) ) {
			_sleep(500);
			Data = READ_DIGITAL( io , CommStatus );
			if ( !(*CommStatus) ) {
				_sleep(1000);
				Data = READ_DIGITAL( io , CommStatus );
				if ( !(*CommStatus) ) {
					_sleep(1000);
					return( READ_DIGITAL( io , CommStatus ) );
				}
				else return Data;
			}
			else return Data;
		}
		else return Data;
	}
	else return Data;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PM_Chuck_Home_Check ( int Station ) {
	int CommStatus , AlmResult;
	int ChuckIo1 , ChuckIo2 , FunctionIo;
	int ChuckAlarm1 , ChuckAlarm2;

	_LOG("PM Chuck Home Check Start[Station : %d]", Station);

	switch ( Station ) {
	case PMA   :
		ChuckIo1    = PMA_1_CHUCK_HOME_STAT;
		ChuckIo2    = PMA_2_CHUCK_HOME_STAT;
		FunctionIo  = PMA_SERV_MAIN;
		ChuckAlarm1 = ALARM_PMA_1_CHUCK_NOTHOME;
		ChuckAlarm2 = ALARM_PMA_2_CHUCK_NOTHOME;
		break;
	case PMB   :
		ChuckIo1    = PMB_1_CHUCK_HOME_STAT;
		ChuckIo2    = PMB_2_CHUCK_HOME_STAT;
		FunctionIo  = PMB_SERV_MAIN;
		ChuckAlarm1 = ALARM_PMB_1_CHUCK_NOTHOME;
		ChuckAlarm2 = ALARM_PMB_2_CHUCK_NOTHOME;
		break;
	case PMC   :
		ChuckIo1    = PMC_1_CHUCK_HOME_STAT;
		ChuckIo2    = PMC_2_CHUCK_HOME_STAT;
		FunctionIo  = PMC_SERV_MAIN;
		ChuckAlarm1 = ALARM_PMC_1_CHUCK_NOTHOME;
		ChuckAlarm2 = ALARM_PMC_2_CHUCK_NOTHOME;
		break;
	}

	while ( TRUE ) {
		if ( READ_DIGITAL ( ChuckIo1 , &CommStatus ) == NOTHOME ) {
			AlmResult = ALARM_MANAGE ( ChuckAlarm1 );
			if		( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult == ALM_ABORT  ) return SYS_ABORTED;
			else {
				if ( RUN_FUNCTION ( FunctionIo , "CHUCKHOME" ) == SYS_ABORTED ) return SYS_ABORTED;
			}
		}
		else break;
	}
	while ( TRUE ) {
		if ( READ_DIGITAL ( ChuckIo2 , &CommStatus ) == NOTHOME ) {
			AlmResult = ALARM_MANAGE ( ChuckAlarm2 );
			if		( AlmResult == ALM_IGNORE ) break;
			else if ( AlmResult == ALM_ABORT  ) return SYS_ABORTED;
			else {
				if ( RUN_FUNCTION ( FunctionIo , "CHUCKHOME" ) == SYS_ABORTED ) return SYS_ABORTED;
			}
		}
		else break;
	}

	_LOG("PM Chuck Home Check End[Station : %d]", Station);
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status SlitVv_Sensor_Check ( int Station ) {
	int CommStatus , AlmResult;
	int SlitVvIo1 , SlitVvIo2;
	int SlitVvAlarm1 , SlitVvAlarm2;

	_LOG("TM SlitVv Sensor Check Start[Station : %d]", Station);

	switch ( Station ) {
	case PMA   :
		SlitVvIo1    = TM_SlitVv_PMA_1;
		SlitVvIo2    = TM_SlitVv_PMA_2;
		SlitVvAlarm1 = ALARM_SLITVV_PMA_1;
		SlitVvAlarm2 = ALARM_SLITVV_PMA_2;
		break;
	case PMB   :
		SlitVvIo1    = TM_SlitVv_PMB_1;
		SlitVvIo2    = TM_SlitVv_PMB_2;
		SlitVvAlarm1 = ALARM_SLITVV_PMB_1;
		SlitVvAlarm2 = ALARM_SLITVV_PMB_2;
		break;
	case PMC   :
		SlitVvIo1    = TM_SlitVv_PMC_1;
		SlitVvIo2    = TM_SlitVv_PMC_2;
		SlitVvAlarm1 = ALARM_SLITVV_PMC_1;
		SlitVvAlarm2 = ALARM_SLITVV_PMC_2;
		break;
	case BM1_1 :
	case BM1_2 :
		SlitVvIo1    = TM_SlitVv_BM1_1;
		SlitVvIo2    = TM_SlitVv_BM1_2;
		SlitVvAlarm1 = ALARM_SLITVV_BM1_1;
		SlitVvAlarm2 = ALARM_SLITVV_BM1_2;
		break;
		//---> End
	case BM2_1 :
	case BM2_2 :
		SlitVvIo1    = TM_SlitVv_BM2_1;
		SlitVvIo2    = TM_SlitVv_BM2_2;
		SlitVvAlarm1 = ALARM_SLITVV_BM2_1;
		SlitVvAlarm2 = ALARM_SLITVV_BM2_2;
		break;
	}

	while ( TRUE ) {
		if ( READ_DIGITAL ( SlitVvIo1 , &CommStatus ) != YESSLIDE ) break;
		_sleep ( 1000 ) ;

		if ( READ_DIGITAL ( SlitVvIo1 , &CommStatus ) != YESSLIDE ) break;
		_sleep ( 1000 ) ;

		if ( READ_DIGITAL ( SlitVvIo1 , &CommStatus ) != YESSLIDE )
		{
			break;
		}
		else
		{
			AlmResult = ALARM_MANAGE ( SlitVvAlarm1 );
			if ( AlmResult == ALM_IGNORE ) break;
			else return SYS_ABORTED;
		}
	}
	while ( TRUE ) {
		if ( READ_DIGITAL ( SlitVvIo2 , &CommStatus ) != YESSLIDE ) break;
		_sleep ( 1000 ) ;

		if ( READ_DIGITAL ( SlitVvIo2 , &CommStatus ) != YESSLIDE ) break;
		_sleep ( 1000 );

		if ( READ_DIGITAL ( SlitVvIo2 , &CommStatus ) != YESSLIDE )
		{
			break;
		}
		else
		{
			AlmResult = ALARM_MANAGE ( SlitVvAlarm2 );
			if ( AlmResult == ALM_IGNORE ) break;
			else return SYS_ABORTED;
		}
	}
	//]

	_LOG("TM SlitVv Sensor Check Start[Station : %d]", Station);
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Slide_Sensor_Check ( int Station ) {
	int CommStatus , AlmResult;
	int SlideIo1 , SlideIo2;
	int SlideAlarm1 , SlideAlarm2;

	_LOG("TM Slide Sensor Check Start[Station : %d]", Station);

	switch ( Station ) {
	case PMA   :
		SlideIo1    = TM_Slide_PMA_1;
		SlideIo2    = TM_Slide_PMA_2;
		SlideAlarm1 = ALARM_SLIDE_PMA_1;
		SlideAlarm2 = ALARM_SLIDE_PMA_2;
		break;
	case PMB   :
		SlideIo1    = TM_Slide_PMB_1;
		SlideIo2    = TM_Slide_PMB_2;
		SlideAlarm1 = ALARM_SLIDE_PMB_1;
		SlideAlarm2 = ALARM_SLIDE_PMB_2;
		break;
	case PMC   :
		SlideIo1    = TM_Slide_PMC_1;
		SlideIo2    = TM_Slide_PMC_2;
		SlideAlarm1 = ALARM_SLIDE_PMC_1;
		SlideAlarm2 = ALARM_SLIDE_PMC_2;
		break;
		//---> 2008/08/08
		//case BM1 :
	case BM1_1 :
	case BM1_2 :
		//---> End
	case BM2_1 :
	case BM2_2 :
		SlideIo1    = TM_Slide_BM_1;
		SlideIo2    = TM_Slide_BM_2;
		SlideAlarm1 = ALARM_SLIDE_BM_1;
		SlideAlarm2 = ALARM_SLIDE_BM_2;
		break;
	}

	//[ 2010/02/23 TM Robot Pick Place 동작 전후 Slide Sensor 중복 Check하도록 수정 ( BY LJH )
	while ( TRUE ) {
		if ( READ_DIGITAL ( SlideIo1 , &CommStatus ) != YESSLIDE ) break;
		_sleep ( 1000 ) ;

		if ( READ_DIGITAL ( SlideIo1 , &CommStatus ) != YESSLIDE ) break;
		_sleep ( 1000 ) ;

		if ( READ_DIGITAL ( SlideIo1 , &CommStatus ) != YESSLIDE )
		{
			break;
		}
		else
		{
			AlmResult = ALARM_MANAGE ( SlideAlarm1 );
			if ( AlmResult == ALM_IGNORE ) break;
			else return SYS_ABORTED;
		}
	}
	while ( TRUE ) {
		if ( READ_DIGITAL ( SlideIo2 , &CommStatus ) != YESSLIDE ) break;
		_sleep ( 1000 ) ;

		if ( READ_DIGITAL ( SlideIo2 , &CommStatus ) != YESSLIDE ) break;
		_sleep ( 1000 );

		if ( READ_DIGITAL ( SlideIo2 , &CommStatus ) != YESSLIDE )
		{
			break;
		}
		else
		{
			AlmResult = ALARM_MANAGE ( SlideAlarm2 );
			if ( AlmResult == ALM_IGNORE ) break;
			else return SYS_ABORTED;
		}
	}
	//]

	_LOG("TM Slide Sensor Check Start[Station : %d]", Station);
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int	CheckError() {
	int i , CommStatus;
	int EB[5] = { 0 , 0 , 0 , 0 , 0 };
	int ErrorCode = 0;

	//[ 2009/07/06 TM Robot Error Bit Read시 Define 되지 않은 Bit값이면 1초 후 한번 더 Erro Bit Read하도록 개선 ( BY LJH )
	int Abnormal_ErrorRead = 0 ;

	while ( TRUE ) {
		for ( i = 0 ; i < 5 ; i++ ) {
			EB[i] = READ_DIGITAL_TRUE ( TM_RB_ErrorBit0 + i , &CommStatus );
		}

		//  Error Bit   :  0             1             2             3             4
		if		( EB[0] == 0 && EB[1] == 0 && EB[2] == 0 && EB[3] == 0 && EB[4] == 0 ) {
			ErrorCode =    0;	// No Error
			break;
		}
		else if	( EB[0] == 0 && EB[1] == 1 && EB[2] == 0 && EB[3] == 0 && EB[4] == 0 ) ErrorCode =  221;	// Attempt was made to perform Remote action without returning home
		else if ( EB[0] == 1 && EB[1] == 1 && EB[2] == 0 && EB[3] == 0 && EB[4] == 0 ) ErrorCode =  226;	// Extention Interlock Error(Gate Valve)
		else if ( EB[0] == 0 && EB[1] == 0 && EB[2] == 1 && EB[3] == 0 && EB[4] == 0 ) ErrorCode =  299;	// Retraction Interlock Error(Gate Valve)
		else if ( EB[0] == 1 && EB[1] == 0 && EB[2] == 1 && EB[3] == 0 && EB[4] == 0 ) ErrorCode =  227;	// Command designation error.
		else if ( EB[0] == 1 && EB[1] == 1 && EB[2] == 1 && EB[3] == 0 && EB[4] == 0 ) ErrorCode =  223;	// Station that has not be taught is specified
		else if ( EB[0] == 0 && EB[1] == 0 && EB[2] == 0 && EB[3] == 1 && EB[4] == 0 ) ErrorCode =  224;	// Excitation is toggled off
		else if ( EB[0] == 1 && EB[1] == 1 && EB[2] == 0 && EB[3] == 1 && EB[4] == 0 ) ErrorCode =  229;	// Scheduled action time over
		else if ( EB[0] == 0 && EB[1] == 0 && EB[2] == 1 && EB[3] == 1 && EB[4] == 0 ) ErrorCode =  230;	// Input of externally actuated interlock contact
		else if ( EB[0] == 1 && EB[1] == 0 && EB[2] == 1 && EB[3] == 1 && EB[4] == 0 ) ErrorCode =  231;	// Host RS-232C communication error detected
		else if ( EB[0] == 0 && EB[1] == 1 && EB[2] == 1 && EB[3] == 1 && EB[4] == 0 ) ErrorCode =  232;	// Driver alarm given
		else if ( EB[0] == 1 && EB[1] == 1 && EB[2] == 1 && EB[3] == 1 && EB[4] == 0 ) ErrorCode =  233;	// Controller unit alarm given
		else if ( EB[0] == 0 && EB[1] == 0 && EB[2] == 0 && EB[3] == 0 && EB[4] == 1 ) ErrorCode =  225;	// Action command was issued in the manual mode
		else if ( EB[0] == 1 && EB[1] == 0 && EB[2] == 0 && EB[3] == 0 && EB[4] == 1 )		// 10진수로 17
		{

			if ( READ_DIGITAL ( MTR7_ErrorCode , &CommStatus ) > 0 )
			{
				ErrorCode = READ_DIGITAL ( MTR7_ErrorCode , &CommStatus );
			}
			else
			{
				ErrorCode = 2349;	// 234~249 를 의미하는 Defualt Value
			}

		}
		else if ( EB[0] == 0 && EB[1] == 1 && EB[2] == 0 && EB[3] == 0 && EB[4] == 1 )		// 10진수로 18
		{

			if ( READ_DIGITAL ( MTR7_ErrorCode , &CommStatus ) > 0 )
			{
				ErrorCode = READ_DIGITAL ( MTR7_ErrorCode , &CommStatus );
			}
			else
			{
				ErrorCode = 2403;	// 240,243 를 의미하는 Defualt Value
			}
		}
		else
		{
			Abnormal_ErrorRead++ ;

			if ( Abnormal_ErrorRead == 1 ) {
				_sleep ( 1000 ) ;
				continue ;
			}

			if ( READ_DIGITAL ( MTR7_ErrorCode , &CommStatus ) > 0 )
			{
				ErrorCode = READ_DIGITAL ( MTR7_ErrorCode , &CommStatus );
			}
			else
			{
				ErrorCode = 999;		// 기타  Defualt Value
			}
		}

		break ;
	}
	//]

	if ( ErrorCode != 0 ) READ_DIGITAL ( MTR7_Error , &CommStatus );   // For TRACE


/*
ERROR	Error 信?
CODE	4 3 2 1 0		?容
0		0 0 0 0 0		NO ERROR
1		0 0 0 0 1		티칭 데이터가 부정확하요 동작할 수 없다
2		0 0 0 1 0		원점미복귀
3		0 0 0 1 1		EXTEND 인터록 bitOFF
4		0 0 1 0 0		RETRACT 인터록 bitOFF
5 		0 0 1 0 1		동작의 순서 미스 / 대응하는 커멘드 없음
6 		0 0 1 1 0		예약
7 		0 0 1 1 1		티칭되어있지 않음
8 		0 1 0 0 0		ServoOFF 에 있음
9 		0 1 0 0 1		예약
10 		0 1 0 1 0		예약
11 		0 1 0 1 1		동작타임아웃
12 		0 1 1 0 0		EMS ON ／ 외부확장 EMS ON
13 		0 1 1 0 1 		HOST232c 통신에러발생
14 		0 1 1 1 0 		Driver Alarm 발생
15		0 1 1 1 1 		Driver 통신에러／Driver 현재위치이상/Driver ServoON 타임아웃/콘트롤러 PCB 에러／T-BOX Version 미스매칭
16		1 0 0 0 0 		메뉴얼모드에 있음
17		1 0 0 0 1 		웨이퍼를 가진 상태로 PICK 지령 / PICK 지령의 가는경로에서 웨이퍼를 검지 / PICK 지령의 오는경로에서 웨이퍼를 검지하지 못함 / 웨이퍼를 가지지 않는 상태로 PLACE 지령
18		1 0 0 1 0 		PLACE 지령의 오는경로에서 웨이퍼를 검지 / PLACE 지령의 가는경로에서 웨이퍼를 검지하지 못함 / HOME2,HOME3 지령으로 뻗는동작전에 웨이퍼를 검지 / 비정상인 조합으로 센서를 검지 / WaferSensing 기능의 센서 지정이 없음
19		1 0 0 1 1 		예약
20		1 0 1 0 0 		AWS 관련에러
21		1 0 1 0 1 		예약
22		1 0 1 1 0		선회 간섭 인터락이기 때문에, 감속 정지
*/

	return ErrorCode;
}
//---> End    2008/04/01 TM Robot ERROR 처리 문제 보완  ( 5 of 5 )
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void ALARM_MESSAGE_REMAPPING ( int id , int err ) {
	char Buffer[256] , Buffer2[32];
	int Count ;
	int Commstatus; // 2014.04.24
	Count = 0 ;

	if ( err == 2349 )
	{
		sprintf( Buffer2 , "(ERROR=234~239)");
	}
	else if ( err == 2403 )
	{
		sprintf( Buffer2 , "(ERROR=240,243)");
	}
	else if ( err <= 1 && READ_DIGITAL( MTR7_Run_Status, &Commstatus ) == UV_IDLE )	//2014.04.24
	{
		sprintf( Buffer2 , "(Action Fail)" );
	}
	else
	{
		sprintf( Buffer2 , "(ERROR=%d)" , err );
	}

	while ( TRUE ) {
		if ( !ALARM_MESSAGE_GET( id , Buffer ) && Count < 10 ) {
			ALARM_MESSAGE_GET ( id , Buffer ) ;
		}else {
			break;
		}
		Count ++ ;
		_sleep( 200 ) ;
	}
	strcat( Buffer , Buffer2 );
	ALARM_MESSAGE_SET( id , Buffer );
}

//[ 2011/01/14 FM , TM Wafer Transfer GUI Interlock 기능 구현 ( BY LJH )
BOOL ALARM_MESSAGE_REMAPPING_ADD_STRING( int id , char *MSG) {
	char Buffer[256] , Buffer2[50];

	sprintf( Buffer2 , "%s" , MSG );

	ALARM_MESSAGE_GET( id , Buffer );
	strcat( Buffer , Buffer2 );
	ALARM_MESSAGE_SET( id , Buffer );
	return TRUE;

}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// ULVAC AXIS 부하율 Reading ( 2013/02/05 , LJH )
Module_Status READ_AXIS_LOAD()
{
	int CS, Load_Ti, Load_A, Load_D, Load_To, Load_B, Load_C, Load_Z;
	int ReadCnt = 0, AlmResult;
	char ReadStr[51], TempStr1[51], TempStr2[52];

	// GETVCDLOG
	//     Ti A D To B  C Z
	// GVL,5,15,7,14,15,6,51
	while (TRUE)
	{
		READ_STRING(ULVAC_LOAD_READ, ReadStr, &CS);
		if (!CS)
			ReadCnt++;
		else
			break;

		_sleep(100);

		if (ReadCnt > 2)
		{
			ALARM_MESSAGE_REMAPPING_ADD_STRING(ALARM_COMM_ERROR, "ULVAC_AXIS_LOAD Reading Error");
			AlmResult = ALARM_MANAGE(ALARM_COMM_ERROR);
			if (AlmResult == ALM_IGNORE)
				break;
			else if (AlmResult != ALM_RETRY)
				return SYS_ABORTED;
			else
				ReadCnt = 0;
		}
	}

	_LOG("--------> AXIS LOAD Driver Read => %s", ReadStr);

	STR_SEPERATE_CHAR(ReadStr, ',', TempStr1, TempStr2, 51);  // GVL
	STR_SEPERATE_CHAR(TempStr2, ',', TempStr1, TempStr2, 51); // Ti
	Load_Ti = atoi(TempStr1);
	STR_SEPERATE_CHAR(TempStr2, ',', TempStr1, TempStr2, 51); // A
	Load_A = atoi(TempStr1);
	STR_SEPERATE_CHAR(TempStr2, ',', TempStr1, TempStr2, 51); // D
	Load_D = atoi(TempStr1);
	STR_SEPERATE_CHAR(TempStr2, ',', TempStr1, TempStr2, 51); // To
	Load_To = atoi(TempStr1);
	STR_SEPERATE_CHAR(TempStr2, ',', TempStr1, TempStr2, 51); // B
	Load_B = atoi(TempStr1);
	STR_SEPERATE_CHAR(TempStr2, ',', TempStr1, TempStr2, 51); // C
	Load_C = atoi(TempStr1);
	STR_SEPERATE_CHAR(TempStr2, ',', TempStr1, TempStr2, 51); // Z
	Load_Z = atoi(TempStr1);

	if (!SIMULATION_MODE)
	{
		WRITE_DIGITAL(ULVAC_LOAD_Ti, Load_Ti, &CS);
		WRITE_DIGITAL(ULVAC_LOAD_A, Load_A, &CS);
		WRITE_DIGITAL(ULVAC_LOAD_D, Load_D, &CS);
		WRITE_DIGITAL(ULVAC_LOAD_To, Load_To, &CS);
		WRITE_DIGITAL(ULVAC_LOAD_B, Load_B, &CS);
		WRITE_DIGITAL(ULVAC_LOAD_C, Load_C, &CS);
		WRITE_DIGITAL(ULVAC_LOAD_Z, Load_Z, &CS);
	}

	/* COVOT6 축별 부하율 ALARM 처리 */
	/*
	AlmRange = READ_DIGITAL ( ULVAC_LOAD_ALM_RANGE , &CS );
	for ( i = ULVAC_LOAD_Ti ; i <= ULVAC_LOAD_Z ; i++ )
	{
		ReadIO = READ_DIGITAL ( i , &CS ) ;
		if ( ReadIO > AlmRange )
		{
			DIGITAL_NAME( i , AlmAxis ) ;
			sprintf ( AlmMsg , " [%s:%d%%] [ErrorRange:%d%%]" , AlmAxis , ReadIO , AlmRange ) ;
			ALARM_MESSAGE_REMAPPING_ADD_STRING ( ALARM_RB_AXIS_LOAD_FACTOR_ERR , AlmMsg ) ;
			if ( ALARM_STATUS ( ALARM_RB_AXIS_LOAD_FACTOR_ERR ) != ALM_PAUSED ) ALARM_POST( ALARM_RB_AXIS_LOAD_FACTOR_ERR );
		}
	}
	*/
	return SYS_SUCCESS;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
Module_Status READ_AXIS_MOVE_CNT()
{
	int CS, MOVE_CNT_N, MOVE_CNT_R, MOVE_CNT_Z, MOVE_TIME;
	int ReadCnt = 0, AlmResult;
	char ReadStr[51], TempStr1[51], TempStr2[52], Buffer[10];

	while (TRUE)
	{
		// 축별 Move Count Reading ( 2013.10.29 , JBM )
		// 예) RQ GOTOCOUNTER
		// Nxxxx , Rxxxx , Zxxxx , TMxx

		READ_STRING(ULVAC_MOVE_CNT_READ, ReadStr, &CS);
		if (!CS)
			ReadCnt++;
		else
			break;

		_sleep(100);

		if (ReadCnt > 2)
		{
			ALARM_MESSAGE_REMAPPING_ADD_STRING(ALARM_COMM_ERROR, "ULVAC_AXIS_MOVE_CNT Reading Error");
			AlmResult = ALARM_MANAGE(ALARM_COMM_ERROR);
			if (AlmResult == ALM_IGNORE)
				break;
			else if (AlmResult != ALM_RETRY)
				return SYS_ABORTED;
			else
				ReadCnt = 0;
		}
	}

	_LOG("--------> AXIS MOVE CNT Driver Read => %s", ReadStr);

	STR_SEPERATE_CHAR(ReadStr, ',', TempStr1, TempStr2, 51); // N
	STR_SEPERATE_CHAR(TempStr1, 'N', Buffer, TempStr1, 51);
	MOVE_CNT_N = atoi(TempStr1);

	STR_SEPERATE_CHAR(TempStr2, ',', TempStr1, TempStr2, 51); // R
	STR_SEPERATE_CHAR(TempStr1, 'R', Buffer, TempStr1, 51);
	MOVE_CNT_R = atoi(TempStr1);

	STR_SEPERATE_CHAR(TempStr2, ',', TempStr1, TempStr2, 51); // Z
	STR_SEPERATE_CHAR(TempStr1, 'Z', Buffer, TempStr1, 51);
	MOVE_CNT_Z = atoi(TempStr1);

	STR_SEPERATE_CHAR(TempStr2, ',', TempStr1, TempStr2, 51); // TM
	STR_SEPERATE_CHAR(TempStr1, 'T', Buffer, TempStr1, 51);
	STR_SEPERATE_CHAR(TempStr1, 'M', Buffer, TempStr1, 51);
	MOVE_TIME = atoi(TempStr1);

	if (!SIMULATION_MODE)
	{
		WRITE_DIGITAL(ULVAC_MOVE_CNT_N, MOVE_CNT_N, &CS);
		WRITE_DIGITAL(ULVAC_MOVE_CNT_R, MOVE_CNT_R, &CS);
		WRITE_DIGITAL(ULVAC_MOVE_CNT_Z, MOVE_CNT_Z, &CS);
		WRITE_DIGITAL(ULVAC_MOVE_TIME, MOVE_TIME, &CS);
	}

	return SYS_SUCCESS;
}
/*
void ALARM_MESSAGE_REMAPPING( int id ) {
	//modified by mgsong 2008.01.09
	char Buffer[256] , Buffer2[32];
	int  err = 0;
	int  errcmd = 0;
	//int err , CommStatus;
	int CommStatus;

	err    = (int) READ_DIGITAL( MTR7_ErrorCode    , &CommStatus );
	errcmd = (int) READ_DIGITAL( MTR7_CmdErrorCode , &CommStatus );
	if (err > 0) {
		sprintf( Buffer2 , "(ERRCMD %d , ERROR=%d)" , errcmd , err );
		ALARM_MESSAGE_GET( id , Buffer );
		strcat( Buffer , Buffer2 );
		ALARM_MESSAGE_SET( id , Buffer );
	}
}
*/


Module_Status AWSLimitSet(int Station) {

	int CommStatus;
	double AWS_LIMIT_PM_HI, AWS_LIMIT_BM_HI, AWS_LIMIT_PM_LOW, AWS_LIMIT_BM_LOW ;
	BOOL Curr_AWS_Control;

	AWS_LIMIT_PM_HI = READ_ANALOG( ULVAC_AWS_LIMIT_PM_HI , &CommStatus );
	AWS_LIMIT_PM_LOW = READ_ANALOG( ULVAC_AWS_LIMIT_PM_LOW , &CommStatus );
	AWS_LIMIT_BM_HI = READ_ANALOG( ULVAC_AWS_LIMIT_BM_HI , &CommStatus );
	AWS_LIMIT_BM_LOW = READ_ANALOG( ULVAC_AWS_LIMIT_BM_LOW , &CommStatus );
	Curr_AWS_Control = READ_DIGITAL( ULVAC_AWS_USE_V , &CommStatus );

	//[ 2010/10/26 TM RB AWS Abort Range PM / LL 분리 ( BY LJH )
	if ( Station == PMA || Station == PMB || Station == PMC )
	{
		if ( AWS_LIMIT_PM_HI != AWS_LIMIT_BM_HI || AWS_LIMIT_PM_LOW != AWS_LIMIT_BM_LOW || Old_AWS_Control != Curr_AWS_Control )
		{
			if ( RUN_FUNCTION ( OBJ_TM_AWS , "SET_AWS_LIMIT PM" ) == SYS_ABORTED ) return SYS_ABORTED ;

			Old_AWS_Control = Curr_AWS_Control;
		}
	}
	else{
		if ( AWS_LIMIT_PM_HI != AWS_LIMIT_BM_HI || AWS_LIMIT_PM_LOW != AWS_LIMIT_BM_LOW || Old_AWS_Control != Curr_AWS_Control )
		{
			if ( RUN_FUNCTION ( OBJ_TM_AWS , "SET_AWS_LIMIT BM" ) == SYS_ABORTED ) return SYS_ABORTED ;

			Old_AWS_Control = Curr_AWS_Control;
		}
	}

	return SYS_SUCCESS;

}

void Get_RobotCmdDetailMsg( int CmdIO , int Station, char DetailMsg[] ) {
	char Buffer2[256] , Buffer3[256];

	memset(Buffer2, 0x00, sizeof(Buffer2));
	memset(Buffer3, 0x00, sizeof(Buffer3));
	memset(DetailMsg, 0x00, sizeof(DetailMsg));

	switch ( CmdIO )
	{
		case MTR7_Get_Robot:
			sprintf( Buffer2 , "[PICK_A"  );
			break;
		case MTR7_Get_RobotL:
			sprintf( Buffer2 , "[PICK_A(Left)"  );
			break;
		case MTR7_Get_RobotR:
			sprintf( Buffer2 , "[PICK_A(Right)"  );
			break;
		case MTR7_Put_Robot:
			sprintf( Buffer2 , "[PLACE_A"  );
			break;
		case MTR7_Put_RobotL:
			sprintf( Buffer2 , "[PLACE_A(Left)"  );
			break;
		case MTR7_Put_RobotR:
			sprintf( Buffer2 , "[PLACE_A(Right)"  );
			break;
		case MTR7_Get_RobotB:
			sprintf( Buffer2 , "[PICK_B"  );
			break;
		case MTR7_Get_RobotBL:
			sprintf( Buffer2 , "[PICK_B(Left)"  );
			break;
		case MTR7_Get_RobotBR:
			sprintf( Buffer2 , "[PICK_B(Right)"  );
			break;
		case MTR7_Put_RobotB:
			sprintf( Buffer2 , "[PLACE_B"  );
			break;
		case MTR7_Put_RobotBL:
			sprintf( Buffer2 , "[PLACE_B(Left)"  );
			break;
		case MTR7_Put_RobotBR:
			sprintf( Buffer2 , "[PLACE_B(Right)"  );
			break;

		case MTR7_Rot_Robot:
		case MTR7_RotG_Robot:
		case MTR7_RotP_Robot:
			sprintf( Buffer2 , "[ROTATE_A"  );
			break;

		case MTR7_Rot_RobotB:
		case MTR7_RotG_RobotB:
		case MTR7_RotP_RobotB:
			sprintf( Buffer2 , "[ROTATE_B"  );
			break;

		default :
			memset(DetailMsg, 0x00, sizeof(DetailMsg));
			return;
	}
	strcat( DetailMsg , Buffer2 );

	if ( Station > 0 )
	{
		switch ( Station )
		{
			case 1:
				sprintf( Buffer3 , " PMA]");
				break;
			case 2:
				sprintf( Buffer3 , " PMB]");
				break;
			case 3:
				sprintf( Buffer3 , " PMC]");
				break;
			case 4:
				sprintf( Buffer3 , " BM1_1Slot]");
				break;
			case 5:
				sprintf( Buffer3 , " BM1_2Slot]");
				break;
			case 6:
				sprintf( Buffer3 , " BM2_1Slot]");
				break;
			case 7:
				sprintf( Buffer3 , " BM2_2Slot]");
				break;
			default :
				memset(DetailMsg, 0x00, sizeof(DetailMsg));
				return;
		}
		strcat( DetailMsg , Buffer3 );
	}
}

Module_Status Wafer_Check_Visually ( int Uarm_left , int Uarm_right , int Larm_left , int Larm_right )
{
	char Msg[512] , ArmChkMsg[256] , ArmChkMsg1[128] , ArmChkMsg2[128];
	int MsgBoxResult ;
	HWND Handle;

	Handle = FindWindow(NULL,NULL); // 육안으로 확인 시 Home2후 TM Robot의 Wafer정보와 동일한 지 확인 기능 추가. ( BY LJH )

	if ( Uarm_left == Present_1 )
	{
		sprintf ( ArmChkMsg1 , "A : Present ,") ;
	}
	else
	{
		sprintf ( ArmChkMsg1 , "A : Absent ,") ;
	}
	if ( Uarm_right == Present_1 )
	{
		sprintf ( ArmChkMsg2 , "B : Present , ") ;
	}
	else
	{
		sprintf ( ArmChkMsg2 , "B : Absent , ") ;
	}

	strcat( ArmChkMsg1 , ArmChkMsg2 );

	if ( Larm_left == Present_1 )
	{
		sprintf ( ArmChkMsg2 , "C : Present , ") ;
	}
	else
	{
		sprintf ( ArmChkMsg2 , "C : Absent , ") ;
	}

	strcat( ArmChkMsg1 , ArmChkMsg2 );

	if ( Larm_right == Present_1 )
	{
		sprintf ( ArmChkMsg2 , "D : Present ??") ;
	}
	else
	{
		sprintf ( ArmChkMsg2 , "D : Absent ??") ;
	}

	strcat( ArmChkMsg1 , ArmChkMsg2 );

	sprintf( ArmChkMsg , "%s\n( TM ROBOT Upper_Arm : Left-A, Right-B | Lower_Arm: Left-C, Right-D )" , ArmChkMsg1 );
	MsgBoxResult = MessageBox( Handle, ArmChkMsg,  "[Visually Wafer Check]", MB_OKCANCEL);

	if ( MsgBoxResult == IDOK ) { sprintf ( Msg , "Message[%s] : Result[IDOK]" , ArmChkMsg ) ; }
	else {						  sprintf ( Msg , "Message[%s] : Result[IDCANCEL]" , ArmChkMsg ) ; }
	_LOG( Msg ) ;

	if ( MsgBoxResult == IDOK ) { return SYS_SUCCESS ; }
	else { return SYS_ERROR ; }
}


Module_Status Wafer_Check_WhileRobotRun ( int ACTION_IO  , int Alarm_ID )
{

	int CommStatus , WaferStsA, WaferStsB, WaferStsC, WaferStsD ;
	char IO_Name[128];

	//---> ADD 2010/10/04 TM Arm의 Wafer정보와 GUI상의 Wafer 위치와 틀리면 Home2 진행 후 처리되도록 수정 by CNL
	int GUI_A_Arm , GUI_B_Arm , GUI_C_Arm , GUI_D_Arm ;
	int Alarm_Result ;

	while (TRUE){
		WaferStsA = READ_DIGITAL_TRUE( ULVAC_Load_Status_A , &CommStatus );
		WaferStsB = READ_DIGITAL_TRUE( ULVAC_Load_Status_B , &CommStatus );
		WaferStsC = READ_DIGITAL_TRUE( ULVAC_Load_Status_C , &CommStatus );
		WaferStsD = READ_DIGITAL_TRUE( ULVAC_Load_Status_D , &CommStatus );

		if ( READ_DIGITAL ( TM_A_Status , &CommStatus ) > 0 ) {
			GUI_A_Arm = Present_1 ;
		}else GUI_A_Arm = Absent_0 ;
		if ( READ_DIGITAL ( TM_A_Status2 , &CommStatus ) > 0 ) {
			GUI_B_Arm = Present_1 ;
		}else GUI_B_Arm = Absent_0 ;
		if ( READ_DIGITAL ( TM_B_Status , &CommStatus ) > 0 ) {
			GUI_C_Arm = Present_1 ;
		}else GUI_C_Arm = Absent_0 ;
		if ( READ_DIGITAL ( TM_B_Status2 , &CommStatus ) > 0 ) {
			GUI_D_Arm = Present_1 ;
		}else GUI_D_Arm = Absent_0 ;

		if ( SIMULATION_MODE )
		{
			WaferStsA = GUI_A_Arm;
			WaferStsB = GUI_B_Arm;
			WaferStsC = GUI_C_Arm;
			WaferStsD = GUI_D_Arm;
		}

		printf("GUI (%d,%d,%d,%d)/ TM (%d,%d,%d,%d)\n",GUI_A_Arm, GUI_B_Arm, GUI_C_Arm, GUI_D_Arm ,WaferStsA,WaferStsB,WaferStsC,WaferStsD ) ;

		if ( Wafer_Check_Visually ( WaferStsA , WaferStsB , WaferStsC , WaferStsD ) == SYS_ERROR )
		{
			ALARM_MESSAGE_REMAPPING_ADD_STRING ( ALARM_TM_ARM_WAFER_SYNCH_MISS , "Retry Home2 Action?" );
			Alarm_Result = ALARM_MANAGE ( ALARM_TM_ARM_WAFER_SYNCH_MISS ) ;
			if ( Alarm_Result == ALM_RETRY ) {
				WRITE_DIGITAL ( MTR7_HOME2 , 0 , &CommStatus );
			}
			else if ( Alarm_Result == ALM_ABORT ) {
				return SYS_ERROR;
			}

		}else break ;

		_sleep ( 1000 );
	}// while End
	//---> END 2010/10/04 TM Arm의 Wafer정보와 GUI상의 Wafer 위치와 틀리면 Home2 진행 후 처리되도록 수정 by CNL

	DIGITAL_NAME( ACTION_IO , IO_Name ) ;

	printf("IO Name[%s] : WaferStsA[%d], WaferStsB[%d], WaferStsC[%d], WaferStsD[%d]\n",
		IO_Name, WaferStsA, WaferStsB, WaferStsC, WaferStsD );

	switch ( ACTION_IO )
	{
		case MTR7_Get_RobotL:

			if ( WaferStsA == Present_1 )
			{
				return SYS_SUCCESS;
			}
			else
			{
				return SYS_ABORTED;
			}

			break;

		case MTR7_Get_RobotR:

			if ( WaferStsB == Present_1 )
			{
				return SYS_SUCCESS;
			}
			else
			{
				return SYS_ABORTED;
			}

			break;

		case MTR7_Get_RobotBL:

			if ( WaferStsC == Present_1 )
			{
				return SYS_SUCCESS;
			}
			else
			{
				return SYS_ABORTED;
			}

			break;

		case MTR7_Get_RobotBR:

			if ( WaferStsD == Present_1 )
			{
				return SYS_SUCCESS;
			}
			else
			{
				return SYS_ABORTED;
			}

			break;

		case MTR7_Get_Robot:


			if ( WaferStsA == Present_1 && WaferStsB == Present_1    )
			{
				return SYS_SUCCESS;
			}
			else if ( WaferStsA == Absent_0 && WaferStsB == Absent_0    )
			{
				return SYS_ABORTED;
			}
			else
			{
				return SYS_ERROR;
			}

			break;

		case MTR7_Get_RobotB:

			if ( WaferStsC == Present_1 && WaferStsD == Present_1    )
			{
				return SYS_SUCCESS;
			}
			else if ( WaferStsC == Absent_0 && WaferStsD == Absent_0    )
			{
				return SYS_ABORTED;
			}
			else
			{
				return SYS_ERROR;
			}
			break;

		case MTR7_Put_RobotL:

			if ( WaferStsA == Absent_0 )
			{
				return SYS_SUCCESS;
			}
			else
			{
				return SYS_ABORTED;
			}

			break;

		case MTR7_Put_RobotR:

			if ( WaferStsB == Absent_0 )
			{
				return SYS_SUCCESS;
			}
			else
			{
				return SYS_ABORTED;
			}

			break;

		case MTR7_Put_RobotBL:

			if ( WaferStsC == Absent_0 )
			{
				return SYS_SUCCESS;
			}
			else
			{
				return SYS_ABORTED;
			}

			break;

		case MTR7_Put_RobotBR:

			if ( WaferStsD == Absent_0 )
			{
				return SYS_SUCCESS;
			}
			else
			{
				return SYS_ABORTED;
			}

			break;

		case MTR7_Put_Robot:

			if ( WaferStsA == Absent_0 && WaferStsB == Absent_0    )
			{
				return SYS_SUCCESS;
			}
			else if ( WaferStsA == Present_1 && WaferStsB == Present_1    )
			{
				return SYS_ABORTED;
			}
			else
			{
				return SYS_ERROR;
			}

			break;

		case MTR7_Put_RobotB:

			if ( WaferStsC == Absent_0 && WaferStsD == Absent_0    )
			{
				return SYS_SUCCESS;
			}
			else if ( WaferStsC == Present_1 && WaferStsD == Present_1    )
			{
				return SYS_ABORTED;
			}
			else
			{
				return SYS_ERROR;
			}

			break;

		//[ 2009/08/31 TM Robot Extend 기능 개선 ( BY LJH )
		case MTR7_Rot_Robot:
		case MTR7_RotG_Robot:
		case MTR7_RotP_Robot:
		case MTR7_Rot_RobotB:
		case MTR7_RotG_RobotB:
		case MTR7_RotP_RobotB:

			return SYS_ABORTED;
			break;
		//]

		default :
			//[ 2009/08/31 TM Robot Extend 기능 개선 ( BY LJH )
		//	return SYS_ABORTED;
			return SYS_ERROR ;
			//]
	}
}

Module_Status	AutoRecovery( int ACTION_IO, int Station, int AlarmID, int ChkError, int *ActionRunComplete, int *AlmResult )
{
	int CommStatus, MsgBoxResult, Sub_F_Result;
	char AlmMsg[256], RobotCmdDetailMsg[256], Msg[256];
	HWND Handle;

	if (  AlarmID == ALARM_AWS_ERROR )
	{
		ALARM_MESSAGE_REMAPPING ( AlarmID , ChkError );
	}
	else
	{
		ALARM_MESSAGE_REMAPPING( AlarmID , ChkError  );
	}

	*AlmResult = ALARM_MANAGE( AlarmID );

	while (TRUE)
	{
		Handle = FindWindow(NULL,NULL);
		MsgBoxResult = 0;

		if ( *AlmResult != ALM_RETRY ) return SYS_ABORTED;		// Case Abort

		WRITE_DIGITAL ( MTR7_HOME  , 0 , &CommStatus );
		WRITE_DIGITAL ( MTR7_HOME2 , 0 , &CommStatus );

		if ( *AlmResult == ALM_RETRY )
		{

			Get_RobotCmdDetailMsg( ACTION_IO , Station, RobotCmdDetailMsg );

			Sub_F_Result = Wafer_Check_WhileRobotRun( ACTION_IO , AlarmID );

			if ( Sub_F_Result == SYS_SUCCESS )
			{
				*ActionRunComplete = TRUE;
				sprintf( AlmMsg , ROBOT_ACTION_COMPLETE, RobotCmdDetailMsg );
			}
			else if ( Sub_F_Result == SYS_ABORTED )
			{
				*ActionRunComplete = FALSE;
				sprintf( AlmMsg , ROBOT_ACTION_NOT_COMPLETE, RobotCmdDetailMsg );
			}
			else  // Abnormal Case  SYS_ERROR
			{
				printf(" Wafer Check  Abnormal Case. ( Extend Command Can't Retry )\n");
				return SYS_ABORTED;
			}

			MsgBoxResult = MessageBox( Handle, AlmMsg,  "Confirm", MB_OKCANCEL);

			//[ MessageBox 처리 결과 Logging ( BY LJH )
			if ( MsgBoxResult == IDOK ) { sprintf ( Msg , "Message[%s] : Result[IDOK]" , AlmMsg ) ; }
			else {						  sprintf ( Msg , "Message[%s] : Result[IDCANCEL]" , AlmMsg ) ; }
			_LOG( Msg ) ;
			//]

			if ( MsgBoxResult == IDOK )
			{
				break;
			}
			else
			{
				if ( AlarmID == ALARM_INDEX_POINTER + ALARM_ROBOT_ERROR )
				{
					ALARM_MESSAGE_REMAPPING( AlarmID , ChkError  );
				}
				else if (  AlarmID == ALARM_INDEX_POINTER + ALARM_AWS_ERROR )
				{
					ALARM_MESSAGE_REMAPPING ( AlarmID , ChkError );
				}

				*AlmResult = ALARM_MANAGE( AlarmID );
			}
		}

		if ( MsgBoxResult == IDOK ) break;

	}  // End While

	return  SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PrepCheckError( int *AlarmID, int *ChkError )
{
	int	Data, CommStatus;

	Data = READ_DIGITAL ( TM_RB_Run_Status , &CommStatus );
	if ( Data == RUNNING )
	{
		_sleep (1000);
		Data = READ_DIGITAL ( TM_RB_Run_Status , &CommStatus );
		if ( Data == RUNNING )
		{
			Data = READ_DIGITAL ( TM_RB_Run_Status , &CommStatus );
			_sleep (1000);
			if ( Data == RUNNING )
			{
				*AlarmID = ALARM_INDEX_POINTER + ALARM_ROBOT_RUNNING;
				return SYS_ERROR;
			}
		}
	}
	*ChkError = CheckError();
	if ( *ChkError > 0 )
	{
		if ( *ChkError >= 420 && *ChkError <= 485 )
		{
			*AlarmID = ALARM_INDEX_POINTER + ALARM_AWS_ERROR;
		}
		else
		{
			*AlarmID = ALARM_INDEX_POINTER + ALARM_ROBOT_ERROR;
		}
		return SYS_ERROR;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PrepCheckRobot( int Station, int Action_IO ) {
	int AlmResult;
	int ChkResult, ChkError, AlarmID, ActionRunComplete;

	while ( TRUE ) {

		ChkResult = PrepCheckError( &AlarmID, &ChkError );

		if( SYS_ABORTED == ChkResult ) {
			return SYS_ABORTED;
		}
		else if( ChkResult == SYS_ERROR )
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			//2014.3.16 User Recovery Added about all error by hyuk
			_LOG("-----> PreCheck VTM Robot (%d) Part (%d) Error Occur and Retry Started" , AlarmID , ChkError);
			Sleep(1000);

			if( SYS_ABORTED == AutoRecovery( Action_IO, Station, AlarmID, ChkError, &ActionRunComplete, &AlmResult) ) {
				//g_nRobotStatus = RB_DOING_STEP2;
				return SYS_ABORTED;
			}

			if( ActionRunComplete == FALSE ) return SYS_RUNNING;
			else {
				return SYS_SUCCESS;
			}
			//2014.3.16 User Recovery Added about all error by hyuk
				////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		return SYS_SUCCESS;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status ReadyCheck() {

	int CommStatus, AlarmID, ChkError, AlmResult;

	ChkError = CheckError();
	if(ChkError >0){
		if(ChkError >= 420 && ChkError <=485){
			_LOG("ALARM_AWS_ERROR : %d", __LINE__);
			AlarmID = ALARM_INDEX_POINTER + ALARM_AWS_ERROR;
			Sleep(2000);
			WRITE_DIGITAL( MTR7_Ret_Robot, 0, &CommStatus);	//All Arm Retract
			ALARM_MESSAGE_REMAPPING( AlarmID, ChkError );
			AlmResult = ALARM_MANAGE( AlarmID );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) {
				g_nRobotStatus = RB_DOING_STEP2;
				return SYS_ABORTED;
			}
		}
		else{
			_LOG("ALARM_ROBOT_ERROR : %d", __LINE__);
			AlarmID = ALARM_INDEX_POINTER + ALARM_ROBOT_ERROR;
			ALARM_MESSAGE_REMAPPING( AlarmID, ChkError );
			AlmResult = ALARM_MANAGE( AlarmID );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) {
				g_nRobotStatus = RB_DOING_STEP2;
				return SYS_ABORTED;
			}
		}
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RunCheckRobot( int Station , int *Data , int retalarm, int Action_IO, int *nPASS, int Arm ) {
//	int  error , errcmd , i , error_code;
	char szRobot_err[32] = {0};
	int CommStatus , AlmResult;
	int ChkError, AlarmID, ActionRunComplete;
	char ParaStr[256] ;
	char AWSStr[20];

	*Data = READ_DIGITAL_TRUE( MTR7_Run_Status , &CommStatus );

	// Test Test
	printf ("ULVAL Run Status data = %d \n" , *Data);

	if ( !CommStatus ) {
		_LOG("ALARM_COMM_ERROR : %d", __LINE__);
		WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_READY , &CommStatus );
		AlmResult = ALARM_MANAGE( ALARM_INDEX_POINTER + ALARM_COMM_ERROR );
		if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
		else if ( AlmResult != ALM_RETRY  ) {
			g_nRobotStatus = RB_DOING_STEP2;
			return SYS_ABORTED;
		}
	} else if ( *Data  == UV_FAIL ) {	// Run_Status FAIL
		ChkError = READ_DIGITAL( MTR7_ErrorCode, &CommStatus);
		if(ChkError >0){
			if(ChkError >= 420 && ChkError <=485){

				// AWS Data Error Log
				switch ( Station ) {
				case 1 :
					sprintf ( AWSStr , "PMA" );
					break;
				case 2 :
					sprintf ( AWSStr , "PMB" );
					break;
				case 3 :
					sprintf ( AWSStr , "PMC" );
					break;
				case 4 :
				case 5 :
					sprintf ( AWSStr , "BM1" );
					break;
				case 6 :
				case 7 :
					sprintf ( AWSStr , "BM2" );
					break;
				}

				if		( Arm == ARM_A ) {
					sprintf ( ParaStr , "(ERROR%d)A_%s %d %d %d" , ChkError ,AWSStr , Arm , READ_DIGITAL(TM_A_Status , &CommStatus) , READ_DIGITAL(TM_A_Status2, &CommStatus) ) ;
				}
				else if ( Arm == ARM_B ) {
					sprintf ( ParaStr , "(ERROR%d)B_%s %d %d %d" , ChkError, AWSStr , Arm , READ_DIGITAL(TM_B_Status , &CommStatus) , READ_DIGITAL(TM_B_Status2, &CommStatus) ) ;
				}
				RUN_SET_FUNCTION ( OBJ_TM_AWS	, ParaStr ) ;
				// AWS Data Error Log

				AlarmID = ALARM_INDEX_POINTER + ALARM_AWS_ERROR;
				Sleep(2000);
				WRITE_DIGITAL( MTR7_Ret_Robot, 0, &CommStatus);	// All Arm Retract
			}
			else{
				AlarmID = ALARM_INDEX_POINTER + ALARM_ROBOT_ERROR;
			}
		}
		else{
			AlarmID = ALARM_INDEX_POINTER + ALARM_ROBOT_ERROR;
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////
		//2014.3.16 User Recovery Added about all error by hyuk
		_LOG("-----> VTM Robot (%d) Part (%d) Error Occur and Retry Started" , AlarmID , ChkError);
		Sleep(1000);

		if( SYS_ABORTED == AutoRecovery( Action_IO, Station, AlarmID, ChkError, &ActionRunComplete, &AlmResult) ) {
			g_nRobotStatus = RB_DOING_STEP2;
			return SYS_ABORTED;
		}

		if( ActionRunComplete == FALSE ) {
			// TM Robot Action후 Error Retry시 AWS Limit Parameter정보 재 인식 후 처리 (2012/11/14,LJH)
			switch ( Station )
			{
			case 1 :
			case 2 :
			case 3 : // PMA,B,C
				if ( RUN_FUNCTION ( OBJ_TM_AWS , "SET_AWS_LIMIT PM" ) == SYS_ABORTED ) return SYS_ABORTED ;
				break ;
			case 4 : // BM1_1 Lower Slot (1,2)
			case 5 : // BM1_2 Upper Slot (3,4)
			case 6 : // BM2_1 Lower Slot (1,2)
			case 7 : // BM2_2 Upper Slot (3,4)
				if ( RUN_FUNCTION ( OBJ_TM_AWS , "SET_AWS_LIMIT BM" ) == SYS_ABORTED ) return SYS_ABORTED ;
				break;
			}
			return SYS_RUNNING;
		}
		else {
			*nPASS = TRUE;
			return SYS_SUCCESS;
		}
		//2014.3.16 User Recovery Added about all error by hyuk
		////////////////////////////////////////////////////////////////////////////////////////////////////

	} else 	return SYS_SUCCESS;	// Run_Status BUSY, COMPLETE
	return SYS_RUNNING;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//2014.03.15 ULVAL 7axis Robot by hyuk
Module_Status RobotMovement(
							int Station , int Arm ,
							int A_IOP , int B_IOP ,
							int ALARM ,
							int Synch_Start , int Synch_End , int Last_Retract ,
							int Arm_RetExt , int Arm_Station , int Arm_Slot , int Arm_UpDown ,
							int rejectalarm
							)
{
	int Data , Check , Res ;
	int CommStatus , AlmResult;
	int Action_IO;
	Module_Status msRet = SYS_RUNNING;
	int nPASS = FALSE;
	int PreCheck;
	int RB_RUN_STATUS, RCV_CHK_COUNT;

	g_nRobotStatus     = RB_DOING_STEP1;
	if ( Last_Retract >= 0 ) WRITE_DIGITAL( ROBOT_RET , 1 , &CommStatus );
	WRITE_DIGITAL(ROBOT_STATION , Station , &CommStatus );
	g_nRobotStatus = RB_DOING_STEP2;

	if ( Arm == ARM_A )	{
		Action_IO = A_IOP;
	}
	else {
		Action_IO = B_IOP;
	}

	do {
		//PreCheck
		PreCheck = PrepCheckRobot( Station, Action_IO );
		if( SYS_ABORTED == PreCheck ) {
			WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_READY , &CommStatus );
			return SYS_ABORTED;
		}
		else if( SYS_RUNNING == PreCheck ) continue;
		else ;

		WRITE_DIGITAL( MTR7_Run_Status, UV_IDLE, &CommStatus );	//2014.04.24

		//Set Command
		if ( Synch_Start >= 0 ) WRITE_DIGITAL( ROBOT_SYNCH , Synch_Start , &CommStatus );
		if ( Arm == ARM_A ) WRITE_DIGITAL( A_IOP , Station , &CommStatus );
		else WRITE_DIGITAL( B_IOP , Station , &CommStatus );
		if(CommStatus) g_nRobotStatus = RB_DONE;
		else {

			RCV_CHK_COUNT = 0 ;
			while ( TRUE )
			{
				RB_RUN_STATUS = READ_DIGITAL ( MTR7_Run_Status , &CommStatus ) ;

				if ( CommStatus )
				{
					if ( RB_RUN_STATUS == UV_COMPLETE )
					{
						CommStatus = TRUE ;
						g_nRobotStatus = RB_DONE;
						break ;
					}
					else if ( RB_RUN_STATUS == UV_FAIL || RB_RUN_STATUS == UV_IDLE )	// 2014.04.24
					{
						CommStatus = FALSE ;
						break ;
					}

				}
				if ( RCV_CHK_COUNT++ > 30 )
				{
					CommStatus = FALSE ;
					break ;
				}

				if ( !WAIT_SECONDS ( 1 ) ) return SYS_ABORTED;
			}

			// 2014.04.07 RDY Response 30sec 들어 오지 않는 경우 다발한다..특히 HOME 진행 시..
			// ROBOT_STATUS CHECK하여 정상적으로 Complete 상태면 Succeess 처리하도록 수정.
			// Driver상에서 Recv Timeout도 25sec에서 50sec로 늘림. ( Code내 Fix )
			// WRITE_DIGITAL가 False로 Return시 추가 30sec 동안 Check.

			/*
			_LOG("ALARM_COMM_ERROR : %d", __LINE__);
			AlmResult = ALARM_MANAGE( ALARM_INDEX_POINTER + ALARM_COMM_ERROR );
			return SYS_ABORTED;	break;
			*/
		}


		S_TIMER_READY();
		do {
			if ( !WAIT_SECONDS( 0.1 ) ) {
				WRITE_DIGITAL( ROBOT_RET , 0 , &CommStatus );
				msRet = SYS_ABORTED;
				break;
			}

			//Check = RunCheckRobot((Station == 0 ) , &Data , rejectalarm);
			Check = RunCheckRobot( Station, &Data, rejectalarm, Action_IO, &nPASS, Arm );
			if      ( Check == SYS_ABORTED ) {msRet =SYS_ABORTED; break; }
			else if ( Check == SYS_RUNNING ) break;
			// Run Status COMPLETE
			if ( Data == UV_COMPLETE || nPASS == TRUE  )
			{
				Res = 0;
				if ( Res == 0 ) {
					if ( Last_Retract >= 0 ) WRITE_DIGITAL( ROBOT_RET , Last_Retract , &CommStatus );
					if ( Synch_End    >= 0 ) WRITE_DIGITAL( ROBOT_SYNCH , Synch_End , &CommStatus );
					{ msRet = SYS_SUCCESS; nPASS = FALSE; break; }
				}
			}
			// Run_Status BUSY
			else if ( S_TIMER_CHECK( 1 , (double) READ_DIGITAL( MOVE_WAFER_TIMEOUT , &CommStatus ) ) ) {
				AlmResult = ALARM_MANAGE( ALARM_INDEX_POINTER + ALARM );
				S_TIMER_READY();
				if      ( AlmResult == ALM_IGNORE ) {
					if ( Last_Retract >= 0 ) WRITE_DIGITAL( ROBOT_RET , Last_Retract , &CommStatus );
					if ( Synch_End    >= 0 ) WRITE_DIGITAL( ROBOT_SYNCH , Synch_End , &CommStatus );
					{ msRet = SYS_SUCCESS; break; }
				}
				else if ( AlmResult != ALM_RETRY  ) { msRet = SYS_ABORTED; break; }
				else                                break;
			}
		} while(1);

	} while(msRet == SYS_RUNNING);

	return msRet;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status HomeRobot() {
	int CommStatus;
	Module_Status Res = SYS_ABORTED;

	/*
	Res = RobotMovement( 0 , 0 ,
			MTR7_HOME , MTR7_HOME ,
			ALARM_HOME_WAFER ,
			-1 , -1 , 0 ,
			-1 , -1 , -1 , -1 , -1 );
	*/
	WRITE_DIGITAL ( MTR7_HOME  , 0 , &CommStatus );

	if(CommStatus){
		WRITE_ANALOG ( CTC_Rb_Rotation   , 270.0 , &CommStatus ); // ULVAC HOME시 PMB 방향
		WRITE_ANALOG ( CTC_Rb_Movement   , 0.0   , &CommStatus );
		WRITE_ANALOG ( CTC_Rb_Extension  , 0.0   , &CommStatus );
		WRITE_ANALOG ( CTC_Rb_Extension2 , 0.0   , &CommStatus );
		Res = SYS_SUCCESS;
	}

	return Res;
}

Module_Status HomeRobot2() {
	int CommStatus, CommStatus2;
	Module_Status Res = SYS_ABORTED;

	/*
	// Home 1 - Arm Status Reset..
	Res = RobotMovement( 0 , 0 ,
		MTR7_HOME , MTR7_HOME ,
		ALARM_HOME_WAFER ,
		-1 , -1 , 0 ,
		-1 , -1 , -1 , -1 , -1 );

	// Home 1 Complete : Home 2 Run - Arm Status Check.. ( PMB Side Check )
	if(SYS_SUCCESS == Res){
		Res = RobotMovement( 0 , 0 ,
			MTR7_HOME2 , MTR7_HOME2 ,
			ALARM_HOME_WAFER ,
			-1 , -1 , 0 ,
			-1 , -1 , -1 , -1 , -1 );
	}else return Res;

   */
	WRITE_DIGITAL ( MTR7_HOME   , 0 , &CommStatus );
	WRITE_DIGITAL ( MTR7_HOME2  , 0 , &CommStatus2 );

	if(CommStatus && CommStatus2){
		WRITE_ANALOG ( CTC_Rb_Rotation   , 270.0 , &CommStatus ); // ULVAC HOME시 PMB 방향
		WRITE_ANALOG ( CTC_Rb_Movement   , 0.0   , &CommStatus );
		WRITE_ANALOG ( CTC_Rb_Extension  , 0.0   , &CommStatus );
		WRITE_ANALOG ( CTC_Rb_Extension2 , 0.0   , &CommStatus );
		Res = SYS_SUCCESS;
	}

	return Res;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status AWS_INIT( int EStation, int Station , int Arm,  BOOL bSingle ) {


	printf("\n===> AWS_INIT Robot int EStation[%d] , int Station[%d] , int Arm[%d]\n,", EStation, Station,Arm );

	if ( SlitVv_Sensor_Check ( EStation ) == SYS_ABORTED ) return SYS_ABORTED;

	if ( EStation < BM1_1 ) { //PMA PMB PMC
		if ( PM_Chuck_Home_Check ( EStation ) == SYS_ABORTED ) return SYS_ABORTED;
	}

	if ( Slide_Sensor_Check  ( EStation ) == SYS_ABORTED ) return SYS_ABORTED;

	// 일단 양쪽씩만 사용하는걸로...hyuk
	/*
	if ( WaferCheckResult == 1 )
	{
		if ( RobotMovement( Station , Arm ,
			ULVAC_AWS_INIT_A , ULVAC_AWS_INIT_C  ) == SYS_ABORTED ) return SYS_ABORTED;
	}
	else if ( WaferCheckResult == 2 )
	{
		if ( RobotMovement( Station , Arm ,
			ULVAC_AWS_INIT_B , ULVAC_AWS_INIT_D ) == SYS_ABORTED ) return SYS_ABORTED;
	}
	*/
	// 일단 양쪽씩만 사용하는걸로...hyuk

	if ( bSingle == FALSE )
	{
		if ( RobotMovement( Station , Arm ,
			ULVAC_AWS_INIT_UP , ULVAC_AWS_INIT_LO ,
			ALARM_PICK_WAFER ,
			ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
			ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_UP , -1 ) == SYS_ABORTED ) return SYS_ABORTED;
	}
	else
	{
		printf( "Parameter Error (Wafer Count) \n");
		return SYS_ABORTED;
	}

	if ( Slide_Sensor_Check  ( EStation ) == SYS_ABORTED ) return SYS_ABORTED;

	return SYS_SUCCESS;

}

//////////////////////////////////////////////////////////////////////////
Module_Status PickRobot( int EStation , int Station , int Arm , BOOL TR , int nSide , BOOL SINGLE) {
	int CommStatus;
	Module_Status msRet = SYS_ABORTED;

	if(EStation < BM1_1)
	{ //PMA PMB PMC
		if ( PM_Chuck_Home_Check ( EStation ) == SYS_ABORTED ) return SYS_ABORTED;
	}

	if ( Slide_Sensor_Check  ( EStation ) == SYS_ABORTED ) return SYS_ABORTED;

	do {

		printf("[DEBUG] PickRobot ESt[%d] StN[%d] Arm[%d], TR[%d] Side[%d], Single[%d]\n",
			EStation, Station, Arm, TR, nSide, SINGLE);

		g_nRobotStatus = RB_READY;
		if ( TR ) {
			ALARM_HAPPEN = FALSE;
			if (RobotMovement( Station , Arm ,
					MTR7_Get_Robot , MTR7_Get_RobotB ,
					ALARM_PICK_WAFER ,
					ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
					-1 , -1 , -1 , -1 , 745 ) == SYS_ABORTED ) break;
		}
		else
		{
			if (TRUE == SINGLE)
			{
				if (0 == nSide)
				{
					printf("^&*^%$# Pick Arm %d Handling Single Wafer \n" , Arm);
					if ( RobotMovement( Station , Arm ,
						MTR7_Get_RobotL , MTR7_Get_RobotBL ,
						ALARM_PICK_WAFER ,
						ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
						ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_UP , -1 ) == SYS_ABORTED ) break;
				}
				else
				{
					printf("^&*^%$# Pick Arm %d Handling Single Wafer \n" , Arm);
					if ( RobotMovement( Station , Arm ,
						MTR7_Get_RobotR , MTR7_Get_RobotBR ,
						ALARM_PICK_WAFER ,
						ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
						ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_UP , -1 ) == SYS_ABORTED ) break;
				}
			}
			else
			{
				if ( RobotMovement( Station , Arm ,
					MTR7_Get_Robot , MTR7_Get_RobotB ,
					ALARM_PICK_WAFER ,
					ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
					ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_UP , -1 ) == SYS_ABORTED ) break;
			}
		}
		msRet = SYS_SUCCESS;
	} while(0);

	if ( Slide_Sensor_Check  ( EStation ) == SYS_ABORTED ) return SYS_ABORTED;

    // added 2003.06.17 hjeon - for wafer counter
	if ( gbDestCM ) RUN_FUNCTION( FUNC_WFR_COUNTER, "COUNT" );

	// 2005.12.20
	if(msRet != SYS_SUCCESS)
	{
		_LOG("----------> Robot Func Aborted !");
		if(g_nRobotStatus != RB_DONE)
		{
			WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
			_sleep(1000);
			_LOG("----------> WaferSync Reversed !");
		}
		else
		{
			_sleep(3000);
			_LOG("----------> WaferSync Updated !");
		}
	}
	WaferImageSync("PICK", EStation);
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status PlaceRobot( int EStation , int Station , int Arm , int nSide , BOOL SINGLE) {
	int CommStatus;
	Module_Status msRet = SYS_ABORTED;

	//...
	if(EStation < BM1_1)
	{ //PMA PMB PMC
		if(PM_Chuck_Home_Check(EStation) == SYS_ABORTED)	return SYS_ABORTED;
	}

	if(Slide_Sensor_Check(EStation) == SYS_ABORTED)			return SYS_ABORTED;

	//...
	do {

		printf("[DEBUG] PlaceRobot ESt[%d] StN[%d] Arm[%d], Side[%d], Single[%d]\n",
			EStation, Station, Arm, nSide, SINGLE);

		g_nRobotStatus = RB_READY;

		if(TRUE == SINGLE)
		{
			if (0 == nSide)
			{
				printf("^&*^%$# Place Arm %d Handling Single Wafer \n" , Arm);
				if ( RobotMovement( Station , Arm ,
					MTR7_Put_RobotL , MTR7_Put_RobotBL ,
					ALARM_PLACE_WAFER ,
					ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
					ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_DOWN , -1 ) == SYS_ABORTED ) break;
			}
			else
			{
				printf("^&*^%$# Place Arm %d Handling Single Wafer \n" , Arm);
				if ( RobotMovement( Station , Arm ,
					MTR7_Put_RobotR , MTR7_Put_RobotBR ,
					ALARM_PLACE_WAFER ,
					ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
					ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_DOWN , -1 ) == SYS_ABORTED ) break;
			}
		}
		else
		{
			if ( RobotMovement( Station , Arm ,
					MTR7_Put_Robot , MTR7_Put_RobotB ,
					ALARM_PLACE_WAFER ,
					ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
					ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_DOWN , -1 ) == SYS_ABORTED ) break;
		}

		msRet = SYS_SUCCESS;
	} while(0);

	if ( Slide_Sensor_Check  ( EStation ) == SYS_ABORTED ) return SYS_ABORTED;

	// 2005.12.20
	if(msRet != SYS_SUCCESS)
	{
		_LOG("----------> Robot Func Aborted !");
		if(g_nRobotStatus != RB_DONE)
		{
			WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
			_sleep(1000);
			_LOG("----------> WaferSync Reversed !");
		}
		else
		{
			_sleep(3000);
			_LOG("----------> Updated !");
		}
	}
	WaferImageSync("PLACE", EStation);
	return msRet;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RotateRobot( int Station , int Arm ) {
	return(
		RobotMovement( Station , Arm ,
			MTR7_Rot_Robot , MTR7_Rot_RobotB ,
			ALARM_ROTATE_WAFER ,
			ROBOT_SYNCH_ROTATE_START , ROBOT_SYNCH_EXTEND_START , 0 ,
			ROBOT_STATUS_RETRACT , Station , -1 , -1 , -1 )
		);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//Remark 2014.03.10 by hyuk

Module_Status ExtendRobot( int EStation, int Station , int Arm ) {

	if ( EStation < BM1_1 ) { //PMA PMB PMC
		if ( PM_Chuck_Home_Check ( EStation ) == SYS_ABORTED ) return SYS_ABORTED;
	}

	return(
		RobotMovement( Station , Arm ,
			MTR7_Ext_Robot , MTR7_Ext_Robot ,
			ALARM_EXTEND_WAFER ,
			ROBOT_SYNCH_EXTEND_START , ROBOT_SYNCH_RETRACT_START , 1 ,
			ROBOT_STATUS_EXTEND , Station , -1 , -1 , -1 )
		);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RetractRobot( int Station , int Arm ) {
	return(
		RobotMovement( Station , Arm ,
			MTR7_Ret_Robot , MTR7_Ret_Robot ,
			ALARM_RETRACT_WAFER ,
			ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_SUCCESS , 0 ,
			ROBOT_STATUS_RETRACT , -1 , -1 , -1 , -1 )
		);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status UpRobot( int Station , int Arm ) {
	return(
		RobotMovement( Station , Arm ,
			MTR7_RotP_Robot , MTR7_RotP_RobotB ,
			ALARM_UP_WAFER ,
			-1 , -1 , -1 ,
			-1 , Station , -1 , ROBOT_STATUS_UP , -1 )
		);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status DownRobot( int Station , int Arm ) {
	return(
		RobotMovement( Station , Arm ,
			MTR7_RotG_Robot , MTR7_RotG_RobotB ,
			ALARM_DOWN_WAFER ,
			-1 , -1 , -1 ,
			-1 , Station , -1 , ROBOT_STATUS_DOWN , -1 )
		);
}

//Remark 2014.03.10 by hyuk
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RotPicRobot( int Station , int Arm ) {
	return(
		RobotMovement( Station , Arm ,
			MTR7_RotG_Robot , MTR7_RotG_RobotB ,
			ALARM_ROTATE_WAFER ,
			ROBOT_SYNCH_ROTATE_START , ROBOT_SYNCH_EXTEND_START , 0 ,
			ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_DOWN , -1 )

		//mgsong remark
		/*
		RobotMovement( Station , Arm ,
			MTR7_RotG_Robot , MTR7_RotG_RobotB ,
			ALARM_ROTATE_WAFER ,
			ROBOT_SYNCH_ROTATE_START , ROBOT_SYNCH_EXTEND_START , 0 ,
			ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_DOWN , -1 )*/
		);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RotPlcRobot( int Station , int Arm ) {
	return(
		RobotMovement( Station , Arm ,
			MTR7_RotP_Robot , MTR7_RotP_RobotB ,
			ALARM_ROTATE_WAFER ,
			ROBOT_SYNCH_ROTATE_START , ROBOT_SYNCH_EXTEND_START , 0 ,
			ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_UP , -1 )

		//mgsong remark
		/*
		RobotMovement( Station , Arm ,
			MTR7_RotP_Robot , MTR7_RotP_RobotB ,
			ALARM_ROTATE_WAFER ,
			ROBOT_SYNCH_ROTATE_START , ROBOT_SYNCH_EXTEND_START , 0 ,
			ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_UP , -1 )*/
		);
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

BOOL Find_Chamber_Finger_From_String( char *String , int *Station , int *Arm , int *rc , char *SlotStr) {
	int offset = 0 , GoChamber , GoArm = ARM_A;
	int Slot = 0;
	if ( strlen( String ) < 2 ) return FALSE;
	if ( String[ 1 ] == '_' ) {
		offset = 2;
		switch( String[ 0 ] ) {
		case 'S' :	case 's' :	case 'A' :	case 'a' :	GoArm = ARM_A;	break;
		case 'T' :	case 't' :	case 'B' :	case 'b' :	GoArm = ARM_B;	break;
		default  :	return FALSE;	break;
		}
	}

	if ( STRCMP_L( String + offset , "BM1"  ) || STRCMP_L( String + offset , "BM2"  )) {
		Slot = atoi( SlotStr );
		if      (201 == Slot) { Slot = 1; }
		else if (403 == Slot) { Slot = 2; }
		else if (Slot >= 1 && Slot <= 4) {
			//2010. 10. 03 Single Slot Transfer
			if (1 == Slot || 2 == Slot) Slot = 1;
			else if (3 == Slot || 4 == Slot) Slot = 2;
		}
		else if (0 >= Slot)	{ Slot = 0; }
		else
		{
			;	// slot is 1 or 2 or 3 or etc...
		}

		if ( ( Slot <= 0 ) || ( Slot >= 3 ) ) {
			printf("Station Setting slot error\n");
			_LOG("--------> Station Setting slot error");
			return FALSE;
		}
	}

	if      ( STRCMP_L( String + offset , "PM1"  ) ) { GoChamber = PMA; }
	else if ( STRCMP_L( String + offset , "PM2"  ) ) { GoChamber = PMB; }
	else if ( STRCMP_L( String + offset , "PM3"  ) ) { GoChamber = PMC; }
	else if ( STRCMP_L( String + offset , "BM1"  ) && Slot == 1) { GoChamber = BM1_1; }
	else if ( STRCMP_L( String + offset , "BM1"  ) && Slot == 2) { GoChamber = BM1_2; }
	else if ( STRCMP_L( String + offset , "BM2"  ) && Slot == 1) { GoChamber = BM2_1; }
	else if ( STRCMP_L( String + offset , "BM2"  ) && Slot == 2) { GoChamber = BM2_2; }
	else {
		return FALSE;
	}

    // added 2003.06.17 hjeon - for wafer counter
	// dest. station is Buffer module..
	if ( GoChamber == BM1_1 || GoChamber == BM1_2 || GoChamber == BM2_1 || GoChamber == BM2_2) gbDestCM = TRUE;
	else gbDestCM = FALSE;

	*Station = MODULE_CONTROL_STATION[GoChamber];
	*Arm = GoArm;
	*rc = GoChamber;
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//2014.08.06
void PostPickEvent(int Station,int Arm)
{
	int nComstatus;
	int nWaferID1 = 0, nWaferID2 = 0;
	int nWaferSource1 = 0, nWaferSource2 = 0;
	char szWaferLotID1[40],szWaferLotID2[40];
	char szWaferMID1[40],szWaferMID2[40];

	memset(szWaferLotID1,0,sizeof(szWaferLotID1));
	memset(szWaferLotID2,0,sizeof(szWaferLotID2));
	memset(szWaferMID1	,0,sizeof(szWaferMID1));
	memset(szWaferMID2	,0,sizeof(szWaferMID2));

	if(Arm == ARM_A)
	{
		nWaferID1 = READ_DIGITAL(TM_A_Status,&nComstatus);
		nWaferID2 = READ_DIGITAL(TM_A_Status2,&nComstatus);
		nWaferSource1 = READ_DIGITAL(TM_A_Source,&nComstatus);
		nWaferSource2 = READ_DIGITAL(TM_A_Source2,&nComstatus);

		switch(nWaferSource1)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID1,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID1,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID1,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID1,"");
				strcpy(szWaferMID1,"");
				nWaferID1 = 0;
				break;
		}
		if(nWaferID1 > 0)
		{
			WRITE_STRING(TM_RB_LOTID1,szWaferLotID1,&nComstatus);
			WRITE_STRING(TM_RB_MIDID1,szWaferMID1,&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID1,nWaferID1,&nComstatus);
		}
		else
		{
			WRITE_STRING(TM_RB_LOTID1,"",&nComstatus);
			WRITE_STRING(TM_RB_MIDID1,"",&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID1,nWaferID1,&nComstatus);
		}

		switch(nWaferSource2)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID2,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID2,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID2,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID2,"");
				strcpy(szWaferMID2,"");
				nWaferID1 = 0;
				break;
		}
		if(nWaferID2 > 0)
		{
			WRITE_STRING(TM_RB_LOTID2,szWaferLotID2,&nComstatus);
			WRITE_STRING(TM_RB_MIDID2,szWaferMID2,&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID2,nWaferID2,&nComstatus);
		}
		else
		{
			WRITE_STRING(TM_RB_LOTID2,"",&nComstatus);
			WRITE_STRING(TM_RB_MIDID2,"",&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID2,nWaferID2,&nComstatus);

		}

	}
	else if(Arm == ARM_B)
	{
		nWaferID1 = READ_DIGITAL(TM_B_Status,&nComstatus);
		nWaferID2 = READ_DIGITAL(TM_B_Status2,&nComstatus);
		nWaferSource1 = READ_DIGITAL(TM_B_Source,&nComstatus);
		nWaferSource2 = READ_DIGITAL(TM_B_Source2,&nComstatus);

		switch(nWaferSource1)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID1,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID1,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID1,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID1,"");
				strcpy(szWaferMID1,"");
				nWaferID1 = 0;
				break;
		}
		if(nWaferID1 > 0)
		{
			WRITE_STRING(TM_RB_LOTID1,szWaferLotID1,&nComstatus);
			WRITE_STRING(TM_RB_MIDID1,szWaferMID1,&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID1,nWaferID1,&nComstatus);
		}
		else
		{
			WRITE_STRING(TM_RB_LOTID1,"",&nComstatus);
			WRITE_STRING(TM_RB_MIDID1,"",&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID1,nWaferID1,&nComstatus);
		}

		switch(nWaferSource2)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID2,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID2,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID2,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID2,"");
				strcpy(szWaferMID2,"");
				nWaferID1 = 0;
				break;
		}
		if(nWaferID2 > 0)
		{
			WRITE_STRING(TM_RB_LOTID2,szWaferLotID2,&nComstatus);
			WRITE_STRING(TM_RB_MIDID2,szWaferMID2,&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID2,nWaferID2,&nComstatus);
		}
		else
		{
			WRITE_STRING(TM_RB_LOTID2,"",&nComstatus);
			WRITE_STRING(TM_RB_MIDID2,"",&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID2,nWaferID2,&nComstatus);
		}
	}

	switch(Station)
	{
		case 1:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_PM1,&nComstatus);
			break;
		case 2:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_PM2,&nComstatus);
			break;
		case 3:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_PM3,&nComstatus);
			break;
		case 4:
		case 5:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_LL1,&nComstatus);
			break;
		case 6:
		case 7:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_LL2,&nComstatus);
			break;
		default:
			break;
	}
}

void PostPlaceEvent(int Station,int Arm, BOOL bIOReadOk)
{
	int nComstatus;
	int nWaferID1 = 0, nWaferID2 = 0;
	int nWaferSource1 = 0, nWaferSource2 = 0;
	char szWaferLotID1[40],szWaferLotID2[40];
	char szWaferMID1[40],szWaferMID2[40];

	memset(szWaferLotID1,0,sizeof(szWaferLotID1));
	memset(szWaferLotID2,0,sizeof(szWaferLotID2));
	memset(szWaferMID1	,0,sizeof(szWaferMID1));
	memset(szWaferMID2	,0,sizeof(szWaferMID2));

	if(Arm == ARM_A && bIOReadOk == TRUE)
	{
		nWaferID1 = READ_DIGITAL(TM_A_Status,&nComstatus);
		nWaferID2 = READ_DIGITAL(TM_A_Status2,&nComstatus);
		nWaferSource1 = READ_DIGITAL(TM_A_Source,&nComstatus);
		nWaferSource2 = READ_DIGITAL(TM_A_Source2,&nComstatus);

		switch(nWaferSource1)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID1,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID1,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID1,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID1,"");
				strcpy(szWaferMID1,"");
				nWaferID1 = 0;
				break;
		}
		if(nWaferID1 > 0)
		{
			WRITE_STRING(TM_RB_LOTID1,szWaferLotID1,&nComstatus);
			WRITE_STRING(TM_RB_MIDID1,szWaferMID1,&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID1,nWaferID1,&nComstatus);
		}
		else
		{
			WRITE_STRING(TM_RB_LOTID1,"",&nComstatus);
			WRITE_STRING(TM_RB_MIDID1,"",&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID1,nWaferID1,&nComstatus);
		}

		switch(nWaferSource2)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID2,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID2,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID2,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID2,"");
				strcpy(szWaferMID2,"");
				nWaferID1 = 0;
				break;
		}
		if(nWaferID2 > 0)
		{
			WRITE_STRING(TM_RB_LOTID2,szWaferLotID2,&nComstatus);
			WRITE_STRING(TM_RB_MIDID2,szWaferMID2,&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID2,nWaferID2,&nComstatus);
		}
		else
		{
			WRITE_STRING(TM_RB_LOTID2,"",&nComstatus);
			WRITE_STRING(TM_RB_MIDID2,"",&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID2,nWaferID2,&nComstatus);
		}

	}
	else if(Arm == ARM_B && bIOReadOk == TRUE)
	{
		nWaferID1 = READ_DIGITAL(TM_B_Status,&nComstatus);
		nWaferID2 = READ_DIGITAL(TM_B_Status2,&nComstatus);
		nWaferSource1 = READ_DIGITAL(TM_B_Source,&nComstatus);
		nWaferSource2 = READ_DIGITAL(TM_B_Source2,&nComstatus);

		switch(nWaferSource1)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID1,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID1,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID1,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID1,"");
				strcpy(szWaferMID1,"");
				nWaferID1 = 0;
				break;
		}
		if(nWaferID1 > 0)
		{
			WRITE_STRING(TM_RB_LOTID1,szWaferLotID1,&nComstatus);
			WRITE_STRING(TM_RB_MIDID1,szWaferMID1,&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID1,nWaferID1,&nComstatus);
		}
		else
		{
			WRITE_STRING(TM_RB_LOTID1,"",&nComstatus);
			WRITE_STRING(TM_RB_MIDID1,"",&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID1,nWaferID1,&nComstatus);
		}

		switch(nWaferSource2)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID2,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID2,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID2,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID2,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID2,"");
				strcpy(szWaferMID2,"");
				nWaferID1 = 0;
				break;
		}
		if(nWaferID2 > 0)
		{
			WRITE_STRING(TM_RB_LOTID2,szWaferLotID2,&nComstatus);
			WRITE_STRING(TM_RB_MIDID2,szWaferMID2,&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID2,nWaferID2,&nComstatus);
		}
		else
		{
			WRITE_STRING(TM_RB_LOTID2,"",&nComstatus);
			WRITE_STRING(TM_RB_MIDID2,"",&nComstatus);
			WRITE_DIGITAL(TM_RB_WAFERID2,nWaferID2,&nComstatus);
		}

	}
	if(bIOReadOk == TRUE) return;
	switch(Station)
	{
		case 1:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_PM1,&nComstatus);
			break;
		case 2:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_PM2,&nComstatus);
			break;
		case 3:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_PM3,&nComstatus);
			break;
		case 4:
		case 5:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_LL1,&nComstatus);
			break;
		case 6:
		case 7:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_LL2,&nComstatus);
			break;
		default:
			break;
	}
}
//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------

Module_Status Simulation_Main()
{
	// 2013.11.15
	BOOL TR      = FALSE;
	BOOL bSingle = FALSE;
	BOOL bDualArm = FALSE;			// 2013.11.15
	int  nSingleSide = 0;
	int	Station = 0, Arm = 0, EStation = 0, Slot = 0, CommStatus = 0;


	char RunStr[41] , TempStr[41] , TempStr2[41] , StationStr[41] , SlotStr[41] , ElseStr[41] , TempStr3[41] , SINGLE[41];
	char ElseStr2[41], SINGLE2[41];		// 2013.11.15
	Module_Status msRet = SYS_SUCCESS;
	char *pParam;
	char szCmd[80];
	char nSlotN = 1;					// 2013.11.19

	// 2013.11.15
	pParam = PROGRAM_PARAMETER_READ();
	STR_SEPERATE( pParam	, RunStr     , TempStr	, 40 ); //PICK,PLACE
	STR_SEPERATE( TempStr	, StationStr , TempStr2	, 40 ); //A_BM1
	STR_SEPERATE( TempStr2	, SlotStr	 , TempStr3	, 40 ); //1:Slot
	STR_SEPERATE( TempStr3	, SINGLE	 , ElseStr	, 40 ); //Single flag
	STR_SEPERATE( ElseStr	, SINGLE2	 , ElseStr2	, 40 ); //Single fla

	do {
		if (STRCMP_L( SlotStr  , "TRANSFER2" )) TR = TRUE;
		if (STRCMP_L( TempStr3 , "TRANSFER2" )) TR = TRUE;
		if (STRCMP_L( SINGLE   , "SINGLE"))     bSingle = TRUE;
		if (STRCMP_L( SINGLE2  , "SINGLE"))     bSingle = TRUE;

		if (!Find_Chamber_Finger_From_String(StationStr , &Station , &Arm , &EStation, SlotStr)) {
			if ( (!STRCMP_L( RunStr , "READY")) && (!STRCMP_L( RunStr , "HOME")) && (!STRCMP_L(RunStr , "HOME2")) )
			{
				printf( "---> Robot Control Error - Not Support Station %s\n" , pParam );
				_MLOG("--------> Robot Control Error - Not Support Station [%s]" , pParam );
				msRet = SYS_ABORTED;
				break;
			}
			else
			{
				EStation = NO_STATION;
			}
		}

		//BM Slot Number Coming such as 201 403 605 so conversion is necessary
		//_LOG("--------> %s %s(%d) Started in TM RbLow Func", RunStr, StationStr, Slot);
		_LOG("--------> %s %s(%s) Started in TM RbLow Func", RunStr, StationStr, SlotStr);

		if ( ( EStation == BM1_1 || EStation == BM1_2 || EStation == BM2_1 || EStation == BM2_2 )) {
			Slot = atoi( SlotStr );
			if      (201 == Slot) { Slot = 1; nSlotN = 1; }		// MARS 2013.11.19
			else if (403 == Slot) { Slot = 2; nSlotN = 3; }		// MARS 2013.11.19
			else if (200 == Slot) { Slot = 1; nSlotN = 2; nSingleSide = 1; }
			else if (400 == Slot) { Slot = 2; nSlotN = 4; nSingleSide = 1; }
			else if (Slot >= 1 && Slot <= 4) {
				nSlotN = Slot;		// MARS 2013.11.19
				//2010. 10. 03 Single Slot Transfer
				if (1 == Slot || 2 == Slot) Slot = 1;
				else if (3 == Slot || 4 == Slot) Slot = 2;
			}
			else if (0 >= Slot)	{ Slot = 0; nSlotN = 1; }		// 2013.11.19
			else
			{
				;	// slot is 1 or 2 or 3 or etc...
			}

			if ( ( Slot <= 0 ) || ( Slot >= 3 ) ) {
				printf("Station Setting slot error\n");
				_LOG("--------> Station Setting slot error");
				msRet = SYS_ABORTED;
				break;
			}

		}
	} while(0);

	_LOG("--------> %s Started in TM RbLow Func(Sim)", pParam);
	// 2013.11.15
	if(bSingle == FALSE) bDualArm = TRUE;
	LogEvent("TM", RunStr, "Start", StationStr, nSlotN, Arm, "", bDualArm);		// 2013.11.19
	_MLOG("-----> Start RunStr:%s Station:%s Slot:%d Arm:%d", RunStr, StationStr, Slot, Arm);

	// if ( !WAIT_SECONDS( 2 ) ) return SYS_ABORTED;
	WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RETRACT_START , &CommStatus );
	WRITE_DIGITAL( ROBOT_RET , 0 , &CommStatus );

	if(STRCMP_L(RunStr,"PLACE")) {PostPlaceEvent(Station,Arm,TRUE);}
	// 2005.12.08
	sprintf(szCmd,"VAC_ROBOT %s %s", RunStr, StationStr);
	if(SYS_SUCCESS == g_DynIO.dRUN_FUNCTION(TMSIM_VAC_ROBOT, szCmd))
	{
		//2014.08.06
		if(STRCMP_L(RunStr,"PICK"))
		{
			PostPickEvent(Station,Arm);
		}
		else if(STRCMP_L(RunStr,"PLACE"))
		{
			PostPlaceEvent(Station,Arm,FALSE);
		}
		//
		WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_SUCCESS , &CommStatus );
		// 2020.02.26 Wafer Sync
		WaferImageSync(RunStr, EStation);
	}
	else
	{
		// 2005.12.20
		WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
		_sleep(1000);
		_LOG("--------> %s Failed in TM RbLow Func(Sim)", pParam);
		msRet = SYS_ABORTED;
	}

	_LOG("--------> %s Finished in TM RbLow Func(Sim)", pParam);
	LogEvent("TM", RunStr, "End", StationStr, nSlotN, Arm, "", bDualArm);		// 2013.11.19
	return msRet;
}
//... 2015.01.19
void DisplayRotatePos(int nStation)
{
	int nCommStatus;

	switch(nStation)
	{
	case 4:
	case 5:
	case 6:
	case 7:
					WRITE_ANALOG ( CTC_Rb_Rotation   , 0.0 , &nCommStatus );
					break;
	case 1:
					WRITE_ANALOG ( CTC_Rb_Rotation   , 180.0 , &nCommStatus );
					break;
	case 2:
					WRITE_ANALOG ( CTC_Rb_Rotation   , 270.0 , &nCommStatus );
					break;
	case 3:
					WRITE_ANALOG ( CTC_Rb_Rotation   , 90.0 , &nCommStatus );
					break;
	default:
					WRITE_ANALOG ( CTC_Rb_Rotation   , 0.0 , &nCommStatus );
					break;
	}
}
//////////////////////////////////////////////////////////////////////////
//... 2016.01.19
Module_Status CycleRotate()
{
	Module_Status msRet = SYS_ABORTED;

	int		nIOStatus	= 0;
	int		nTgtCycCnt	= 0;
	int		nCurCycCnt  = 0;
	int		nStation	= 0;

	do
	{
		//... Check Move Cycle Count
		nTgtCycCnt = (int) READ_DIGITAL(PRMD_TM_ROTATE_CNT, &nIOStatus);
		if(nTgtCycCnt < 1)	{	msRet = SYS_SUCCESS;	break;	}

		//... Run Init Home Position Before Cycle Move
		if(HomeRobot() == SYS_ABORTED)  break;

		//... Run Cycle Move TEST
		for(nCurCycCnt = 1; nCurCycCnt <= nTgtCycCnt; nCurCycCnt++)
		{
			//... Calculate Rotate Position
			nStation = nCurCycCnt % 5;
			nStation = nStation + 1;
			if(nStation <= 0 || nStation >= 5 ) nStation = 1;

			//... Move Home Position
			msRet = RotateRobot(nStation,ARM_A);

			if(SYS_SUCCESS != msRet)	break;
			if(! WAIT_SECONDS(3))		break;		//... Aborted by User

			//... Display Rotate Position
			DisplayRotatePos(nStation);
		}

		//... Run Init Home Position after Cycle Move
		if(HomeRobot() == SYS_ABORTED)  break;
		msRet = SYS_SUCCESS;
	} while (0);

	//... Save Information Parameter

	return	msRet;
}
//////////////////////////////////////////////////////////////////////////
Module_Status Real_Main()
{
	BOOL TR      = FALSE;
	BOOL bSingle = FALSE;
	BOOL bDualArm = FALSE;			// 2013.11.15
	int  nSingleSide = 0;

	int	Station = 0, Arm = 0, EStation = 0, Slot = 0, CommStatus = 0;
	char RunStr[41] , TempStr[41] , TempStr2[41] , TempStr3[41] , StationStr[41] , SlotStr[41] , ElseStr[41] , ElseStr2[41] , SINGLE[41] , SINGLE2[41];
	char nSlotN = 1;					// 2013.11.19
	char ParaStr[256] ;

	//.....
	memset(RunStr,		0, sizeof(RunStr));
	memset(TempStr,		0, sizeof(TempStr));
	memset(TempStr2,	0, sizeof(TempStr2));
	memset(StationStr,	0, sizeof(StationStr));
	memset(SlotStr,		0, sizeof(SlotStr));
	memset(ElseStr,		0, sizeof(ElseStr));


	Module_Status msRet = SYS_SUCCESS;
	char *pParam = NULL;

	if ( READ_DIGITAL( CONTROL , &Station ) == 1 ) return SYS_SUCCESS;
	pParam = PROGRAM_PARAMETER_READ();
	STR_SEPERATE( pParam	, RunStr     , TempStr	, 40 ); //PICK,PLACE
	STR_SEPERATE( TempStr	, StationStr , TempStr2	, 40 ); //A_BM1
	STR_SEPERATE( TempStr2	, SlotStr	 , TempStr3	, 40 ); //1:Slot
	STR_SEPERATE( TempStr3	, SINGLE	 , ElseStr	, 40 ); //Single flag
	STR_SEPERATE( ElseStr	, SINGLE2	 , ElseStr2	, 40 ); //Single flag

	do {
		printf( "++++++> TM Robot Rblow Control= %s\n" , pParam );

		if (STRCMP_L( SlotStr  , "TRANSFER2" )) TR = TRUE;
		if (STRCMP_L( TempStr3 , "TRANSFER2" )) TR = TRUE;
		if (STRCMP_L( SINGLE   , "SINGLE") || STRCMP_L( SINGLE   , "SINGLE2"))     bSingle = TRUE;
		if (STRCMP_L( SINGLE2  , "SINGLE") || STRCMP_L( SINGLE2  , "SINGLE2"))     bSingle = TRUE;

		if (!Find_Chamber_Finger_From_String(StationStr , &Station , &Arm , &EStation , SlotStr)) {
			if ( (!STRCMP_L( RunStr , "READY")) && (!STRCMP_L( RunStr , "HOME")) && (!STRCMP_L( RunStr , "HOME2"))&& (!STRCMP_L( RunStr , "CYLROTATE")))
			{
				printf( "---> Robot Control Error - Not Support Station %s\n" , pParam );
				_LOG("--------> Robot Control Error - Not Support Station [%s]" , pParam );
				msRet = SYS_ABORTED;
				break;
			}
			else
			{
				EStation = NO_STATION;
			}
		}

		_LOG("--------> %s %s(%s) Started in TM RbLow Func", RunStr, StationStr, SlotStr);

		//.....
		if(EStation == BM1_1 || EStation == BM1_2 || EStation == BM2_1 || EStation == BM2_2)
		{
			Slot = atoi( SlotStr );
			if     (201 == Slot) { Slot = 1; nSlotN = 1; }		// MARS 2013.11.19
			else if(403 == Slot) { Slot = 2; nSlotN = 3; }		// MARS 2013.11.19
			else if(200 == Slot) { Slot = 1; nSlotN = 2; nSingleSide = 1; }
			else if(400 == Slot) { Slot = 2; nSlotN = 4; nSingleSide = 1; }
			else if(Slot >= 1 && Slot <= 4)
			{
				nSlotN = Slot;		// MARS 2013.11.19
				//2010. 10. 03 Single Slot Transfer
				if (1 == Slot || 2 == Slot) Slot = 1;
				else if (3 == Slot || 4 == Slot) Slot = 2;
			}
			else if (0 >= Slot)	{ Slot = 0; nSlotN = 1; }		// 2013.11.19

			if(Slot <= 0 || Slot >= 3)
			{
				printf("Station Setting slot error\n");
				_LOG("--------> Station Setting slot error");
				msRet = SYS_ABORTED;
				break;
			}

		}
		else
		{  // case PM
			Slot = atoi(SlotStr);
			if(200 == Slot)
			{	//... Only Right Arm Use
				Slot = 1;
				nSlotN = 2;
				nSingleSide = 1;
			}
		}

		// MARS 2013.11.15
		if(bSingle == FALSE) bDualArm = TRUE;
		LogEvent("TM", RunStr, "Start", StationStr, nSlotN, Arm, "", bDualArm);		// MARS 2013.11.19
		if(STRCMP_L(RunStr,"PLACE")) {PostPlaceEvent(Station,Arm, TRUE);}

		if (STRCMP_L(RunStr , "READY")) {
			msRet = (ReadyCheck());
		} else if (STRCMP_L(RunStr , "HOME")) {
			printf ("VTM Robot Home\n");
			if (HomeRobot () == SYS_ABORTED) msRet =  SYS_ABORTED;
		} else if (STRCMP_L(RunStr , "HOME2")) {
			printf ("VTM Robot Home2\n");
			if (HomeRobot2 () == SYS_ABORTED) msRet =  SYS_ABORTED;
		}
		else if ( STRCMP_L( RunStr , "PICK"      ) ) msRet = (PickRobot   (EStation , Station , Arm , TR , nSingleSide , bSingle));
		else if ( STRCMP_L( RunStr , "PLACE"     ) ){

			// AWS Set
			if( SYS_ABORTED == AWSLimitSet(Station)) return SYS_ABORTED;
			// AWS Set

			msRet = (PlaceRobot  (EStation , Station , Arm , nSingleSide , bSingle));

			if		( Arm == ARM_A ) {
				sprintf ( ParaStr , "%s %d %d %d" , StationStr , Arm , READ_DIGITAL(TM_A_Status , &CommStatus) , READ_DIGITAL(TM_A_Status2, &CommStatus) ) ;
			}
			else if ( Arm == ARM_B ) {
				sprintf ( ParaStr , "%s %d %d %d" , StationStr , Arm , READ_DIGITAL(TM_B_Status , &CommStatus) , READ_DIGITAL(TM_B_Status2, &CommStatus) ) ;
			}
			RUN_SET_FUNCTION ( OBJ_TM_AWS	, ParaStr ) ;
		}
	    else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = (RotateRobot (Station , Arm));
	    else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = (ExtendRobot (EStation, Station , Arm));
	    else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = (RetractRobot(Station , Arm));
	    else if ( STRCMP_L( RunStr , "UP"        ) ) msRet = (UpRobot     (Station , Arm));
	    else if ( STRCMP_L( RunStr , "DOWN"      ) ) msRet = (DownRobot   (Station , Arm));
		else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = (RotPicRobot (Station , Arm));
		else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = (RotPlcRobot (Station , Arm));
		else if ( STRCMP_L( RunStr , "CYLROTATE" ) ) msRet = (CycleRotate  ());   //... 2016.01.19
		else if ( STRCMP_L( RunStr , "AWS_INIT"  ) )
		{
			msRet = AWS_INIT(EStation,  Station , Arm, bSingle );
		}
		else {
			printf( "---> Robot Control Error - Not Support Code %s\n" , pParam );
			msRet =  SYS_ABORTED;
		}
	} while(0);
	//2014.08.06
	if(STRCMP_L(RunStr,"PICK"))
	{
		PostPickEvent(Station,Arm);
	}
	else if(STRCMP_L(RunStr,"PLACE"))
	{
		PostPlaceEvent(Station,Arm, FALSE);
	}
	//

	if(STRCMP_L( RunStr , "ROTATE"    ))
	{
		if(msRet == SYS_SUCCESS)
		{
			//... 2016.01.19
			DisplayRotatePos(Station);
		}
	}

	// Parameter Check.
	if (READ_DIGITAL(PRMD_TM_ROBOT_Load , &CommStatus) == check)
	{
		READ_AXIS_LOAD();
		READ_AXIS_MOVE_CNT();
	}

	LogEvent("TM", RunStr, "End", StationStr, nSlotN, Arm, "", bDualArm);		// 2013.11.19
	_LOG("--------> %s %s(Slot) Finished in TM RbLow Func", RunStr, StationStr, Station);
	return msRet;
}


//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {

	int nIOStatus;

	//... 2017.03.07
	if(All_Enable != READ_DIGITAL(INTLKS_CONTROL,&nIOStatus))
	{
		WRITE_DIGITAL(INTLKS_CONTROL,All_Enable,&nIOStatus);
	}

	// 2005.12.20
	if(CheckAbortControl()) return SYS_ABORTED;
	g_nRobotStatus = RB_IDLE;

	if ( SIMULATION_MODE ) return Simulation_Main();
	else return Real_Main();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void Program_Enter_Code() {
	int i;
	char TempStr[256] , TempStr2[256] , TempStr3[256];
	//---------------------------------------------------
//	REGIST_FILE_INFO( "Brooks Robot(VTR/MTR/MAG) Control Program(2001.06.01)" );
	REGIST_FILE_INFO( "Brooks Robot(VTR/MTR/MAG) Control Program(2001.09.01)" );
	//---------------------------------------------------
	if ( strlen( PROGRAM_ARGUMENT_READ() ) <= 0 ) return;
	strcpy( TempStr , PROGRAM_ARGUMENT_READ() );
	for ( i = 0 ; i < 22 ; i++ ) {
		STR_SEPERATE_CHAR( TempStr , '|' , TempStr2 , TempStr3 , 255 );
		MODULE_CONTROL_STATION[i] = atoi( TempStr2 );
		strcpy( TempStr , TempStr3 );
	}
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	// 2005.12.20
	g_DynIO.InitIOList(g_pIOList, false);

	Old_AWS_Control = READ_DIGITAL( ULVAC_AWS_USE_V , &CommStatus );

	return TRUE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		_gCLOSE_LOG();
	}
}

} // End of [extern "C"]
//==========================================================================================
