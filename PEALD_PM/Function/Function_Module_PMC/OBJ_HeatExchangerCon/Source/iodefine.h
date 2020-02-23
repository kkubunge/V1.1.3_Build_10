#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	SW_GbIntlckDM,		
	SW_ModuleFailDM,
	
	HTE_StaStpDIO,	
	HTE2_StaStpDIO,		
	HTE_CommStsDI,		
	HTE2_CommStsDI,	
	HTE_ChillFailDI,
	HTE2_ChillFailDI,
    HTE_WarningDI,	
	HTE2_WarningDI,	

	HTE_SetTempAIO,		
	HTE2_SetTempAIO,	
	HTE_ReadTempAI,		
	HTE2_ReadTempAI,	
	HTE_FlowRateAI,		
	HTE2_FlowRateAI,	

	HTE_StaStpDI,	
	HTE2_StaStpDI,		
	HTE_CtrlModeDI,		
	HTE2_CtrlModeDI,	
	HTE_TmpRngOvrDI,
	HTE2_TmpRngOvrDI,
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif


