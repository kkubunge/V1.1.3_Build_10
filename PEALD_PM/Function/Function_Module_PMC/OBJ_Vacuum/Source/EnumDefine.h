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

typedef enum {
	RVlv_Unknown = 0,
	RVlv_Open,
	RVlv_Close,
	RVlv_Error
} RoughVlvStsEnum;

typedef enum
{
	Transition	= 0,
	ATM,
	Vacuum,
	Error
}PrssSnsEnum;

typedef enum {
	EM6_StopRun = 0,
	EM6_Open,
	EM6_Close
} EM651Vlv;

typedef enum
{
	PM_ATM,
	PM_VACUUM,
	GO_ATM,
	GO_VAC,
	ABORT
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
	LC_NOTRUNNING,
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
	LC_STARTED,
	LC_COMPLETED,
	LC_ERROR
} LeakCheckStsEnum;

typedef enum
{
	LeakCheck_Normal = 0,
	LeakCheck_Stop
} LeakCheckControlEnum;

//... 2015.03.23 Add Check Teach Mode for Safety PLC
typedef enum
{
	ePLCMODE_SYSRUN,
	ePLCMODE_TEACH
} PLCModeEnum;

//... 2015.03.24 Add Process, Vac Switch, Foreline Gauge Isolation Valve Auto OPEN
typedef enum
{
	eACT_PUMP,
	eACT_VENT
} PreChkActionEnum;

typedef enum
{
	eUnknown = 0, eLeakCheck = 1, eLnLeckCheck = 2, eALDLeakCheck = 3
} LeakCheckTypeEnum;

// 2014.10.27
enum { Vlv_CLOSE, Vlv_OPEN };
enum {eNORMAL, eNORMAL_LEAKCHK, eNormal_GASLINE, eNORMAL_ALDCHK, eNormal_OTHERLK };
//... 2016.01.19
enum { eNotDefine, eNormalLeakCheck, eGasLineLeckCheck, eALDValveLeakCheck, eOtherLeckCheck };
enum { ePURGELINE, eCLEANLINE, eRESET };
enum { eNotUse, eUse };
enum { eAr_Line1, eAr_Line2, eAr_Line3, eAr_Line4};
enum { eRelayOFF, eRelayON};
//... 2016.06.14
enum { eAr_1_9, eAr_6};
// 2020. 01. 22
enum {CTRL_OFF, CTRL_ON};
enum {RPG_Unknown, RPG_Open, RPG_Close, RPG_Error};
#endif
