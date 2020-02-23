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
	CTC_Alarm_Status		,
	CTC_Alarm_Happen		,

	// 2020. 01. 16 Signal Tower Comm Spec
	PRMD_SIGNALTOWER_OPT,

	PM1_SW_GbIntlckDM,
	PM2_SW_GbIntlckDM,
	PM3_SW_GbIntlckDM,

	PM1_CommStatus,
	PM2_CommStatus,
	PM3_CommStatus,

	// 2004.07.27 by Kornic Inform, for FA
	GEM_Port1_LotID,
	GEM_Port2_LotID,
	GEM_Port3_LotID,

	RED_SIGNAL_ONOFF		,
	GREEN_SIGNAL_ONOFF		,
	YELLOW_SIGNAL_ONOFF		,
	BLUE_SIGNAL_ONOFF		,
	ALARM_BUZZER_ONOFF		,

	RED_SIGNAL_Flash_ONOFF		,
	GREEN_SIGNAL_Flash_ONOFF		,
	YELLOW_SIGNAL_Flash_ONOFF		,
	BLUE_SIGNAL_Flash_ONOFF		,

	CONTROL_STATE           ,

	BM1_ATMVACSensorXI		,
	BM1_Vacuum_Status		,
	BM2_ATMVACSensorXI		,
	BM2_Vacuum_Status		,
	TM_ATMVACSensorXI		,
	TM_Vacuum_Status		,

	Pressure_CG_BM1			,
	F_PUMP_PRESSURE_BM1		,
	Pressure_CG_BM2			,
	F_PUMP_PRESSURE_BM2		,
	Pressure_CG_TM			,
	F_PUMP_PRESSURE_TM		,

	CONFIG_LOADING			,

	LOT_CONTROL			    ,
	LOT2_CONTROL			,
	LOT3_CONTROL		    ,
   //2014.10.14
	PM1_LOTID			    ,
	PM1_LOTID2				,
	PM2_LOTID			    ,
	PM2_LOTID2				,
	PM3_LOTID			    ,
	PM3_LOTID2				,

	CTC_TR_Clear		    ,

	SLOT_NUM			    ,
	SLOT_NUM2			    ,
	MAINT_STATION		    ,
