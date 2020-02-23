#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	F_SCHEDULER			,
	S_SCHEDULER			,
	GEM_CONTROL			,
	PORT_CONTROL		,
	MID_NAME			,
	JID_READ			,
	RECIPE_FILE			,
	START_SLOT			,
	END_SLOT			,
	LOOP_COUNT			,
	IN_PATH				,
	OUT_PATH			,
	PORT_STATUS			,
	CONTROL_STATE		,
	STARTOPTION			, // jelee Added 2005.07.25
	OPERATOR_START3		,

	PRM_System_WFR_Check,
	CM3_C01_Wafer		,
	CM3_C02_Wafer		,
	CM3_C03_Wafer		,
	CM3_C04_Wafer		,
	CM3_C05_Wafer		,
	CM3_C06_Wafer		,
	CM3_C07_Wafer		,
	CM3_C08_Wafer		,
	CM3_C09_Wafer		,
	CM3_C10_Wafer		,
	CM3_C11_Wafer		,
	CM3_C12_Wafer		,
	CM3_C13_Wafer		,
	CM3_C14_Wafer		,
	CM3_C15_Wafer		,
	CM3_C16_Wafer		,
	CM3_C17_Wafer		,
	CM3_C18_Wafer		,
	CM3_C19_Wafer		,
	CM3_C20_Wafer		,
	CM3_C21_Wafer		,
	CM3_C22_Wafer		,
	CM3_C23_Wafer		,
	CM3_C24_Wafer		,
	CM3_C25_Wafer		,

	//2010.08.05 Wafer Sync Confirm
	WfrSyncConfirm		,
	ManualTRConfirm		,

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
