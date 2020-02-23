#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_Driver_Loding,
	SCHEDULER,
	S_SCHEDULER,
	GEM_CONTROL,

	CM1_MAIN_CONTROL,
	CM2_MAIN_CONTROL,
	CM3_MAIN_CONTROL,
	CM4_MAIN_CONTROL,

	FC_CM1_MID_CONTROL,
	FC_CM2_MID_CONTROL,
	FC_CM3_MID_CONTROL,

	CTC_JID_READ,
	CTC_JID_READ2,
	CTC_JID_READ3,
	CTC_JID_READ4,

	CTC_RECIPE_FILE,
	CTC_RECIPE_FILE2,
	CTC_RECIPE_FILE3,

	CTC_MID_NAME,
	CTC_MID_NAME2,
	CTC_MID_NAME3,

	CM1_CASS_STATUS,
	CM2_CASS_STATUS,
	CM3_CASS_STATUS,

	CM1_CASS_SenSor_STATUS,
	CM2_CASS_SenSor_STATUS,
	CM3_CASS_SenSor_STATUS,

	CM1_CLAMP,
	CM2_CLAMP,
	CM3_CLAMP,

	CM1_CLAMP_STATUS,
	CM2_CLAMP_STATUS,
	CM3_CLAMP_STATUS,

	CM1_DOCK_STATUS,
	CM2_DOCK_STATUS,
	CM3_DOCK_STATUS,

	CM1_OPEN_STATUS,
	CM2_OPEN_STATUS,
	CM3_OPEN_STATUS,

	CM1_C01_Wafer,
	CM1_C02_Wafer,
	CM1_C03_Wafer,
	CM1_C04_Wafer,
	CM1_C05_Wafer,
	CM1_C06_Wafer,
	CM1_C07_Wafer,
	CM1_C08_Wafer,
	CM1_C09_Wafer,
	CM1_C10_Wafer,
	CM1_C11_Wafer,
	CM1_C12_Wafer,
	CM1_C13_Wafer,
	CM1_C14_Wafer,
	CM1_C15_Wafer,
	CM1_C16_Wafer,
	CM1_C17_Wafer,
	CM1_C18_Wafer,
	CM1_C19_Wafer,
	CM1_C20_Wafer,
	CM1_C21_Wafer,
	CM1_C22_Wafer,
	CM1_C23_Wafer,
	CM1_C24_Wafer,
	CM1_C25_Wafer,

	CM2_C01_Wafer,
	CM2_C02_Wafer,
	CM2_C03_Wafer,
	CM2_C04_Wafer,
	CM2_C05_Wafer,
	CM2_C06_Wafer,
	CM2_C07_Wafer,
	CM2_C08_Wafer,
	CM2_C09_Wafer,
	CM2_C10_Wafer,
	CM2_C11_Wafer,
	CM2_C12_Wafer,
	CM2_C13_Wafer,
	CM2_C14_Wafer,
	CM2_C15_Wafer,
	CM2_C16_Wafer,
	CM2_C17_Wafer,
	CM2_C18_Wafer,
	CM2_C19_Wafer,
	CM2_C20_Wafer,
	CM2_C21_Wafer,
	CM2_C22_Wafer,
	CM2_C23_Wafer,
	CM2_C24_Wafer,
	CM2_C25_Wafer,

	CM3_C01_Wafer,
	CM3_C02_Wafer,
	CM3_C03_Wafer,
	CM3_C04_Wafer,
	CM3_C05_Wafer,
	CM3_C06_Wafer,
	CM3_C07_Wafer,
	CM3_C08_Wafer,
	CM3_C09_Wafer,
	CM3_C10_Wafer,
	CM3_C11_Wafer,
	CM3_C12_Wafer,
	CM3_C13_Wafer,
	CM3_C14_Wafer,
	CM3_C15_Wafer,
	CM3_C16_Wafer,
	CM3_C17_Wafer,
	CM3_C18_Wafer,
	CM3_C19_Wafer,
	CM3_C20_Wafer,
	CM3_C21_Wafer,
	CM3_C22_Wafer,
	CM3_C23_Wafer,
	CM3_C24_Wafer,
	CM3_C25_Wafer,

	CTC_BM1_Wafer_Status,	
	CTC_BM1_Wafer_Status2,	
	CTC_BM1_Wafer_Status3,	
	CTC_BM1_Wafer_Status4,	

	CTC_BM2_Wafer_Status,	
	CTC_BM2_Wafer_Status2,	
	CTC_BM2_Wafer_Status3,	
	CTC_BM2_Wafer_Status4,	

	CTC_TA_Wafer_Status,	
	CTC_TA2_Wafer_Status,	
	CTC_TB_Wafer_Status,	
	CTC_TB2_Wafer_Status,	
		
	CTC_PM1_Wafer_Status,	
	CTC_PM1_Wafer_Status2,	

	CTC_PM2_Wafer_Status,	
	CTC_PM2_Wafer_Status2,	

	CTC_PM3_Wafer_Status,	
	CTC_PM3_Wafer_Status2,	

	CTC_FA_Wafer_Status,	
	CTC_FB_Wafer_Status,	
	CTC_FM_AL_Wafer_Status,	


	GEM_START_CTRL,
	GEM_START_CTRL2,
	GEM_START_CTRL3,

	CM1_START_SLOT,	
	CM1_END_SLOTT,		
	CM2_START_SLOT,		
	CM2_END_SLOTT,		
	CM3_START_SLOT,		
	CM3_END_SLOTT,		
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
