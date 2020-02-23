#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {

	PRMD_AL_RFChmOpt,
	PRMD_AL_RFChmOptLp, //... 2015.11.25
	SW_GbIntlckDM,
	SW_ModuleFailDM,

	RF1CommStsDI,
	RF2CommStsDI,
	
	RF1LoadCntAIO,
	RF2LoadCntAIO,

	RF1LoadTolAO,
	RF2LoadTolAO,

	PRMA_TL_RF1LdCnt,
	PRMA_TL_RF2LdCnt,

	//... 2015.11.25
	PRMA_TL_RF1LdCntLp,
	PRMA_TL_RF2LdCntLp,

	PRMA_TL_RF1LdPw,
	PRMA_TL_RF2LdPw,

	//... 2015.11.25
	PRMA_TL_RF1LdPwLp,
	PRMA_TL_RF2LdPwLp,

	RF1RefWrnRstDO,
	RF2RefWrnRstDO,

	RF1ReflCntAI,
	RF2ReflCntAI,
	
	RF1RefWrnThrsAO,
	RF2RefWrnThrsAO,
	
	RF1RefWrnTOAO,
	RF2RefWrnTOAO,
	
	PRMA_TL_RF1RefCnt,
	PRMA_TL_RF2RefCnt,

	//... 2015.11.25
	PRMA_TL_RF1RefCntLp,
	PRMA_TL_RF2RefCntLp,
	
	PRMA_TL_RF1RefPw,
	PRMA_TL_RF2RefPw,

	//... 2015.11.25
	PRMA_TL_RF1RefPwLp,
	PRMA_TL_RF2RefPwLp,
	
	PRMA_TL_RF1RefTm,
	PRMA_TL_RF2RefTm,
	
	//... 2015.11.25
	PRMA_TL_RF1RefTmLp,
	PRMA_TL_RF2RefTmLp,
	
	RF1RefFltRstDO,		// 2014.12.02
	RF2RefFltRstDO,		// 2014.12.02

	HF_END_DO_A,
	HF_END_DO_B,

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
