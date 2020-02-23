#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_DRIVER_LOAD		,
		
	SCHEDULER			,

	MAIN_STATUS			,
	MAIN_STATUS2		,
	MAIN_STATUS3		,

	MAIN_CONTROL		,
	MAIN_CONTROL2		,
	MAIN_CONTROL3		,

	MDL_STATUS			,
	MDL_STATUS2			,
	MDL_STATUS3			,

	CM1_LOAD_BUTTON		,
	CM2_LOAD_BUTTON		,
	CM3_LOAD_BUTTON		,

	CM1_UNLOAD_BUTTON	,
	CM2_UNLOAD_BUTTON	,
	CM3_UNLOAD_BUTTON	,
	
	CM1_CASS_STATUS		,
	CM2_CASS_STATUS		,
	CM3_CASS_STATUS		,

	CM1_CLAMP_STATUS	,
	CM2_CLAMP_STATUS	,
	CM3_CLAMP_STATUS	,

	CM1_DOCK_STATUS		,
	CM2_DOCK_STATUS		,
	CM3_DOCK_STATUS		,

	CM1_OPEN_STATUS		,
	CM2_OPEN_STATUS		,
	CM3_OPEN_STATUS		,

	CM1_AARM_Status		,
	CM2_AARM_Status		,
	CM3_AARM_Status		,

	CM1_ALARM_LAMP		,
	CM2_ALARM_LAMP		,
	CM3_ALARM_LAMP		,

	CM1_AMHS_Status		,
	CM2_AMHS_Status		,
	CM3_AMHS_Status		,

	CM1_AUTO_LAMP		,
	CM2_AUTO_LAMP		,
	CM3_AUTO_LAMP		,

	CM1_RESERVE_Status	,
	CM2_RESERVE_Status	,
	CM3_RESERVE_Status	,

	CM1_RESERVE_LAMP	,
	CM2_RESERVE_LAMP	,
	CM3_RESERVE_LAMP	,

	CM1_LOADING_SIGNAL	,
	CM2_LOADING_SIGNAL	,
	CM3_LOADING_SIGNAL	,

	CM1_LOAD_LAMP		,
	CM2_LOAD_LAMP		,
	CM3_LOAD_LAMP		,

	CM1_UNLOADING_SIGNAL,
	CM2_UNLOADING_SIGNAL,
	CM3_UNLOADING_SIGNAL,

	CM1_UNLOAD_LAMP		,
	CM2_UNLOAD_LAMP		,
	CM3_UNLOAD_LAMP		,
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif

