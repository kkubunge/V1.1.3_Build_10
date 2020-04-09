#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "TextLogDll.h"
#include "AlarmDefine.h"
#include "EnumDefine.h"
#include "ModuleControl.h"
#include "ValveSeq.h"
#include "TextParser.h"

#include "LeakCheckHistory.h"

CModuleControl g_ModuleControl;
char APP_NAME[] = "VACUUM_CONTROL";

#define PMC_CONFIG_FILE		".\\Parameter\\PM_SEQ_PARM\\PMCConfig.ini"
BOOL g_bInit = FALSE;

//... 2016.01.19
#define MAX_LK_PRAMETER 4
#define MAX_LINE_LK_SEQ	2
#define MAX_AR_CHARGE_LINE 4
#define MAX_TOXIC_PURGE_LINE 2
//... 2016.05.13
#define MAX_PURGE_LINE	3
#define MAX_MFC	3

typedef struct
{
	double	dbChamberPressure;
	double	dbLCBasePressure;
	double	dbLCLimitPres;
	double	dbLCPumpingTO;
	double	dbLCDelayTime;
	double	dbLCMaxCount;
	double	dbLCCount;

	double	dbLastRate;
	double  dbAvgRate;
	double	dbAvgLeakRate;
	double  dbRghOpenAndLCDelayTime;
	double	dbRoRLimit;

	BOOL bLeakCheckStopped;
} sLeakcheckParaIO;

sLeakcheckParaIO g_LKParameter[MAX_LK_PRAMETER];
//... 2016.01.19

#define NorLeakChkDynMacro {eLeakCheck,CHUCK_CONTROL,"DYN_CYCLE","(Chuck Control)"}
#define LnLeakChkDynMacro {eLnLeckCheck,ALD_VLV_CTRL,"DYN_ALDTEST","(ALD Valve)"}

typedef struct
{
	int nLeakCheckType;
	int nFunctionIndex;
	char* pCmd;
	char* pAlarmDes;
}sDynamicParaIO;

sDynamicParaIO g_DynamicPara[] = {NorLeakChkDynMacro,LnLeakChkDynMacro};

//... 2016.05.12
#define MAX_LK_MACRO	2
#define MAX_MACRO_IO	2
typedef struct _LEAK_CHECK_MACRO
{
	int nMacroIndex;
	int nIOLeakChkSts;
} LEAK_CHECK_MACRO;

#define NormalLeakCheck	{eLeakCheck, ALECHK_NormDM}
#define GASLeakCheck	{eLnLeckCheck, ALECHK_GASLnDM}

LEAK_CHECK_MACRO	g_LeakCheckMacro[] = {NormalLeakCheck,GASLeakCheck};
//...

// 2014.12.02
#define	TV_FIRST_POSTION_FOR_PUMPING		3.5

//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "TMIsoVlvClosedDI",		_K_D_IO,	TMIsoVlvClosedDI,		0},

	{ "FC_VACUUM",				_K_D_IO,	FC_VACUUM,				0},
	{ "ChmPrsAI",				_K_A_IO,	ChmPrsAI,				0},
	{ "ChmPrssDI1",				_K_D_IO,	ChmPrssDI1,				0},
	{ "M651_Valve",				_K_D_IO,	M651_Valve,				0},
	{ "VacuumStateDM",			_K_D_IO,	VacuumStateDM,			0},
	{ "FastRoughVlvDO",			_K_D_IO,	FastRoughVlvDO,			0},
	{ "RoughValveDI",			_K_D_IO,	FastRoughVlvDI,			0},
	{ "DPRunDI",				_K_D_IO,	DPRunDI,				0},
	{ "DPDO",					_K_D_IO,	DPDO,					0},
	{ "PumpPrsAI",				_K_A_IO,	PumpPrsAI,				0},
	{ "MBStsDI",				_K_D_IO,	MBStsDI,				0},
	{ "MBCmdDO",				_K_D_IO,	MBCmdDO,				0},
	{ "PumpTime",				_K_A_IO,	PumpTime,				0},
	{ "M651_Pressure",			_K_A_IO,	M651_Pressure,			0},
	{ "M651_Position",			_K_A_IO,	M651_Position,			0},

	{ "V1ValveDO",				_K_D_IO	,	V1ValveDO,				0},
	{ "V2ValveDO",				_K_D_IO	,	V2ValveDO,				0},
	{ "V3ValveDO",				_K_D_IO	,	V3ValveDO,				0},
	{ "V4ValveDO",				_K_D_IO	,	V4ValveDO,				0},
	{ "V5ValveDO",				_K_D_IO	,	V5ValveDO,				0},
	{ "V6ValveDO",				_K_D_IO	,	V6ValveDO,				0},
	{ "V7ValveDO",				_K_D_IO	,	V7ValveDO,				0},
	{ "V8ValveDO",				_K_D_IO	,	V8ValveDO,				0},
	{ "V9ValveDO",				_K_D_IO	,	V9ValveDO,				0},
	{ "V10ValveDO",				_K_D_IO	,	V10ValveDO,				0},
	{ "V11ValveDO",				_K_D_IO	,	V11ValveDO,				0},
	{ "V12ValveDO",				_K_D_IO	,	V12ValveDO,				0},
	{ "V13ValveDO",				_K_D_IO	,	V13ValveDO,				0},
	{ "V14ValveDO",				_K_D_IO	,	V14ValveDO,				0},
	{ "V15ValveDO",				_K_D_IO	,	V15ValveDO,				0},
	{ "V16ValveDO",				_K_D_IO	,	V16ValveDO,				0},
	{ "V17ValveDO",				_K_D_IO	,	V17ValveDO,				0},
	{ "V18ValveDO",				_K_D_IO	,	V18ValveDO,				0},
	{ "V19ValveDO",				_K_D_IO	,	V19ValveDO,				0},
	{ "V20ValveDO",				_K_D_IO	,	V20ValveDO,				0},
	{ "V21ValveDO",				_K_D_IO	,	V21ValveDO,				0},
	{ "V22ValveDO",				_K_D_IO	,	V22ValveDO,				0},
	{ "V23ValveDO",				_K_D_IO	,	V23ValveDO,				0},
	{ "V24ValveDO",				_K_D_IO	,	V24ValveDO,				0},
	{ "V25ValveDO",				_K_D_IO	,	V25ValveDO,				0},
	{ "V26ValveDO",				_K_D_IO	,	V26ValveDO,				0},
	{ "V27ValveDO",				_K_D_IO	,	V27ValveDO,				0},
	{ "V28ValveDO",				_K_D_IO	,	V28ValveDO,				0},
	{ "V29ValveDO",				_K_D_IO	,	V29ValveDO,				0},
	{ "V30ValveDO",				_K_D_IO	,	V30ValveDO,				0},
	{ "V31ValveDO",				_K_D_IO	,	V31ValveDO,				0},
	{ "V32ValveDO",				_K_D_IO	,	V32ValveDO,				0},
	{ "V33ValveDO",				_K_D_IO	,	V33ValveDO,				0},
	{ "V34ValveDO",				_K_D_IO	,	V34ValveDO,				0},
	{ "V35ValveDO",				_K_D_IO	,	V35ValveDO,				0},
	{ "V36ValveDO",				_K_D_IO	,	V36ValveDO,				0},
	{ "V37ValveDO",				_K_D_IO	,	V37ValveDO,				0},
	{ "V38ValveDO",				_K_D_IO	,	V38ValveDO,				0},
	{ "V39ValveDO",				_K_D_IO	,	V39ValveDO,				0},
	{ "V40ValveDO",				_K_D_IO	,	V40ValveDO,				0},
	{ "V41ValveDO",				_K_D_IO	,	V41ValveDO,				0},
	{ "V42ValveDO",				_K_D_IO	,	V42ValveDO,				0},
	{ "V43ValveDO",				_K_D_IO	,	V43ValveDO,				0},
	{ "V44ValveDO",				_K_D_IO	,	V44ValveDO,				0},
	{ "V45ValveDO",				_K_D_IO	,	V45ValveDO,				0},
	{ "V46ValveDO",				_K_D_IO	,	V46ValveDO,				0},
	{ "V47ValveDO",				_K_D_IO	,	V47ValveDO,				0},
	{ "V48ValveDO",				_K_D_IO	,	V48ValveDO,				0},
	{ "V49ValveDO",				_K_D_IO	,	V49ValveDO,				0},
	{ "V50ValveDO",				_K_D_IO	,	V50ValveDO,				0},
	{ "V51ValveDO",				_K_D_IO	,	V51ValveDO,				0},
	{ "V52ValveDO",				_K_D_IO	,	V52ValveDO,				0},
	{ "V53ValveDO",				_K_D_IO	,	V53ValveDO,				0},
	{ "V54ValveDO",				_K_D_IO	,	V54ValveDO,				0},
	{ "V55ValveDO",				_K_D_IO	,	V55ValveDO,				0},
	{ "V56ValveDO",				_K_D_IO	,	V56ValveDO,				0},
	{ "V57ValveDO",				_K_D_IO	,	V57ValveDO,				0},
	{ "V58ValveDO",				_K_D_IO	,	V58ValveDO,				0},
	{ "V59ValveDO",				_K_D_IO	,	V59ValveDO,				0},
	{ "V60ValveDO",				_K_D_IO	,	V60ValveDO,				0},
	{ "V61ValveDO",				_K_D_IO	,	V61ValveDO,				0},
	{ "V62ValveDO",				_K_D_IO	,	V62ValveDO,				0},
	{ "V63ValveDO",				_K_D_IO	,	V63ValveDO,				0},
	{ "V64ValveDO",				_K_D_IO	,	V64ValveDO,				0},
	{ "V65ValveDO",				_K_D_IO	,	V65ValveDO,				0},
	{ "V66ValveDO",				_K_D_IO	,	V66ValveDO,				0},
	{ "V67ValveDO",				_K_D_IO	,	V67ValveDO,				0},
	{ "V68ValveDO",				_K_D_IO	,	V68ValveDO,				0},
	{ "V69ValveDO",				_K_D_IO	,	V69ValveDO,				0},
	{ "V70ValveDO",				_K_D_IO	,	V70ValveDO,				0},
	{ "V71ValveDO",				_K_D_IO	,	V71ValveDO,				0},
	{ "V72ValveDO",				_K_D_IO	,	V72ValveDO,				0},
	{ "V73ValveDO",				_K_D_IO	,	V73ValveDO,				0},
	{ "V74ValveDO",				_K_D_IO	,	V74ValveDO,				0},
	{ "V75ValveDO",				_K_D_IO	,	V75ValveDO,				0},
	{ "V76ValveDO",				_K_D_IO	,	V76ValveDO,				0},
	{ "V77ValveDO",				_K_D_IO	,	V77ValveDO,				0},
	{ "V78ValveDO",				_K_D_IO	,	V78ValveDO,				0},
	{ "V79ValveDO",				_K_D_IO	,	V79ValveDO,				0},
	{ "V80ValveDO",				_K_D_IO	,	V80ValveDO,				0},
	{ "V81ValveDO",				_K_D_IO	,	V81ValveDO,				0},
	{ "V82ValveDO",				_K_D_IO	,	V82ValveDO,				0},
	{ "V83ValveDO",				_K_D_IO	,	V83ValveDO,				0},
	{ "V84ValveDO",				_K_D_IO	,	V84ValveDO,				0},
	{ "V85ValveDO",				_K_D_IO	,	V85ValveDO,				0},
	{ "V86ValveDO",				_K_D_IO	,	V86ValveDO,				0},
	{ "V87ValveDO",				_K_D_IO	,	V87ValveDO,				0},
	{ "V88ValveDO",				_K_D_IO	,	V88ValveDO,				0},
	{ "V89ValveDO",				_K_D_IO	,	V89ValveDO,				0},
	{ "V90ValveDO",				_K_D_IO	,	V90ValveDO,				0},
	{ "V91ValveDO",				_K_D_IO	,	V91ValveDO,				0},
	{ "V92ValveDO",				_K_D_IO	,	V92ValveDO,				0},
	{ "V93ValveDO",				_K_D_IO	,	V93ValveDO,				0},
	{ "V94ValveDO",				_K_D_IO	,	V94ValveDO,				0},
	{ "V95ValveDO",				_K_D_IO	,	V95ValveDO,				0},
	{ "V96ValveDO",				_K_D_IO	,	V96ValveDO,				0},
	{ "V97ValveDO",				_K_D_IO	,	V97ValveDO,				0},
	{ "V98ValveDO",				_K_D_IO	,	V98ValveDO,				0},
	{ "V99ValveDO",				_K_D_IO	,	V99ValveDO,				0},
	{ "V100ValveDO",			_K_D_IO	,	V100ValveDO,			0},
	{ "V101ValveDO",			_K_D_IO	,	V101ValveDO,			0},
	{ "V102ValveDO",			_K_D_IO	,	V102ValveDO,			0},
	{ "V103ValveDO",			_K_D_IO	,	V103ValveDO,			0},
	{ "V104ValveDO",			_K_D_IO	,	V104ValveDO,			0},
	{ "V105ValveDO",			_K_D_IO	,	V105ValveDO,			0},
	{ "V106ValveDO",			_K_D_IO	,	V106ValveDO,			0},
	{ "V107ValveDO",			_K_D_IO	,	V107ValveDO,			0},
	{ "V108ValveDO",			_K_D_IO	,	V108ValveDO,			0},
	{ "V109ValveDO",			_K_D_IO	,	V109ValveDO,			0},
	{ "V110ValveDO",			_K_D_IO	,	V110ValveDO,			0},
	{ "V111ValveDO",			_K_D_IO	,	V111ValveDO,			0},
	{ "V112ValveDO",			_K_D_IO	,	V112ValveDO,			0},

	{ "LkChkStartPress1AM",		_K_A_IO,	LkChkStartPress1AM,		0},
	{ "LkChkStartPress2AM",		_K_A_IO,	LkChkStartPress2AM,		0},
	{ "LkChkStartPress3AM",		_K_A_IO,	LkChkStartPress3AM,		0},
	{ "LkChkStartPress4AM",		_K_A_IO,	LkChkStartPress4AM,		0},
	{ "LkChkStartPress5AM",		_K_A_IO,	LkChkStartPress5AM,		0},
	{ "LkChkStartPress6AM",		_K_A_IO,	LkChkStartPress6AM,		0},
	{ "LkChkStartPress7AM",		_K_A_IO,	LkChkStartPress7AM,		0},
	{ "LkChkStartPress8AM",		_K_A_IO,	LkChkStartPress8AM,		0},
	{ "LkChkStartPress9AM",		_K_A_IO,	LkChkStartPress9AM,		0},
	{ "LkChkStartPress10AM",	_K_A_IO,	LkChkStartPress10AM,	0},

	{ "LkChkEndPress1AM",		_K_A_IO,	LkChkEndPress1AM,		0},
	{ "LkChkEndPress2AM",		_K_A_IO,	LkChkEndPress2AM,		0},
	{ "LkChkEndPress3AM",		_K_A_IO,	LkChkEndPress3AM,		0},
	{ "LkChkEndPress4AM",		_K_A_IO,	LkChkEndPress4AM,		0},
	{ "LkChkEndPress5AM",		_K_A_IO,	LkChkEndPress5AM,		0},
	{ "LkChkEndPress6AM",		_K_A_IO,	LkChkEndPress6AM,		0},
	{ "LkChkEndPress7AM",		_K_A_IO,	LkChkEndPress7AM,		0},
	{ "LkChkEndPress8AM",		_K_A_IO,	LkChkEndPress8AM,		0},
	{ "LkChkEndPress9AM",		_K_A_IO,	LkChkEndPress9AM,		0},
	{ "LkChkEndPress10AM",		_K_A_IO,	LkChkEndPress10AM,		0},

	{ "LeakChkRate1AM",			_K_A_IO,	LeakChkRate1AM,			0},
	{ "LeakChkRate2AM",			_K_A_IO,	LeakChkRate2AM,			0},
	{ "LeakChkRate3AM",			_K_A_IO,	LeakChkRate3AM,			0},
	{ "LeakChkRate4AM",			_K_A_IO,	LeakChkRate4AM,			0},
	{ "LeakChkRate5AM",			_K_A_IO,	LeakChkRate5AM,			0},
	{ "LeakChkRate6AM",			_K_A_IO,	LeakChkRate6AM,			0},
	{ "LeakChkRate7AM",			_K_A_IO,	LeakChkRate7AM,			0},
	{ "LeakChkRate8AM",			_K_A_IO,	LeakChkRate8AM,			0},
	{ "LeakChkRate9AM",			_K_A_IO,	LeakChkRate9AM,			0},
	{ "LeakChkRate10AM",		_K_A_IO,	LeakChkRate10AM,		0},
	{ "LeakChkAvgAM",			_K_A_IO,	LeakChkAvgAM,			0},

	{ "LeakChkCountAM",			_K_A_IO,	LeakChkCountAM,			0},
	{ "LeakChkProgrssTimeSM",	_K_S_IO,	LeakChkProgrssTimeSM,	0},
	{ "LeakChkProgrssStepSM",	_K_S_IO,	LeakChkProgrssStepSM,	0},
	{ "LeakChkStartTimeSM",		_K_S_IO,	LeakChkStartTimeSM,		0},
	{ "LeakChkEndTimeSM",		_K_S_IO,	LeakChkEndTimeSM,		0},

	{ "BaseDelayTimeAM",		_K_A_IO,	BaseDelayTimeAM,		0},  //... 2016.01.19
	{ "LeakChkBasePressAM",		_K_A_IO,	LeakChkBasePressAM,		0},
	{ "LeakChkLimitPressAM",	_K_A_IO,	LeakChkLimitPressAM,	0},
	{ "LeakChkPumpTimeOutAM",	_K_A_IO,	LeakChkPumpTimeOutAM,	0},
	{ "LeakChkDelayTimeAM",		_K_A_IO,	LeakChkDelayTimeAM,		0},
	{ "LeakChkCountSPAM",		_K_A_IO,	LeakChkCountSPAM,		0},
	{ "LeakChkStsDM",			_K_D_IO,	LeakChkStsDM,			0},
	{ "LeakChkStopDM",			_K_D_IO,	LeakChkStopDM,			0},

	//2008.04.04
	{ "LeakChk_Log_Rate1"		, _K_A_IO	, Log_Rate1			,	0} ,
	{ "LeakChk_Log_Rate2"		, _K_A_IO	, Log_Rate2			,	0} ,
	{ "LeakChk_Log_Rate3"		, _K_A_IO	, Log_Rate3			,	0} ,
	{ "LeakChk_Log_Rate4"		, _K_A_IO	, Log_Rate4			,	0} ,
	{ "LeakChk_Log_Rate5"		, _K_A_IO	, Log_Rate5			,	0} ,
	{ "LeakChk_Log_Avg1"		, _K_A_IO	, Log_Avg1			,	0} ,
	{ "LeakChk_Log_Avg2"		, _K_A_IO	, Log_Avg2			,	0} ,
	{ "LeakChk_Log_Avg3"		, _K_A_IO	, Log_Avg3			,	0} ,
	{ "LeakChk_Log_Avg4"		, _K_A_IO	, Log_Avg4			,	0} ,
	{ "LeakChk_Log_Avg5"		, _K_A_IO	, Log_Avg5			,	0} ,
	{ "LeakChk_Log_Time1"       , _K_S_IO   , Log_Time1			,	0} ,
	{ "LeakChk_Log_Time2"       , _K_S_IO   , Log_Time2			,	0} ,
	{ "LeakChk_Log_Time3"       , _K_S_IO   , Log_Time3			,	0} ,
	{ "LeakChk_Log_Time4"       , _K_S_IO   , Log_Time4			,	0} ,
	{ "LeakChk_Log_Time5"       , _K_S_IO   , Log_Time5			,	0} ,

	{ "PRMA_VS_PrsBaRough",		_K_A_IO,	PRMA_VS_PrsBaRough,		0},
	{ "PRMD_AI_DPCheck",		_K_D_IO,	PRMD_AI_DPCheck,		0},
	{ "PRMA_TO_DryPumpOn",		_K_A_IO,	PRMA_TO_DryPumpOn,		0},
	{ "PRMA_VS_PrsBaPmping",	_K_A_IO,	PRMA_VS_PrsBaPmping,	0},
	{ "PRMA_TO_PmpLnPmping",	_K_A_IO,	PRMA_TO_PmpLnPmping,	0},
	{ "PRMA_TO_SlowRghPrs",		_K_A_IO,	PRMA_TO_SlowRghPrs,		0},
	{ "PRMA_TL_ThrtlPos",		_K_A_IO,	PRMA_TL_ThrtlPos,		0},
	{ "PRMA_TO_ThrtlPos",		_K_A_IO,	PRMA_TO_ThrtlPos,		0},
	{ "PRMA_TO_BaseRghPrs",		_K_A_IO,	PRMA_TO_BaseRghPrs,		0},
	{ "PRMD_HW_DPType",			_K_D_IO,	PRMD_HW_DPType,			0},
	{ "PRMA_WT_AfterFstRgh",	_K_A_IO,	PRMA_WT_AfterFstRgh,	0},
	{ "PRMA_VS_PrsBaVent",		_K_A_IO,	PRMA_VS_PrsBaVent,		0},
	{ "PRMA_VS_VCSnVlvOpPs",	_K_A_IO,	PRMA_VS_VCSnVlvOpPs,	0},
	{ "PRMA_TO_PressVent",		_K_A_IO,	PRMA_TO_PressVent,		0},
	{ "PRMA_AL_VentOKTemp",		_K_A_IO,	PRMA_AL_VentOKTemp,		0},
	{ "PRMA_WT_AfterVent",		_K_A_IO,	PRMA_WT_AfterVent,		0},
	{ "PRMA_VS_TrPrs",			_K_A_IO,	PRMA_VS_TrPrs,			0},
	//
	{ "PRESS_CONTROL",			_K_F_IO,	PRESS_CONTROL,			0},
	{ "ABORT_GAS",				_K_F_IO,	ABORT_GAS,				0},
	{ "MFC_CONTROL",			_K_F_IO,	MFC_CONTROL,			0},
	{ "STGHTR_CONTROL",			_K_F_IO,	STGHTR_CONTROL,			0},

	//... 2015.03.23 Add Check Teach Mode for Safety PLC
	{ "PLCSysRunModeDI",		_K_D_IO,	PLCSysRunModeDI,		0},
	{ "PLCTeachModeDI",			_K_D_IO,	PLCTeachModeDI,			0},

	//... 2015.03.24 Add Process, Vac Switch, Foreline Gauge Isolation Valve Auto OPEN
	{ "ProcBGIsoVlvDO",			_K_D_IO,	ProcBGIsoVlvDO,			0},		//... Proc Gauge Iso Vlv
	{ "VacSwIsoVlvDO",			_K_D_IO,	VacSwIsoVlvDO,			0},		//... Vac Switch Iso Vlv
	{ "PumplnCVGIsoVlvDO",		_K_D_IO,	PumplnCVGIsoVlvDO,		0},		//... Vac Switch Iso Vlv
	//...2015.09.11
	{ "ALDBaseDelayTimeAM",		_K_A_IO,	ALDBaseDelayTimeAM,		0},
	{ "ALDLkChkBasePressAM",	_K_A_IO,	ALDLkChkBasePressAM,	0},
	{ "ALDLkChkLimitPressAM",	_K_A_IO,	ALDLkChkLimitPressAM,		0},
	{ "ALDLkChkDelayTimeAM",	_K_A_IO,	ALDLkChkDelayTimeAM,	0},
	{ "ALDLkChkPumpTimeOutAM",	_K_A_IO,	ALDLkChkPumpTimeOutAM,	0},
	{ "ALDLkChkCountSPAM",		_K_A_IO,	ALDLkChkCountSPAM,		0},
	{ "LEAK_ModeDM",			_K_D_IO,	LEAK_ModeDM,			0},
	//...2016.01.19
	{ "PurgeLineLK_DM",			_K_D_IO,	PurgeLineLK_DM,			0},
	{ "CleanLineLK_DM",			_K_D_IO,	CleanLineLK_DM,			0},
	{ "SrcLineLK_DM",			_K_D_IO,	SrcLineLK_DM,			0},
	//... 2016.04.04
	{ "ALECHK_MTRDM",			_K_D_IO,	DyALECHK_MTRDM,			0},
	{ "ALECHK_VLVDM",			_K_D_IO,	DyALECHK_VLVDM,			0},
	{ "CHUCK_CONTROL",			_K_F_IO,	CHUCK_CONTROL,			0},
	{ "ALD_VLV_CTRL",			_K_F_IO,	ALD_VLV_CTRL,			0},
	//... Gas Line Leak check Parameter
	{ "LnBaseDelayTimeAM",		_K_A_IO,	LnBaseDelayTimeAM,		0},
	{ "LnLkChkBasePressAM",		_K_A_IO,	LnLkChkBasePressAM,		0},
	{ "LnLkChkLimitPressAM",	_K_A_IO,	LnLkChkLimitPressAM,	0},
	{ "LnLkChkDelayTimeAM",		_K_A_IO,	LnLkChkDelayTimeAM,		0},
	{ "LnLkChkPumpTimeOutAM",	_K_A_IO,	LnLkChkPumpTimeOutAM,	0},
	{ "LnLkChkCountSPAM",		_K_A_IO,	LnLkChkCountSPAM,		0},
	//...User Mode Leak Check Parameter
	{ "UrBaseDelayTimeAM",		_K_A_IO,	UrBaseDelayTimeAM,		0},
	{ "UrLkChkBasePressAM",		_K_A_IO,	UrLkChkBasePressAM,		0},
	{ "UrLkChkLimitPressAM",	_K_A_IO,	UrLkChkLimitPressAM,	0},
	{ "UrLkChkDelayTimeAM",		_K_A_IO,	UrLkChkDelayTimeAM,		0},
	{ "UrLkChkPumpTimeOutAM",	_K_A_IO,	UrLkChkPumpTimeOutAM,	0},
	{ "UrLkChkCountSPAM",		_K_A_IO,	UrLkChkCountSPAM,		0},

	{ "MFC01FlwAI",				_K_A_IO,	MFC01FlwAI,				0},
	{ "MFC02FlwAI",				_K_A_IO,	MFC02FlwAI,				0},
	{ "MFC03FlwAI",				_K_A_IO,	MFC03FlwAI,				0},
	{ "MFC04FlwAI",				_K_A_IO,	MFC04FlwAI,				0},
	{ "MFC05FlwAI",				_K_A_IO,	MFC05FlwAI,				0},
	{ "MFC06FlwAI",				_K_A_IO,	MFC06FlwAI,				0},
	{ "MFC07FlwAI",				_K_A_IO,	MFC07FlwAI,				0},
	{ "MFC08FlwAI",				_K_A_IO,	MFC08FlwAI,				0},
	{ "MFC09FlwAI",				_K_A_IO,	MFC09FlwAI,				0},

	{ "LeakChk_ParmSaveDM",		_K_D_IO,	LeakChk_ParmSaveDM,		0},
	{ "StgHtr1RelayDI",			_K_D_IO,	StgHtr1RelayDI,			0},
	{ "StgHtr2RelayDI",			_K_D_IO,	StgHtr2RelayDI,			0},

	{ "SysLeakChkRateAM",		_K_A_IO,	SysLeakChkRateAM,		0},
	{ "SysPurLkChkRateAM",		_K_A_IO,	SysPurLkChkRateAM,		0},
	{ "SysClnLkChkRateAM",		_K_A_IO,	SysClnLkChkRateAM,		0},

	{ "PRMA_TM_ToxicGasPurge",	_K_A_IO,	PRMA_TM_ToxicGasPurge,	0},
	{ "PRMA_TO_ArCharging",		_K_A_IO,	PRMA_TO_ArCharging,		0},
	//... 2016.05.12
	{ "LKMacroType1DM",			_K_D_IO,	LKMacroType1DM,			0},
	{ "LKMacroType2DM",			_K_D_IO,	LKMacroType2DM,			0},
	{ "ALECHK_NormDM",			_K_D_IO,	ALECHK_NormDM,			0},
	{ "ALECHK_GASLnDM",			_K_D_IO,	ALECHK_GASLnDM,			0},
	{ "MFC_PurgeTmAM",			_K_A_IO,	MFC_PurgeTmAM,			0},

	{ "MFC01PressAI",			_K_A_IO,	MFC01PressAI,			0},
	{ "MFC02PressAI",			_K_A_IO,	MFC02PressAI,			0},
	{ "MFC03PressAI",			_K_A_IO,	MFC03PressAI,			0},
	{ "MFC04PressAI",			_K_A_IO,	MFC04PressAI,			0},
	{ "MFC05PressAI",			_K_A_IO,	MFC05PressAI,			0},
	{ "MFC06PressAI",			_K_A_IO,	MFC06PressAI,			0},
	{ "MFC07PressAI",			_K_A_IO,	MFC07PressAI,			0},
	{ "MFC08PressAI",			_K_A_IO,	MFC08PressAI,			0},
	{ "MFC09PressAI",			_K_A_IO,	MFC09PressAI,			0},
	//... 2016.06.14
	{ "PRMA_TO_BeforeVent",		_K_A_IO,	PRMA_TO_BeforeVent,		0},
	{ "RRMD_VENT_SEQ",			_K_D_IO,	RRMD_VENT_SEQ,			0},
	//... 2016.08.16
	{ "SW_GbIntlckDM",			_K_D_IO,	SW_GbIntlckDM,			0},
	{ "RoRLimitAM",				_K_A_IO,	RoRLimitAM,				0},
	{ "LnRoRLimitAM",			_K_A_IO,	LnRoRLimitAM,			0},
	{ "ALDRoRLimitAM",			_K_A_IO,	ALDRoRLimitAM,			0},
	{ "UrRoRLimitAM",			_K_A_IO,	UrRoRLimitAM,			0},
	{ "PRMD_GATE_VALVE_OPT",	_K_D_IO,	PRMD_GATE_VALVE_OPT,	0},
	{ "RPGGateVlvPosDI",		_K_D_IO,	RPGGateVlvPosDI,		0},
	{ "RPGGateVlv2PosDI",		_K_D_IO,	RPGGateVlv2PosDI,		0},

	""
};

