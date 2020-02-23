#ifndef __ENUMDEFINE_H___
#define __ENUMDEFINE_H___

typedef enum {
	EO_OFF = 0,
	EO_ON
} EOffOnEnum; 

typedef enum 
{
	NO = 0,
	YES
} ENoYesEnum;

typedef enum {
	ENU_NotUse = 0,
	ENU_Use
} ENotUseUseEnum;

typedef enum {
	EMFC_SetPoint = 0,
	EMFC_Open,
	EMFC_Close
} EMFCCmdEnum;

typedef enum {
	EVlv_Close= 0,
	EVlv_Open
} CloseOpenEnum;

typedef enum 
{
	Transition	= 0,
	Vacuum, 
	ATM, 
	Error
}PrssSnsEnum;

typedef enum {
	EM6_StopRun = 0,
	EM6_Open, 
	EM6_Close
} EM651Vlv;

typedef enum 
{
	PUMPING,	
	PUMPED,
	PUMP_ABORTED,
	VENTING,
	VENTED,
	VENT_ABORTED
} PMVacStateEnum;
typedef enum
{
	SHIMADZU= 0, 
	EDWARD,
	ALCATEL
}DPTypeEnum;

typedef enum 
{
	DP_STOPPED,
	SHIMADZU_DP_RUNNING = 1,
	EDWARD_DP_RUNNING = 4
} DPStatusTypeEnum;

typedef enum 
{
	DPSTS_STOPPED = 0,
	DPSTS_RUNNING
} DPStatusEnum;

typedef enum
{
	DP_Unknown	= 0,
	DP_Start,
	DP_Stop,
	DP_Error
} DPStSpEnum;

typedef enum 
{
	MB_STOPPED = 0,
	MB_RUNNING
}DPMBStsEnum;

typedef enum 
{	
	MB_STOP = 0,
	MB_START = 1
} DPMBCmdEnum;

typedef enum
{
	ACV_OFF = 0,
	ACV_ON
} ACVPosStsEnum;

typedef enum
{
	ACV_MOVETO_PROCESS_POSITION = 0,
	ACV_MOVETO_CLEAN_POSITION
} ACVCmdEnum;

typedef enum
{
	LC_STARTED = 0,
	LC_STEP1,
	LC_STEP2,
	LC_STEP3,
	LC_STEP4,
	LC_STEP5,
	LC_STEP6,
	LC_STEP7,
	LC_STEP8,
	LC_STEP9,
	LC_STEP10,
	LC_COMPLETED,
	LC_ERROR
} LeakCheckStsEnum;

#endif
