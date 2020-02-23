#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_Driver_Loding	,

	SCHEDULER			,
	
	MID_CONTROL			,

	LP_LOADING_SIGNAL	,
	LP_UNLOADING_SIGNAL	,
	LP_PROCESS_END		,
	LP_OPERATOR_CANCEL	,
	LP_AMHS_MOVING		,
	LP_MDL_STATUS		,
	
	LP_AGV_OUTPUT_HOAVBL,
	LP_AGV_OUTPUT_ESTOP	,
	LP_AGV_OUTPUT_LREQ	,
	LP_AGV_OUTPUT_UREQ	,
	LP_AGV_OUTPUT_READY	,
	
	LP_AGV_INPUT_CS		,
	LP_AGV_INPUT_CS1	,
	LP_AGV_INPUT_VALID	,
	LP_AGV_INPUT_TRREQ	,
	LP_AGV_INPUT_BUSY	,
	LP_AGV_INPUT_COMPLETE	,
	LP_AGV_INPUT_CONT	,

// jelee Added Start.
	AMHS_TP1TIMEOUT			,
	AMHS_TP2TIMEOUT			,
	AMHS_TP3TIMEOUT			,
	AMHS_TP4TIMEOUT			,
	AMHS_TP5TIMEOUT			,
	AMHS_TP6TIMEOUT			,
// jelee Added End.

	LP_MODE_AGV_SET		,
	LP_MODE_AGV_STS		,

	LP_AGV_STS			,
	LP_AGV_ERROR		,

	PIO_ERR_CLEAR		,

	LP_FOUP_STS			,
	PRESENT_STS			, // jelee Added.
	
	LP_CLAMP			,
	LP_CLAMP_STS		,
	
	LP_LOAD_BUTTON		,
	LP_UNLOAD_BUTTON	,
	
	LP_UNLOAD_LAMP		,
	LP_LOAD_LAMP		,

// jelee Added Start.
    EVENT_SERVICE		,
	GEM_LoadPortState	,
	Light_Curtain		,
// jelee Added End.
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
