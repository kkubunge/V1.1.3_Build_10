#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	SW_GbIntlckDM,
	SW_ModuleFailDM,
	PLC_GbIntlckDI,
	DN_CommStsDI,
	VRCP_RunningDI,
	DPRunDI,
	
	RefillSysAlmDI,
	RefillSysAlm2DI,
	RefillSysAlm3DI,
	RefillStatusDM,
	RefillSrcNoDM,

	SrcLvHighLmtDI,
	Src2LvHighLmtDI,
	Src3LvHighLmtDI,

	SrcLvHighDI,
	Src2LvHighDI,
	Src3LvHighDI,

	SrcLvLowDI,
	Src2LvLowDI,
	Src3LvLowDI,

	SrcLvLowLmtDI,
	Src2LvLowLmtDI,
	Src3LvLowLmtDI,

	RefillReadyDI,
	RefillReady2DI,
	RefillReady3DI,

	RefillReqDO,
	RefillReq2DO,
	RefillReq3DO,

	SrcCanisterAI,
	Src2CanisterAI,
	Src3CanisterAI,

	PRMD_Refill_Opt,
	PRMD_Refill_Level,
	PRMA_TO_RefillPump,
	PRMA_TO_Refill,
	PRMA_SP_RefillPer,
	PRMA_VS_Ref_SrcLow,				// 20

	ABORT_GAS,

	V14ValveDO,		//... 2014.07.22

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
