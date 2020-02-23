#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_Driver_Loding,
	CONTROL				,
	IsoControlType		,

	BM1_SlotVvLXO		,
	BM1_SlotVvRXO		,
	BM2_SlotVvLXO		,
	BM2_SlotVvRXO		,
	PM1_SlotValve1XO	,
	PM1_SlotValve2XO	,
	PM2_SlotValve1XO	,
	PM2_SlotValve2XO	,
	PM3_SlotValve1XO	,
	PM3_SlotValve2XO	,

	BM1_SlotVvLXI		,
	BM1_SlotVvRXI		,
	BM2_SlotVvLXI		,
	BM2_SlotVvRXI		,
	PM1_SlotValve1XI	,
	PM1_SlotValve2XI	,
	PM2_SlotValve1XI	,
	PM2_SlotValve2XI	,
	PM3_SlotValve1XI	,
	PM3_SlotValve2XI	,

// MAHA BB Modify 2014.03.10 by hyuk
	BM_WaferSlideL1DI	,
	BM_WaferSlideR1DI   ,

	PM1_WaferSlide1DI	,
	PM1_WaferSlide2DI   ,
	PM2_WaferSlide1DI   ,
	PM2_WaferSlide2DI   ,
	PM3_WaferSlide1DI   ,
	PM3_WaferSlide2DI	,
	
	TM_VacRbRet_ARMADI	,
	TM_VacRbRet_ARMBDI	,
	TM_VacRbRet_ARMCDI	,
	TM_VacRbRet_ARMDDI	,
// MAHA BB Modify 2014.03.10 by hyuk

	BM_Gate_UseNoUse	,
	PM1_Gate_UseNoUse	,
	PM2_Gate_UseNoUse	,
	PM3_Gate_UseNoUse	,

	PRMD_OVER_PRES_OPT	,
	TM_OverPresVvDO		,
	TM_RoughVvXO		,
	TM_ATMVACSensorXI	,
	TM_CGChmPrsAI		,

	BM1_OverPresVvDO	,
	BM1_RoughVvXO		,
	BM1_ATMVACSensorXI	,
	BM2_OverPresVvDO	,
	BM2_RoughVvXO		,
	BM2_ATMVACSensorXI	,

	ISO_OPCL_TIMEOUT	,
	SENSOR_CHECK		,

	LOT_CONTROL			,	
	LOT2_CONTROL		,
	LOT3_CONTROL		,

	CTC_EVENT_LOG_SVR,				// 2013.11.15

	//... 2015.09.28 Add Slot Valve OPEN Pressure Check & Swap Purge
	FC_TM_SV_OPEN_CHECK,

	PRMA_SV_LLOpPrsAtm,
	PRMA_SV_TMOpPrsAtm,
	PRMA_SV_PMOpPrsAtm,

	PRMA_SV_LLOpPrsLow,
	PRMA_SV_TMOpPrsLow,
	PRMA_SV_PMOpPrsLow,

	PRMA_SV_LLOpPrsHigh,
	PRMA_SV_TMOpPrsHigh,
	PRMA_SV_PMOpPrsHigh,

	PRMA_SV_LLOpDiffPrs,
	PRMA_SV_PMOpDiffPrs,

	PRMD_SV_LLPrsChkTMO,
	PRMD_SV_TMPrsChkTMO,
	PRMD_SV_PMPrsChkTMO,

	PRMA_SV_LLOpDelay,
	PRMA_SV_PMOpDelay,

	PRMA_SV_LLOpDelay2,
	PRMA_SV_PMOpDelay2,

	PM1_ChmPrsAI,
	PM2_ChmPrsAI,
	PM3_ChmPrsAI,

	PM1_M651_Pressure,
	PM2_M651_Pressure,
	PM3_M651_Pressure,

	BM1_CGChmPrsAI,
	BM2_CGChmPrsAI,

	PM1_ABORT_GAS,
	PM2_ABORT_GAS,
	PM3_ABORT_GAS,
	//... 2016.02.10  Open Time
	BM1_Slit_OPTm_AM,
	BM2_Slit_OPTm_AM,	
	PM1_Slit_OPTm_AM,	
	PM2_Slit_OPTm_AM,	
	PM3_Slit_OPTm_AM,	
	//... Close Time
	BM1_Slit_CLTm_AM,	
	BM2_Slit_CLTm_AM,	
	PM1_Slit_CLTm_AM,	
	PM2_Slit_CLTm_AM,	
	PM3_Slit_CLTm_AM,

	M_SVlv_ParmSaveDM,
	M_SVlv_CycTgtMdlDM,

	M_SVlv_CycTgtCntAM,
	M_SVlv_CycCurCntAM,

	M_SVlvBM1_OP_MinTmAM,
	M_SVlvBM2_OP_MinTmAM,
	M_SVlvPM1_OP_MinTmAM,
	M_SVlvPM2_OP_MinTmAM,
	M_SVlvPM3_OP_MinTmAM,

	M_SVlvBM1_OP_MaxTmAM,
	M_SVlvBM2_OP_MaxTmAM,
	M_SVlvPM1_OP_MaxTmAM,
	M_SVlvPM2_OP_MaxTmAM,
	M_SVlvPM3_OP_MaxTmAM,

	M_SVlvBM1_OP_AvgTmAM,
	M_SVlvBM2_OP_AvgTmAM,
	M_SVlvPM1_OP_AvgTmAM,
	M_SVlvPM2_OP_AvgTmAM,
	M_SVlvPM3_OP_AvgTmAM,

	M_SVlvBM1_CL_MinTmAM,
	M_SVlvBM2_CL_MinTmAM,
	M_SVlvPM1_CL_MinTmAM,
	M_SVlvPM2_CL_MinTmAM,
	M_SVlvPM3_CL_MinTmAM,

	M_SVlvBM1_CL_MaxTmAM,
	M_SVlvBM2_CL_MaxTmAM,
	M_SVlvPM1_CL_MaxTmAM,
	M_SVlvPM2_CL_MaxTmAM,
	M_SVlvPM3_CL_MaxTmAM,

	M_SVlvBM1_CL_AvgTmAM,
	M_SVlvBM2_CL_AvgTmAM,
	M_SVlvPM1_CL_AvgTmAM,
	M_SVlvPM2_CL_AvgTmAM,
	M_SVlvPM3_CL_AvgTmAM,

	CTC_AbortControl,
	BM1_VacuumStatusDM,
	BM2_VacuumStatusDM,
	PM1_PRO_RunStsDM,	
	PM2_PRO_RunStsDM,	
	PM3_PRO_RunStsDM,	
	PM1_SW_GbIntlckDM,	
	PM2_SW_GbIntlckDM,	
	PM3_SW_GbIntlckDM,	
	M_SVlv_Chk_PrsDM,
	M_SVlv_StartStopDM,
	CTC_PM1_CommStatus,
	CTC_PM2_CommStatus,	
	CTC_PM3_CommStatus,	
	//...
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
