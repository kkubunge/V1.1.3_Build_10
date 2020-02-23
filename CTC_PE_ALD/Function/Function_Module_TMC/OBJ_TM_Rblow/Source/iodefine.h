#ifndef IODEFINE_H
#define IODEFINE_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
// IO List
//
typedef enum {
		IO_Driver_Loding	,
		SCHEDULER			,
		CONTROL				,
		
		MTR7_Comm_Status	,
//		MTR7_T_Error_Clear	,
		MTR7_Run_Status		,
		TM_RB_Run_Status	,
		
		MTR7_Get_Robot		,
		MTR7_Get_RobotL		,
		MTR7_Get_RobotR		,
		MTR7_Put_Robot		,
		MTR7_Put_RobotL		,
		MTR7_Put_RobotR		,
		
		MTR7_Rot_Robot		,
		MTR7_RotG_Robot		,
		MTR7_RotP_Robot		,
		
		MTR7_Ext_Robot		,
		MTR7_Ret_Robot		,
//		MTR7_Up_Robot		,
//		MTR7_Down_Robot		,
		
		MTR7_Get_RobotB		,
		MTR7_Get_RobotBL	,
		MTR7_Get_RobotBR	,
		MTR7_Put_RobotB		,
		MTR7_Put_RobotBL	,
		MTR7_Put_RobotBR	,
		
		MTR7_Rot_RobotB		,
		MTR7_RotG_RobotB	,
		MTR7_RotP_RobotB	,
		
//		MTR7_Ext_RobotB		,
//		MTR7_Ret_RobotB		,
//		MTR7_Up_RobotB		,
//		MTR7_Down_RobotB	,
		
		MTR7_HOME			,
		MTR7_HOME2			,
		MTR7_STOP			,
		MTR7_ErrorCode		,
		MTR7_Error			,
		
		TM_RB_ErrorBit0 	,
		TM_RB_ErrorBit1		,
		TM_RB_ErrorBit2		,
		TM_RB_ErrorBit3		,
		TM_RB_ErrorBit4		,
		
		ROBOT_SYNCH			,
		ROBOT_RET			,
		ROBOT_STATION		,
		MOVE_WAFER_TIMEOUT	,

		CTC_Rb_Rotation 	,
		CTC_Rb_Movement 	,
		CTC_Rb_Extension 	,
		CTC_Rb_Extension2 	,		
		
		FUNC_WFR_COUNTER	,
		
		TM_A_Status			,
		TM_A_Status2		,
		TM_B_Status			,
		TM_B_Status2		,
		BM2_Status1			,
		BM2_Status2			,
		BM2_Status3			,
		BM2_Status4			,
		TM_Sensor			,
		TM_Pressure			,
		BM2_Sensor			,
		BM2_Pressure		,
		TMC_ISOGATE_CONTROL	,

		ULVAC_Load_Status_A	,
		ULVAC_Load_Status_B ,
		ULVAC_Load_Status_C ,
		ULVAC_Load_Status_D ,

		OBJ_TM_AWS				,
		ULVAC_AWS_LIMIT_PM_HI	,
		ULVAC_AWS_LIMIT_BM_HI	,
		ULVAC_AWS_LIMIT_PM_LOW	,
		ULVAC_AWS_LIMIT_BM_LOW	,
		ULVAC_AWS_USE_V			,

		ULVAC_AWS_INIT_A	,
		ULVAC_AWS_INIT_B	,
		ULVAC_AWS_INIT_C	,
		ULVAC_AWS_INIT_D	,
		ULVAC_AWS_INIT_UP	,
		ULVAC_AWS_INIT_LO	,


		PMA_1_CHUCK_HOME_STAT,
		PMA_2_CHUCK_HOME_STAT,
		PMB_1_CHUCK_HOME_STAT,
		PMB_2_CHUCK_HOME_STAT,
		PMC_1_CHUCK_HOME_STAT,
		PMC_2_CHUCK_HOME_STAT,

		PMA_SERV_MAIN		,
		PMB_SERV_MAIN		,
		PMC_SERV_MAIN		,

		TM_Slide_PMA_1		,
		TM_Slide_PMA_2		,
		TM_Slide_PMB_1		,
		TM_Slide_PMB_2		,
		TM_Slide_PMC_1		,
		TM_Slide_PMC_2		,
		TM_Slide_BM_1		,
		TM_Slide_BM_2		,

		TM_SlitVv_PMA_1		,
		TM_SlitVv_PMA_2		,
		TM_SlitVv_PMB_1		,
		TM_SlitVv_PMB_2		,
		TM_SlitVv_PMC_1		,
		TM_SlitVv_PMC_2		,
		TM_SlitVv_BM1_1		,
		TM_SlitVv_BM1_2		,
		TM_SlitVv_BM2_1		,
		TM_SlitVv_BM2_2		,
			
		CTC_EVENT_LOG_SVR	,

		// 2014.10.04
		TM_A_Source			,	
		TM_A_Source2		,
		TM_B_Source			,	
		TM_B_Source2		,	

		JOB_NAME			,	
		JOB_NAME2			,	
		JOB_NAME3			,	
		
		MID_NAME			,
		MID_NAME2			,
		MID_NAME3			,	

		TM_RB_LOTID1		,
		TM_RB_LOTID2		,
	 	TM_RB_MIDID1		,	
		TM_RB_MIDID2		,
		TM_RB_WAFERID1		,
		TM_RB_WAFERID2		,
		//... 2016.01.19
		PRMD_TM_ROTATE_CNT	,
		//...
		//... 2017.03.07
		INTLKS_CONTROL		,
		
		//... 2019.11.12
		ULVAC_LOAD_AXIS_CTR,
		ULVAC_LOAD_READ,
		ULVAC_LOAD_Ti,
		ULVAC_LOAD_A,
		ULVAC_LOAD_D,
		ULVAC_LOAD_To,
		ULVAC_LOAD_B,
		ULVAC_LOAD_C,
		ULVAC_LOAD_Z,
		ULVAC_LOAD_ALM_RANGE,
		ULVAC_MOVE_CNT_READ,
		ULVAC_MOVE_CNT_N,
		ULVAC_MOVE_CNT_R,
		ULVAC_MOVE_CNT_Z,
		ULVAC_MOVE_TIME,
		PRMD_TM_ROBOT_Load,
		
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#endif
