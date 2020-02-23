#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_Driver_Loding		,

	TM_VACUUM_CONTROL		,
	RBLOW_CONTROL			,
	FMRBLOW_CONTROL			,
	BM1_VAC_CONTROL			,
	BM2_VAC_CONTROL			,
//	MAINBM_VAC_CONTROL		,
	CM1_CONTROLL			,
	CM2_CONTROLL			,
	CM3_CONTROLL			,

	INIT_CONTROL			,
	CONFIG_LOADING			,
	TM_M_CONTROL			,
	FM_M_CONTROL			,
	BM1M_CONTROL			,
	BM2M_CONTROL			,
//	MAINBMM_CONTROL			,
	CM1M_CONTROL			,
	CM2M_CONTROL			,
	CM3M_CONTROL			,
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
