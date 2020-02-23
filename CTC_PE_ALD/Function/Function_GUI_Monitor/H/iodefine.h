#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_Driver_Loding			,

	Bottom_Operate				,

	Bottom_Config				,

	Bottom_Maint1				,
	Bottom_Maint2				,

	Config_CMC					,
	Config_TMC					,
	Config_TMC_Nav				,
	Config_PM1					,
	Config_PM2					,
	Config_PM3					,
	Config_PM4					,
	Config_PM5					,

	Right_Utility_Prev			,
	Right_Utility_Prev_Nav		,
	Right_Utility_Prev2			,
	Right_Utility_Prev2_Nav		,
	Right_Utility_Prev3			,
	Right_Utility_Prev3_Nav		,
	
	PM1_PREVENTIVE_NEXT			,
	PM2_PREVENTIVE_NEXT			,
	PM3_PREVENTIVE_NEXT			,
	PM4_PREVENTIVE_NEXT			,
	PM5_PREVENTIVE_NEXT			,
	
	PM1_PREVENTIVE_PREV			,
	PM2_PREVENTIVE_PREV			,
	PM3_PREVENTIVE_PREV			,
	PM4_PREVENTIVE_PREV			,
	PM5_PREVENTIVE_PREV			,

	TM_Config_Next				,
	TM_Config_Prev				,

	LotPre_Config_Next			,
	LotPre_Config_Prev			,

	Config_LotPre				,
	Config_LotPre_Nav			,

	Right_Service2_PM_Heater1	,
	Right_Service2_PM_Heater1_Nav	,
	Right_Service2_PM_Heater2	,
	Right_Service2_PM_Heater2_Nav	,
	Right_Service2_PM_Heater3,
	Right_Service2_PM_Heater3_Nav	,
	Right_Service2_PM_Heater4	,
	Right_Service2_PM_Heater4_Nav	,
	Right_Service2_PM_Heater5	,
	Right_Service2_PM_Heater5_Nav	,
	
	PM1_Heater_Next				,
	PM2_Heater_Next				,
	PM3_Heater_Next				,
	PM4_Heater_Next				,
	PM5_Heater_Next				,
	
	PM1_Heater_Prev				,
	PM2_Heater_Prev				,
	PM3_Heater_Prev				,
	PM4_Heater_Prev				,
	PM5_Heater_Prev				,

	PM1_GasFlow_Config_Nav		,
	PM1_GasFlow_Config_Next		,
	PM1_GasFlow_Config_Prev		,

	PM1_Moving_Config_Nav		,
	PM1_Moving_Config_Next		,
	PM1_Moving_Config_Prev		,

	PM2_GasFlow_Config_Nav		,
	PM2_GasFlow_Config_Next		,
	PM2_GasFlow_Config_Prev		,

	PM2_Moving_Config_Nav		,
	PM2_Moving_Config_Next		,
	PM2_Moving_Config_Prev		,

	PM3_GasFlow_Config_Nav		,
	PM3_GasFlow_Config_Next		,
	PM3_GasFlow_Config_Prev		,

	PM3_Moving_Config_Nav		,
	PM3_Moving_Config_Next		,
	PM3_Moving_Config_Prev		,

	PM4_GasFlow_Config_Nav		,
	PM4_GasFlow_Config_Next		,
	PM4_GasFlow_Config_Prev		,

	PM4_Moving_Config_Nav		,
	PM4_Moving_Config_Next		,
	PM4_Moving_Config_Prev		,

	PM5_GasFlow_Config_Nav		,
	PM5_GasFlow_Config_Next		,
	PM5_GasFlow_Config_Prev		,

	PM5_Moving_Config_Nav		,
	PM5_Moving_Config_Next		,
	PM5_Moving_Config_Prev		,

	TMC_Status					,
	PM1_Status					,
	PM2_Status					,
	PM3_Status					,
	PM4_Status					,
	PM5_Status					,
	EFEM_Status					,

	Operate_Status   			,
	Operate_Status_Nav   		,

	Move_To_Maint_TMC		 	,
	Move_To_Status_TMC			,
	Move_To_Maint_PM1			,
	Move_To_Status_PM1			,
	Move_To_Maint_PM2			,
	Move_To_Status_PM2			,
	Move_To_Maint_PM3			,
	Move_To_Status_PM3			,
	Move_To_Maint_PM4			,
	Move_To_Status_PM4			,
	Move_To_Maint_PM5			,
	Move_To_Status_PM5			,

	Right_Service1_TMC_Vacuum	,
	Right_Service1_TMC_FEM		,
	Right_Service2_PM_Main		,
	Right_Service2_PM_Main_Nav	,

	Navi_TM						,
	Navi_PM1					,
	Navi_PM2					,
	Navi_PM3					,
	Navi_PM4					,
	Navi_PM5					,
	Navi_CM1					,
	Navi_CM2					,
	Navi_CM3					,
	Navi_FEM					,
	Right_Direct_Navigation		,
	
	Service_Old					,

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif





