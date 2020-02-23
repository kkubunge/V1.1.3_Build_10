#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	SW_GbIntlckDM,
	SW_ModuleFailDM,

	PRO_RunStsDM,
	PRO_FlowControlDM,		//... 2015.02.07 Add Retry for RPG Alarm

	ChmPrsAI,
	VacuumStateDM,

	StgHtr1RelayDI,
	StgHtr2RelayDI,
	GLHtrRelayDI,
	GLHtrRelay2DI,

	StgHtr1RelayDO,
	StgHtr2RelayDO,
	GLHtrRelayDO,
	GLHtrRelay2DO,

	VRCP_StaStpDO,

	M651_CommStatus,
	DN_CommStsDI,

	// 2006.03.02
	FastRoughVlvDO,
	RoughValveDI,

	RPGPlasmaDetectDI,
	RPGPwOnDO,
	PRMA_EC_RPGPlsmDtc,
	
	RPGRefPowAI,
	PRMA_TL_RPGRefl,
	PRMA_EC_RPGFwPw,
	RPG_UseDM,
	
	RPGGateVlvPosDI,
	RPGGateVlv2PosDI,
	RPGGateVv1DO,
	RPGGateVv2DO,
	PRMA_TO_RPGGateVv,

	ABORT_GAS,
	AUTO_MFC_VERIFY_CTR, //2014.04.25
	//... 2019.01.17
	AirCoolingVlv,
	PRMD_HO_ChuckCoolPrs,
	ChuckCoolingOut1Prs,
	ChuckCoolingOut2Prs,
	PRMA_COOLPRS_HIGH,
	PRMA_COOLPRS_LOW,
	//...
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
