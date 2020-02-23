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
	OPERATOR_START		,

	PRM_System_WFR_Check,
	CM1_C01_Wafer		,
	CM1_C02_Wafer		,
	CM1_C03_Wafer		,
	CM1_C04_Wafer		,
	CM1_C05_Wafer		,
	CM1_C06_Wafer		,
	CM1_C07_Wafer		,
	CM1_C08_Wafer		,
	CM1_C09_Wafer		,
	CM1_C10_Wafer		,
	CM1_C11_Wafer		,
	CM1_C12_Wafer		,
	CM1_C13_Wafer		,
	CM1_C14_Wafer		,
	CM1_C15_Wafer		,
	CM1_C16_Wafer		,
	CM1_C17_Wafer		,
	CM1_C18_Wafer		,
	CM1_C19_Wafer		,
	CM1_C20_Wafer		,
	CM1_C21_Wafer		,
	CM1_C22_Wafer		,
	CM1_C23_Wafer		,
	CM1_C24_Wafer		,
	CM1_C25_Wafer		,

	//2012.06.02 Wafer Sync Confirm
	WfrSyncConfirm		,
	ManualTRConfirm		,

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
