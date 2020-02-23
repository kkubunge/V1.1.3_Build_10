#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {

	EasyCluster_Connect,

	SW_GbIntlckDM,
	SW_ModuleFailDM,
	PRO_FlowControlDM,
	//... 2016.08.20
	SysLeakChkDM,
	PRO_RunStsDM,
	//...
	MFC01_UseDM,
	MFC02_UseDM,
	MFC03_UseDM,
	MFC04_UseDM,
	MFC05_UseDM,
	MFC06_UseDM,
	MFC07_UseDM,
	MFC08_UseDM,
	MFC09_UseDM,

	MFC01_MonOptDM,
	MFC02_MonOptDM,
	MFC03_MonOptDM,
	MFC04_MonOptDM,
	MFC05_MonOptDM,
	MFC06_MonOptDM,
	MFC07_MonOptDM,
	MFC08_MonOptDM,
	MFC09_MonOptDM,

	PRMA_TL_MFC01PrsHi,
	PRMA_TL_MFC02PrsHi,
	PRMA_TL_MFC03PrsHi,
	PRMA_TL_MFC04PrsHi,
	PRMA_TL_MFC05PrsHi,
	PRMA_TL_MFC06PrsHi,
	PRMA_TL_MFC07PrsHi,
	PRMA_TL_MFC08PrsHi,
	PRMA_TL_MFC09PrsHi,

	PRMA_TL_MFC01PrsLw,
	PRMA_TL_MFC02PrsLw,
	PRMA_TL_MFC03PrsLw,
	PRMA_TL_MFC04PrsLw,
	PRMA_TL_MFC05PrsLw,
	PRMA_TL_MFC06PrsLw,
	PRMA_TL_MFC07PrsLw,
	PRMA_TL_MFC08PrsLw,
	PRMA_TL_MFC09PrsLw,

	MFC01PressAI,
	MFC02PressAI,
	MFC03PressAI,
	MFC04PressAI,
	MFC05PressAI,
	MFC06PressAI,
	MFC07PressAI,
	MFC08PressAI,
	MFC09PressAI,

	MFC01_FontVlv,
	MFC02_FontVlv,
	MFC03_FontVlv,
	MFC04_FontVlv,
	MFC05_FontVlv,
	MFC06_FontVlv,
	MFC07_FontVlv,
	MFC08_FontVlv,
	MFC09_FontVlv,

	PRMA_EC_MFC01,
	PRMA_EC_MFC02,
	PRMA_EC_MFC03,
	PRMA_EC_MFC04,
	PRMA_EC_MFC05,
	PRMA_EC_MFC06,
	PRMA_EC_MFC07,
	PRMA_EC_MFC08,
	PRMA_EC_MFC09,
	PRMA_EC_MFC10,
	
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