//------------------------------------------------------------------------------------------
double	g_dbBrGgMax			= 10;
BOOL	g_bDPBooster		= FALSE;
BOOL	g_bSafetyPLC_Use	= FALSE;		//... 2015.03.23 Add Check Teach Mode for Safety PLC

CValveSeq	g_ValveSeq;
//------------------------------------------------------------------------------------------
// 2005.02.23 by cskim

typedef struct _VENT_MFC
{
	int nMfcN;
	int nMfcFlow;
	int nValve1;
	int nValve2;
	int nValve3;
	int nValve4;    //2014.04.09
	int nValve5;
} VENT_MFC;

VENT_MFC g_VentMfc[5];
//... 2016.06.14
VENT_MFC g_VentMfc2[5];
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//==========================================================================================
//extern "C"
//{

int READ_DIGITAL_COMMCHECK(char * pszFtnName, int nIO_Enum, int * pnCommStatus)
{
	int nRetData = -1;
	int nTempData = -1;
	do
	{
		nTempData = READ_DIGITAL(nIO_Enum, pnCommStatus);
		if( FALSE == *pnCommStatus )
		{
			_LOG("READ_DIGITAL(%s) is failed", IO_STR_Map_Table[nIO_Enum].IO_Name);
			ALARM_POST(DigitCommErr);
			break;
		}
		nRetData = nTempData;
	}while(0);
	return nRetData;
}

void WRITE_DIGITAL_COMMCHECK(char * pszFtnName, int nIO_Enum, int nWrite_Value, int *pnCommStatus )
{
	do
	{
		WRITE_DIGITAL(nIO_Enum, nWrite_Value, pnCommStatus);
		if( FALSE == *pnCommStatus )
		{
			_LOG("WRITE_DIGITAL(%s) is failed", IO_STR_Map_Table[nIO_Enum].IO_Name);
			ALARM_POST(DigitCommErr);
			break;
		}

	}while(0);
	return;
}
//////////////////////////////////////////////////////////////////////////
//... 2015.03.23 Add Check Teach Mode for Safety PLC
BOOL CheckSafetyPLCMode(int nPLCMode)
{
	BOOL	bRet = FALSE;
	int		nIOStatus =0;
	int		nAlarmRecovery = ALM_CLEAR;

	while (1)
	{
		//... Not Use Safety PLC
		if(! g_bSafetyPLC_Use)
		{
			bRet = TRUE;
			break;
		}

		if(ePLCMODE_SYSRUN == nPLCMode)
		{	//... Check PLC Run MODE for Pump or Process
			if(EO_ON != READ_DIGITAL(PLCSysRunModeDI, &nIOStatus))
			{	//... Only Alarm Post
				ALARM_POST(ALARM_SAFETY_PLC_NOT_RUN_MODE);
				break;
			}
		}
		else if(ePLCMODE_TEACH == nPLCMode)
		{	//... Check PLC Teach MODE for Vent
			if(EO_ON != READ_DIGITAL(PLCTeachModeDI, &nIOStatus))
			{	//... Alarm Manager for Retry
				nAlarmRecovery = ALARM_MANAGE(ALARM_SAFETY_PLC_NOT_TEACH_MODE);
				if(ALM_RETRY == nAlarmRecovery)		continue;	//... Retry
				else								break;		//... Abort
			}
		}

		//... Not Delete. Fixed Result
		bRet = TRUE;
		break;
	}

	return	bRet;
}
//2007.07.18 by jhlee
Module_Status CheckRoughValveSts(RoughVlvStsEnum enSts)
{
	Module_Status nStatus = SYS_SUCCESS;
	int nIOSts;
    int nElapsedTime = 0;
	int nAlarmRecovery;
	RoughVlvStsEnum enCurSts;

	char pszLocFtnName[] = "CheckRoughValveSts";
	char pszFtnName[256];
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	while (1) {
		enCurSts = (RoughVlvStsEnum)READ_DIGITAL_COMMCHECK(pszFtnName , FastRoughVlvDI , &nIOSts);
		if (enSts == enCurSts)  break;
		if (!WAIT_SECONDS(0.1)) return SYS_ABORTED;

		nElapsedTime += 100;
		if (nElapsedTime >= 5000) {
			nAlarmRecovery  = ALARM_MANAGE(ALARM_ROUGH_VALVE_T_OUT);
			if (nAlarmRecovery != ALM_ABORT) {nElapsedTime = 0;}
			else return SYS_ABORTED;
		}
	}

	printf("Leaved %s(%d)\n", pszFtnName, nStatus);

	return nStatus;
}
//------------------------------------------------------------------------------------------

// 2004.12.27 by cskim
double GetChamberPressure()
{
	double fChmPres = 10;
	int nIOStatus;

	do {
		fChmPres = READ_ANALOG(M651_Pressure, &nIOStatus);
		if(fChmPres >= g_dbBrGgMax)
		{
			fChmPres  = READ_ANALOG(ChmPrsAI, &nIOStatus);
		}
	} while(0);

	return fChmPres;
}

BOOL IsRoughingNeeded()
{
	char pszLocFtnName[] = "IsRoughingNeeded";
	char pszFtnName[256];
	BOOL bRet = FALSE;
	int nIOStatus = 0;
	int nData     = -1;
	__KF_DECLARE_ALARM

	double fBaseRoughPressure = 0.002f;
	double fChmPres = 10;
	int		nPressureSense = ATM;
	int    nThrottleValveStatus = EM6_Close;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		fBaseRoughPressure = READ_ANALOG(PRMA_VS_PrsBaRough, &nIOStatus);
		fChmPres = GetChamberPressure();

		if( fChmPres <= fBaseRoughPressure)
		{
			nPressureSense = ATM;
			__KF_BEGIN_ALARM
			nPressureSense = READ_DIGITAL_COMMCHECK( pszFtnName, ChmPrssDI1, &nIOStatus);
			if( Vacuum != nPressureSense )
			{
				__SET_RECOVERY_CODE(ALARM_MANAGE(VACUUM_SENSOR_OR_GAUGE_FAIL))
				bRet = FALSE;
			}
			__KF_END_ALARM

			if( EM6_Open != READ_DIGITAL_COMMCHECK( pszFtnName, M651_Valve, &nIOStatus ))
			{
				bRet = TRUE;// Roughing is needed
			}
			else
				bRet = FALSE;// Roughing is Not Needed
		}
		else
			bRet = TRUE; // Roughing is needed
	}while(0);
	printf("Leave %s\n", pszFtnName);
	return bRet;
}