//------------------------------------------------------------------------------------------
	CM1_C01_Wafer			,
	CM1_C02_Wafer			,
	CM1_C03_Wafer			,
	CM1_C04_Wafer			,
	CM1_C05_Wafer			,
	CM1_C06_Wafer			,
	CM1_C07_Wafer			,
	CM1_C08_Wafer			,
	CM1_C09_Wafer			,
	CM1_C10_Wafer			,
	CM1_C11_Wafer			,
	CM1_C12_Wafer			,
	CM1_C13_Wafer			,
	CM1_C14_Wafer			,
	CM1_C15_Wafer			,
	CM1_C16_Wafer			,
	CM1_C17_Wafer			,
	CM1_C18_Wafer			,
	CM1_C19_Wafer			,
	CM1_C20_Wafer			,
	CM1_C21_Wafer			,
	CM1_C22_Wafer			,
	CM1_C23_Wafer			,
	CM1_C24_Wafer			,
	CM1_C25_Wafer			,

	CM2_C01_Wafer			,
	CM2_C02_Wafer			,
	CM2_C03_Wafer			,
	CM2_C04_Wafer			,
	CM2_C05_Wafer			,
	CM2_C06_Wafer			,
	CM2_C07_Wafer			,
	CM2_C08_Wafer			,
	CM2_C09_Wafer			,
	CM2_C10_Wafer			,
	CM2_C11_Wafer			,
	CM2_C12_Wafer			,
	CM2_C13_Wafer			,
	CM2_C14_Wafer			,
	CM2_C15_Wafer			,
	CM2_C16_Wafer			,
	CM2_C17_Wafer			,
	CM2_C18_Wafer			,
	CM2_C19_Wafer			,
	CM2_C20_Wafer			,
	CM2_C21_Wafer			,
	CM2_C22_Wafer			,
	CM2_C23_Wafer			,
	CM2_C24_Wafer			,
	CM2_C25_Wafer			,

	CM3_C01_Wafer			,
	CM3_C02_Wafer			,
	CM3_C03_Wafer			,
	CM3_C04_Wafer			,
	CM3_C05_Wafer			,
	CM3_C06_Wafer			,
	CM3_C07_Wafer			,
	CM3_C08_Wafer			,
	CM3_C09_Wafer			,
	CM3_C10_Wafer			,
	CM3_C11_Wafer			,
	CM3_C12_Wafer			,
	CM3_C13_Wafer			,
	CM3_C14_Wafer			,
	CM3_C15_Wafer			,
	CM3_C16_Wafer			,
	CM3_C17_Wafer			,
	CM3_C18_Wafer			,
	CM3_C19_Wafer			,
	CM3_C20_Wafer			,
	CM3_C21_Wafer			,
	CM3_C22_Wafer			,
	CM3_C23_Wafer			,
	CM3_C24_Wafer			,
	CM3_C25_Wafer			,

	MID_NAME				,
	MID_NAME2				,
	MID_NAME3				,
	MID_READ				,
	MID_READ2				,
	MID_READ3				,

	JID_NAME				,
	JID_NAME2				,
	JID_NAME3				,
	JID_READ				,
	JID_READ2				,
	JID_READ3				,

	RECIPE_NAME				,
	RECIPE_NAME2			,
	RECIPE_NAME3			,

	// add 2006.06.01 by Jelee
	START_SLOT			,
	START_SLOT2			,
	START_SLOT3			,

	END_SLOT			,
	END_SLOT2			,
	END_SLOT3			,

	LOOP_COUNT			,
	LOOP_COUNT2			,
	LOOP_COUNT3			,

	CM1_CASS_STATUS			,
	CM2_CASS_STATUS			,
	CM3_CASS_STATUS			,

	CM1_CASS_SenSor_STATUS	,
	CM2_CASS_SenSor_STATUS	,
	CM3_CASS_SenSor_STATUS	,

	CM1_AGV_INPUT_VALID		,
	CM2_AGV_INPUT_VALID		,
	CM3_AGV_INPUT_VALID		,

	CM1_AGV_OUTPUT_LREQ		,
	CM1_AGV_OUTPUT_UREQ		,
	CM1_AGV_OUTPUT_READY	,

	CM2_AGV_OUTPUT_LREQ		,
	CM2_AGV_OUTPUT_UREQ		,
	CM2_AGV_OUTPUT_READY	,

	CM3_AGV_OUTPUT_LREQ		,
	CM3_AGV_OUTPUT_UREQ		,
	CM3_AGV_OUTPUT_READY	,

	CM1_DoorUpDownXI		,
	CM2_DoorUpDownXI		,
	CM3_DoorUpDownXI		,

	CM1_ClampUnclampXI		,
	CM2_ClampUnclampXI		,
	CM3_ClampUnclampXI		,

	Transfer_Status			,
	CTC_TR_FINGER			,
	TM_OverPresVvDO			,
	TM_RoughVvXO			,

	SCHEDULER				,

	CM1_OPERATOR_CANCEL		,
	CM2_OPERATOR_CANCEL		,
	CM3_OPERATOR_CANCEL		,

	CM1_PROCESS_END			,
	CM2_PROCESS_END			,
	CM3_PROCESS_END			,

	CM1_MAPWAFER_COUNT		,
	CM2_MAPWAFER_COUNT		,
	CM3_MAPWAFER_COUNT		,

	CM1_MAPWAFER_INFO		,
	CM2_MAPWAFER_INFO		,
	CM3_MAPWAFER_INFO		,

	PM1_Wafer_Source		,
	PM1_Wafer_Source2		,
	PM2_Wafer_Source		,
	PM2_Wafer_Source2		,
	PM3_Wafer_Source		,
	PM3_Wafer_Source2		,

	PM1_Wafer_Status		,
	PM1_Wafer_Status2		,
	PM2_Wafer_Status		,
	PM2_Wafer_Status2		,
	PM3_Wafer_Status		,
	PM3_Wafer_Status2		,

	PM1_MIDName				,
	PM1_MIDName2			,
	PM2_MIDName				,
	PM2_MIDName2			,
	PM3_MIDName				,
	PM3_MIDName2			,

	PM1_PRCS_WFR_NUMBER,
	PM1_PRCS_WFR_NUMBER2,
	PM2_PRCS_WFR_NUMBER,
	PM2_PRCS_WFR_NUMBER2,
	PM3_PRCS_WFR_NUMBER,
	PM3_PRCS_WFR_NUMBER2,

	CM1_RFID_MID_PAGE	,
	CM1_RFID_PAGE1		,
	CM1_RFID_PAGE2		,
	CM1_RFID_PAGE3		,
	CM1_RFID_PAGE4		,
	CM1_RFID_PAGE5		,
	CM1_RFID_PAGE6		,
	CM1_RFID_PAGE7		,
	CM1_RFID_PAGE8		,
	CM1_RFID_PAGE9		,
	CM1_RFID_PAGE10		,
	CM1_RFID_PAGE11		,
	CM1_RFID_PAGE12		,
	CM1_RFID_PAGE13		,
	CM1_RFID_PAGE14		,
	CM1_RFID_PAGE15		,
	CM1_RFID_PAGE16		,
	CM1_RFID_PAGE17		,

	CM2_RFID_MID_PAGE	,
	CM2_RFID_PAGE1		,
	CM2_RFID_PAGE2		,
	CM2_RFID_PAGE3		,
	CM2_RFID_PAGE4		,
	CM2_RFID_PAGE5		,
	CM2_RFID_PAGE6		,
	CM2_RFID_PAGE7		,
	CM2_RFID_PAGE8		,
	CM2_RFID_PAGE9		,
	CM2_RFID_PAGE10		,
	CM2_RFID_PAGE11		,
	CM2_RFID_PAGE12		,
	CM2_RFID_PAGE13		,
	CM2_RFID_PAGE14		,
	CM2_RFID_PAGE15		,
	CM2_RFID_PAGE16		,
	CM2_RFID_PAGE17		,

	CM3_RFID_MID_PAGE	,
	CM3_RFID_PAGE1		,
	CM3_RFID_PAGE2		,
	CM3_RFID_PAGE3		,
	CM3_RFID_PAGE4		,
	CM3_RFID_PAGE5		,
	CM3_RFID_PAGE6		,
	CM3_RFID_PAGE7		,
	CM3_RFID_PAGE8		,
	CM3_RFID_PAGE9		,
	CM3_RFID_PAGE10		,
	CM3_RFID_PAGE11		,
	CM3_RFID_PAGE12		,
	CM3_RFID_PAGE13		,
	CM3_RFID_PAGE14		,
	CM3_RFID_PAGE15		,
	CM3_RFID_PAGE16		,
	CM3_RFID_PAGE17		,

	PM1_MODULE			,
	PM2_MODULE			,
	PM3_MODULE			,

	PM1_CONTROL			,
	PM2_CONTROL			,
	PM3_CONTROL			,
	PM4_CONTROL			,
	PM5_CONTROL			,

	LPMA_AMHS_MODE      ,
	LPMB_AMHS_MODE      ,
	LPMC_AMHS_MODE      ,

	// add 2006.05.08 by ycchoi
	LPMA_ErrorStatusDI	,
	LPMB_ErrorStatusDI	,
	LPMC_ErrorStatusDI	,

	OPERATOR_START			,
	OPERATOR_START2			,
	OPERATOR_START3			,

	LPMA_AGV_OUTPUT_HOAVBL,
	LPMB_AGV_OUTPUT_HOAVBL,
	LPMC_AGV_OUTPUT_HOAVBL,
	PRMD_BUZZER_OPT,

} IOPointIndex;

#endif
