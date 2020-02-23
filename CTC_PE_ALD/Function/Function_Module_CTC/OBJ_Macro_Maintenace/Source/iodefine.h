#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_Driver_Loding,

    SCHEDULER,
	SCHEDULER2,
	PM_CommStatus,
	AutoPMEventName,
	AutoPMMacroName,
	CTC_MAIN_CONTROL,
	CTC_MAIN_CONTROL2,
	CTC_MAIN_CONTROL3,
	CTC_MAIN_CONTROL4,

	SCHEDULER_MAINT_PM,

	CHAMBER_CON_CTL_PM,
	CHAMBER_CON_ABORT_PM,

	PRMD_PreMEn,

	PRMS_PreMRcp_01, 
	PRMS_PreMRcp_02, 

 	PRMS_PreMRcp2_01, 	 
 	PRMS_PreMRcp2_02, 	 

	PRMD_PreMRcp_01, 
	PRMD_PreMRcp_02, 

	PRMD_PreMRcp2_01, 	 
 	PRMD_PreMRcp2_02, 	 

	PRMD_PostMEn,

	PRMS_PostMRcp_01, 
	PRMS_PostMRcp_02, 

	PRMS_PostMRcp2_01, 
	PRMS_PostMRcp2_02, 	 

	PRMD_PostMRcp_01, 
	PRMD_PostMRcp_02, 

	PRMD_PostMRcp2_01, 
	PRMD_PostMRcp2_02, 	 
	
	PRMD_ALC_Use_After_PreM,
	PRMD_ALC_Use_PostM,
	PRMD_MFCveriUse,

    PLCInkUnlockStsDO,   
    PLCInkRvrStsDO,      
    PLCInkUnlockStsDI,   
    PLCInkRvrStsDI,     
	ChmPrsAI,
	M651_Pressure,
	RoughValveDI,
	//RoughValve2DI,
	PRMA_VS_PrsBaRough,
	CTC_AutoPM_SAVE,
 	ABORT_PROCESS,	 
	PRMD_AutoPMFlw,

	ChmPrssDI,
	PRMA_VS_PrsBaVent,

	PRMD_PreMacroFlw,
	PRMD_PostMacroFlw,

	PRO_DryClnStsDM,
	Pre_PM_Resumed,
	Post_PM_Resumed,
	PREVENTIVE,
	ReserveDI,
	PMJobStart,
	MacroName_01,
	MacroName_02,
	MacroName_03,
	MacroName_04,
	MacroName_05,
	MacroName_06,
	AutoPMRcpName,
	AutoPMOwner,
	WaferDI,
	TopLidSwitchDI,
	PRMD_AUTOPM_POST_LEAK,
	MANUAL_SSP,
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