BOOL IsVentNeeded()
{
	char pszLocFtnName[] = "IsVentNeeded";
	char pszFtnName[256];
	BOOL bRet = FALSE;
	int nIOStatus = 0;
	int nData     = -1;
	__KF_DECLARE_ALARM

	double fBaseVentPressure = 760;
	double fChamberPressure = 0;
	int		nPressureSense = Vacuum;
	int    nThrottleValveStatus = EM6_Close;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		fChamberPressure = GetChamberPressure();

		// Retrieves 'fBaseVentPressure'  value from PM Configuration.
		//float fBaseVentPressure = 760;
		//m_pPMConfig->GetParamVal(m_strName,"Pressure_Base_Vent",fBaseVentPressure);
		fBaseVentPressure = READ_ANALOG(PRMA_VS_PrsBaVent, &nIOStatus);

		if(fChamberPressure >= fBaseVentPressure)
		{
			// Read pressure sense (i.e. ATM or VTM )
			nPressureSense = Vacuum;
			__KF_BEGIN_ALARM

			//ReadIOCValue(m_strName+".Pressure_Sense_XI",TYPE_ENUM,&enPressureSense);
			nPressureSense = READ_DIGITAL_COMMCHECK( pszFtnName, ChmPrssDI1, &nIOStatus);
			if(ATM != nPressureSense )
			{
				// (H/W problem of VaccuumSensor or VacuumGuage)
				//RaiseAlarm(CPMSequenceAlarm(m_strName+"_VACUUM_SENSOR_OR_GAUGE_FAIL"));
				__SET_RECOVERY_CODE(ALARM_MANAGE(VACUUM_SENSOR_OR_GAUGE_FAIL))
				bRet = FALSE;
			}
			__KF_END_ALARM
			bRet = FALSE;

		}
		else
		{// Venting needed.
			bRet = TRUE;
		}
	}while(0);
	printf("Leave %s\n", pszFtnName);
	return bRet;
}
Module_Status StartDryPump()
{
	char pszLocFtnName[] = "StartDryPump";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	int nData     = -1;


	__KF_DECLARE_ALARM
	int nTimeOut = 5;
	int nElapsedTime = 0;
	int nDPRunStatusValue = DPSTS_RUNNING;
	int nCurDPRunStatus   = DPSTS_STOPPED;
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		nData = READ_DIGITAL_COMMCHECK( pszFtnName, PRMD_AI_DPCheck, &nIOStatus);
		if( NO == nData )
		{
			msRet = SYS_SUCCESS;
			break;
		}

		nCurDPRunStatus = READ_DIGITAL_COMMCHECK( pszFtnName, DPRunDI, &nIOStatus);
		if( nDPRunStatusValue == nCurDPRunStatus )
		{
			_LOG("RunStatusValue[%d] == DPRunDI[%d]", nDPRunStatusValue, nCurDPRunStatus);
			msRet = SYS_SUCCESS;
			break;
		}

		__KF_BEGIN_ALARM
		nElapsedTime = 0;
		do
		{
			nTimeOut = (int)READ_ANALOG( PRMA_TO_DryPumpOn, &nIOStatus);
			nTimeOut = nTimeOut * 1000;
			WRITE_DIGITAL_COMMCHECK( pszFtnName, DPDO, DP_Start, &nIOStatus );
			_LOG("Write DPDO[%d]", DP_Start);
			if( FALSE == WAIT_SECONDS(1))
			{
				_LOG("Function is aborted" );
				msRet = SYS_ABORTED;
				ALARM_POST(VACUUM_CONTROL_FUNCTION_FAIL);
				__SET_RECOVERY_CODE(_ALM_ABORT)
				break;
			}
			nElapsedTime = nElapsedTime+1000;
			nCurDPRunStatus = READ_DIGITAL_COMMCHECK( pszFtnName, DPRunDI, &nIOStatus);
			if( nDPRunStatusValue == nCurDPRunStatus )
			{
				msRet = SYS_SUCCESS;
				break;
			}
			else if( nElapsedTime > nTimeOut )
			{
				__SET_RECOVERY_CODE(ALARM_MANAGE(DRY_PUMP_NOT_RUNNING));
				msRet = SYS_ABORTED;
				break;
			}
		}while(1);
		__KF_END_ALARM

		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leave %s\n", pszFtnName);
	return msRet;
}

// 2004.07.27 by cskim, Add 'DryPump Check Sequence'
BOOL CheckDryPump()
{
	BOOL bRet = TRUE;
	int nIOStatus;

	if(YES == READ_DIGITAL(PRMD_AI_DPCheck, &nIOStatus))
	{
		// DPRunDI => 0:Off 1:On
		if(READ_DIGITAL(DPRunDI, &nIOStatus) == 0)
		{
			printf("---> DryPump Down in CheckDryPump\n");
			ALARM_POST(DRY_PUMP_NOT_RUNNING);
			bRet = FALSE;
		}
	}
	return bRet;
}

Module_Status WaitTillRequiredPumpingPressure()
{
	char pszLocFtnName[] = "WaitTillRequiredPumpingPressure";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	int nData     = -1;

	__KF_DECLARE_ALARM
	double	fBasePumpingPressure	= 0.05f;
	double	fPumpingPressure		= 0.0f;
	double	fTimeOut				= 100;
	double	fElapsedTime			= 0;
	AlarmStyle eRecovery			= (AlarmStyle) _ALM_ABORT;
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		__KF_BEGIN_ALARM
		fElapsedTime		 = 0;
		fBasePumpingPressure = 0.05f;
		fTimeOut             = READ_ANALOG(PRMA_TO_PmpLnPmping, &nIOStatus);
		fTimeOut			 = fTimeOut*1000;
		fBasePumpingPressure = READ_ANALOG(PRMA_VS_PrsBaPmping, &nIOStatus);
		do
		{
			fPumpingPressure = READ_ANALOG(PumpPrsAI, &nIOStatus);
			if( fPumpingPressure <= fBasePumpingPressure)
			{
				msRet = SYS_SUCCESS;
				break;
			}
			if( FALSE == WAIT_SECONDS(0.5))
			{
				msRet = SYS_ABORTED;
				ALARM_POST(VACUUM_CONTROL_FUNCTION_FAIL);
				__SET_RECOVERY_CODE(_ALM_ABORT)
				break;
			}

			// 2004.07.27 by cskim
			if(CheckDryPump() == FALSE)
			{
				msRet = SYS_ABORTED;
				break;
			}

			fElapsedTime = fElapsedTime + 500;
			if( fElapsedTime > fTimeOut )
			{
				eRecovery = ALARM_MANAGE(PUMPING_LINES_NOT_PUMPED_DOWN);
				if( _ALM_RETRY == eRecovery )
				{
					// 2004.07.27 by cskim
					/*
					msRet = StartDryPump();
					if( SYS_SUCCESS != msRet )
						eRecovery = _ALM_ABORT;
					*/
				}
				__SET_RECOVERY_CODE(eRecovery)
				msRet = SYS_ABORTED;
				break;
			}
		}while(1);
		__KF_END_ALARM
		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leave %s\n", pszFtnName);
	return msRet;
}

Module_Status StartDPBooster()
{
	char pszLocFtnName[] = "StartDPBooster";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	int nData     = -1;
	int nElapsedTime = 0;
	__KF_DECLARE_ALARM
	int nCurMBRunStatus = MB_STOPPED;
	int nTimeOut		= 0;

	// Booster Use Check
	if(g_bDPBooster == FALSE)
	{
		msRet = SYS_SUCCESS;
		return msRet;
	}

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		// check DP Check Option
		nData = READ_DIGITAL_COMMCHECK( pszFtnName, PRMD_AI_DPCheck, &nIOStatus);
		if( NO == nData )
		{
			msRet = SYS_SUCCESS;
			break;
		}

		nData = READ_DIGITAL_COMMCHECK( pszFtnName, PRMD_HW_DPType, &nIOStatus);
		if( EDWARD == nData || ALCATEL == nData)
		{
			msRet = SYS_SUCCESS;
			break;
		}

		// Coding is needed MB_Status_SDI, MB_StartStop_SDO가 현재 없음.
		nCurMBRunStatus = READ_DIGITAL_COMMCHECK( pszFtnName, MBStsDI, &nIOStatus);
		if( MB_RUNNING == nCurMBRunStatus )
		{
			msRet = SYS_SUCCESS;
			break;
		}

		__KF_BEGIN_ALARM
		nElapsedTime = 0;
		do
		{
			// 2004.07.27 by cskim
			if(CheckDryPump() == FALSE)
			{
				msRet = SYS_ABORTED;
				break;
			}

			nTimeOut = (int)READ_ANALOG(PRMA_TO_DryPumpOn, &nIOStatus);
			nTimeOut = nTimeOut * 1000;
			WRITE_DIGITAL_COMMCHECK( pszFtnName, MBCmdDO, MB_START, &nIOStatus );
			if( FALSE == WAIT_SECONDS(1))
			{
				_LOG("Function is aborted");
				msRet = SYS_ABORTED;
				ALARM_POST(VACUUM_CONTROL_FUNCTION_FAIL);
				__SET_RECOVERY_CODE(_ALM_ABORT)
				break;
			}
			nElapsedTime = nElapsedTime+1000;
			nCurMBRunStatus = READ_DIGITAL_COMMCHECK( pszFtnName, MBStsDI, &nIOStatus);
			if( MB_RUNNING == nCurMBRunStatus )
			{
				msRet = SYS_SUCCESS;
				break;
			}
			else if( nElapsedTime > nTimeOut )
			{
				__SET_RECOVERY_CODE(ALARM_MANAGE(DP_BOOSTER_NOT_RUNNING));
				msRet = SYS_ABORTED;
				break;
			}
		}while(1);
		__KF_END_ALARM


		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leave %s\n", pszFtnName);
	return msRet;
}

Module_Status WaitTillChamberPressure(double fPres, double fTOut, int nAlarm)
{
	char pszLocFtnName[] = "WaitTillChamberPressure";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	int nData     = -1;

	__KF_DECLARE_ALARM
	double	fChamberPressure = 0;
	int		nElapsedTime = 0;
	double	fPumpTime = 0;
	int		nTOut	= (int)fTOut;
	nTOut *= 1000;
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{

		__KF_BEGIN_ALARM

		fPumpTime = 0;
		WRITE_ANALOG( PumpTime, fPumpTime, &nIOStatus);
		///////////////////////////////////////////////////////////////////
		// Wait until chamber pressure  <=  'fPres' value
		do
		{
			//ReadIOCValue(m_strName+"_ThrottleValve.Pressure_SAI",TYPE_FLOAT,&fChamberPressure);//2003.12.16
			fChamberPressure = GetChamberPressure();
			if(fChamberPressure <= fPres)
			{
				msRet = SYS_SUCCESS;
				break;
			}

			// 2004.07.27 by cskim
			if(CheckDryPump() == FALSE)
			{
				msRet = SYS_ABORTED;
				break;
			}

			if( FALSE == WAIT_SECONDS(1))
			{
				_LOG("WaitTillChamberPressure is Aborted");
				ALARM_POST(VACUUM_CONTROL_FUNCTION_FAIL);
				__SET_RECOVERY_CODE(_ALM_ABORT);
				msRet = SYS_ABORTED;
				break;
			}
			nElapsedTime += 1000;

			fPumpTime++;
			WRITE_ANALOG( PumpTime, fPumpTime, &nIOStatus);

			if(nElapsedTime > nTOut)
			{
				__SET_RECOVERY_CODE(ALARM_MANAGE(nAlarm));
				msRet = SYS_ABORTED;
				break;
			}
		}while(1);
		__KF_END_ALARM

		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leave %s\n", pszFtnName);
	return msRet;
}

Module_Status WaitTillThrottlePosition(double fPos)
{
	char pszLocFtnName[] = "WaitTillThrottlePosition";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	int nData     = -1;

	__KF_DECLARE_ALARM
	double fTVTolerance = 5;
	double fTVTimeOut = 30;
	double fThrtlTol = 10;
	double fElapsedTime = 0;
	double fTVCurPos	= -1;
	double fTVPosDiff	= 0;
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		//PMSeqSendMessage("Checking throttle valve");
		fTVTimeOut = 30;

		__KF_BEGIN_ALARM
		//m_pPMConfig->GetParamVal(m_strName,"Throttle_Position_T_Out",nTimeOut);
		fTVTimeOut		= READ_ANALOG(PRMA_TO_ThrtlPos, &nIOStatus);
		fTVTolerance	= READ_ANALOG(PRMA_TL_ThrtlPos,&nIOStatus);
		fTVTimeOut = fTVTimeOut * 1000;
		fElapsedTime = 0;
		do
		{
			//m_pSeq->ReadIOCValue(m_strName+".Position_SAI",TYPE_FLOAT,&fCurValue);
			fTVCurPos = READ_ANALOG( M651_Position, &nIOStatus);

			//if(m_pTV->IsPositionOK(nPos))
			//	break;
			fTVPosDiff = fabs(fTVCurPos - fPos);
			if( fTVPosDiff <= fTVTolerance)
			{
				break;
			}

			//Sleep(1000);
			//IF_ABORTED_DO_ABORT("During Throttle vlv position checking");
			if( FALSE == WAIT_SECONDS(1))
			{
				_LOG("WaitTillChamberPressure is Aborted");
				__SET_RECOVERY_CODE(_ALM_ABORT);
				msRet = SYS_ABORTED;
				break;
			}
			fElapsedTime = fElapsedTime + 1000;

			if(fElapsedTime > fTVTimeOut)
			{
				//RaiseAlarm(CPMSequenceAlarm(m_strName+"_THROTTLE_VALVE_POSITION_INCORRECT"));
				__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT));
				break;
			}

		}while(1);
		__KF_END_ALARM

		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leave %s\n", pszFtnName);
	return msRet;
}

