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

	IO_Driver_Loding		,
	//============================

	ULVAC_AWS_RESULT_A		,
	ULVAC_AWS_RESULT_B		,
	ULVAC_AWS_RESULT_C		,
	ULVAC_AWS_RESULT_D		,

	ULVAC_AWS_RESULT_A_X	,
	ULVAC_AWS_RESULT_A_Y	,
	ULVAC_AWS_RESULT_B_X	,
	ULVAC_AWS_RESULT_B_Y	,
	ULVAC_AWS_RESULT_C_X	,
	ULVAC_AWS_RESULT_C_Y	,
	ULVAC_AWS_RESULT_D_X	,
	ULVAC_AWS_RESULT_D_Y	,

	ULVAC_AWS_READ_A		,
	ULVAC_AWS_READ_B		,
	ULVAC_AWS_READ_C		,
	ULVAC_AWS_READ_D		,

	ULVAC_AWS_READ_A_X		,
	ULVAC_AWS_READ_A_Y		,
	ULVAC_AWS_READ_B_X		,
	ULVAC_AWS_READ_B_Y		,
	ULVAC_AWS_READ_C_X		,
	ULVAC_AWS_READ_C_Y		,
	ULVAC_AWS_READ_D_X		,
	ULVAC_AWS_READ_D_Y		,

	ULVAC_AWS_LIMIT_WARN_LO	,
	ULVAC_AWS_LIMIT_WARN_HI	,

	//[ 2009/06/01 AWS Datalogging시 어느 Chamber에서 Pick한 Wafer인지 Logging추가 ( BY LJH )
	TM_Pick_U_Arm_Station			,
	TM_Pick_L_Arm_Station			,
	//]
	//[ added 2009/05/28 TM Robot Slide data svid 등록
	PMA_SLIDE_A1_X_AXIS		,
	PMA_SLIDE_A1_Y_AXIS		,
	PMA_SLIDE_A2_X_AXIS		,
	PMA_SLIDE_A2_Y_AXIS		,
                  
	PMA_SLIDE_B1_X_AXIS		,
	PMA_SLIDE_B1_Y_AXIS		,
	PMA_SLIDE_B2_X_AXIS		,
	PMA_SLIDE_B2_Y_AXIS		,
                  
	PMB_SLIDE_A1_X_AXIS		,
	PMB_SLIDE_A1_Y_AXIS		,
	PMB_SLIDE_A2_X_AXIS		,
	PMB_SLIDE_A2_Y_AXIS		,
                  
	PMB_SLIDE_B1_X_AXIS		,
	PMB_SLIDE_B1_Y_AXIS		,
	PMB_SLIDE_B2_X_AXIS		,
	PMB_SLIDE_B2_Y_AXIS		,
                  
	PMC_SLIDE_A1_X_AXIS		,
	PMC_SLIDE_A1_Y_AXIS		,
	PMC_SLIDE_A2_X_AXIS		,
	PMC_SLIDE_A2_Y_AXIS		,
                  
	PMC_SLIDE_B1_X_AXIS		,
	PMC_SLIDE_B1_Y_AXIS		,
	PMC_SLIDE_B2_X_AXIS		,
	PMC_SLIDE_B2_Y_AXIS		,
                  
	BM1_SLIDE_A1_X_AXIS		,
	BM1_SLIDE_A1_Y_AXIS		,
	BM1_SLIDE_A2_X_AXIS		,
	BM1_SLIDE_A2_Y_AXIS		,
                  
	BM1_SLIDE_B1_X_AXIS		,
	BM1_SLIDE_B1_Y_AXIS		,
	BM1_SLIDE_B2_X_AXIS		,
	BM1_SLIDE_B2_Y_AXIS		,
                  
	BM2_SLIDE_A1_X_AXIS		,
	BM2_SLIDE_A1_Y_AXIS		,
	BM2_SLIDE_A2_X_AXIS		,
	BM2_SLIDE_A2_Y_AXIS		,
                  
	BM2_SLIDE_B1_X_AXIS		,
	BM2_SLIDE_B1_Y_AXIS		,
	BM2_SLIDE_B2_X_AXIS		,
	BM2_SLIDE_B2_Y_AXIS		,
	//]
	//[ 2010/10/26 TM RB AWS Abort Range PM / LL 분리 ( BY LJH )
	ULVAC_AWS_LIMIT_HI		,
	ULVAC_AWS_LIMIT_LOW		,
	ULVAC_AWS_LIMIT_PM_HI	,
	ULVAC_AWS_LIMIT_BM_HI	,
	ULVAC_AWS_LIMIT_PM_LOW	,
	ULVAC_AWS_LIMIT_BM_LOW	,
	//]

	
	ULVAC_AWS_USE				,
	ULVAC_AWS_USE_V				,
	ULVAC_AWS_LIMIT_LOW_V		,
	ULVAC_AWS_LIMIT_HI_V		,

//	Run_Resume_Scenario			,

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#endif
