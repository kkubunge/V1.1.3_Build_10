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
	OPERATOR_START2		,
	PRM_System_WFR_Check,
	CM2_C01_Wafer		,
	CM2_C02_Wafer		,
	CM2_C03_Wafer		,
	CM2_C04_Wafer		,
	CM2_C05_Wafer		,
	CM2_C06_Wafer		,
	CM2_C07_Wafer		,
	CM2_C08_Wafer		,
	CM2_C09_Wafer		,
	CM2_C10_Wafer		,
	CM2_C11_Wafer		,
	CM2_C12_Wafer		,
	CM2_C13_Wafer		,
	CM2_C14_Wafer		,
	CM2_C15_Wafer		,
	CM2_C16_Wafer		,
	CM2_C17_Wafer		,
	CM2_C18_Wafer		,
	CM2_C19_Wafer		,
	CM2_C20_Wafer		,
	CM2_C21_Wafer		,
	CM2_C22_Wafer		,
	CM2_C23_Wafer		,
	CM2_C24_Wafer		,
	CM2_C25_Wafer		,

	//2010.08.05 Wafer Sync Confirm
	WfrSyncConfirm		,
	ManualTRConfirm		,

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