Module_Status Pump()
{
	char pszLocFtnName[] = "Pump";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	int nData     = -1;
	__KF_DECLARE_ALARM

	BOOL bAborted = FALSE;
	char pszParam[256];

	double	fTVTolerance				= 5;
	double  fTVTimeOut					= 5;
	double	fTVValue					= 0;

	double  fSlowRoughTimeOut			= 5;
	double	fPressureBaseRough			= 0;
	double	fBaseRoughPressureTimeOut	= 0;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	do
	{
		// 2004.07.27 by cskim, Added 'DryPump Check Sequence'
		if(CheckDryPump() == FALSE)
		{
			printf("DryPump failed in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

		// 2. if Check Roughing need or not
		if( FALSE == IsRoughingNeeded())
		{
			msRet = SYS_SUCCESS;
			WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, PM_VACUUM, &nIOStatus);
			break;
		}

		// 2.1 Run AbortGas Function
		__KF_BEGIN_ALARM
		msRet = RUN_FUNCTION_FREE(ABORT_GAS, "");  //... 2016.03.22
		if( SYS_SUCCESS != msRet )
		{
			_LOG("ABORT_GAS Function is failed");
			__SET_RECOVERY_CODE(ALARM_MANAGE(ABORT_GAS_FAIL))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM

		// 2.2 Set Vacuum State(PM#.VacuumState, PUMPING)
		WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, GO_VAC, &nIOStatus);

		// 2.3 if Function Aborted, Abort Pump
		bAborted = MANAGER_ABORT();
		if( TRUE == bAborted )
		{
			msRet = SYS_ABORTED;
			break;
		}

		// 2020. 01. 22 RPG gate valve open
		if (READ_DIGITAL(PRMD_GATE_VALVE_OPT, &nIOStatus) == CTRL_ON)
		{
			if(FALSE == CheckSafetyPLCMode(ePLCMODE_TEACH))
			{
				_LOG("CheckSafetyPLCMode[%d] is failed", ePLCMODE_TEACH);
				break;
			}
			__KF_BEGIN_ALARM
		    WRITE_DIGITAL(V103ValveDO, Vlv_OPEN, &nIOStatus);
		    WRITE_DIGITAL(V111ValveDO, Vlv_OPEN, &nIOStatus);
		    Sleep(100);
			if(READ_DIGITAL(RPGGateVlvPosDI, &nIOStatus) == RPG_Close)
			{
				_LOG("RPG gate valve (V103Valve) Not Open");
				printf("[%s] RPG gate valve (V103Valve) Not Open \n", pszFtnName);
				__SET_RECOVERY_CODE(ALARM_MANAGE(ALARM_RPG_Gate_Valve1_FAILED))
				msRet = SYS_ABORTED;
			}
			else if(READ_DIGITAL(RPGGateVlv2PosDI, &nIOStatus) == RPG_Close)
			{
				_LOG("RPG gate valve (V111Valve) Not Open");
				printf("[%s] RPG gate valve (V111Valve) Not Open \n", pszFtnName);
				__SET_RECOVERY_CODE(ALARM_MANAGE(ALARM_RPG_Gate_Valve2_FAILED))
			}
		    if ((READ_DIGITAL(RPGGateVlvPosDI, &nIOStatus) == RPG_Open) && (READ_DIGITAL(RPGGateVlv2PosDI, &nIOStatus) == RPG_Open))
		    {
		        break;
		    }
		    __KF_END_ALARM
		}

		// 2.4 Close Rough Valve DO(PM#.Rough_Valve_DO, CLOSE_1BIT)
		WRITE_DIGITAL_COMMCHECK( pszFtnName, FastRoughVlvDO, EVlv_Close, &nIOStatus);
		if (SYS_SUCCESS != CheckRoughValveSts(RVlv_Close)) {
			return SYS_ABORTED;
		}

		// 2.8 WaitTillRequiredPumpingPressure
		msRet = WaitTillRequiredPumpingPressure();
		if( SYS_SUCCESS != msRet )
		{
			_LOG("WaitTillRequiredPumpingPressure is failed");
			break;
		}

		// 2.9 StartDPBooster
		msRet = StartDPBooster();
		if( SYS_SUCCESS != msRet )
		{
			_LOG("StartDPBooster is failed");
			break;
		}

		// 2.11 Setting throttle vlv Wait  POSITION [Value] [TOL] [TimeOut]
		__KF_BEGIN_ALARM
		fTVValue		= TV_FIRST_POSTION_FOR_PUMPING;
		fTVTolerance	= READ_ANALOG(PRMA_TL_ThrtlPos,&nIOStatus);
		fTVTimeOut		= READ_ANALOG(PRMA_TO_ThrtlPos, &nIOStatus);
		// 2004.04.18 by cskim
		sprintf( pszParam, "POSITION %d %d %d", (int) fTVValue, (int) fTVTolerance, (int) fTVTimeOut);
		msRet = RUN_FUNCTION_FREE(PRESS_CONTROL, pszParam);
		if( SYS_SUCCESS != msRet )
		{
			_LOG("Throttle valve position Error[%s]", pszParam);
			__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM

		// 5 sec sleep
		if( FALSE == WAIT_SECONDS(5))
		{
			_LOG("Function is aborted");
			msRet = SYS_ABORTED;
			break;
		}

		// 2.12 Open Rough Valve (PM#.Rough_Valve_DO, OPEN_1BIT)
		WRITE_DIGITAL_COMMCHECK( pszFtnName, FastRoughVlvDO, EVlv_Open, &nIOStatus);
		if (SYS_SUCCESS != CheckRoughValveSts(RVlv_Open)) {
			return SYS_ABORTED;
		}

		// 2.13 Sleep 5 sec
		if( FALSE == WAIT_SECONDS(5))
		{
			_LOG("Function is aborted");
			msRet = SYS_ABORTED;
			break;
		}

		// 2014.12.01 Setting throttle vlv(4.5% open) and 5 sec sleep
		__KF_BEGIN_ALARM
		fTVValue		= 4.5;
		fTVTolerance	= READ_ANALOG(PRMA_TL_ThrtlPos,&nIOStatus);
		fTVTimeOut		= READ_ANALOG(PRMA_TO_ThrtlPos, &nIOStatus);

		sprintf( pszParam, "POSITION %d %d %d", (int) fTVValue, (int) fTVTolerance, (int) fTVTimeOut);
		msRet = RUN_FUNCTION_FREE(PRESS_CONTROL, pszParam);
		if( SYS_SUCCESS != msRet )
		{
			_LOG("Throttle valve position Error[%s]", pszParam);
			__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM

		if( FALSE == WAIT_SECONDS(5))
		{
			_LOG("Function is aborted");
			msRet = SYS_ABORTED;
			break;
		}
		//2009.06.09

		// 2014.12.01 Setting throttle vlv(5.5% open) and 5 sec sleep
		__KF_BEGIN_ALARM
		fTVValue		= 5.5;
		fTVTolerance	= READ_ANALOG(PRMA_TL_ThrtlPos,&nIOStatus);
		fTVTimeOut		= READ_ANALOG(PRMA_TO_ThrtlPos, &nIOStatus);

		sprintf( pszParam, "POSITION %d %d %d", (int) fTVValue, (int) fTVTolerance, (int) fTVTimeOut);
		msRet = RUN_FUNCTION_FREE(PRESS_CONTROL, pszParam);
		if( SYS_SUCCESS != msRet )
		{
			_LOG("Throttle valve position Error[%s]", pszParam);
			__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM

		if( FALSE == WAIT_SECONDS(5))
		{
			_LOG("Function is aborted");
			msRet = SYS_ABORTED;
			break;
		}

		fSlowRoughTimeOut = READ_ANALOG(PRMA_TO_SlowRghPrs, &nIOStatus);
		msRet = WaitTillChamberPressure( 1, fSlowRoughTimeOut,SLOW_ROUGHING_TIMED_OUT);
		if( SYS_SUCCESS != msRet )
		{
			_LOG("WaitTillChamberPressure[1 Torr, %f sec]", fSlowRoughTimeOut);
			break;
		}

		// 2.16 Set Position(20% Open) and 5 sec delay
		__KF_BEGIN_ALARM
		fTVValue		= 10;
		fTVTolerance	= READ_ANALOG(PRMA_TL_ThrtlPos,&nIOStatus);
		fTVTimeOut		= READ_ANALOG(PRMA_TO_ThrtlPos, &nIOStatus);
		// 2004.04.18 by cskim
		sprintf( pszParam, "POSITION %d %d %d", (int) fTVValue, (int) fTVTolerance, (int) fTVTimeOut);
		msRet = RUN_FUNCTION_FREE(PRESS_CONTROL, pszParam);
		if( SYS_SUCCESS != msRet )
		{
			_LOG("Throttle valve position Error[%s]", pszParam);
			__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM

		if( FALSE == WAIT_SECONDS(8))
		{
			_LOG("Function is aborted");
			msRet = SYS_ABORTED;
			break;
		}

		// 2.17 Set Position(50% Open) and 5 sec delay
		__KF_BEGIN_ALARM
		fTVValue		= 50;
		fTVTolerance	= READ_ANALOG(PRMA_TL_ThrtlPos, &nIOStatus);
		fTVTimeOut		= READ_ANALOG(PRMA_TO_ThrtlPos, &nIOStatus);
		// 2004.04.18 by cskim
		sprintf( pszParam, "POSITION %d %d %d", (int) fTVValue, (int) fTVTolerance, (int) fTVTimeOut);
		msRet = RUN_FUNCTION_FREE(PRESS_CONTROL, pszParam);
		if( SYS_SUCCESS != msRet )
		{
			_LOG("Throttle valve position Error[%s]", pszParam);
			__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM

		// 2.18 WaitTillChamberPressure (0.1,  Slow_Rough_Pressure_T_Out, _SLOW_ROUGHING_TIMED_OUT)
		fSlowRoughTimeOut = READ_ANALOG(PRMA_TO_SlowRghPrs, &nIOStatus);
		msRet = WaitTillChamberPressure( 0.1, fSlowRoughTimeOut,SLOW_ROUGHING_TIMED_OUT);
		if( SYS_SUCCESS != msRet )
		{
			_LOG("WaitTillChamberPressure[0.1 Torr, %f sec]", fSlowRoughTimeOut);
			break;
		}

		// 2.19 Throttle Valve Full Open
		__KF_BEGIN_ALARM
		msRet = RUN_FUNCTION_FREE(PRESS_CONTROL, "OPEN");
		if( SYS_SUCCESS != msRet )
		{
			_LOG("Throttle valve position Error[%s]", pszParam);
			__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM

		msRet = WaitTillThrottlePosition( 100 );
		if( SYS_SUCCESS != msRet )
		{
			_LOG("WaitTillThrottlePosition(100) is Failed");
			break;
		}

		// 2.20 check function is aborted or not
		bAborted = MANAGER_ABORT();
		if( TRUE == bAborted )
		{
			msRet = SYS_ABORTED;
			break;
		}

		// 2.22 WaitTillChamberPressure ( Pressure_Base_Rough, Base_Rough_Pressure_T_Out, _FAST_ROUGHING_TIMED_OUT)
		fPressureBaseRough			= READ_ANALOG(PRMA_VS_PrsBaRough, &nIOStatus);
		fBaseRoughPressureTimeOut	= READ_ANALOG( PRMA_TO_BaseRghPrs, &nIOStatus);
		msRet = WaitTillChamberPressure( fPressureBaseRough, fBaseRoughPressureTimeOut,FAST_ROUGHING_TIMED_OUT);
		if( SYS_SUCCESS != msRet )
		{
			_LOG("WaitTillChamberPressure[%f Torr, %f sec]", fPressureBaseRough, fBaseRoughPressureTimeOut);
			break;
		}

		// 2.23 SetVacuumState(PUMPED);
		WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, PM_VACUUM, &nIOStatus);

		msRet = SYS_SUCCESS;

	}while(0);

	// 2020. 01.22 gate valve close
	if (READ_DIGITAL(PRMD_GATE_VALVE_OPT, &nIOStatus) == CTRL_ON)
	{
		WRITE_DIGITAL(V103ValveDO, Vlv_CLOSE, &nIOStatus);
		WRITE_DIGITAL(V111ValveDO, Vlv_CLOSE, &nIOStatus);
	}

	if( SYS_SUCCESS != msRet )
		WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, ABORT, &nIOStatus);
	else
	{
		WRITE_DIGITAL_COMMCHECK( pszFtnName, FastRoughVlvDO, EVlv_Open, &nIOStatus);
		if (SYS_SUCCESS != CheckRoughValveSts(RVlv_Open))
			return SYS_ABORTED;

		RUN_FUNCTION_FREE(PRESS_CONTROL, "OPEN");
	}

	printf("Leave %s\n", pszFtnName);
	return msRet;
}
//------------------------------------------------------------------------------------------
Module_Status FastRough()
{
	char pszLocFtnName[] = "FastRough";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	int nData     = -1;

	__KF_DECLARE_ALARM
	double fBaseRoughPressure = 0.002f;// from PM configuration
	double fBaseRoughPressureTimeOut = 100; // from PM configuration
	double fAfterFastRoughWait  = 1;
	double	fTVTolerance				= 5;
	double  fTVTimeOut					= 5;
	double	fTVValue					= 0;
	BOOL bAborted = FALSE;
	char pszParam[256];
	double fChmPres = 10;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do {
		// 2004.12.17 by cskim
		fBaseRoughPressure = READ_ANALOG(PRMA_VS_PrsBaRough, &nIOStatus);
		fChmPres = GetChamberPressure();

		// Compare Pressure
		if(fChmPres <= fBaseRoughPressure)
		{
			RUN_FUNCTION_FREE(PRESS_CONTROL, "OPEN");   //2014.12.01
			RUN_FUNCTION_FREE(ABORT_GAS, "");	   //2014.12.01, //...2016.03.22
			msRet = SYS_SUCCESS;
			break;
		}

		// 2004.07.27 by cskim, Added 'DryPump Check Sequence'
		if(CheckDryPump() == FALSE)
		{
			msRet = SYS_ABORTED;
			break;
		}

		// 2004.08.03 by cskim
		msRet = RUN_FUNCTION_FREE(ABORT_GAS, "");   //...2016.03.22
		if( SYS_SUCCESS != msRet )
		{
			_LOG("ABORT_GAS failed");
			msRet = SYS_ABORTED;
			break;
		}

		//Sleep(200);
		if( FALSE == WAIT_SECONDS(0.2))
		{
			_LOG("Function is aborted");
			msRet = SYS_ABORTED;
			break;
		}

		// Set Vacuum Status
		//SetVacuumState(PUMPING);
		WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, GO_VAC, &nIOStatus);

		// Start Dry Pump Booster
		//StartDPBooster();
		msRet = StartDPBooster();
		if( SYS_SUCCESS != msRet )
		{
			_LOG("StartDPBooster is failed");
			break;
		}

		// Setting throttle vlv(Full open) and Wait till Position 100
		//m_pTV->ValveCmd(TV_OPEN);
		__KF_BEGIN_ALARM
		msRet = RUN_FUNCTION_FREE(PRESS_CONTROL, "OPEN");  //... 2016.03.22
		if( SYS_SUCCESS != msRet )
		{
			_LOG("Throttle valve position Error[%s]", pszParam);
			__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM

		msRet = WaitTillThrottlePosition( 100 );
		if( SYS_SUCCESS != msRet )
		{
			_LOG("WaitTillThrottlePosition(100) is Failed");
			break;
		}

		//Sleep(200);
		if( FALSE == WAIT_SECONDS(0.2))
		{
			_LOG("Function is aborted");
			msRet = SYS_ABORTED;
			break;
		}

		// Open Rough vlv
		//OperateValve(m_strName+".Rough_Valve_DO",OPEN_1BIT);
		WRITE_DIGITAL_COMMCHECK( pszFtnName, FastRoughVlvDO, EVlv_Open, &nIOStatus);
		if (SYS_SUCCESS != CheckRoughValveSts(RVlv_Open)) {
			return SYS_ABORTED;
		}

		//WaitTillThrottlePosition(100); Above PRESS_CONTROL has this functionality.
		//IF_ABORTED_DO_ABORT("During 'FastRoughing'");
		bAborted = MANAGER_ABORT();
		if( TRUE == bAborted )
		{
			_LOG("FAST_ROUGH is aborted");
			msRet = SYS_ABORTED;
			break;

		}

		// Wait till required pressure
		fBaseRoughPressure			= READ_ANALOG(PRMA_VS_PrsBaRough, &nIOStatus);
		fBaseRoughPressureTimeOut   = READ_ANALOG( PRMA_TO_BaseRghPrs, &nIOStatus);
		msRet = WaitTillChamberPressure( fBaseRoughPressure, fBaseRoughPressureTimeOut,FAST_ROUGHING_TIMED_OUT);
		if( SYS_SUCCESS != msRet )
		{
			_LOG("WaitTillChamberPressure[%f Torr, %f sec]", fBaseRoughPressure, fBaseRoughPressureTimeOut);
			break;
		}

		fAfterFastRoughWait = READ_ANALOG(PRMA_WT_AfterFstRgh, &nIOStatus);
		if( FALSE == WAIT_SECONDS(fAfterFastRoughWait))
		{
			msRet = SYS_ABORTED;
			break;
		}

		//SetVacuumState(PUMPED);
		WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, PM_VACUUM, &nIOStatus);

		msRet = SYS_SUCCESS;
	}while(0);

	if( SYS_SUCCESS != msRet )
		WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, ABORT, &nIOStatus);
	else
	{
		WRITE_DIGITAL_COMMCHECK( pszFtnName, FastRoughVlvDO, EVlv_Open, &nIOStatus);
		if (SYS_SUCCESS != CheckRoughValveSts(RVlv_Open)) {
			return SYS_ABORTED;
		}

		RUN_FUNCTION_FREE(PRESS_CONTROL, "OPEN");   //... 2016.03.22
	}

	printf("Leave %s\n", pszFtnName);
	return msRet;
}
//------------------------------------------------------------------------------------------
Module_Status CheckStageHeaterTemp()
{
	char pszLocFtnName[] = "CheckStageHeaterTemp";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	int nData     = -1;
	char szTemp[256];

	__KF_DECLARE_ALARM
	double fStgHeaterTemp = 0;
	double fVentOKTemp = 50; // from PM configuration

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		__KF_BEGIN_ALARM

		fVentOKTemp = READ_ANALOG(PRMA_AL_VentOKTemp, &nIOStatus);
		if(fVentOKTemp <= 0)
		{
			break;
		}

		//ReadIOCValue(m_strName+"_"+strHeater+".Temperature_Reading_SAI",TYPE_FLOAT,&fStgHeaterTemp);
		sprintf(szTemp, "IS_LOWER %d", (int) fVentOKTemp);
		if(SYS_SUCCESS != RUN_FUNCTION(STGHTR_CONTROL, szTemp))
		{
			//RaiseAlarm(CPMSequenceAlarm(m_strName+"_STAGE_HEATER_TEMP_ABOVE_LIMIT"));
			_LOG("Stage Heater Temp[%f] > fVentOKTemp[%f]", fStgHeaterTemp, fVentOKTemp);
			__SET_RECOVERY_CODE(ALARM_MANAGE(STAGE_HEATER_TEMP_ABOVE_LIMIT))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM

		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leave %s\n", pszFtnName);
	return msRet;
}

//------------------------------------------------------------------------------------------

void OpenVentLine()
{
	int i;
	int nIOStatus;
	char szTemp[40] = "";
	char szParam[256] = "";

	strcpy(szParam, "SET_VALUE 0");
	for(i=0; i < 5; i++)
	{
		if(g_VentMfc[i].nMfcN > 0)
		{
			sprintf(szTemp, " %d %.3f N 0", g_VentMfc[i].nMfcN,(float)g_VentMfc[i].nMfcFlow);
			strcat(szParam, szTemp);

			if(g_VentMfc[i].nValve1 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc[i].nValve1-1, 1, &nIOStatus);
			if(g_VentMfc[i].nValve2 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc[i].nValve2-1, 1, &nIOStatus);
			if(g_VentMfc[i].nValve3 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc[i].nValve3-1, 1, &nIOStatus);
			// 2014.04.09
			if(g_VentMfc[i].nValve4 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc[i].nValve4-1, 1, &nIOStatus);
			if(g_VentMfc[i].nValve5 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc[i].nValve5-1, 1, &nIOStatus);

		}
	}

	printf("Vent Open : %s\n", szParam);
	RUN_FUNCTION(MFC_CONTROL, szParam);
}

void CloseVentLine()
{
	int i;
	int nIOStatus;
	char szTemp[40] = "";
	char szParam[256] = "";

	strcpy(szParam, "SET_VALUE 10");
	for(i=0; i < 5; i++)
	{
		if(g_VentMfc[i].nMfcN > 0)
		{
			sprintf(szTemp, " %d 0 F 0", g_VentMfc[i].nMfcN);
			strcat(szParam, szTemp);

			if(g_VentMfc[i].nValve1 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc[i].nValve1-1, 0, &nIOStatus);
			if(g_VentMfc[i].nValve2 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc[i].nValve2-1, 0, &nIOStatus);
			if(g_VentMfc[i].nValve3 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc[i].nValve3-1, 0, &nIOStatus);
			// 2014.04.09
			if(g_VentMfc[i].nValve4 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc[i].nValve4-1, 0, &nIOStatus);
			if(g_VentMfc[i].nValve5 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc[i].nValve5-1, 0, &nIOStatus);
		}
	}
	printf("Vent Close : %s\n", szParam);
	RUN_FUNCTION(MFC_CONTROL, szParam);
}

//////////////////////////////////////////////////////////////////////////
//...Add MFC#6 Ar Line Vent Sequence
//... 2016.06.14
void OpenVentLine2()
{
	int i;
	int nIOStatus;
	char szTemp[40] = "";
	char szParam[256] = "";

	strcpy(szParam, "SET_VALUE 0");
	for(i=0; i < 5; i++)
	{
		if(g_VentMfc2[i].nMfcN > 0)
		{
			sprintf(szTemp, " %d %.3f N 0", g_VentMfc2[i].nMfcN,(float)g_VentMfc2[i].nMfcFlow);
			strcat(szParam, szTemp);

			if(g_VentMfc2[i].nValve1 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc2[i].nValve1-1, 1, &nIOStatus);
			if(g_VentMfc2[i].nValve2 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc2[i].nValve2-1, 1, &nIOStatus);
			if(g_VentMfc2[i].nValve3 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc2[i].nValve3-1, 1, &nIOStatus);
			if(g_VentMfc2[i].nValve4 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc2[i].nValve4-1, 1, &nIOStatus);
			if(g_VentMfc2[i].nValve5 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc2[i].nValve5-1, 1, &nIOStatus);

		}
	}

	printf("Vent Open : %s\n", szParam);
	RUN_FUNCTION(MFC_CONTROL, szParam);
}

void CloseVentLine2()
{
	int i;
	int nIOStatus;
	char szTemp[40] = "";
	char szParam[256] = "";

	strcpy(szParam, "SET_VALUE 10");
	for(i=0; i < 5; i++)
	{
		if(g_VentMfc2[i].nMfcN > 0)
		{
			sprintf(szTemp, " %d 0 F 0", g_VentMfc2[i].nMfcN);
			strcat(szParam, szTemp);

			if(g_VentMfc2[i].nValve1 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc2[i].nValve1-1, 0, &nIOStatus);
			if(g_VentMfc2[i].nValve2 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc2[i].nValve2-1, 0, &nIOStatus);
			if(g_VentMfc2[i].nValve3 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc2[i].nValve3-1, 0, &nIOStatus);
			if(g_VentMfc2[i].nValve4 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc2[i].nValve4-1, 0, &nIOStatus);
			if(g_VentMfc2[i].nValve5 > 0)
				WRITE_DIGITAL(V1ValveDO+g_VentMfc2[i].nValve5-1, 0, &nIOStatus);
		}
	}
	printf("Vent Close : %s\n", szParam);
	RUN_FUNCTION(MFC_CONTROL, szParam);
}
//////////////////////////////////////////////////////////////////////////

Module_Status DoVenting()
{
	char pszLocFtnName[] = "DoVenting";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	int nData     = -1;

	__KF_DECLARE_ALARM
	double	fChamberPressure		= 0;
	double	fBaseVentPressure		= 760;// from PM configuration
	double	fVACSenseOpenPressure	= 500;// from pM Configuration
	double	fTimeOut				= 100;
	int		nTimeOut				= 100;
	int		nElapsedTime			= 0;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		__KF_BEGIN_ALARM
		//m_pPMConfig->GetParamVal(m_strName ,"Pressure_Base_Vent",fBaseVentPressure);
		fBaseVentPressure = READ_ANALOG(PRMA_VS_PrsBaVent, &nIOStatus);

		//m_pPMConfig->GetParamVal(m_strName ,"VAC_Sense_IsolVlv_Open_Pressure" , fVACSenseOpenPressure);
		fVACSenseOpenPressure = READ_ANALOG(PRMA_VS_VCSnVlvOpPs, &nIOStatus);

		//ReadIOCValue(m_strName+".Chamber_Pressure_AI",TYPE_FLOAT,&fChamberPressure);
		fChamberPressure   = READ_ANALOG(ChmPrsAI, &nIOStatus);

		//int nTimeOut = 100; // from PM configuration
		//m_pPMConfig->GetParamVal(m_strName,"Pressure_Vent_T_Out",nTimeOut);
		//nTimeOut *= 1000;
		//DWORD dwElapsedTime = 0;
		fTimeOut = READ_ANALOG(PRMA_TO_PressVent, &nIOStatus);
		fTimeOut = fTimeOut * 1000;
		nTimeOut = (int)fTimeOut;
		nElapsedTime = 0;

		//... 2016.06.14
		if(eAr_1_9 == READ_DIGITAL(RRMD_VENT_SEQ,&nIOStatus))OpenVentLine();
		else												 OpenVentLine2();

		/////////////////////////////////////////////////////////////////////////////////////
		// Wait until chamber pressure  >=  base vent pressure
		/////////////////////////////////////////////////////////////////////////////////////

		do
		{
			//Sleep(1000);
			if( FALSE == WAIT_SECONDS(1) )
			{
				__SET_RECOVERY_CODE(_ALM_ABORT)
				msRet = SYS_ABORTED;
				break;
			}
			nElapsedTime += 1000;

			//ReadIOCValue(m_strName+".Chamber_Pressure_AI",TYPE_FLOAT,&fChamberPressure);
			fChamberPressure   = READ_ANALOG(ChmPrsAI, &nIOStatus);
			if(fChamberPressure >= fBaseVentPressure)
			{
				msRet = SYS_SUCCESS;
				break;
			}

			if(nElapsedTime > nTimeOut )
			{
				//RaiseAlarm(CPMSequenceAlarm(m_strName+"_VENTING_TIMED_OUT"));
				_LOG("VENTING_TIMED_OUT(%d>%d)", nElapsedTime, nTimeOut);
				__SET_RECOVERY_CODE(ALARM_MANAGE(VENTING_TIMED_OUT));
				msRet = SYS_ABORTED;
				break;
			}
		}while(1);

		__KF_END_ALARM

		msRet = SYS_SUCCESS;
	}while(0);
	printf("Leave %s\n", pszFtnName);
	return msRet;
}
//////////////////////////////////////////////////////////////////////////
//... 2016.04.15
void ToxicLineOpen(int nToxicLine)
{
	char szArLineSeqCmd[256] = {"0",};
	int nIndex = 0;

	nIndex = nToxicLine + 1;

	sprintf(szArLineSeqCmd,"TOXICGAS_PURGE%d_OP",nIndex);
	g_ValveSeq.StartValveSeq(szArLineSeqCmd,TRUE);
}
//////////////////////////////////////////////////////////////////////////
//... 2016.03.16
void ArChargingLineOpen(int nArLine)
{
	char szArLineSeqCmd[256] = {"0",};
	int nArLnIndex = 0;

	nArLnIndex = nArLine + 1;

	sprintf(szArLineSeqCmd,"AR_CHARGE_LINE%d_OP",nArLnIndex);
	g_ValveSeq.StartValveSeq(szArLineSeqCmd,TRUE);
}
//////////////////////////////////////////////////////////////////////////
BOOL IsOKMfcFlowRateStatus(int nArLine)
{
	BOOL bRet = FALSE;
	int nIOStatus;
	double dbMaxAO = 0;
	double dbMFCFlowRate = 0;
	int nVlvStepIdx;
	int nMFCIndex;
	char szArLineSeqCmd[256] = {"0",};
	int nArLnIndex = 0;

	nArLnIndex = nArLine + 1;

	sprintf(szArLineSeqCmd,"AR_CHARGE_LINE%d_OP",nArLnIndex);

	nVlvStepIdx = g_ValveSeq.FindStepIdx(szArLineSeqCmd);
	nMFCIndex	= g_ValveSeq.m_anMFCNo[nVlvStepIdx][0] - 1;
	dbMaxAO		= g_ValveSeq.m_adbMFCSet[nVlvStepIdx][0];

	//... Minimum Range 1000 sccm
	if(dbMaxAO < 1000) dbMaxAO = 1000;

	dbMFCFlowRate = READ_ANALOG(MFC01FlwAI + nMFCIndex,&nIOStatus);

	if(dbMFCFlowRate < dbMaxAO *0.01) bRet = TRUE;

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
//... Toxic gas Puge
//... 1.MFC4 Line Charge, 2.MFC5
BOOL IsOkToxicGasPuge()
{
	BOOL bRet = FALSE;
	int nArLine = eAr_Line1;
	int nIOStatus;
	double dbTimeOut = 0;
	double dbParamTimeOut = 0;

	dbParamTimeOut = READ_ANALOG(PRMA_TM_ToxicGasPurge,&nIOStatus);
	if(dbParamTimeOut <= 0) return bRet;


	if(! CheckSafetyPLCMode(ePLCMODE_SYSRUN)) return FALSE;

	do
	{
		//... Open Toxic Line Valve
		ToxicLineOpen(nArLine);

		//... initialize time
		S_TIMER_READY();
		do
		{
			//... Check System Abort
			if(!WAIT_SECONDS(1)) {bRet = FALSE; break;}

			dbTimeOut = S_TIMER_ELAPSED();

			//... Wait
			if(dbTimeOut > dbParamTimeOut) {bRet = TRUE; break;}
		}while(TRUE);

		//... Close Toxic gas Line Valve
		g_ValveSeq.StartValveSeq("CLOSE_LINE_VLV",TRUE);

		if(bRet != TRUE) break;
	}
	while(++nArLine < MAX_TOXIC_PURGE_LINE);

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
//... 1.MFC3 Line Charge, 2.MFC4 Line Charge, 3.MFC6 Line Charge, 4.MFC7 Line charge
BOOL IsOkArCharging()
{
	BOOL bRet = FALSE;
	int nArLine = eAr_Line1;
	int nIOStatus;
	double dbTimeOut = 0;
	double dbParamTimeOut = 0;

	dbParamTimeOut = READ_ANALOG(PRMA_TO_ArCharging,&nIOStatus);
	if(dbParamTimeOut <= 0) return bRet;

	if(! CheckSafetyPLCMode(ePLCMODE_SYSRUN)) return FALSE;

	do
	{
		//... Open Ar Charging Line Valve
		ArChargingLineOpen(nArLine);

		//... initialize time
		S_TIMER_READY();
		do
		{
			//... Check System Abort
			if(!WAIT_SECONDS(1)) {bRet = FALSE; break;}

			//////////////////////////////////////////////////////////////////////////
			//... Check MFC Flow Rate : 2000sccm -> 0
			//.... Gas Flow at Closed Final Valve and It is checked until MFC Flow Rate "0"
			if(TRUE == IsOKMfcFlowRateStatus(nArLine))
			{
				bRet = TRUE;
				break;
			}
			//////////////////////////////////////////////////////////////////////////

			dbTimeOut = S_TIMER_ELAPSED();

			//... Wait 100sec
			if(dbTimeOut > dbParamTimeOut) {bRet = FALSE; break;}
		}while(TRUE);

		//... Close Ar Charging Line Valve
		g_ValveSeq.StartValveSeq("CLOSE_LINE_VLV",TRUE);

		if(bRet != TRUE) break;
	}
	while(++nArLine < MAX_AR_CHARGE_LINE);

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
Module_Status IsChuckRelayOFF()
{
	char pszLocFtnName[] = "IsChuckRelayOFF";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int IOStatus = 0;
	__KF_DECLARE_ALARM
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	do
	{
		__KF_BEGIN_ALARM
		//... Read Chuck Relay I/O
		//... It is Checked whether chuck Relay Status Off or ON
		if( eRelayOFF != READ_DIGITAL(StgHtr1RelayDI,&IOStatus)
		    &&eRelayOFF != READ_DIGITAL(StgHtr2RelayDI,&IOStatus))
		{
			_LOG("Chuck Relay is On So Can't Vent");
			__SET_RECOVERY_CODE(ALARM_MANAGE(ALARM_CANNOT_VENT))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM
		msRet = SYS_SUCCESS;
	}
	while (0);


	return msRet;
}
//////////////////////////////////////////////////////////////////////////
Module_Status Vent()
{
	char pszLocFtnName[] = "Vent";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int nIOStatus = 0;
	int nData     = -1;

	__KF_DECLARE_ALARM
	int	nPressureSense = Vacuum;
	double fWaitTimeAfterVent = 10;
	BOOL bAborted = FALSE;
	double 	fTVValue		= 0;
	double	fTVTolerance	= 0;
	double 	fTVTimeOut		= 0;
	char pszParam[256];


	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	do
	{
		if(FALSE == IsVentNeeded())
		{
			WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, PM_ATM, &nIOStatus);
			msRet = SYS_SUCCESS;
			break;
		}

		__KF_BEGIN_ALARM
		msRet = RUN_FUNCTION(ABORT_GAS, "");
		if(SYS_SUCCESS != msRet)
		{
			_LOG("ABORT_GAS Function is failed");
			__SET_RECOVERY_CODE(ALARM_MANAGE(ABORT_GAS_FAIL))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM


		bAborted = MANAGER_ABORT();
		if(TRUE == bAborted)
		{
			_LOG("Vent is aborted");
			msRet = SYS_ABORTED;
			break;
		}
		/*  2016.04.28
		//... 2016.04.15 Toxic Gas Line Check Sequence
		if(FALSE == IsOkToxicGasPuge())
		{
			RUN_FUNCTION_FREE(ABORT_GAS, "");
			_LOG("IsOkToxicGasPuge Sequence is failed");
			ALARM_POST(ALARM_TOXICGAS_PURGE_FAILL);
			break;
		}
		*/
		//... 2016.03.16 Gas Line Ar Charging Sequence
		if(FALSE == IsOkArCharging())
		{
			RUN_FUNCTION_FREE(ABORT_GAS, "");
			_LOG("IsOkArCharging Sequence is failed");
			ALARM_POST(ALARM_AR_CHARGING_FAIL);
			break;
		}

		//... Check Chuck Relay Status
		msRet = IsChuckRelayOFF();
		if(SYS_SUCCESS != msRet)
		{
			_LOG("Chuck Relay is On");
			break;
		}

		// Check stage heater temperature
		msRet = CheckStageHeaterTemp();
		if(SYS_SUCCESS != msRet)
		{
			_LOG("CheckStageHeaterTemp is failed");
			break;
		}

		//... 2015.03.23 Add Check Teach Mode for Safety PLC
		if(FALSE == CheckSafetyPLCMode(ePLCMODE_TEACH))
		{
			_LOG("CheckSafetyPLCMode[%d] is failed", ePLCMODE_TEACH);
			break;
		}

		//... 2016.06.14
		if( FALSE == WAIT_SECONDS(READ_ANALOG(PRMA_TO_BeforeVent, &nIOStatus)) )
		{
			_LOG("Function is aborted");
			__SET_RECOVERY_CODE(_ALM_ABORT);
			msRet = SYS_ABORTED;
			break;
		}

		// 2020. 01. 22 RPG gate valve open
		if (READ_DIGITAL(PRMD_GATE_VALVE_OPT, &nIOStatus) == CTRL_ON)
		{
			__KF_BEGIN_ALARM
			WRITE_DIGITAL(V103ValveDO, Vlv_OPEN, &nIOStatus);
			WRITE_DIGITAL(V111ValveDO, Vlv_OPEN, &nIOStatus);
			Sleep(100);
			if(READ_DIGITAL(RPGGateVlvPosDI, &nIOStatus) == RPG_Close)
			{
				_LOG("RPG gate valve (V103Valve) Not Open");
				printf("[%s] RPG gate valve (V103Valve) Not Open \n", pszFtnName);
				__SET_RECOVERY_CODE(ALARM_MANAGE(ALARM_RPG_Gate_Valve1_FAILED))
				msRet = SYS_ABORTED;
			}
			else if(READ_DIGITAL(RPGGateVlv2PosDI, &nIOStatus) == RPG_Close)
			{
				_LOG("RPG gate valve (V111Valve) Not Open");
				printf("[%s] RPG gate valve (V111Valve) Not Open \n", pszFtnName);
				__SET_RECOVERY_CODE(ALARM_MANAGE(ALARM_RPG_Gate_Valve2_FAILED))
			}
			if ((READ_DIGITAL(RPGGateVlvPosDI, &nIOStatus) == RPG_Open) && (READ_DIGITAL(RPGGateVlv2PosDI, &nIOStatus) == RPG_Open))
			{
				break;
			}
			__KF_END_ALARM
		}

		// Close Rough vlv
		WRITE_DIGITAL_COMMCHECK( pszFtnName, FastRoughVlvDO, EVlv_Close, &nIOStatus);
		if(SYS_SUCCESS != CheckRoughValveSts(RVlv_Close))
		{
			return SYS_ABORTED;
		}

		// Close throttle valve
		__KF_BEGIN_ALARM
		msRet = RUN_FUNCTION(PRESS_CONTROL, "CLOSE");
		if(SYS_SUCCESS != msRet)
		{
			_LOG("Throttle valve position Error[%s]",  pszParam);
			__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT))
			msRet = SYS_ABORTED;
		}
		__KF_END_ALARM

		// Set Throttle Valve Close Position
		msRet = WaitTillThrottlePosition(1);
		if(SYS_SUCCESS != msRet)
		{
			_LOG("WaitTillThrottlePosition(1) is Failed");
			break;
		}

		bAborted = MANAGER_ABORT();
		if(TRUE == bAborted)
		{
			_LOG("Vent is aborted");
			msRet = SYS_ABORTED;
			break;
		}

		WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, GO_ATM, &nIOStatus);

		//////////////////////////////////////////////////////////////////////////
		// Replaced to DoVenting
		msRet = DoVenting();
		if(SYS_SUCCESS != msRet)
		{
			// 2014.12.01 close vent line before break
			//... 2016.06.14
			if(eAr_1_9 == READ_DIGITAL(RRMD_VENT_SEQ,&nIOStatus))CloseVentLine();
			else											     CloseVentLine2();

			_LOG("DoVenting is failed");
			break;
		}

		//Checking the pressure sensor
		nPressureSense = READ_DIGITAL_COMMCHECK( pszFtnName, ChmPrssDI1, &nIOStatus);
		if(ATM != nPressureSense)
		{
			// Just wait ....
			//int nWait = 1;
			//m_pPMConfig->GetParamVal(m_strName,"After_Vent_Wait",nWait);
			fWaitTimeAfterVent = READ_ANALOG(PRMA_WT_AfterVent, &nIOStatus);
			if( FALSE == WAIT_SECONDS(fWaitTimeAfterVent) )
			{
				// 2014.12.01 close vent line before break
				//... 2016.06.14
				if(eAr_1_9 == READ_DIGITAL(RRMD_VENT_SEQ,&nIOStatus))CloseVentLine();
				else											     CloseVentLine2();

				_LOG("Function is aborted");
				__SET_RECOVERY_CODE(_ALM_ABORT);
				msRet = SYS_ABORTED;
				break;
			}
		}

		// Close Ar Flow Vent line
		//... 2016.06.14
		if(eAr_1_9 == READ_DIGITAL(RRMD_VENT_SEQ,&nIOStatus))CloseVentLine();
		else											     CloseVentLine2();

		//ReadIOCValue(m_strName+".Pressure_Sense_XI",TYPE_ENUM,&enValue);
		nPressureSense = READ_DIGITAL_COMMCHECK( pszFtnName, ChmPrssDI1, &nIOStatus);
		if( ATM != nPressureSense)
		{
			//RaiseAlarm(CPMSequenceAlarm(m_strName+"_Vent_Not_OK"),false);
			//SetVacuumState(VENT_ABORTED);
			//ERROR_EXIT(m_strName+" -> Venting Not OK",0,__XFILE|__XCONSOLE)
			ALARM_POST(VENT_NOT_OK);
			WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, ABORT, &nIOStatus);
			msRet = SYS_ABORTED;
			_LOG("Vent Not OK!");
			break;
		}

		//SetVacuumState(VENTED);
		WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, PM_ATM, &nIOStatus);

		msRet = SYS_SUCCESS;
	}while(0);

	// 2020. 01.22 gate valve close
	if (READ_DIGITAL(PRMD_GATE_VALVE_OPT, &nIOStatus) == CTRL_ON)
	{
		WRITE_DIGITAL(V103ValveDO, Vlv_CLOSE, &nIOStatus);
		WRITE_DIGITAL(V111ValveDO, Vlv_CLOSE, &nIOStatus);
	}

	if(SYS_SUCCESS != msRet)
	{
		WRITE_DIGITAL_COMMCHECK(pszFtnName, VacuumStateDM, ABORT, &nIOStatus);
		//... 2016.06.14
		if(eAr_1_9 == READ_DIGITAL(RRMD_VENT_SEQ,&nIOStatus))CloseVentLine();
		else											     CloseVentLine2();
	}

	printf("Leave %s\n", pszFtnName);

	return msRet;
}

//------------------------------------------------------------------------------------------
//... 2016.01.19
Module_Status ReadLeakCheckParameter(int nLeakCheckFunc)
{
	Module_Status msRet = SYS_SUCCESS;
	char pszFtnName[256];
	char pszProgressTime[256];
	char pszProgressStep[256];
	int i = 0;
	int nIOStatus;

	memset(pszFtnName,0,sizeof(pszFtnName));
	memset(pszProgressStep,0,sizeof(pszProgressStep));
	memset(pszProgressTime,0,sizeof(pszProgressTime));

	for (i = 0 ; i < MAX_LK_PRAMETER ; i++)
	{
		g_LKParameter[i].dbChamberPressure = 0;
		g_LKParameter[i].dbLCBasePressure = 0;
    	g_LKParameter[i].dbLCLimitPres = 0;
		g_LKParameter[i].dbLCPumpingTO = 0;
		g_LKParameter[i].dbLCDelayTime = 0;
		g_LKParameter[i].dbLCMaxCount = 0;
		g_LKParameter[i].dbRghOpenAndLCDelayTime = 0;
		g_LKParameter[i].dbRoRLimit = 0;
	}

	do
	{
		if(nLeakCheckFunc == eNormalLeakCheck)
		{
			g_LKParameter[eNormalLeakCheck].dbRghOpenAndLCDelayTime	= READ_ANALOG( BaseDelayTimeAM,	&nIOStatus);
			g_LKParameter[eNormalLeakCheck].dbLCBasePressure	= READ_ANALOG( LeakChkBasePressAM,	&nIOStatus);
			g_LKParameter[eNormalLeakCheck].dbLCLimitPres		= READ_ANALOG( LeakChkLimitPressAM, &nIOStatus);
			g_LKParameter[eNormalLeakCheck].dbLCPumpingTO		= READ_ANALOG( LeakChkPumpTimeOutAM,&nIOStatus);
			g_LKParameter[eNormalLeakCheck].dbLCDelayTime		= READ_ANALOG( LeakChkDelayTimeAM,	&nIOStatus);
			g_LKParameter[eNormalLeakCheck].dbLCMaxCount		= READ_ANALOG( LeakChkCountSPAM,	&nIOStatus);
			g_LKParameter[eNormalLeakCheck].dbRoRLimit			= READ_ANALOG( RoRLimitAM,			&nIOStatus);

			if( g_LKParameter[eNormalLeakCheck].dbLCMaxCount	  < 1  ||
				g_LKParameter[eNormalLeakCheck].dbLCMaxCount	  > 10 ||
				g_LKParameter[eNormalLeakCheck].dbLCBasePressure <= 0 ||
				g_LKParameter[eNormalLeakCheck].dbLCLimitPres	  <= 0 ||
				g_LKParameter[eNormalLeakCheck].dbLCPumpingTO	  < 0  ||
				g_LKParameter[eNormalLeakCheck].dbLCDelayTime	  < 0  ||
				g_LKParameter[eNormalLeakCheck].dbRghOpenAndLCDelayTime < 0 /*||
				g_LKParameter[eNormalLeakCheck].dbRoRLimit		  <= 0*/    //... 2019.02.11 ROR Limit 값이 "-3" 까지 설정 할 수 있다.
				) //... Check Invalid Parameters
			{
				msRet = SYS_ABORTED;
				break;
			}
		}
		else if(nLeakCheckFunc == eALDValveLeakCheck)
		{
			g_LKParameter[eALDValveLeakCheck].dbRghOpenAndLCDelayTime	= READ_ANALOG( ALDBaseDelayTimeAM,	&nIOStatus);
			g_LKParameter[eALDValveLeakCheck].dbLCBasePressure	= READ_ANALOG( ALDLkChkBasePressAM,	&nIOStatus);
			g_LKParameter[eALDValveLeakCheck].dbLCLimitPres		= READ_ANALOG( ALDLkChkLimitPressAM, &nIOStatus);
			g_LKParameter[eALDValveLeakCheck].dbLCPumpingTO		= READ_ANALOG( ALDLkChkPumpTimeOutAM,&nIOStatus);
			g_LKParameter[eALDValveLeakCheck].dbLCDelayTime		= READ_ANALOG( ALDLkChkDelayTimeAM,	&nIOStatus);
			g_LKParameter[eALDValveLeakCheck].dbLCMaxCount		= READ_ANALOG( ALDLkChkCountSPAM,	&nIOStatus);
			g_LKParameter[eALDValveLeakCheck].dbRoRLimit		= READ_ANALOG( ALDRoRLimitAM,		&nIOStatus);

			if( g_LKParameter[eALDValveLeakCheck].dbLCMaxCount	  < 1  ||
				g_LKParameter[eALDValveLeakCheck].dbLCMaxCount	  > 10 ||
				g_LKParameter[eALDValveLeakCheck].dbLCBasePressure <= 0 ||
				g_LKParameter[eALDValveLeakCheck].dbLCLimitPres	  <= 0 ||
				g_LKParameter[eALDValveLeakCheck].dbLCPumpingTO	  < 0  ||
				g_LKParameter[eALDValveLeakCheck].dbLCDelayTime	  < 0  ||
				g_LKParameter[eALDValveLeakCheck].dbRghOpenAndLCDelayTime < 0 /*||
				g_LKParameter[eALDValveLeakCheck].dbRoRLimit	  <= 0
				*/) //... Check Invalid Parameters
			{
				msRet = SYS_ABORTED;
				break;
			}
		}
		else if(nLeakCheckFunc == eGasLineLeckCheck)
		{
			g_LKParameter[eGasLineLeckCheck].dbRghOpenAndLCDelayTime	= READ_ANALOG( LnBaseDelayTimeAM,	&nIOStatus);
			g_LKParameter[eGasLineLeckCheck].dbLCBasePressure	= READ_ANALOG( LnLkChkBasePressAM,	&nIOStatus);
			g_LKParameter[eGasLineLeckCheck].dbLCLimitPres		= READ_ANALOG( LnLkChkLimitPressAM, &nIOStatus);
			g_LKParameter[eGasLineLeckCheck].dbLCPumpingTO		= READ_ANALOG( LnLkChkPumpTimeOutAM,&nIOStatus);
			g_LKParameter[eGasLineLeckCheck].dbLCDelayTime		= READ_ANALOG( LnLkChkDelayTimeAM,	&nIOStatus);
			g_LKParameter[eGasLineLeckCheck].dbLCMaxCount		= READ_ANALOG( LnLkChkCountSPAM,	&nIOStatus);
			g_LKParameter[eGasLineLeckCheck].dbRoRLimit			= READ_ANALOG( LnRoRLimitAM,		&nIOStatus);

			if( g_LKParameter[eGasLineLeckCheck].dbLCMaxCount	  < 1  ||
				g_LKParameter[eGasLineLeckCheck].dbLCMaxCount	  > 10 ||
				g_LKParameter[eGasLineLeckCheck].dbLCBasePressure <= 0 ||
				g_LKParameter[eGasLineLeckCheck].dbLCLimitPres	  <= 0 ||
				g_LKParameter[eGasLineLeckCheck].dbLCPumpingTO	  < 0  ||
				g_LKParameter[eGasLineLeckCheck].dbLCDelayTime	  < 0  ||
				g_LKParameter[eGasLineLeckCheck].dbRghOpenAndLCDelayTime < 0 /*||
				g_LKParameter[eGasLineLeckCheck].dbRoRLimit       <= 0*/
				) //... Check Invalid Parameters
			{
				msRet = SYS_ABORTED;
				break;
			}
		}
		else if(nLeakCheckFunc == eOtherLeckCheck)
		{
			g_LKParameter[eOtherLeckCheck].dbRghOpenAndLCDelayTime	= READ_ANALOG( UrBaseDelayTimeAM,	&nIOStatus);
			g_LKParameter[eOtherLeckCheck].dbLCBasePressure		= READ_ANALOG( UrLkChkBasePressAM,	&nIOStatus);
			g_LKParameter[eOtherLeckCheck].dbLCLimitPres		= READ_ANALOG( UrLkChkLimitPressAM, &nIOStatus);
			g_LKParameter[eOtherLeckCheck].dbLCPumpingTO		= READ_ANALOG( UrLkChkPumpTimeOutAM,&nIOStatus);
			g_LKParameter[eOtherLeckCheck].dbLCDelayTime		= READ_ANALOG( UrLkChkDelayTimeAM,	&nIOStatus);
			g_LKParameter[eOtherLeckCheck].dbLCMaxCount			= READ_ANALOG( UrLkChkCountSPAM,	&nIOStatus);
			g_LKParameter[eOtherLeckCheck].dbRoRLimit			= READ_ANALOG( UrRoRLimitAM,		&nIOStatus);

			if( g_LKParameter[eOtherLeckCheck].dbLCMaxCount		  < 1  ||
				g_LKParameter[eOtherLeckCheck].dbLCMaxCount		  > 10 ||
				g_LKParameter[eOtherLeckCheck].dbLCBasePressure	  <= 0 ||
				g_LKParameter[eOtherLeckCheck].dbLCLimitPres	  <= 0 ||
				g_LKParameter[eOtherLeckCheck].dbLCPumpingTO	  < 0  ||
				g_LKParameter[eOtherLeckCheck].dbLCDelayTime	  < 0  ||
				g_LKParameter[eOtherLeckCheck].dbRghOpenAndLCDelayTime < 0 /*||
				g_LKParameter[eOtherLeckCheck].dbRoRLimit		  <= 0*/
				) //... Check Invalid Parameters
			{
				msRet = SYS_ABORTED;
				break;
			}
		}
		else
		{
			printf("[Vaccum] UnKnown LeakCheck Parameter\n");
		}
	}while(0);

	WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, LC_STARTED, &nIOStatus);

	//.. Display Progress Time
	sprintf( pszProgressTime, "0 of %d (sec)", (int)(60 * g_LKParameter[nLeakCheckFunc].dbLCMaxCount));
	WRITE_STRING(LeakChkProgrssTimeSM, pszProgressTime, &nIOStatus);

	//... Display Progress Step
	sprintf( pszProgressStep, "0 of %d", (int)g_LKParameter[nLeakCheckFunc].dbLCMaxCount);
	WRITE_STRING( LeakChkProgrssStepSM, pszProgressStep, &nIOStatus);

	//... Leak Check Count Display
	WRITE_ANALOG( LeakChkCountAM, 0, &nIOStatus);

	//... Leak Check Start/End Time Display
	WRITE_STRING( LeakChkStartTimeSM, "", &nIOStatus);
	WRITE_STRING( LeakChkEndTimeSM, "", &nIOStatus);

	// Leak Rate Average Display
	WRITE_ANALOG( LeakChkAvgAM, 0, &nIOStatus);
	for( i=0; i < 10 ; i++)
	{
		WRITE_ANALOG( LkChkEndPress1AM + i, 0, &nIOStatus);
		WRITE_ANALOG( LkChkStartPress1AM + i, 0, &nIOStatus);
		WRITE_ANALOG( LeakChkRate1AM + i, 0, &nIOStatus);
	}


	return msRet;
}

//... 2016.01.19
Module_Status CheckBasePressure(BOOL bAbortGasUse, int nLeakCheckFunc)
{
	BOOL bAborted = FALSE;
	Module_Status msRet = SYS_ERROR;
	int nIOStatus;
	int i;
	int	nLeakCheckEnd = 0;
	double dbElapsedTime = 0;
	char pszFtnName[256];
	__KF_DECLARE_ALARM
	memset(pszFtnName, 0, sizeof(pszFtnName));

	for (i = 0 ; i < MAX_LK_PRAMETER ; i++)
	{
		g_LKParameter[i].dbChamberPressure = 0;
	}

	do
	{
		//... Just for safety
		if(bAbortGasUse == eUse)
		{
			__KF_BEGIN_ALARM
			msRet = RUN_FUNCTION(ABORT_GAS, "");

			if( SYS_SUCCESS != msRet )
			{
				_LOG("ABORT_GAS Function is failed");
				__SET_RECOVERY_CODE(ALARM_MANAGE(ABORT_GAS_FAIL))
				msRet = SYS_ABORTED;
			}
			__KF_END_ALARM
		}
		//... Turn On Dry pump Booster
		msRet = StartDPBooster();
		if( SYS_SUCCESS != msRet )
		{
			_LOG("StartDPBooster is failed");
			break;
		}

		//... Read chamber Baratron pressure
		g_LKParameter[nLeakCheckFunc].dbChamberPressure = READ_ANALOG(M651_Pressure, &nIOStatus);
		if(g_LKParameter[nLeakCheckFunc].dbChamberPressure > g_LKParameter[nLeakCheckFunc].dbLCBasePressure)
		{//... requires pumping

			WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, GO_VAC, &nIOStatus);

			//... Open Rough Valve
			WRITE_DIGITAL_COMMCHECK( pszFtnName, FastRoughVlvDO, EVlv_Open, &nIOStatus);
			if (SYS_SUCCESS != CheckRoughValveSts(RVlv_Open)) {
				return SYS_ABORTED;
			}

			__KF_BEGIN_ALARM
			msRet = RUN_FUNCTION(PRESS_CONTROL, "OPEN");
			if( SYS_SUCCESS != msRet )
			{
				_LOG("Throttle valve position Error [OPEN]");
				__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT))
				msRet = SYS_ABORTED;
			}
			__KF_END_ALARM
			msRet = WaitTillThrottlePosition( 100 );
			if( SYS_SUCCESS != msRet )
			{
				_LOG("WaitTillThrottlePosition(100) is Failed");
				break;
			}

			bAborted = MANAGER_ABORT();
			if( TRUE == bAborted )
			{
				_LOG("Vent is aborted");
				msRet = SYS_ABORTED;
				break;
			}

			g_LKParameter[nLeakCheckFunc].dbLCPumpingTO = g_LKParameter[nLeakCheckFunc].dbLCPumpingTO * 1000;  //... Converting to milli-seconds
			__KF_BEGIN_ALARM
			dbElapsedTime = 0;
			do
			{
				if( FALSE == WAIT_SECONDS(1))
				{
					_LOG("Function is aborted");
					__SET_RECOVERY_CODE(_ALM_ABORT);
					msRet = SYS_ABORTED;
					break;
				}
				dbElapsedTime = dbElapsedTime + 1000;
				g_LKParameter[nLeakCheckFunc].dbChamberPressure = READ_ANALOG(M651_Pressure, &nIOStatus);
				if(g_LKParameter[nLeakCheckFunc].dbChamberPressure <= g_LKParameter[nLeakCheckFunc].dbLCBasePressure)
				{
					break;
				}
				//... Check Time Out
				if(dbElapsedTime > g_LKParameter[nLeakCheckFunc].dbLCPumpingTO)
				{
					__SET_RECOVERY_CODE(ALARM_MANAGE(FAST_ROUGHING_TIMED_OUT));
					msRet = SYS_ABORTED;
					break;
				}
			}while(1);
			__KF_END_ALARM

			WRITE_DIGITAL_COMMCHECK( pszFtnName, VacuumStateDM, PM_VACUUM, &nIOStatus);
		}

		bAborted = MANAGER_ABORT();
		if( TRUE == bAborted )
		{
			_LOG("Vent is aborted");
			msRet = SYS_ABORTED;
			break;
		}

		nLeakCheckEnd = READ_DIGITAL_COMMCHECK( pszFtnName, LeakChkStopDM, &nIOStatus);
		if( LeakCheck_Stop == nLeakCheckEnd )
		{
			_LOG("Leak Check is stopped");
			g_LKParameter[nLeakCheckFunc].bLeakCheckStopped = TRUE;
			msRet = SYS_SUCCESS;
			break;
		}
	} while (0);

	printf("Leaved %s(%d)\n", pszFtnName, msRet);

	return msRet;
}
//////////////////////////////////////////////////////////////////////////
//... 2016.05.12
void MFCPurgeLineOpen(int nPurIdex)
{
	char szArLineSeqCmd[256] = {"0",};
	int nIndex = 0;

	nIndex = nPurIdex + 1;

	sprintf(szArLineSeqCmd,"MFC_PUREG_LINE%d_OP",nIndex);
	g_ValveSeq.StartValveSeq(szArLineSeqCmd,TRUE);
}
//////////////////////////////////////////////////////////////////////////
BOOL IsOKMfcPresRate()
{
	BOOL bRet = TRUE;
	int nIOStatus;
	double dbPARM_MFCPres = 0;

	//for Test
	dbPARM_MFCPres = 50;

	for(int i = 0; i < MAX_MFC; i++)
	{
		if(dbPARM_MFCPres > READ_ANALOG(MFC01PressAI + i,&nIOStatus)){bRet = FALSE; break;}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
BOOL IsOkMFCPurge()
{
	BOOL bRet = FALSE;
	int nPurIdex = 0;
	int nIOStatus;
	double dbTimeOut = 0;
	double dbParamTimeOut = 0;

	//... Read Parameter
	dbParamTimeOut = READ_ANALOG(MFC_PurgeTmAM,&nIOStatus);
	if(dbParamTimeOut <= 0) return bRet;

	do
	{
		//... Open MFC Purge Line
		MFCPurgeLineOpen(nPurIdex);

		//... initialize time
		S_TIMER_READY();
		do
		{
			//... Check System Abort
			if(!WAIT_SECONDS(1)) {bRet = FALSE; break;}

			dbTimeOut = S_TIMER_ELAPSED();

			//... Check Purge Time
			if(dbTimeOut > dbParamTimeOut) {bRet = TRUE; break;}
		}while(TRUE);

		//... Close MFC Purge Line
		g_ValveSeq.StartValveSeq("CLOSE_LINE_VLV",TRUE);

		if(bRet == FALSE) break;

		//... Check MFC Pressure Rate
		if(TRUE == IsOKMfcPresRate())
		{
			return TRUE;
		}
	}
	while(++nPurIdex < MAX_PURGE_LINE);

	return bRet;
}
//////////////////////////////////////////////////////////////////////////
Module_Status DoLeakCheck(int nLeakCheckFunc,int nLineLeakCheckSeq)
{
	Module_Status msRet = SYS_ERROR;
	char pszFtnName[256];
	char pszProgressStep[256];
	char pszProgressTime[256];
	double dbLCCount = 0;
	double dbStartPressure = 0;
	double dbElapsedTime = 0;
	double dbLCDurationTime	= 60;
	double dbEndPressure = 0;
	double dbSumLeakRate = 0;
	double dbAvgLeakRate = 0;
	double dbLeakRate = 0;
	double dbUpPosition = 0;
	double dbDownPosition = 0;
	int nIOStatus;
	int i,j;
	int nLeakCheckEnd = 0;
	int nUseMtrDynLK = eNotUse;
	int nUseALDVlvDynLK = eNotUse;
	BOOL bLeakCheckOK = TRUE;
	int nTimeout = 0;  //... 2016.04.04
	char szAlarmBuffer[256];

	__KF_DECLARE_ALARM
	memset(pszFtnName,0,sizeof(pszFtnName));
	memset(pszProgressStep,0,sizeof(pszProgressStep));
	memset(pszProgressTime,0,sizeof(pszProgressTime));
	memset(szAlarmBuffer,0,sizeof(szAlarmBuffer));

	for (i = 0 ; i < MAX_LK_PRAMETER ; i++)
	{
		g_LKParameter[i].dbAvgLeakRate = 0;
		g_LKParameter[i].dbAvgRate    = 0;
	}

	do
	{
		//... Delay time after Base Pressure Check
		if( FALSE == WAIT_SECONDS(g_LKParameter[nLeakCheckFunc].dbRghOpenAndLCDelayTime) )
		{
			_LOG("Rough Valve Open Leak Check delay function is aborted.");
			msRet = SYS_ABORTED;
			break;
		}

		//... Close Rough Valve
		WRITE_DIGITAL_COMMCHECK( pszFtnName, FastRoughVlvDO, EVlv_Close, &nIOStatus);
		if (SYS_SUCCESS != CheckRoughValveSts(RVlv_Close)) {
			return SYS_ABORTED;
		}

		__KF_BEGIN_ALARM
		// msRet = RUN_FUNCTION(PRESS_CONTROL, "CLOSE");
		// OPEN throttle valve
		// Sequence Change
		msRet = RUN_FUNCTION(PRESS_CONTROL, "OPEN");
		if( SYS_SUCCESS != msRet )
		{
			_LOG("Throttle valve position Error[OPEN]");
			__SET_RECOVERY_CODE(ALARM_MANAGE(THROTTLE_VALVE_POSITION_INCORRECT))
				msRet = SYS_ABORTED;
		}
		__KF_END_ALARM
		msRet = WaitTillThrottlePosition(100);
		if( SYS_SUCCESS != msRet )
		{
			_LOG("WaitTillThrottlePosition(100) is Failed");
			break;
		}
		//...

		//... Rough Valve Close And Leak check delay
		if( FALSE == WAIT_SECONDS(g_LKParameter[nLeakCheckFunc].dbLCDelayTime) )
		{
			_LOG("After Leak Check delay function is aborted.");
			msRet = SYS_ABORTED;
			break;
		}

		//... 2016.04.04 Dynamic Leak Check Start
		nUseMtrDynLK = READ_DIGITAL(DyALECHK_MTRDM, &nIOStatus);
		nUseALDVlvDynLK = READ_DIGITAL(DyALECHK_VLVDM, &nIOStatus);

		if((nLeakCheckFunc == eNormalLeakCheck && nUseMtrDynLK == eUse)
			|| (nUseALDVlvDynLK == eUse && nLineLeakCheckSeq == ePURGELINE))
		{
			for(i = 0; i < eGasLineLeckCheck ; i++)
			{
				if(g_DynamicPara[i].nLeakCheckType == nLeakCheckFunc)
				{
					_LOG("Dynamic Leak check Start Cmd %s",g_DynamicPara[i].pCmd);
					RUN_SET_FUNCTION(g_DynamicPara[i].nFunctionIndex,g_DynamicPara[i].pCmd);
				}
			}
		}

		for(i= 0 ; i< (int)g_LKParameter[nLeakCheckFunc].dbLCMaxCount ; i++)
		{
			//... Updating Leak check step status
			WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, i+1, &nIOStatus);

			//... display progressed steps.
			sprintf( pszProgressStep, "%d of %d", i+1, (int)g_LKParameter[nLeakCheckFunc].dbLCMaxCount);
			WRITE_STRING( LeakChkProgrssStepSM, pszProgressStep, &nIOStatus);

			dbStartPressure = READ_ANALOG(M651_Pressure, &nIOStatus);
			WRITE_ANALOG( LkChkStartPress1AM + i,dbStartPressure, &nIOStatus);
			if( dbStartPressure > g_LKParameter[nLeakCheckFunc].dbLCLimitPres)
			{
				_LOG("Start Pressure[%f] is Over Limit Pressure[%f]",
									 dbStartPressure, g_LKParameter[nLeakCheckFunc].dbLCLimitPres);
				bLeakCheckOK = FALSE;
				ALARM_POST(ALARM_START_PRESSURE_LIMIT);
				msRet = SYS_ERROR;
				break;
			}

			dbElapsedTime = 0;
			do
			{
				if( FALSE == WAIT_SECONDS(1) )
				{
					_LOG("Function is aborted");
					msRet = SYS_ABORTED;
					bLeakCheckOK = FALSE;
					break;
				}
				dbElapsedTime++;

				//... display progressed time
				sprintf( pszProgressTime, "%d of %d (sec)", (int)dbElapsedTime + 60*i, (int)(60 *  g_LKParameter[nLeakCheckFunc].dbLCMaxCount));
				WRITE_STRING(LeakChkProgrssTimeSM, pszProgressTime, &nIOStatus);

				if(dbElapsedTime > dbLCDurationTime)
					break;
			}while(1);
			if( FALSE == bLeakCheckOK ) break;

			dbEndPressure = READ_ANALOG(M651_Pressure, &nIOStatus);
			WRITE_ANALOG( LkChkEndPress1AM + i, dbEndPressure, &nIOStatus);

			//dbLeakRate = fabs(dbEndPressure - dbStartPressure) * 1000; // mTorr/min
			//... 2016.05.18
			dbLeakRate = (dbEndPressure - dbStartPressure) * 1000;
			g_LKParameter[nLeakCheckFunc].dbLastRate = dbLeakRate;
			WRITE_ANALOG( LeakChkRate1AM + i, dbLeakRate, &nIOStatus);

			dbSumLeakRate = dbSumLeakRate + dbLeakRate;

			if( dbEndPressure > g_LKParameter[nLeakCheckFunc].dbLCLimitPres )
			{
				_LOG("End Pressure[%f] is Over Limit Pressure[%f]",
									 dbEndPressure, g_LKParameter[nLeakCheckFunc].dbLCLimitPres);
				ALARM_POST(LEAK_CHECK_LIMIT_PRESS_OVER);
				msRet = SYS_ERROR;
				break;
			}

			nLeakCheckEnd = READ_DIGITAL_COMMCHECK( pszFtnName, LeakChkStopDM, &nIOStatus);
			if( LeakCheck_Stop == nLeakCheckEnd )
			{
				_LOG("Leak Check is stopped");
				g_LKParameter[nLeakCheckFunc].bLeakCheckStopped = TRUE;
				msRet = SYS_SUCCESS;
				break;
			}

		}

		//... 2016.04.04 Waiting Dynamic Leak Check
		if((nLeakCheckFunc == eNormalLeakCheck && nUseMtrDynLK == eUse)
			|| (nUseALDVlvDynLK == eUse && nLineLeakCheckSeq == ePURGELINE))
		{
			for(j = 0; j < eGasLineLeckCheck; j++)
			{
				if(g_DynamicPara[j].nLeakCheckType == nLeakCheckFunc) break;
			}

			if(j == 2) break;

			if(SYS_RUNNING == READ_FUNCTION(g_DynamicPara[j].nFunctionIndex))
			{
				if(!WAIT_SECONDS(1))
				{
					bLeakCheckOK = FALSE;
					break;
				}

				ALARM_MESSAGE_GET(ALARM_DYNAMICLK_FAILL,szAlarmBuffer);
				strcat(szAlarmBuffer,g_DynamicPara[j].pAlarmDes);
				ALARM_MESSAGE_SET(ALARM_DYNAMICLK_FAILL,szAlarmBuffer);
				ALARM_POST(ALARM_DYNAMICLK_FAILL);

				RUN_FUNCTION_ABORT(g_DynamicPara[j].nFunctionIndex);

				if(nUseMtrDynLK == eUse) RUN_SET_FUNCTION_FREE(CHUCK_CONTROL,"HOME 2");
				break;
			}
		}

	} while (0);

	if( FALSE == bLeakCheckOK ) msRet = SYS_ABORTED;


	//... Total Leak Check Step Count Display
	dbLCCount = (double)i;
	WRITE_ANALOG( LeakChkCountAM, dbLCCount, &nIOStatus);

	//... Leak Rate Average Display
	//... g_LKParameter[nLeakCheckFunc].dbAvgLeakRate = dbSumLeakRate / i;
	//... 2016.05.18
	if(dbSumLeakRate != 0 && i != 0)
	{
		g_LKParameter[nLeakCheckFunc].dbAvgRate    = dbSumLeakRate / i;
		//... 17L 요청 사항: Limit Torr Rate은 -3 Torr 까지 가능하지만 최소한의 Inner Leak 사고는 방지 하자
		if(g_LKParameter[nLeakCheckFunc].dbRoRLimit  < g_LKParameter[nLeakCheckFunc].dbAvgRate
			|| -3 > g_LKParameter[nLeakCheckFunc].dbAvgRate)
		{
			_LOG("ROR Rate[%f] is Over Limit ROR[%f]",
				g_LKParameter[nLeakCheckFunc].dbAvgRate, g_LKParameter[nLeakCheckFunc].dbRoRLimit);
			ALARM_POST(LEAK_CHECK_LIMIT_ROR_OVER);
			msRet = SYS_ERROR;
		}
	}
	WRITE_ANALOG( LeakChkAvgAM, g_LKParameter[nLeakCheckFunc].dbAvgRate, &nIOStatus);
	WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, LC_COMPLETED, &nIOStatus);

	return msRet;
}
//------------------------------------------------------------------------------------------
BOOL bIsGasLineLeakCheckSeq(int nLineLeakCheckSeq)
{
	int nIOStatus = 0;
	BOOL bRet = FALSE;

	switch(nLineLeakCheckSeq)
	{
	case ePURGELINE:
		if(eUse == READ_DIGITAL(PurgeLineLK_DM,&nIOStatus)) bRet = TRUE;
		break;
	case eCLEANLINE:
		if(eUse == READ_DIGITAL(CleanLineLK_DM,&nIOStatus)) bRet = TRUE;
		break;
	case eRESET:
		bRet = TRUE;
		break;
	default:
		bRet = FALSE;
	}
	return bRet;
}
//------------------------------------------------------------------------------------------
//... 2016.01.19
Module_Status GasLineLeakCheck()
{
	char pszLocFtnName[] = "GasLine_LeakCheck";
	char pszFtnName[256];
	char pszStartTime[256];
	char pszEndTime[256];
	Module_Status msRet = SYS_SUCCESS;;
	SYSTEMTIME strtSysTime;
	double dbCurPressure = 0;
	int nIOStatus = 0;
	int nData     = -1;
	int nLineLeakCheckSeq = 0;

	__KF_DECLARE_ALARM

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{

		if(!bIsGasLineLeakCheckSeq(nLineLeakCheckSeq)) continue;

		if(CheckDryPump() == FALSE)
		{
			printf("DryPump failed in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

        if(SYS_SUCCESS != ReadLeakCheckParameter(eGasLineLeckCheck))
		{
			printf("Parameter Read Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

        if(SYS_SUCCESS != CheckBasePressure(eUse,eGasLineLeckCheck))
		{
			printf("Base Pressure check Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

		//... 2016.04.04 Valve Open
		if(nLineLeakCheckSeq == ePURGELINE)		g_ValveSeq.StartValveSeq("PURGE_LINE_LK",TRUE);
		else/*nLineLeakCheckSeq == eCLEANLINE*/ g_ValveSeq.StartValveSeq("CLEAN_LINE_LK",TRUE);

		//... Get Start Leak Check Time
		GetLocalTime( &strtSysTime );
		sprintf(pszStartTime, "%04d.%02d.%02d %02d:%02d:%02d:%03d(ms)",
															strtSysTime.wYear,
															strtSysTime.wMonth,
															strtSysTime.wDay,
															strtSysTime.wHour,
															strtSysTime.wMinute,
															strtSysTime.wSecond,
															strtSysTime.wMilliseconds);
		if(SYS_SUCCESS != DoLeakCheck(eGasLineLeckCheck,nLineLeakCheckSeq))
		{
			printf("Leak Check Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

		//... 2016.04.04 Valve Close
		g_ValveSeq.StartValveSeq("CLOSE_LINE_VLV",TRUE);

		//... Get End Leak Check Time
		GetLocalTime( &strtSysTime );
		sprintf(pszEndTime, "%04d.%02d.%02d %02d:%02d:%02d:%03d(ms)",
															strtSysTime.wYear,
															strtSysTime.wMonth,
															strtSysTime.wDay,
															strtSysTime.wHour,
															strtSysTime.wMinute,
															strtSysTime.wSecond,
															strtSysTime.wMilliseconds);


		WRITE_STRING( LeakChkStartTimeSM, pszStartTime, &nIOStatus);
		WRITE_STRING( LeakChkEndTimeSM, pszEndTime, &nIOStatus);

		//... Leak Check Start/End Time Display
		CLeakCheckHistory::GetInstance()->SaveLeakCheckHistory(g_LKParameter[eGasLineLeckCheck].dbLastRate , g_LKParameter[eGasLineLeckCheck].dbAvgRate, eGasLineLeckCheck, nLineLeakCheckSeq);

		dbCurPressure = READ_ANALOG( M651_Pressure, &nIOStatus);
		if( dbCurPressure  <= 10 ) FastRough();
		else Pump();

	}while(++nLineLeakCheckSeq < MAX_LINE_LK_SEQ);

	g_ValveSeq.StartValveSeq("CLOSE_LINE_VLV",TRUE);

	dbCurPressure = READ_ANALOG( M651_Pressure, &nIOStatus);
	if( dbCurPressure  <= 10 ) FastRough();
	else Pump();

	bIsGasLineLeakCheckSeq(eRESET);

	WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStopDM, LeakCheck_Normal, &nIOStatus);
	if( SYS_SUCCESS != msRet )
		WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, LC_ERROR, &nIOStatus);
	else if( TRUE == g_LKParameter[eGasLineLeckCheck].bLeakCheckStopped )
		WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, LC_COMPLETED, &nIOStatus);

	printf("Leave %s\n", pszFtnName);
	return msRet;
}
//------------------------------------------------------------------------------------------
//... 2016.01.19
Module_Status UserModeLeakCheck()
{
	char pszLocFtnName[] = "UserMode_LeakCheck";
	char pszFtnName[256];
	char pszStartTime[256];
	char pszEndTime[256];
	Module_Status msRet = SYS_SUCCESS;
	SYSTEMTIME strtSysTime;
	double dbCurPressure = 0;
	int nIOStatus = 0;
	int nData     = -1;

	__KF_DECLARE_ALARM

		sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		if(CheckDryPump() == FALSE)
		{
			printf("DryPump failed in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

        if(SYS_SUCCESS != ReadLeakCheckParameter(eOtherLeckCheck))
		{
			printf("Parameter Read Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

        if(SYS_SUCCESS != CheckBasePressure(eNotUse,eOtherLeckCheck))
		{
			printf("Base Pressure check Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

		//... Get Start Leak Check Time
		GetLocalTime( &strtSysTime );
		sprintf(pszStartTime, "%04d.%02d.%02d %02d:%02d:%02d:%03d(ms)",
															strtSysTime.wYear,
															strtSysTime.wMonth,
															strtSysTime.wDay,

															strtSysTime.wHour,
															strtSysTime.wMinute,
															strtSysTime.wSecond,
															strtSysTime.wMilliseconds);


        if(SYS_SUCCESS != DoLeakCheck(eOtherLeckCheck,eRESET))
		{
			printf("Leak Check Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

		//... Get End Leak Check Time
		GetLocalTime( &strtSysTime );
		sprintf(pszEndTime, "%04d.%02d.%02d %02d:%02d:%02d:%03d(ms)",
															strtSysTime.wYear,
															strtSysTime.wMonth,
															strtSysTime.wDay,

															strtSysTime.wHour,
															strtSysTime.wMinute,
															strtSysTime.wSecond,
															strtSysTime.wMilliseconds);


		WRITE_STRING( LeakChkStartTimeSM, pszStartTime, &nIOStatus);
		WRITE_STRING( LeakChkEndTimeSM, pszEndTime, &nIOStatus);

		//... Leak Check Start/End Time Display
		CLeakCheckHistory::GetInstance()->SaveLeakCheckHistory(g_LKParameter[eOtherLeckCheck].dbLastRate , g_LKParameter[eOtherLeckCheck].dbAvgRate, eOtherLeckCheck, eRESET);

	}while(0);

	g_ValveSeq.StartValveSeq("CLOSE_LINE_VLV",TRUE);

	dbCurPressure = READ_ANALOG( M651_Pressure, &nIOStatus);
	if( dbCurPressure  <= 10 ) FastRough();
	else Pump();

	WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStopDM, LeakCheck_Normal, &nIOStatus);
	if( SYS_SUCCESS != msRet )
		WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, LC_ERROR, &nIOStatus);
	else if( TRUE == g_LKParameter[eOtherLeckCheck].bLeakCheckStopped )
		WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, LC_COMPLETED, &nIOStatus);

	printf("Leave %s\n", pszFtnName);
	return msRet;
}
//------------------------------------------------------------------------------------------
//... 2015.09.11
Module_Status VlvLeakCheck()
{
	char pszLocFtnName[] = "ALDVlv_LeakCheck";
	char pszFtnName[256];
	char pszStartTime[256];
	char pszEndTime[256];
	Module_Status msRet = SYS_SUCCESS;
	SYSTEMTIME strtSysTime;
	double dbCurPressure = 0;
	int nIOStatus = 0;
	int nData     = -1;

	__KF_DECLARE_ALARM

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		if(CheckDryPump() == FALSE)
		{
			printf("DryPump failed in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

        if(SYS_SUCCESS != ReadLeakCheckParameter(eALDValveLeakCheck))
		{
			printf("Parameter Read Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

        if(SYS_SUCCESS != CheckBasePressure(eUse,eALDValveLeakCheck))
		{
			printf("Base Pressure check Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

		//... Get Start Leak Check Time
		GetLocalTime( &strtSysTime );
		sprintf(pszStartTime, "%04d.%02d.%02d %02d:%02d:%02d:%03d(ms)",
															strtSysTime.wYear,
															strtSysTime.wMonth,
															strtSysTime.wDay,

															strtSysTime.wHour,
															strtSysTime.wMinute,
															strtSysTime.wSecond,
															strtSysTime.wMilliseconds);

		//... 2016.04.04 ALD Valve Open
		g_ValveSeq.StartValveSeq("ALD_LEAK_CHK",TRUE);

        if(SYS_SUCCESS != DoLeakCheck(eALDValveLeakCheck,eRESET))
		{
			printf("Leak Check Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

		//... Get End Leak Check Time
		GetLocalTime( &strtSysTime );
		sprintf(pszEndTime, "%04d.%02d.%02d %02d:%02d:%02d:%03d(ms)",
															strtSysTime.wYear,
															strtSysTime.wMonth,
															strtSysTime.wDay,

															strtSysTime.wHour,
															strtSysTime.wMinute,
															strtSysTime.wSecond,
															strtSysTime.wMilliseconds);

		//... 2016.04.04. ALD Valve Close
		g_ValveSeq.StartValveSeq("CLOSE_LINE_VLV",TRUE);

		WRITE_STRING( LeakChkStartTimeSM, pszStartTime, &nIOStatus);
		WRITE_STRING( LeakChkEndTimeSM, pszEndTime, &nIOStatus);

		//... Leak Check Start/End Time Display
		CLeakCheckHistory::GetInstance()->SaveLeakCheckHistory(g_LKParameter[eALDValveLeakCheck].dbLastRate , g_LKParameter[eALDValveLeakCheck].dbAvgRate, eALDValveLeakCheck, eRESET);
	}while(0);

	g_ValveSeq.StartValveSeq("CLOSE_LINE_VLV",TRUE);

	dbCurPressure = READ_ANALOG( M651_Pressure, &nIOStatus);
	if( dbCurPressure  <= 10 ) FastRough();
	else Pump();

	WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStopDM, LeakCheck_Normal, &nIOStatus);
	if( SYS_SUCCESS != msRet )
		WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, LC_ERROR, &nIOStatus);
	else if( TRUE == g_LKParameter[eALDValveLeakCheck].bLeakCheckStopped )
		WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, LC_COMPLETED, &nIOStatus);

	printf("Leave %s\n", pszFtnName);
	return msRet;
}
//------------------------------------------------------------------------------------------
Module_Status LeakCheck()
{
	char pszLocFtnName[] = "LeakCheck";
	char pszFtnName[256];
	char pszStartTime[256];
	char pszEndTime[256];
	Module_Status msRet = SYS_SUCCESS;
	SYSTEMTIME strtSysTime;
	double dbCurPressure = 0;
	int nIOStatus = 0;
	int nData     = -1;

	__KF_DECLARE_ALARM

		sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do
	{
		if(CheckDryPump() == FALSE)
		{
			printf("DryPump failed in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

        if(SYS_SUCCESS != ReadLeakCheckParameter(eNormalLeakCheck))
		{
			printf("Parameter Read Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

        if(SYS_SUCCESS != CheckBasePressure(eUse,eNormalLeakCheck))
		{
			printf("Base Pressure check Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

		//... Get Start Leak Check Time
		GetLocalTime( &strtSysTime );
		sprintf(pszStartTime, "%04d.%02d.%02d %02d:%02d:%02d:%03d(ms)",
			strtSysTime.wYear,
			strtSysTime.wMonth,
			strtSysTime.wDay,

			strtSysTime.wHour,
			strtSysTime.wMinute,
			strtSysTime.wSecond,
			strtSysTime.wMilliseconds);

        if(SYS_SUCCESS != DoLeakCheck(eNormalLeakCheck,eRESET))
		{
			printf("Leak Check Fail in %s\n", pszFtnName);
			msRet = SYS_ABORTED;
			break;
		}

		//... Get End Leak Check Time
		GetLocalTime( &strtSysTime );
		sprintf(pszEndTime, "%04d.%02d.%02d %02d:%02d:%02d:%03d(ms)",
			strtSysTime.wYear,
			strtSysTime.wMonth,
			strtSysTime.wDay,

			strtSysTime.wHour,
			strtSysTime.wMinute,
			strtSysTime.wSecond,
			strtSysTime.wMilliseconds);

		WRITE_STRING( LeakChkStartTimeSM, pszStartTime, &nIOStatus);
		WRITE_STRING( LeakChkEndTimeSM, pszEndTime, &nIOStatus);

		//... Leak Check Start/End Time Display
		CLeakCheckHistory::GetInstance()->SaveLeakCheckHistory(g_LKParameter[eNormalLeakCheck].dbLastRate , g_LKParameter[eNormalLeakCheck].dbAvgRate, eNormalLeakCheck, eRESET);
	}while(0);

	dbCurPressure = READ_ANALOG( M651_Pressure, &nIOStatus);
	if( dbCurPressure  <= 10 ) FastRough();
	else Pump();

	WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStopDM, LeakCheck_Normal, &nIOStatus);
	if( SYS_SUCCESS != msRet )
		WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, LC_ERROR, &nIOStatus);
	else if( TRUE == g_LKParameter[eNormalLeakCheck].bLeakCheckStopped )
		WRITE_DIGITAL_COMMCHECK( pszFtnName, LeakChkStsDM, LC_COMPLETED, &nIOStatus);

	printf("Leave %s\n", pszFtnName);
	return msRet;
}
//////////////////////////////////////////////////////////////////////////
//... 2016.05.12
Module_Status StartLeakCheckMacro(int nMacroType)
{
	Module_Status msRet = SYS_ABORTED;
	int nIOStatus;

	_LOG("Start LeakCheck Macro%d ",nMacroType);
	WRITE_DIGITAL(LEAK_ModeDM,nMacroType,&nIOStatus);

	switch(nMacroType)
	{
	case eLeakCheck:
		_LOG("Start Normal LeakCheck");
		msRet = LeakCheck();
		break;
	case eLnLeckCheck:
		_LOG("Start GasLine LeakCheck");
		msRet = GasLineLeakCheck();
		break;
	}

	if(msRet != SYS_SUCCESS)
	{
		_LOG("Fail LeakCheck Macro%d ",nMacroType);
	}
	return msRet;
}
//////////////////////////////////////////////////////////////////////////
Module_Status SysLeakCheck()
{
	Module_Status msRet = SYS_ABORTED;
	int nIOStatus;
	int nIdex;
	int nMacroType = 0;
	int i;
	printf("[Vacuum Function]Entered System Leak Check \n");

	//1. Leak Check Start (Normal & Gas Line Leak Check)
	for(i = 0; i <MAX_MACRO_IO; i++)
	{
		nMacroType = READ_DIGITAL(LKMacroType1DM + i,&nIOStatus);
		printf("Select LeakCheck Macro%d Option\n",nMacroType);

		for( nIdex = 0; nIdex < MAX_LK_MACRO; nIdex++)
		{
			if(nMacroType == g_LeakCheckMacro[nIdex].nMacroIndex)
			{
				if(eUse == READ_DIGITAL(g_LeakCheckMacro[nIdex].nIOLeakChkSts,&nIOStatus))
				{
					msRet = StartLeakCheckMacro(nMacroType);
					if(msRet != SYS_SUCCESS)
					{
						//... 2016.08.16
						WRITE_DIGITAL(SW_GbIntlckDM,1,&nIOStatus);
						return SYS_ABORTED;
					}
				}
			}
		}
	}

	//2. Check Leak Check Status
	//if(msRet != SYS_SUCCESS) return SYS_ABORTED;

	//3. MFC Manual Purge
	if(eUse == READ_DIGITAL(ALECHK_GASLnDM,&nIOStatus))
	{
		if(FALSE == IsOkMFCPurge())
		{
			RUN_FUNCTION_FREE(ABORT_GAS, "");
			msRet = SYS_ABORTED;
			_LOG("IsMFCPurge Sequence is failed");
		}
	}

	return msRet;
}
//------------------------------------------------------------------------------------------
// 2004.07.27 by cskim, Add 'Pressure Check Function'
Module_Status CheckTransPres()
{
	Module_Status msRet = SYS_ABORTED;
	char pszLocFtnName[] = "CheckTransPres";
	char pszFtnName[256];
	int nIOStatus = 0;
	int nAlarmRecovery;
	double fChmPres = 10;
	double fTransPres = 0;
	int nIsoVlvClose = 0;
	int nErrCnt = 0;
	BOOL bAbortEvent = FALSE;

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);
	do {
		nErrCnt = 0;
		do {
			fTransPres = READ_ANALOG(PRMA_VS_TrPrs, &nIOStatus);
			nIsoVlvClose = READ_DIGITAL(TMIsoVlvClosedDI, &nIOStatus);	// 0:False 1:True
			if(fTransPres == 0 || nIsoVlvClose == 0)
			{
				msRet = SYS_SUCCESS;
				break;
			}
			fChmPres = GetChamberPressure();
			if(fChmPres <= fTransPres) { msRet = SYS_SUCCESS; break; }
			if( ! WAIT_SECONDS(1) ) { bAbortEvent = TRUE; break; }
		} while(++nErrCnt < 5);
		if(bAbortEvent == TRUE) break;

		if(msRet != SYS_SUCCESS)
		{
			nAlarmRecovery = ALARM_MANAGE(ALARM_TRANS_PRESS_HIGH);
			if(nAlarmRecovery == ALM_IGNORE) {msRet = SYS_SUCCESS; break; }
			if(nAlarmRecovery != ALM_RETRY) break;
			if(! WAIT_SECONDS(1)) break;
		}
		else break;
	} while(1);
	printf("Leaved %s\n", pszFtnName);
	return msRet;
}

//////////////////////////////////////////////////////////////////////////
//... 2015.03.24 Add Gauge Isolation Valve Auto OPEN
void GaugeIsoVlvAutoOPEN(int nAction)
{
	int	nIOStatus;

	if(eACT_PUMP == nAction)
	{
		//... Check Foreline Isolation Valve OPEN
		if(EVlv_Open != READ_DIGITAL(PumplnCVGIsoVlvDO, &nIOStatus))
			WRITE_DIGITAL(PumplnCVGIsoVlvDO, EVlv_Open, &nIOStatus);

		//... Check Vacuum Switch Isolation Valve OPEN
		if(EVlv_Open != READ_DIGITAL(VacSwIsoVlvDO, &nIOStatus))
			WRITE_DIGITAL(VacSwIsoVlvDO, EVlv_Open, &nIOStatus);

		//... Check Process Gauge Isolation Valve OPEN
		if(EVlv_Open != READ_DIGITAL(ProcBGIsoVlvDO, &nIOStatus))
			WRITE_DIGITAL(ProcBGIsoVlvDO, EVlv_Open, &nIOStatus);
	}
	else if(eACT_VENT == nAction)
	{
		//... Check Vacuum Switch Isolation Valve OPEN
		if(EVlv_Open != READ_DIGITAL(VacSwIsoVlvDO, &nIOStatus))
			WRITE_DIGITAL(VacSwIsoVlvDO, EVlv_Open, &nIOStatus);
	}
}

//////////////////////////////////////////////////////////////////////////
void ParseVacuumControl(char *szData)
{
	char szItem[40] = "";
	char szValue[40] = "";
	float fTemp = 10;
	CTextParser tp(szData);

	do {
		if(! tp.GetWord(szItem, 0))		break;

		if(strcmp(szItem, "Baratron_Guage_Max") == 0)
		{
			if(tp.GetFloat(&fTemp, 0))		g_dbBrGgMax = fTemp;
		}
		else if(strcmp(szItem, "DP_Booster_Use") == 0)		tp.GetBOOL(&g_bDPBooster,		0);
		//... 2015.03.23 Add Check Teach Mode for Safety PLC
		else if(strcmp(szItem, "SafetyPLC_Use") == 0)		tp.GetBOOL(&g_bSafetyPLC_Use,	0);

	} while(0);
}

void ParseVentMfcList(char *szData)
{
	int n, nIdx;
	CTextParser tp(szData);
	do {
		if(! tp.GetInt(&n, 0)) break;
		nIdx = n - 1;
		tp.GetInt(&g_VentMfc[nIdx].nMfcN, 0);
		tp.GetInt(&g_VentMfc[nIdx].nMfcFlow, 0);
		tp.GetInt(&g_VentMfc[nIdx].nValve1, 0);
		tp.GetInt(&g_VentMfc[nIdx].nValve2, 0);
		tp.GetInt(&g_VentMfc[nIdx].nValve3, 0);
		//2014.04.09
		tp.GetInt(&g_VentMfc[nIdx].nValve4, 0);
		tp.GetInt(&g_VentMfc[nIdx].nValve5, 0);
	} while(0);
}

void ParseVentMfcList2(char *szData)
{
	int n, nIdx;
	CTextParser tp(szData);
	do {
		if(! tp.GetInt(&n, 0)) break;
		nIdx = n - 1;
		tp.GetInt(&g_VentMfc2[nIdx].nMfcN, 0);
		tp.GetInt(&g_VentMfc2[nIdx].nMfcFlow, 0);
		tp.GetInt(&g_VentMfc2[nIdx].nValve1, 0);
		tp.GetInt(&g_VentMfc2[nIdx].nValve2, 0);
		tp.GetInt(&g_VentMfc2[nIdx].nValve3, 0);
		tp.GetInt(&g_VentMfc2[nIdx].nValve4, 0);
		tp.GetInt(&g_VentMfc2[nIdx].nValve5, 0);
	} while(0);
}

BOOL ReadPmcConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle;

	do {
		fp = fopen(PMC_CONFIG_FILE, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("Vacuum Control", szItem) == 0)			nTitle = 1;
				else if(strcmp("Vent MFC List", szItem) == 0)		nTitle = 2;
				else if(strcmp("Vent MFC List2", szItem) == 0)		nTitle = 3;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseVacuumControl(szRead); break;
					case 2: ParseVentMfcList(szRead); break;
					case 3: ParseVentMfcList2(szRead); break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : %s\n", PMC_CONFIG_FILE);
	}
	return bRet;
}

BOOL Initialize()
{
	BOOL bRet = TRUE;
	int i;
	for(i=0; i<5; i++)
	{
		g_VentMfc[i].nMfcN = 0;
		g_VentMfc[i].nMfcFlow = 1000;
		g_VentMfc[i].nValve1 = 0;
		g_VentMfc[i].nValve2 = 0;
		g_VentMfc[i].nValve3 = 0;
		//2014.04.09
		g_VentMfc[i].nValve4 = 0;
		g_VentMfc[i].nValve5 = 0;

		//... 2016.06.14
		g_VentMfc2[i].nMfcN = 0;
		g_VentMfc2[i].nMfcFlow = 1000;
		g_VentMfc2[i].nValve1 = 0;
		g_VentMfc2[i].nValve2 = 0;
		g_VentMfc2[i].nValve3 = 0;
		g_VentMfc2[i].nValve4 = 0;
		g_VentMfc2[i].nValve5 = 0;
	}

	ReadPmcConfig();
	return bRet;
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

extern "C"
{

Module_Status Program_Main() {
	char pszLocFtnName[] = "Program_Main";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int  nIOResult = 0;
	int  nData     = -1;

	char * pszParam = NULL;
	int  nParLen = -1;

	if(g_bInit == FALSE)	g_bInit = Initialize();

	sprintf( pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", APP_NAME);
	do
	{
		nData = READ_DIGITAL_COMMCHECK(pszFtnName, FC_VACUUM, &nIOResult);
		if( FALSE == nIOResult )  break;
		if( EO_OFF == nData )
		{
			msRet = SYS_SUCCESS;
			break;
		}

		pszParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pszParam);
		if(nParLen < 1 || nParLen >= 256)
		{
			_LOG("Param Length Error[%d]", nParLen);
			msRet = SYS_ERROR;
			break;
		}

		//... Leak Check Parameter Save
		if(STRCMP_L( pszParam, "LEAK_CHECK")||STRCMP_L( pszParam, "ALD_CHECK")
			||STRCMP_L( pszParam, "GAS_CHECK")||STRCMP_L( pszParam, "OTHER_CHECK"))
		{
			WRITE_DIGITAL(LeakChk_ParmSaveDM,1,&nIOResult);
		}

		//////////////////////////////////////////////////////////////////////////
		//... 2015.03.24 Add Process, Vac Switch, Foreline Gauge Isolation Valve Auto OPEN
		if(STRCMP_L(pszParam, "VENT"))			GaugeIsoVlvAutoOPEN(eACT_VENT);
		else									GaugeIsoVlvAutoOPEN(eACT_PUMP);

		//////////////////////////////////////////////////////////////////////////
		//... Main Run Routine
		if( STRCMP_L(pszParam, "PUMP"))
		{
			msRet = Pump();
			break;
		}
		else if( STRCMP_L( pszParam, "FAST_ROUGH"))
		{
			msRet = FastRough();
			break;
		}
		else if( STRCMP_L( pszParam, "VENT"))
		{
			msRet = Vent();
			break;
		}
		else if( STRCMP_L( pszParam, "LEAK_CHECK"))
		{
			WRITE_DIGITAL(LEAK_ModeDM,eNORMAL_LEAKCHK,&nIOResult);
			msRet = LeakCheck();
			break;
		}
		else if( STRCMP_L( pszParam, "ALD_CHECK"))
		{
			WRITE_DIGITAL(LEAK_ModeDM,eNORMAL_ALDCHK,&nIOResult);
			msRet = VlvLeakCheck();
			break;
		}
		//... 2016.01.19 Gas Line Leak Check
		else if( STRCMP_L( pszParam, "GAS_CHECK"))
		{
			WRITE_DIGITAL(LEAK_ModeDM,eNormal_GASLINE,&nIOResult);
			msRet = GasLineLeakCheck();
			break;
		}
		//... 2016.01.19 User Mode Leak Check
		else if( STRCMP_L( pszParam, "OTHER_CHECK"))
		{
			WRITE_DIGITAL(LEAK_ModeDM,eNormal_OTHERLK,&nIOResult);
			msRet = UserModeLeakCheck();
			break;
		}
		else if( STRCMP_L( pszParam, "CHECK_TR_PRS"))
		{
			msRet = CheckTransPres();
			break;
		}
		else if( STRCMP_L( pszParam, "FAST_ROUGH_&_CARRIER_AR_FLOW"))
		{
			g_ModuleControl.StopCarrierArFlow();
			msRet = FastRough();
			if(msRet == SYS_SUCCESS) g_ModuleControl.StartCarrierArFlow();
			break;
		}
		else if( STRCMP_L( pszParam, "START_CARRIER_AR_FLOW"))
		{
			g_ModuleControl.StartCarrierArFlow();
			msRet = SYS_SUCCESS;
		}
		else if( STRCMP_L( pszParam, "STOP_CARRIER_AR_FLOW"))
		{
			g_ModuleControl.StopCarrierArFlow();
			msRet = SYS_SUCCESS;
		}
		else if( STRCMP_L( pszParam, "SYSTEM_LEAK_CHECK"))
		{
			msRet = SysLeakCheck();
			break;
		}
		else
		{
			_LOG("Invalid Command [%s]", PROGRAM_PARAMETER_READ());
			msRet = SYS_ERROR;
			break;
		}
		msRet = SYS_SUCCESS;

	} while(0);

	//////////////////////////////////////////////////////////////////////////
	//... 2015.03.23 Add Check Teach Mode for Safety PLC
	if(msRet == SYS_SUCCESS)
	{
		if(! STRCMP_L(pszParam, "VENT"))
		{	//... Check Run Mode, Except VENT
			if(! CheckSafetyPLCMode(ePLCMODE_SYSRUN))
			{
				_LOG("[INFO] CheckSafetyPLCMode[%d] Failed in %s[%s]",
					ePLCMODE_SYSRUN,
					PROGRAM_FUNCTION_READ(), PROGRAM_PARAMETER_READ());
			}
		}
	}

	printf("Leave %s\n", APP_NAME);
	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {

	// 2006.10.26
	g_ModuleControl.Initialize();
	g_ValveSeq.Initialize("VacuumControl");

	CLeakCheckHistory::GetInstance()->Init();

	return TRUE;
}

} // End of [extern "C"]
//==========================================================================================
