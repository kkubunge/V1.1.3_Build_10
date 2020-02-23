#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_Driver_Loding			,

	PM1_Gate_Status				,
	PM2_Gate_Status				,
	PM3_Gate_Status				,
	PM4_Gate_Status				,
	PM5_Gate_Status				,
//	PM6_Gate_Status				,

	PM1_Wafer_Status			,
	PM2_Wafer_Status			,
	PM3_Wafer_Status			,
	PM4_Wafer_Status			,
	PM5_Wafer_Status			,
//	PM6_Wafer_Status			,

	ATM_Status_ForeDry			,
	ATM_Status_RoughDry			,

	TM_ForeCONPressure			,
	TM_RoughCONPressure			,

	ForeDRP_Status				,
	RoughDRP_Status				,

	MPM1_Gate_Status			,
	MPM2_Gate_Status			,
	MPM3_Gate_Status			,
	MPM4_Gate_Status			,
	MPM5_Gate_Status			,
//	MPM6_Gate_Status			,

	MPM1_Wafer_Status			,
	MPM2_Wafer_Status			,
	MPM3_Wafer_Status			,
	MPM4_Wafer_Status			,
	MPM5_Wafer_Status			,
//	MPM6_Wafer_Status			,

	MPM1_ForeDRY1_ATM_Status	,
	MPM2_ForeDRY1_ATM_Status	,
	MPM3_ForeDRY1_ATM_Status	,
	MPM4_ForeDRY1_ATM_Status	,
	MPM5_ForeDRY1_ATM_Status	,
//	MPM6_ForeDRY1_ATM_Status	,

	MPM1_RoughDRY2_ATM_Status	,
	MPM2_RoughDRY2_ATM_Status	,
	MPM3_RoughDRY2_ATM_Status	,
	MPM4_RoughDRY2_ATM_Status	,
	MPM5_RoughDRY2_ATM_Status	,
//	MPM6_RoughDRY2_ATM_Status	,

	MPM1_ForeDRY1_Pressure		,
	MPM2_ForeDRY1_Pressure		,
	MPM3_ForeDRY1_Pressure		,
	MPM4_ForeDRY1_Pressure		,
	MPM5_ForeDRY1_Pressure		,
//	MPM6_ForeDRY1_Pressure		,

	MPM1_RoughDRY2_Pressure		,
	MPM2_RoughDRY2_Pressure		,
	MPM3_RoughDRY2_Pressure		,
	MPM4_RoughDRY2_Pressure		,
	MPM5_RoughDRY2_Pressure		,
//	MPM6_RoughDRY2_Pressure		,

	MPM1_ForeDRY1Pump_Status	,
	MPM2_ForeDRY1Pump_Status	,
	MPM3_ForeDRY1Pump_Status	,
	MPM4_ForeDRY1Pump_Status	,
	MPM5_ForeDRY1Pump_Status	,
//	MPM6_ForeDRY1Pump_Status	,

	MPM1_RoughDRY2Pump_Status	,
	MPM2_RoughDRY2Pump_Status	,
	MPM3_RoughDRY2Pump_Status	,
	MPM4_RoughDRY2Pump_Status	,
	MPM5_RoughDRY2Pump_Status	,
//	MPM6_RoughDRY2Pump_Status	,

	RUN_CONTROL					,

	MAIN_CONTROL				,
	MAIN_CONTROL2				,

	PM1_Rotaion_Speed			,
	PM2_Rotaion_Speed			,
	PM3_Rotaion_Speed			,
	PM4_Rotaion_Speed			,
	PM5_Rotaion_Speed			,
//	PM6_Rotaion_Speed			,

	SCHEDULER_MAINT_PM1			,
	SCHEDULER_MAINT_PM2			,
	SCHEDULER_MAINT_PM3			,
	SCHEDULER_MAINT_PM4			,
	SCHEDULER_MAINT_PM5			,
//	SCHEDULER_MAINT_PM6			,

	PRM_HEATEROFF_TIME			,

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif




