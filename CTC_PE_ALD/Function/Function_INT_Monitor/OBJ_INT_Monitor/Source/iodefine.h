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

	FM_RB_Comm_Status	,
	FM_RB_Run_Status		,
	FM_RB_Arm_Status		,
	FM_RB_Arm_StatusB	,
	FM_RB_Arms_Status,

	FM_RB_Get_Robot		,
//	FM_RB_GetAL_Robot	,
	FM_RB_Put_Robot		,
	FM_RB_Rot_Robot		,

//	FM_RB_RotG_Robot		,
//	FM_RB_RotP_Robot		,
	FM_RB_Ext_Robot		,
	FM_RB_Ret_Robot		,
	FM_RB_Up_Robot		,
	FM_RB_Down_Robot		,

	FM_RB_Get_RobotB		,
//	FM_RB_GetAL_RobotB	,
	FM_RB_Put_RobotB		,
	FM_RB_Rot_RobotB		,

	FM_RB_Get_RobotAB		,
	FM_RB_Put_RobotAB		,
	FM_RB_Rot_RobotAB		,

//	FM_RB_RotG_RobotB	,
//	FM_RB_RotP_RobotB	,
	FM_RB_Ext_RobotB		,
	FM_RB_Ret_RobotB		,
	FM_RB_Up_RobotB		,
	FM_RB_Down_RobotB	,
	FM_RB_HOME			,
//	FM_RB_STOP			,

//	FM_RB_Read_Coord		,
//	FM_RB_Read_CoordB	,
//	FM_RB_Set_Coord		,
//	FM_RB_Set_CoordB		,

	FM_RB_ErrorCode		,

//	FM_RB_S_R_CoordPos	,
//	FM_RB_S_T_CoordPos	,
//	FM_RB_R_CoordPos		,
//	FM_RB_T_CoordPos		,

	FM_RB_T_Slot_Pos		,
	FM_RB_Reset	,
	FM_RB_CPUReset,

	ROBOT_SYNCH     ,
	ROBOT_RET       ,
	ROBOT_STATION	,

	MOVE_WAFER_TIMEOUT ,

//	CASSETTE_MAPPING	,
//	CASSETTE_MAP_UPDATE	,
//	CASSETTE_Wafer_Map	,

	CASSETTE_MAPPING	,
	CASSETTE_MAPPING2	,
	CASSETTE_MAPPING3	,

	CASSETTE_MAP_UPDATE	,
	CASSETTE_MAP_UPDATE2,
	CASSETTE_MAP_UPDATE3,

	CASSETTE_Wafer_Map	,
	CASSETTE_Wafer_Map2	,
	CASSETTE_Wafer_Map3	,

//	LP_Mapping_Status,
//	LP_Mapping_Status2,
//	LP_Mapping_Status3,


//	CASSETTE1_DoorUp	,
//	CASSETTE1_DoorDown	,
	CASSETTE1_DoorUpDownXI	,

//	CASSETTE2_DoorUp	,
//	CASSETTE2_DoorDown	,
	CASSETTE2_DoorUpDownXI	,

//	CASSETTE3_DoorUp	,
//	CASSETTE3_DoorDown	,
	CASSETTE3_DoorUpDownXI	,

	CASSETTE1_Slide_Status	,
	CASSETTE2_Slide_Status	,
	CASSETTE3_Slide_Status	,

//	CASSETTE1_MAPL_STATUS	,
//	CASSETTE1_MAP_STATUS	,
	CASSETTE1_WAFER01		,
	CASSETTE1_WAFER02		,
	CASSETTE1_WAFER03		,
	CASSETTE1_WAFER04		,
	CASSETTE1_WAFER05		,
	CASSETTE1_WAFER06		,
	CASSETTE1_WAFER07		,
	CASSETTE1_WAFER08		,
	CASSETTE1_WAFER09		,
	CASSETTE1_WAFER10		,
	CASSETTE1_WAFER11		,
	CASSETTE1_WAFER12		,
	CASSETTE1_WAFER13		,
	CASSETTE1_WAFER14		,
	CASSETTE1_WAFER15		,
	CASSETTE1_WAFER16		,
	CASSETTE1_WAFER17		,
	CASSETTE1_WAFER18		,
	CASSETTE1_WAFER19		,
	CASSETTE1_WAFER20		,
	CASSETTE1_WAFER21		,
	CASSETTE1_WAFER22		,
	CASSETTE1_WAFER23		,
	CASSETTE1_WAFER24		,
	CASSETTE1_WAFER25		,
	CASSETTE1_WAFER26		,

//	CASSETTE2_MAPL_STATUS	,
//	CASSETTE2_MAP_STATUS	,
	CASSETTE2_WAFER01		,
	CASSETTE2_WAFER02		,
	CASSETTE2_WAFER03		,
	CASSETTE2_WAFER04		,
	CASSETTE2_WAFER05		,
	CASSETTE2_WAFER06		,
	CASSETTE2_WAFER07		,
	CASSETTE2_WAFER08		,
	CASSETTE2_WAFER09		,
	CASSETTE2_WAFER10		,
	CASSETTE2_WAFER11		,
	CASSETTE2_WAFER12		,
	CASSETTE2_WAFER13		,
	CASSETTE2_WAFER14		,
	CASSETTE2_WAFER15		,
	CASSETTE2_WAFER16		,
	CASSETTE2_WAFER17		,
	CASSETTE2_WAFER18		,
	CASSETTE2_WAFER19		,
	CASSETTE2_WAFER20		,
	CASSETTE2_WAFER21		,
	CASSETTE2_WAFER22		,
	CASSETTE2_WAFER23		,
	CASSETTE2_WAFER24		,
	CASSETTE2_WAFER25		,
	CASSETTE2_WAFER26		,

//	CASSETTE3_MAPL_STATUS	,
//	CASSETTE3_MAP_STATUS	,
	CASSETTE3_WAFER01		,
	CASSETTE3_WAFER02		,
	CASSETTE3_WAFER03		,
	CASSETTE3_WAFER04		,
	CASSETTE3_WAFER05		,
	CASSETTE3_WAFER06		,
	CASSETTE3_WAFER07		,
	CASSETTE3_WAFER08		,
	CASSETTE3_WAFER09		,
	CASSETTE3_WAFER10		,
	CASSETTE3_WAFER11		,
	CASSETTE3_WAFER12		,
	CASSETTE3_WAFER13		,
	CASSETTE3_WAFER14		,
	CASSETTE3_WAFER15		,
	CASSETTE3_WAFER16		,
	CASSETTE3_WAFER17		,
	CASSETTE3_WAFER18		,
	CASSETTE3_WAFER19		,
	CASSETTE3_WAFER20		,
	CASSETTE3_WAFER21		,
	CASSETTE3_WAFER22		,
	CASSETTE3_WAFER23		,
	CASSETTE3_WAFER24		,
	CASSETTE3_WAFER25		,
	CASSETTE3_WAFER26		,

    FM_RB_VAC_STATUS         ,

	CTC_BM1_Wafer_Status 	,
	CTC_BM1_Wafer_Status2 	,

	CTC_BM2_Wafer_Status 	,
	CTC_BM2_Wafer_Status2 	,

	TMC_LCASSDOOR1_CONTROL	,
	TMC_LCASSDOOR2_CONTROL	,

	CM1_MAPWAFER_COUNT		,
	CM2_MAPWAFER_COUNT		,
	CM3_MAPWAFER_COUNT		,

	CM1_MAPWAFER_INFO		,
	CM2_MAPWAFER_INFO		,
	CM3_MAPWAFER_INFO		,
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
