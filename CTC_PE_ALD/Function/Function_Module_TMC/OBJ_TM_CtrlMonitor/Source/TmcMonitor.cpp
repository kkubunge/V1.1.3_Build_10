// TmcMonitor.cpp: implementation of the CTmcMonitor class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//====================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//====================================================================

#include "TextParser.h"
#include "TmcMonitor.h"
#include "IPS_Log.h"

//--------------------------------------------------------------------
// #include "TextLogDll.h"
extern void _LOG(const char *pFormat, ...);

//--------------------------------------------------------------------
#define TMC_CONFIG_FILE		".\\Function\\Function_Module_TMC\\TMCConfig.ini"

char	pszFtnName[256] = "";

//--------------------------------------------------------------------
#include "SharedMemory.h"
#define	S_MEMORY_SIZE	1024*8
#define SM_FILE_NAME	"POST_RECIPE_DATA_FILE"

//--------------------------------------------------------------------
#define APP_NAME	"Control Monitor"

#define PM1_NOT_CONNECTED					9901
#define PM2_NOT_CONNECTED					9902
#define PM3_NOT_CONNECTED					9903
#define PM4_NOT_CONNECTED					9904
#define PM5_NOT_CONNECTED					9905

// PWD Count
#define IN_TIME_SECOND		5
#define PRMD_PWD_SHOUR		23
#define PRMD_PWD_SMIN		59

#define MAX_PM_NUMBER		3
#define MAX_WAFER_IN_PM		2
#define MAX_CREATE_THREAD	11

//--------------------------------------------------------------------
// Enumeration  Definition
enum { RUN };
enum { OFF , ON   };
enum { SIM , REAL };
enum { ABSENT , PRESENT };
enum { EMPTY  , FULL    };
enum { UNKNOWN1, CLAMP	, UNCLAMP  , ERROR1		};
enum { UNKNOWN2, IN_    , OUT_	   , ERROR2		};
enum { UNKNOWN3, UPPED  , DOWNED   , ERROR3		};
enum { UNKNOWN4, OPEN   , CLOSE    , ERROR4		};
enum { NotUse , Enable  , Disable2 , DisableHW , EnablePM };
enum { Idle = 0  , Running , Pausing  , Paused    , Aborting , Disable , Waiting , WaitHandOff };

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {

	CTC_PM1_Wafer_Status,
	CTC_PM1_Wafer_Status2,
	CTC_PM2_Wafer_Status,
	CTC_PM2_Wafer_Status2,
	CTC_PM3_Wafer_Status,
	CTC_PM3_Wafer_Status2,

	PM1_WaferDI,
	PM1_Wafer2DI,
	PM2_WaferDI,
	PM2_Wafer2DI,
	PM3_WaferDI,
	PM3_Wafer2DI,

	PM1_HTREdit,
	PM2_HTREdit,
	PM3_HTREdit,

	PM1_ScreenCtrlDM,
	PM2_ScreenCtrlDM,
	PM3_ScreenCtrlDM,

	PM1_PRO_IdleRcpStsDM,
	PM2_PRO_IdleRcpStsDM,
	PM3_PRO_IdleRcpStsDM,

	PM1_LotIdSM,
	PM2_LotIdSM,
	PM3_LotIdSM,

	PM1_SW_GbIntlckDM,
	PM2_SW_GbIntlckDM,
	PM3_SW_GbIntlckDM,

	PM1_PLC_GbIntlckDM,
	PM2_PLC_GbIntlckDM,
	PM3_PLC_GbIntlckDM,

	// 2005.10.28 PM Step Sync
	PM1_PRO_PMStepSyncDM,
	PM2_PRO_PMStepSyncDM,
	PM3_PRO_PMStepSyncDM,

	PM1_PRO_PMStepWaitDM,
	PM2_PRO_PMStepWaitDM,
	PM3_PRO_PMStepWaitDM,

	PM1_PRO_PostRunDM,
	PM2_PRO_PostRunDM,
	PM3_PRO_PostRunDM,
	PM4_PRO_PostRunDM,
	PM5_PRO_PostRunDM,

	PM1_PRO_PostRunDM1,
	PM2_PRO_PostRunDM1,
	PM3_PRO_PostRunDM1,
	PM4_PRO_PostRunDM1,
	PM5_PRO_PostRunDM1,

	PM1_PRO_PostRunDM2,
	PM2_PRO_PostRunDM2,
	PM3_PRO_PostRunDM2,
	PM4_PRO_PostRunDM2,
	PM5_PRO_PostRunDM2,

	PM1_PRO_PostRunDM3,
	PM2_PRO_PostRunDM3,
	PM3_PRO_PostRunDM3,
	PM4_PRO_PostRunDM3,
	PM5_PRO_PostRunDM3,

	PM1_PRO_PostRunDM4,
	PM2_PRO_PostRunDM4,
	PM3_PRO_PostRunDM4,
	PM4_PRO_PostRunDM4,
	PM5_PRO_PostRunDM4,

	PM1_PRO_PostRunDM5,
	PM2_PRO_PostRunDM5,
	PM3_PRO_PostRunDM5,
	PM4_PRO_PostRunDM5,
	PM5_PRO_PostRunDM5,

	PM1_PRO_PostRunDM6,
	PM2_PRO_PostRunDM6,
	PM3_PRO_PostRunDM6,
	PM4_PRO_PostRunDM6,
	PM5_PRO_PostRunDM6,

	PM1_PRO_PostRunDM7,
	PM2_PRO_PostRunDM7,
	PM3_PRO_PostRunDM7,
	PM4_PRO_PostRunDM7,
	PM5_PRO_PostRunDM7,

	PM1_PRO_PostRunDM8,
	PM2_PRO_PostRunDM8,
	PM3_PRO_PostRunDM8,
	PM4_PRO_PostRunDM8,
	PM5_PRO_PostRunDM8,

	PM1_PRO_PostRunDM9,
	PM2_PRO_PostRunDM9,
	PM3_PRO_PostRunDM9,
	PM4_PRO_PostRunDM9,
	PM5_PRO_PostRunDM9,

	PM1_PRO_PostCmptDM,
	PM2_PRO_PostCmptDM,
	PM3_PRO_PostCmptDM,
	PM4_PRO_PostCmptDM,
	PM5_PRO_PostCmptDM,

	PM1_PRO_PostCmptDM1,
	PM2_PRO_PostCmptDM1,
	PM3_PRO_PostCmptDM1,
	PM4_PRO_PostCmptDM1,
	PM5_PRO_PostCmptDM1,

	PM1_PRO_PostCmptDM2,
	PM2_PRO_PostCmptDM2,
	PM3_PRO_PostCmptDM2,
	PM4_PRO_PostCmptDM2,
	PM5_PRO_PostCmptDM2,

	PM1_PRO_PostCmptDM3,
	PM2_PRO_PostCmptDM3,
	PM3_PRO_PostCmptDM3,
	PM4_PRO_PostCmptDM3,
	PM5_PRO_PostCmptDM3,

	PM1_PRO_PostCmptDM4,
	PM2_PRO_PostCmptDM4,
	PM3_PRO_PostCmptDM4,
	PM4_PRO_PostCmptDM4,
	PM5_PRO_PostCmptDM4,

	PM1_PRO_PostCmptDM5,
	PM2_PRO_PostCmptDM5,
	PM3_PRO_PostCmptDM5,
	PM4_PRO_PostCmptDM5,
	PM5_PRO_PostCmptDM5,

	PM1_PRO_PostCmptDM6,
	PM2_PRO_PostCmptDM6,
	PM3_PRO_PostCmptDM6,
	PM4_PRO_PostCmptDM6,
	PM5_PRO_PostCmptDM6,

	PM1_PRO_PostCmptDM7,
	PM2_PRO_PostCmptDM7,
	PM3_PRO_PostCmptDM7,
	PM4_PRO_PostCmptDM7,
	PM5_PRO_PostCmptDM7,

	PM1_PRO_PostCmptDM8,
	PM2_PRO_PostCmptDM8,
	PM3_PRO_PostCmptDM8,
	PM4_PRO_PostCmptDM8,
	PM5_PRO_PostCmptDM8,

	PM1_PRO_PostCmptDM9,
	PM2_PRO_PostCmptDM9,
	PM3_PRO_PostCmptDM9,
	PM4_PRO_PostCmptDM9,
	PM5_PRO_PostCmptDM9,

	PRMD_PM1_Leaf_AuEn_01,
	PRMD_PM2_Leaf_AuEn_01,
	PRMD_PM3_Leaf_AuEn_01,
	PRMD_PM4_Leaf_AuEn_01,
	PRMD_PM5_Leaf_AuEn_01,

	PRMS_PM1_Leaf_Rcp_01,
	PRMS_PM2_Leaf_Rcp_01,
	PRMS_PM3_Leaf_Rcp_01,
	PRMS_PM4_Leaf_Rcp_01,
	PRMS_PM5_Leaf_Rcp_01,
	// 2007.02.05 for purging
	PRMD_PM1_Leaf_AuEn_07,
	PRMD_PM2_Leaf_AuEn_07,
	PRMD_PM3_Leaf_AuEn_07,
	PRMD_PM4_Leaf_AuEn_07,
	PRMD_PM5_Leaf_AuEn_07,

	PRMS_PM1_Leaf_Rcp_07,
	PRMS_PM2_Leaf_Rcp_07,
	PRMS_PM3_Leaf_Rcp_07,
	PRMS_PM4_Leaf_Rcp_07,
	PRMS_PM5_Leaf_Rcp_07,

	// 2006.02.24
	PM1_PRO_ComptDM,
	PM2_PRO_ComptDM,
	PM3_PRO_ComptDM,
	PM4_PRO_ComptDM,
	PM5_PRO_ComptDM,

	PM1_PRO_ComptDM1,
	PM2_PRO_ComptDM1,
	PM3_PRO_ComptDM1,
	PM4_PRO_ComptDM1,
	PM5_PRO_ComptDM1,

	PM1_PRO_ComptDM2,
	PM2_PRO_ComptDM2,
	PM3_PRO_ComptDM2,
	PM4_PRO_ComptDM2,
	PM5_PRO_ComptDM2,

	PM1_PRO_ComptDM3,
	PM2_PRO_ComptDM3,
	PM3_PRO_ComptDM3,
	PM4_PRO_ComptDM3,
	PM5_PRO_ComptDM3,

	PM1_PRO_ComptDM4,
	PM2_PRO_ComptDM4,
	PM3_PRO_ComptDM4,
	PM4_PRO_ComptDM4,
	PM5_PRO_ComptDM4,

	PM1_PRO_ComptDM5,
	PM2_PRO_ComptDM5,
	PM3_PRO_ComptDM5,
	PM4_PRO_ComptDM5,
	PM5_PRO_ComptDM5,

	PM1_PRO_ComptDM6,
	PM2_PRO_ComptDM6,
	PM3_PRO_ComptDM6,
	PM4_PRO_ComptDM6,
	PM5_PRO_ComptDM6,

	PM1_PRO_ComptDM7,
	PM2_PRO_ComptDM7,
	PM3_PRO_ComptDM7,
	PM4_PRO_ComptDM7,
	PM5_PRO_ComptDM7,

	PM1_PRO_ComptDM8,
	PM2_PRO_ComptDM8,
	PM3_PRO_ComptDM8,
	PM4_PRO_ComptDM8,
	PM5_PRO_ComptDM8,

	PM1_PRO_ComptDM9,
	PM2_PRO_ComptDM9,
	PM3_PRO_ComptDM9,
	PM4_PRO_ComptDM9,
	PM5_PRO_ComptDM9,

	//-----------------------------------------------
	// Fixed_IO_Area
	Fixed_IO_Area,

	//.....
	TM_CGChmPrsAI,
	PRMA_TM_FPUMP_PRES,
	TM_VacuumStatusDM,
	TM_ATMVACSensorXI,

	PM1_CommStatus,
	PM2_CommStatus,
	PM3_CommStatus,

	CTC_MID_NAME,
	CTC_MID_NAME2,
	CTC_MID_NAME3,
	CTC_MID_NAME4,

	CTC_MAIN_CONTROL,
	CTC_MAIN_CONTROL2,
	CTC_MAIN_CONTROL3,
	CTC_MAIN_CONTROL4,

	//2008.04.04
	TMC_AUTO_LEAK_CHECK,
	//-----------------------------------------------
	// Only for Metal
	// 2005.12.20
	TMC_RBLOW_CONTROL,
	TMC_FMRBLOW_CONTROL,
	TMC_FMALIGNER_CONTROL,
	CTC_AbortControl,
	//2007.07.30
	CTC_MANUAL_TRANSFER,
	CTC_ManualControl,
	CTC_RUN_CONTROL,

	PM1_COMP_MODE,
	PM2_COMP_MODE,
	PM3_COMP_MODE,

	// WPD Count
	PM1_PROCESS_COUNT,
	PM1_DMinusCount,
	PM1_WPDCount,
	PM2_PROCESS_COUNT,
	PM2_DMinusCount,
	PM2_WPDCount,
	PM3_PROCESS_COUNT,
	PM3_DMinusCount,
	PM3_WPDCount,

	//2012.02.23
	PM1_PRO_CHAMBER_STATUS,
	PM2_PRO_CHAMBER_STATUS,
	PM3_PRO_CHAMBER_STATUS,
	//... 2017.03.28
	CTC_PM1_CommStatus,
	CTC_PM2_CommStatus,
	CTC_PM3_CommStatus,
	//... 2018.08.27
	PM1_ModeChangeIgnore,
	PM2_ModeChangeIgnore,
	PM3_ModeChangeIgnore,
	PM4_ModeChangeIgnore,
	PM5_ModeChangeIgnore,

} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{

	{ "CTC.PM1_Wafer_Status",	_K_D_IO, 0 } ,
	{ "CTC.PM1_Wafer_Status2",	_K_D_IO, 0 } ,
	{ "CTC.PM2_Wafer_Status",	_K_D_IO, 0 } ,
	{ "CTC.PM2_Wafer_Status2",	_K_D_IO, 0 } ,
	{ "CTC.PM3_Wafer_Status",	_K_D_IO, 0 } ,
	{ "CTC.PM3_Wafer_Status2",	_K_D_IO, 0 } ,

	{ "PM1.WaferDI",			_K_D_IO, 0 } ,
	{ "PM1.Wafer2DI",			_K_D_IO, 0 } ,
	{ "PM2.WaferDI",			_K_D_IO, 0 } ,
	{ "PM2.Wafer2DI",			_K_D_IO, 0 } ,
	{ "PM3.WaferDI",			_K_D_IO, 0 } ,
	{ "PM3.Wafer2DI",			_K_D_IO, 0 } ,

	{ "PM1.HTREditEnableDisable",		_K_D_IO, 0},
	{ "PM2.HTREditEnableDisable",		_K_D_IO, 0},
	{ "PM3.HTREditEnableDisable",		_K_D_IO, 0},

	{ "PM1.ScreenCtrlDM",		_K_D_IO, 0 } ,
	{ "PM2.ScreenCtrlDM",		_K_D_IO, 0 } ,
	{ "PM3.ScreenCtrlDM",		_K_D_IO, 0 } ,

	{ "PM1.PRO_IdleRcpStsDM",	_K_D_IO, 0 } ,
	{ "PM2.PRO_IdleRcpStsDM",	_K_D_IO, 0 } ,
	{ "PM3.PRO_IdleRcpStsDM",	_K_D_IO, 0 } ,

	{ "CTC.PM1_LotIdSM",		_K_S_IO, 0 } ,
	{ "CTC.PM2_LotIdSM",		_K_S_IO, 0 } ,
	{ "CTC.PM3_LotIdSM",		_K_S_IO, 0 } ,

	{ "PM1.SW_GbIntlckDM",		_K_D_IO, 0 } ,
	{ "PM2.SW_GbIntlckDM",		_K_D_IO, 0 } ,
	{ "PM3.SW_GbIntlckDM",		_K_D_IO, 0 } ,

	{ "PM1.PLC_GbIntlckDI",		_K_D_IO, 0 } ,
	{ "PM2.PLC_GbIntlckDI",		_K_D_IO, 0 } ,
	{ "PM3.PLC_GbIntlckDI",		_K_D_IO, 0 } ,

	// 2005.10.28 PM Step Sync
	{"PM1.PRO_PMStepSyncDM", _K_D_IO, 0},
	{"PM2.PRO_PMStepSyncDM", _K_D_IO, 0},
	{"PM3.PRO_PMStepSyncDM", _K_D_IO, 0},

	{"PM1.PRO_PMStepWaitDM", _K_D_IO, 0},
	{"PM2.PRO_PMStepWaitDM", _K_D_IO, 0},
	{"PM3.PRO_PMStepWaitDM", _K_D_IO, 0},

	{"PM1.PRO_PostRunDM", _K_D_IO, 0},
	{"PM2.PRO_PostRunDM", _K_D_IO, 0},
	{"PM3.PRO_PostRunDM", _K_D_IO, 0},
	{"PM4.PRO_PostRunDM", _K_D_IO, 0},
	{"PM5.PRO_PostRunDM", _K_D_IO, 0},

	{"PM1.PRO_PostRunDM1", _K_D_IO, 0},
	{"PM2.PRO_PostRunDM1", _K_D_IO, 0},
	{"PM3.PRO_PostRunDM1", _K_D_IO, 0},
	{"PM4.PRO_PostRunDM1", _K_D_IO, 0},
	{"PM5.PRO_PostRunDM1", _K_D_IO, 0},

	{"PM1.PRO_PostRunDM2", _K_D_IO, 0},
	{"PM2.PRO_PostRunDM2", _K_D_IO, 0},
	{"PM3.PRO_PostRunDM2", _K_D_IO, 0},
	{"PM4.PRO_PostRunDM2", _K_D_IO, 0},
	{"PM5.PRO_PostRunDM2", _K_D_IO, 0},

	{"PM1.PRO_PostRunDM3", _K_D_IO, 0},
	{"PM2.PRO_PostRunDM3", _K_D_IO, 0},
	{"PM3.PRO_PostRunDM3", _K_D_IO, 0},
	{"PM4.PRO_PostRunDM3", _K_D_IO, 0},
	{"PM5.PRO_PostRunDM3", _K_D_IO, 0},

	{"PM1.PRO_PostRunDM4", _K_D_IO, 0},
	{"PM2.PRO_PostRunDM4", _K_D_IO, 0},
	{"PM3.PRO_PostRunDM4", _K_D_IO, 0},
	{"PM4.PRO_PostRunDM4", _K_D_IO, 0},
	{"PM5.PRO_PostRunDM4", _K_D_IO, 0},

	{"PM1.PRO_PostRunDM5", _K_D_IO, 0},
	{"PM2.PRO_PostRunDM5", _K_D_IO, 0},
	{"PM3.PRO_PostRunDM5", _K_D_IO, 0},
	{"PM4.PRO_PostRunDM5", _K_D_IO, 0},
	{"PM5.PRO_PostRunDM5", _K_D_IO, 0},

	{"PM1.PRO_PostRunDM6", _K_D_IO, 0},
	{"PM2.PRO_PostRunDM6", _K_D_IO, 0},
	{"PM3.PRO_PostRunDM6", _K_D_IO, 0},
	{"PM4.PRO_PostRunDM6", _K_D_IO, 0},
	{"PM5.PRO_PostRunDM6", _K_D_IO, 0},

	{"PM1.PRO_PostRunDM7", _K_D_IO, 0},
	{"PM2.PRO_PostRunDM7", _K_D_IO, 0},
	{"PM3.PRO_PostRunDM7", _K_D_IO, 0},
	{"PM4.PRO_PostRunDM7", _K_D_IO, 0},
	{"PM5.PRO_PostRunDM7", _K_D_IO, 0},

	{"PM1.PRO_PostRunDM8", _K_D_IO, 0},
	{"PM2.PRO_PostRunDM8", _K_D_IO, 0},
	{"PM3.PRO_PostRunDM8", _K_D_IO, 0},
	{"PM4.PRO_PostRunDM8", _K_D_IO, 0},
	{"PM5.PRO_PostRunDM8", _K_D_IO, 0},

	{"PM1.PRO_PostRunDM9", _K_D_IO, 0},
	{"PM2.PRO_PostRunDM9", _K_D_IO, 0},
	{"PM3.PRO_PostRunDM9", _K_D_IO, 0},
	{"PM4.PRO_PostRunDM9", _K_D_IO, 0},
	{"PM5.PRO_PostRunDM9", _K_D_IO, 0},

	{"PM1.PRO_PostCmptDM", _K_D_IO, 0},
	{"PM2.PRO_PostCmptDM", _K_D_IO, 0},
	{"PM3.PRO_PostCmptDM", _K_D_IO, 0},
	{"PM4.PRO_PostCmptDM", _K_D_IO, 0},
	{"PM5.PRO_PostCmptDM", _K_D_IO, 0},

	{"PM1.PRO_PostCmptDM1", _K_D_IO, 0},
	{"PM2.PRO_PostCmptDM1", _K_D_IO, 0},
	{"PM3.PRO_PostCmptDM1", _K_D_IO, 0},
	{"PM4.PRO_PostCmptDM1", _K_D_IO, 0},
	{"PM5.PRO_PostCmptDM1", _K_D_IO, 0},

	{"PM1.PRO_PostCmptDM2", _K_D_IO, 0},
	{"PM2.PRO_PostCmptDM2", _K_D_IO, 0},
	{"PM3.PRO_PostCmptDM2", _K_D_IO, 0},
	{"PM4.PRO_PostCmptDM2", _K_D_IO, 0},
	{"PM5.PRO_PostCmptDM2", _K_D_IO, 0},

	{"PM1.PRO_PostCmptDM3", _K_D_IO, 0},
	{"PM2.PRO_PostCmptDM3", _K_D_IO, 0},
	{"PM3.PRO_PostCmptDM3", _K_D_IO, 0},
	{"PM4.PRO_PostCmptDM3", _K_D_IO, 0},
	{"PM5.PRO_PostCmptDM3", _K_D_IO, 0},

	{"PM1.PRO_PostCmptDM4", _K_D_IO, 0},
	{"PM2.PRO_PostCmptDM4", _K_D_IO, 0},
	{"PM3.PRO_PostCmptDM4", _K_D_IO, 0},
	{"PM4.PRO_PostCmptDM4", _K_D_IO, 0},
	{"PM5.PRO_PostCmptDM4", _K_D_IO, 0},

	{"PM1.PRO_PostCmptDM5", _K_D_IO, 0},
	{"PM2.PRO_PostCmptDM5", _K_D_IO, 0},
	{"PM3.PRO_PostCmptDM5", _K_D_IO, 0},
	{"PM4.PRO_PostCmptDM5", _K_D_IO, 0},
	{"PM5.PRO_PostCmptDM5", _K_D_IO, 0},

	{"PM1.PRO_PostCmptDM6", _K_D_IO, 0},
	{"PM2.PRO_PostCmptDM6", _K_D_IO, 0},
	{"PM3.PRO_PostCmptDM6", _K_D_IO, 0},
	{"PM4.PRO_PostCmptDM6", _K_D_IO, 0},
	{"PM5.PRO_PostCmptDM6", _K_D_IO, 0},

	{"PM1.PRO_PostCmptDM7", _K_D_IO, 0},
	{"PM2.PRO_PostCmptDM7", _K_D_IO, 0},
	{"PM3.PRO_PostCmptDM7", _K_D_IO, 0},
	{"PM4.PRO_PostCmptDM7", _K_D_IO, 0},
	{"PM5.PRO_PostCmptDM7", _K_D_IO, 0},

	{"PM1.PRO_PostCmptDM8", _K_D_IO, 0},
	{"PM2.PRO_PostCmptDM8", _K_D_IO, 0},
	{"PM3.PRO_PostCmptDM8", _K_D_IO, 0},
	{"PM4.PRO_PostCmptDM8", _K_D_IO, 0},
	{"PM5.PRO_PostCmptDM8", _K_D_IO, 0},

	{"PM1.PRO_PostCmptDM9", _K_D_IO, 0},
	{"PM2.PRO_PostCmptDM9", _K_D_IO, 0},
	{"PM3.PRO_PostCmptDM9", _K_D_IO, 0},
	{"PM4.PRO_PostCmptDM9", _K_D_IO, 0},
	{"PM5.PRO_PostCmptDM9", _K_D_IO, 0},

	{"PRMD_PM1_Leaf_AuEn_01", _K_D_IO, 0},
	{"PRMD_PM2_Leaf_AuEn_01", _K_D_IO, 0},
	{"PRMD_PM3_Leaf_AuEn_01", _K_D_IO, 0},
	{"PRMD_PM4_Leaf_AuEn_01", _K_D_IO, 0},
	{"PRMD_PM5_Leaf_AuEn_01", _K_D_IO, 0},

	{"PRMS_PM1_Leaf_Rcp_01", _K_S_IO, 0},
	{"PRMS_PM2_Leaf_Rcp_01", _K_S_IO, 0},
	{"PRMS_PM3_Leaf_Rcp_01", _K_S_IO, 0},
	{"PRMS_PM4_Leaf_Rcp_01", _K_S_IO, 0},
	{"PRMS_PM5_Leaf_Rcp_01", _K_S_IO, 0},

	// 2007.02.05 for purging
	{"PRMD_PM1_Leaf_AuEn_07", _K_D_IO, 0},
	{"PRMD_PM2_Leaf_AuEn_07", _K_D_IO, 0},
	{"PRMD_PM3_Leaf_AuEn_07", _K_D_IO, 0},
	{"PRMD_PM4_Leaf_AuEn_07", _K_D_IO, 0},
	{"PRMD_PM5_Leaf_AuEn_07", _K_D_IO, 0},

	{"PRMS_PM1_Leaf_Rcp_07", _K_S_IO, 0},
	{"PRMS_PM2_Leaf_Rcp_07", _K_S_IO, 0},
	{"PRMS_PM3_Leaf_Rcp_07", _K_S_IO, 0},
	{"PRMS_PM4_Leaf_Rcp_07", _K_S_IO, 0},
	{"PRMS_PM5_Leaf_Rcp_07", _K_S_IO, 0},

	// 2006.02.24
	{"PM1.PRO_ComptDM", _K_D_IO, 0},
	{"PM2.PRO_ComptDM", _K_D_IO, 0},
	{"PM3.PRO_ComptDM", _K_D_IO, 0},
	{"PM4.PRO_ComptDM", _K_D_IO, 0},
	{"PM5.PRO_ComptDM", _K_D_IO, 0},

	{"PM1.PRO_ComptDM1", _K_D_IO, 0},
	{"PM2.PRO_ComptDM1", _K_D_IO, 0},
	{"PM3.PRO_ComptDM1", _K_D_IO, 0},
	{"PM4.PRO_ComptDM1", _K_D_IO, 0},
	{"PM5.PRO_ComptDM1", _K_D_IO, 0},

	{"PM1.PRO_ComptDM2", _K_D_IO, 0},
	{"PM2.PRO_ComptDM2", _K_D_IO, 0},
	{"PM3.PRO_ComptDM2", _K_D_IO, 0},
	{"PM4.PRO_ComptDM2", _K_D_IO, 0},
	{"PM5.PRO_ComptDM2", _K_D_IO, 0},

	{"PM1.PRO_ComptDM3", _K_D_IO, 0},
	{"PM2.PRO_ComptDM3", _K_D_IO, 0},
	{"PM3.PRO_ComptDM3", _K_D_IO, 0},
	{"PM4.PRO_ComptDM3", _K_D_IO, 0},
	{"PM5.PRO_ComptDM3", _K_D_IO, 0},

	{"PM1.PRO_ComptDM4", _K_D_IO, 0},
	{"PM2.PRO_ComptDM4", _K_D_IO, 0},
	{"PM3.PRO_ComptDM4", _K_D_IO, 0},
	{"PM4.PRO_ComptDM4", _K_D_IO, 0},
	{"PM5.PRO_ComptDM4", _K_D_IO, 0},

	{"PM1.PRO_ComptDM5", _K_D_IO, 0},
	{"PM2.PRO_ComptDM5", _K_D_IO, 0},
	{"PM3.PRO_ComptDM5", _K_D_IO, 0},
	{"PM4.PRO_ComptDM5", _K_D_IO, 0},
	{"PM5.PRO_ComptDM5", _K_D_IO, 0},

	{"PM1.PRO_ComptDM6", _K_D_IO, 0},
	{"PM2.PRO_ComptDM6", _K_D_IO, 0},
	{"PM3.PRO_ComptDM6", _K_D_IO, 0},
	{"PM4.PRO_ComptDM6", _K_D_IO, 0},
	{"PM5.PRO_ComptDM6", _K_D_IO, 0},

	{"PM1.PRO_ComptDM7", _K_D_IO, 0},
	{"PM2.PRO_ComptDM7", _K_D_IO, 0},
	{"PM3.PRO_ComptDM7", _K_D_IO, 0},
	{"PM4.PRO_ComptDM7", _K_D_IO, 0},
	{"PM5.PRO_ComptDM7", _K_D_IO, 0},

	{"PM1.PRO_ComptDM8", _K_D_IO, 0},
	{"PM2.PRO_ComptDM8", _K_D_IO, 0},
	{"PM3.PRO_ComptDM8", _K_D_IO, 0},
	{"PM4.PRO_ComptDM8", _K_D_IO, 0},
	{"PM5.PRO_ComptDM8", _K_D_IO, 0},

	{"PM1.PRO_ComptDM9", _K_D_IO, 0},
	{"PM2.PRO_ComptDM9", _K_D_IO, 0},
	{"PM3.PRO_ComptDM9", _K_D_IO, 0},
	{"PM4.PRO_ComptDM9", _K_D_IO, 0},
	{"PM5.PRO_ComptDM9", _K_D_IO, 0},
	//-----------------------------------------------
	// Fixed_IO_Area
	{"NotDefined", _K_D_IO, 0},

	//.....
	// 2005.03.23
	{ "TM.CGChmPrsAI",			_K_A_IO,	0	},
	{ "PRMA_TM_FPUMP_PRES",		_K_A_IO,	0	},
	{ "TM.VacuumStatusDM",		_K_D_IO,	0	},
	{ "TM.ATMVACSensorXI",		_K_D_IO,	0	},

	{ "CTC.PM1_CommStatus",		_K_D_IO,	0	},
	{ "CTC.PM2_CommStatus",		_K_D_IO,	0	},
	{ "CTC.PM3_CommStatus",		_K_D_IO,	0	},

	{ "CTC.MID_NAME",			_K_S_IO,	0	},
	{ "CTC.MID_NAME2",			_K_S_IO,	0	},
	{ "CTC.MID_NAME3",			_K_S_IO,	0	},
	{ "CTC.MID_NAME4",			_K_S_IO,	0	},

	{ "CTC.MAIN_CONTROL",		_K_D_IO,	0	},
	{ "CTC.MAIN_CONTROL2",		_K_D_IO,	0	},
	{ "CTC.MAIN_CONTROL3",		_K_D_IO,	0	},
	{ "CTC.MAIN_CONTROL4",		_K_D_IO,	0	},

	//2008.04.04
	{"TMC_AUTO_LEAK_CHECK"  ,   _K_F_IO,    0   },
	//-----------------------------------------------
	// Only for Metal
	// 2005.12.20
	{ "TMC_RBLOW_CONTROL",		_K_F_IO,	0	},
	{ "TMC_FMRBLOW_CONTROL",	_K_F_IO,	0	},
	{ "TMC_FMALIGNER_CONTROL",	_K_F_IO,	0	},
	{ "CTC.AbortControl",		_K_D_IO,	0	},
	//2007.07.30
	{ "SCHEDULER_MAINT_MOVE",	_K_F_IO,	0	},
	{ "CTC.MANUAL_TR_CONTROL",	_K_D_IO,	0	},
	{ "CTC.RUN_CONTROL",		_K_D_IO,	0	},

	{ "PRMD_PM1_Comp_ModeEn_01",		_K_D_IO,	0	},
	{ "PRMD_PM2_Comp_ModeEn_01",		_K_D_IO,	0	},
	{ "PRMD_PM3_Comp_ModeEn_01",		_K_D_IO,	0	},

	// WPD
	{ "CTC.PM1_PROCESS_COUNT",		_K_D_IO,	0	},
	{ "CTC.PM1_DMinusCount",		_K_D_IO,	0	},
	{ "CTC.PM1_WPDCount",			_K_D_IO,	0	},
	{ "CTC.PM2_PROCESS_COUNT",		_K_D_IO,	0	},
	{ "CTC.PM2_DMinusCount",		_K_D_IO,	0	},
	{ "CTC.PM2_WPDCount",			_K_D_IO,	0	},
	{ "CTC.PM3_PROCESS_COUNT",		_K_D_IO,	0	},
	{ "CTC.PM3_DMinusCount",		_K_D_IO,	0	},
	{ "CTC.PM3_WPDCount",			_K_D_IO,	0	},

	//2012.02.23 Dry Lot Pause
	{ "PM1.PRO_ChamberStatus"	,	_K_D_IO,	0   } ,
	{ "PM2.PRO_ChamberStatus"	,	_K_D_IO,	0   } ,
	{ "PM3.PRO_ChamberStatus"	,	_K_D_IO,	0   } ,

	{ "PM1.CTC_CommStatus"		,	_K_D_IO,	0   } ,
	{ "PM2.CTC_CommStatus"		,	_K_D_IO,	0   } ,
	{ "PM3.CTC_CommStatus"		,	_K_D_IO,	0   } ,
	//2018.08.27 Wet Running with Not Change Mode Status
	{ "PM1_ModeChangeIgnore"	,	_K_D_IO,	0   } ,
	{ "PM2_ModeChangeIgnore"	,	_K_D_IO,	0   } ,
	{ "PM3_ModeChangeIgnore"	,	_K_D_IO,	0   } ,
	{ "PM4_ModeChangeIgnore"	,	_K_D_IO,	0   } ,
	{ "PM5_ModeChangeIgnore"	,	_K_D_IO,	0   } ,
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTmcMonitor::CTmcMonitor()
{
	m_szLotID[0] = 0;
	m_bPMIntModuleDis = TRUE;
	m_bPLCIntModuleDis = TRUE;

	m_bPM1WaferSync = TRUE;
	m_bPM2WaferSync = TRUE;
	m_bPM3WaferSync = TRUE;

	m_nPMSyncWaitTime = 15;

	int i,j=0;
	for(i=0; i<10; i++)
	{
		for(j=0; j<5; j++)
		{
			m_RcpData[i][j].pRcpName[0] = NULL;
			m_RcpData[i][j].nEnable = 0;
		}
	}
}

CTmcMonitor::~CTmcMonitor()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CTmcMonitor::Initialize()
{
	BOOL bRet = TRUE;

	if(! InitIOList(g_pIOList, true, Fixed_IO_Area))	printf("-----> InitIOList Failed in %s\n", APP_NAME);

	ReadTmcConfig();

	// Init TM Vacuum Status
	int nIOStatus;
	int nATMSns;
	nATMSns = dREAD_DIGITAL(TM_ATMVACSensorXI, &nIOStatus);		// 0:Unk 1:ATM 2:Vac 3:Err
	if(nATMSns == 2) dWRITE_DIGITAL(TM_VacuumStatusDM, 1, &nIOStatus);	// 0:ATM 1:Vac

	for(int i = 1; i <= MAX_CREATE_THREAD; i++)
	{
		CreateThread(i); // 2007.02.20
	}

	return bRet;
}


//--------------------------------------------------------------------
// Initialize Function
// 2004.12.24 by cskim
void CTmcMonitor::ParsePmcSyncOption(char *szData)
{
	char szItem[80];
	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szItem)) break;

		if     (strcmp(szItem, "PM1_Wafer_Sync") == 0)			tp.GetBOOL(&m_bPM1WaferSync);
		else if(strcmp(szItem, "PM2_Wafer_Sync") == 0)			tp.GetBOOL(&m_bPM2WaferSync);
		else if(strcmp(szItem, "PM3_Wafer_Sync") == 0)			tp.GetBOOL(&m_bPM3WaferSync);
	} while(0);
}

void CTmcMonitor::ParseMonitoringOption(char *szData)
{
	char szItem[80];
	CTextParser tp(szData);
	do {
		if(! tp.GetWord(szItem)) break;
		if(strcmp(szItem, "PM_Interlock_Module_Disable") == 0) tp.GetBOOL(&m_bPMIntModuleDis);
		if(strcmp(szItem, "PLC_Interlock_Module_Disable") == 0) tp.GetBOOL(&m_bPLCIntModuleDis);
	} while(0);
}

BOOL CTmcMonitor::ReadTmcConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle = 0;

	do {
		fp = fopen(TMC_CONFIG_FILE, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("PMC Synchronization", szItem) == 0) nTitle = 1;
				else if(strcmp("PMC Monitoring Option", szItem) == 0) nTitle = 2;
				else if(strcmp("PLC Interlock List", szItem) == 0) nTitle = 3;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParsePmcSyncOption(szRead); break;
					case 2: ParseMonitoringOption(szRead); break;
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
		printf("-----> Config File Reading Failed : %s\n", TMC_CONFIG_FILE);
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////
// Thread Function

void CTmcMonitor::DoMonitor(int nThreadNo)
{
	printf("-----> Thread[%d] Created in CTmcMonitor\n", nThreadNo);

	switch(nThreadNo)
	{
	// PM Comm Status Monitoring
	case 1: PM1_COMM_STATUS_MONITORING(nThreadNo);		break;
	case 2: PM2_COMM_STATUS_MONITORING(nThreadNo);		break;
	case 3: PM3_COMM_STATUS_MONITORING(nThreadNo);		break;
	case 4: WaferSyncThread(nThreadNo);					break;		// 2005.01.27 by cskim
	case 5: CM_MonitorThread(nThreadNo);				break;		// 2005.01.27 by cskim// 2005.05.24 by cskim, CM Status Monitoring
	case 6: PM_SWIntMonThread(nThreadNo);				break;		// 2005.07.28
	case 7: PM_PLCIntMonThread(nThreadNo);				break;		// 2005.07.29
	case 8: PM_StepSyncMonThread(nThreadNo);			break;		// 2005.10.28 PM Step Sync
	case 9: PostRunMonThread(nThreadNo);				break;
	case 10: CTC_AbortControlThread(nThreadNo);			break;
	case 11: WPD_Count(nThreadNo);						break;
	//... Check MAX_CREATE_THREAD Count
	}
}

//////////////////////////////////////////////////////////////////////////
void CTmcMonitor::PM_COMM_STATUS_MONITORING(int nThreadNo, int nPM)
{
	int nIOStatus;
	int nValue;
	int nAlarmRecovery;
	int nPM_CommSts = 0;
	int nIdx;

	nIdx = nPM - 1;

	//... PM Number Limit Until PM3
	if(nPM > MAX_PM_NUMBER)
	{
		printf("[ERR] Invalid PM Number[%d] in %s\n", nPM, APP_NAME);
		return;
	}

	// 2004.12.23 by cskim
	do {
		if(! WAIT_SEC(5))	break;
		nValue = dREAD_DIGITAL(PM1_CommStatus + nIdx, &nIOStatus);	// 2: disconnect
	} while(nValue == 2);

	WAIT_SEC(3);

	if(nValue == 3)		dWRITE_DIGITAL(PM1_PRO_IdleRcpStsDM + nIdx, 0, &nIOStatus);

	while(WAIT_SEC(3))
	{
		nValue = dREAD_DIGITAL(PM1_CommStatus + nIdx, &nIOStatus);	// 2: disconnect
		//...2017.03.28
		dWRITE_DIGITAL(CTC_PM1_CommStatus + nIdx, nValue,&nIOStatus);
		//...
		switch(nValue)
		{
		case 2:	// Disconnect
			nAlarmRecovery = ALARM_MANAGE(PM1_NOT_CONNECTED+nIdx);
			WAIT_SEC(5); break;
		case 7:	// CTC in Use
			if(nPM_CommSts == nValue)		break;

			//... Screen Control Mode OFF
			if(dREAD_DIGITAL(PM1_ScreenCtrlDM + nIdx, &nIOStatus) == 1)		dWRITE_DIGITAL(PM1_ScreenCtrlDM + nIdx, 0, &nIOStatus);

			//... Heater Edit Button OFF
			if(dREAD_DIGITAL(PM1_HTREdit + nIdx, &nIOStatus) == 1)			dWRITE_DIGITAL(PM1_HTREdit + nIdx, 0, &nIOStatus);

			WAIT_SEC(5);
			dWRITE_STRING(PM1_LotIdSM + nIdx, m_szLotID, &nIOStatus);
			break;
		default:
			if(nPM_CommSts == nValue) break;

			if(dREAD_DIGITAL(PM1_ScreenCtrlDM + nIdx, &nIOStatus) == 0)		dWRITE_DIGITAL(PM1_ScreenCtrlDM + nIdx, 1, &nIOStatus);

			dWRITE_STRING(PM1_LotIdSM + nIdx, "", &nIOStatus);
			break;
		}
		nPM_CommSts = nValue;
	}
}

void CTmcMonitor::PM1_COMM_STATUS_MONITORING(int nThreadNo)
{
	PM_COMM_STATUS_MONITORING(nThreadNo, 1);
}
void CTmcMonitor::PM2_COMM_STATUS_MONITORING(int nThreadNo)
{
	PM_COMM_STATUS_MONITORING(nThreadNo, 2);
}
void CTmcMonitor::PM3_COMM_STATUS_MONITORING(int nThreadNo)
{
	PM_COMM_STATUS_MONITORING(nThreadNo, 3);
}

//////////////////////////////////////////////////////////////////////////
void CTmcMonitor::WaferSyncThread(int nThreadNo)
{
	BOOL	bIOStatus;
	BOOL	bWaferPresent = FALSE;;
	BOOL	bPM1_WaferPresent[MAX_WAFER_IN_PM];
	BOOL	bPM2_WaferPresent[MAX_WAFER_IN_PM];
	BOOL	bPM3_WaferPresent[MAX_WAFER_IN_PM];

	int	i = 0;

	for(i = 0; i < MAX_WAFER_IN_PM; i++)
	{
		bPM1_WaferPresent[i] = FALSE;
		bPM2_WaferPresent[i] = FALSE;
		bPM3_WaferPresent[i] = FALSE;

		dWRITE_DIGITAL(PM1_WaferDI + i, 0, &bIOStatus);
		dWRITE_DIGITAL(PM2_WaferDI + i, 0, &bIOStatus);
		dWRITE_DIGITAL(PM3_WaferDI + i, 0, &bIOStatus);
	}

	while(WAIT_SEC(1))
	{
		// PM1
		if(m_bPM1WaferSync)
		{
			for(i = 0; i < MAX_WAFER_IN_PM; i++) // side 1, 2
			{
				if(0 < dREAD_DIGITAL(CTC_PM1_Wafer_Status + i, &bIOStatus))		bWaferPresent = TRUE;
				else															bWaferPresent = FALSE;

				if(bWaferPresent != bPM1_WaferPresent[i])
				{
					if(bWaferPresent)	dWRITE_DIGITAL(PM1_WaferDI + i, 1, &bIOStatus);
					else				dWRITE_DIGITAL(PM1_WaferDI + i, 0, &bIOStatus);
					bPM1_WaferPresent[i] = bWaferPresent;
				}
			}
		}

		// PM2
		if(m_bPM2WaferSync)
		{
			for(i = 0; i < MAX_WAFER_IN_PM; i++)
			{
				if(0 < dREAD_DIGITAL(CTC_PM2_Wafer_Status + i, &bIOStatus))		bWaferPresent = TRUE;
				else															bWaferPresent = FALSE;

				if(bWaferPresent != bPM2_WaferPresent[i])
				{
					if(bWaferPresent)	dWRITE_DIGITAL(PM2_WaferDI + i, 1, &bIOStatus);
					else				dWRITE_DIGITAL(PM2_WaferDI + i, 0, &bIOStatus);
					bPM2_WaferPresent[i] = bWaferPresent;
				}
			}
		}

		// PM3
		if(m_bPM3WaferSync)
		{
			for(i = 0; i < MAX_WAFER_IN_PM; i++)
			{
				if(0 < dREAD_DIGITAL(CTC_PM3_Wafer_Status + i, &bIOStatus))		bWaferPresent = TRUE;
				else															bWaferPresent = FALSE;

				if(bWaferPresent != bPM3_WaferPresent[i])
				{
					if(bWaferPresent)	dWRITE_DIGITAL(PM3_WaferDI + i, 1, &bIOStatus);
					else				dWRITE_DIGITAL(PM3_WaferDI + i, 0, &bIOStatus);
					bPM3_WaferPresent[i] = bWaferPresent;
				}
			}
		}
	}
}

//--------------------------------------------------------------------
// 2005.05.24 by cskim, CM Status Monitoring

void CTmcMonitor::CM_MonitorThread(int nThreadNo)
{
	int pnMainCtrl[4];
	int nIOStatus;
	int nChannel;
	int i, nValue;

	for(i=0; i<4; i++) pnMainCtrl[i] = 0;
	do {
		for(i=0; i<4; i++)
		{
			nChannel = CTC_MAIN_CONTROL + i;
			nValue = dREAD_DIGITAL(nChannel, &nIOStatus);
			if(nValue == pnMainCtrl[i]) continue;
			if(nValue == 1)
			{
				nChannel = CTC_MID_NAME + i;
				dREAD_STRING(nChannel, m_szLotID, &nIOStatus);
			}
			pnMainCtrl[i] = nValue;
		}
	} while(WAIT_SEC(3));
}

//////////////////////////////////////////////////////////////////////////
// 2005.07.28 PM SW Interlock Monitoring
void CTmcMonitor::PM_SWIntMonThread(int nThreadNo)
{
	int nIOStatus;
	BOOL pbMonEnable[MAX_PM_NUMBER];
	int i;
	char szEvent[256] = "";
	int nPMCommSts;

	WAIT_SEC(10);

	for(i = 0; i < MAX_PM_NUMBER; i++)
	{
		//... Initial Flag
		pbMonEnable[i] = FALSE;
		if(IOValid(PM1_SW_GbIntlckDM + i))	pbMonEnable[i] = TRUE;
	}

	do {
		for(i = 0; i < MAX_PM_NUMBER; i++)
		{
			if(pbMonEnable[i] && m_bPMIntModuleDis)
			{
				nPMCommSts = dREAD_DIGITAL(PM1_CommStatus + i, &nIOStatus);	// 2:DisCon 3:Standby 4:Maint 7:CTCInUse
				if(nPMCommSts == 3 || nPMCommSts == 7)
				{
					if(dREAD_DIGITAL(PM1_SW_GbIntlckDM + i, &nIOStatus) == 1)
					{
						sprintf(szEvent, "SET_MODULE_INFO_LOCAL PM%d|DISABLE.STOP", i+1);
						WRITE_FUNCTION_EVENT(SCHEDULER, szEvent);
					}
				}
			}
		}
	} while(WAIT_SEC(1));

}

//////////////////////////////////////////////////////////////////////////
// 2005.07.29 PM PLC Interlock Monitoring
void CTmcMonitor::PM_PLCIntMonThread(int nThreadNo)
{
	int nIOStatus;
	BOOL pbMonEnable[MAX_PM_NUMBER];
	int i;
	char szEvent[256] = "";
	int nPMCommSts;

	WAIT_SEC(10);

	for(i = 0; i < MAX_PM_NUMBER; i++)
	{
		//... Initial Flag
		pbMonEnable[i] = FALSE;
		if(IOValid(PM1_PLC_GbIntlckDM + i))		pbMonEnable[i] = TRUE;
	}

	do {
		for(i = 0; i < MAX_PM_NUMBER; i++)
		{
			if(pbMonEnable[i] && m_bPLCIntModuleDis)
			{
				nPMCommSts = dREAD_DIGITAL(PM1_CommStatus+i, &nIOStatus);	// 2:DisCon 3:Standby 4:Maint 7:CTCInUse
				if(nPMCommSts == 3 || nPMCommSts == 7)
				{
					if(dREAD_DIGITAL(PM1_PLC_GbIntlckDM + i, &nIOStatus) == 1)
					{
						sprintf(szEvent, "SET_MODULE_INFO_LOCAL PM%d|DISABLE.STOP", i+1);
						WRITE_FUNCTION_EVENT(SCHEDULER, szEvent);
					}
				}
			}
		}
	} while(WAIT_SEC(1));

}

//////////////////////////////////////////////////////////////////////////
// 2005.10.28 PM Step Sync
void CTmcMonitor::PM_StepSyncMonThread(int nThreadNo)
{
	int i = 0;
	BOOL bIOStatus;
	BOOL bNextStep = FALSE;

	do {
		// Check the begin of PM Step Sync
		bNextStep = FALSE;
		do {
			for(i = 0; i < MAX_PM_NUMBER; i++)
			{
				if(IOValid(PM1_PRO_PMStepSyncDM + i))
				{
					if(dREAD_DIGITAL(PM1_PRO_PMStepSyncDM + i, &bIOStatus) == 1)
						bNextStep = TRUE;
				}
			}

			if(bNextStep)	break;

		} while(WAIT_SEC(3));

		// Wait for all PM LotPre Starting
		if(! WAIT_SEC(m_nPMSyncWaitTime)) break;

		// Check the end of PM Step Sync
		do {
			bNextStep = TRUE;
			for(i = 0; i < MAX_PM_NUMBER; i++)
			{
				if(IOValid(PM1_PRO_PMStepSyncDM + i))
				{
					if( dREAD_DIGITAL(PM1_PRO_PMStepSyncDM + i, &bIOStatus) == 1 &&
						dREAD_DIGITAL(PM1_PRO_PMStepWaitDM + i, &bIOStatus) == 0)
					{
						bNextStep = FALSE;
					}
				}
			}
			if(bNextStep)	break;

		} while(WAIT_SEC(3));

		for(i = 0; i < MAX_PM_NUMBER; i++)
		{
			if(IOValid(PM1_PRO_PMStepSyncDM + i))	dWRITE_DIGITAL(PM1_PRO_PMStepSyncDM + i, 0, &bIOStatus);
		}

	} while(WAIT_SEC(3));

}

//////////////////////////////////////////////////////////////////////////
void CTmcMonitor::SetPMModeEnable(int nPM)
{
	BOOL	bIOStatus;
	char	Event[256];

	memset(Event, 0, sizeof(Event));
	strcpy(Event,"SET_MODULE_INFO_LOCAL ");		//... 2014.10.22 Add Space

	switch(nPM)  // reset wafer count
	{
		case 0:	// PM1
			//if(dREAD_DIGITAL(PM1_ModeChangeIgnore, &bIOStatus) == ON)	break; //... Not Use

			if(dREAD_DIGITAL(PM1_COMP_MODE, &bIOStatus) == 1)
			{
				if(dREAD_DIGITAL(PM1_CommStatus, &bIOStatus) == 4)
				{	//... 2014.10.22 Only PM Maint Mode
					strcat(Event, "PM1|ENABLE");
					WRITE_FUNCTION_EVENT(SCHEDULER, Event);
				}
			}
			break;

		case 1:	// PM2
			//if(dREAD_DIGITAL(PM2_ModeChangeIgnore, &bIOStatus) == ON)	break; //... Not Use

			if(dREAD_DIGITAL(PM2_COMP_MODE, &bIOStatus) == 1)
			{
				if(dREAD_DIGITAL(PM2_CommStatus, &bIOStatus) == 4)
				{	//... 2014.10.22 Only PM Maint Mode
					strcat(Event, "PM2|ENABLE");
					WRITE_FUNCTION_EVENT(SCHEDULER, Event);
				}
			}
			break;
		case 2:	// PM3
			//if(dREAD_DIGITAL(PM3_ModeChangeIgnore, &bIOStatus) == ON)	break; //... Not Use

			if(dREAD_DIGITAL(PM3_COMP_MODE, &bIOStatus) == 1 )
			{
				if(dREAD_DIGITAL(PM3_CommStatus, &bIOStatus) == 4)
				{	//... 2014.10.22 Only PM Maint Mode
					strcat(Event, "PM3|ENABLE");
					WRITE_FUNCTION_EVENT(SCHEDULER, Event);
				}
			}
			break;
		default:
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
// 2005.01.27 by cskim
void CTmcMonitor::PostRunMonThread(int nThreadNo)
{
	BOOL bIOStatus;
	int i,j;
	int nPRO_ComptDM;		// 2006.02.24
	int nPRO_PostRunDM;
	int nPRO_PostCmptDM;

	int nPREVENTIVE_F;
	BOOL pbPostMon[10][5]  = { FALSE, FALSE, FALSE, FALSE, FALSE };

	int nEnableCh;
	int nRcpNameCh;

	CSharedMemory shrdMem;
	if(! shrdMem.CreateSM(SM_FILE_NAME, S_MEMORY_SIZE, false))
	{
		printf("-----> SharedMemory Creation Failed !(%s)\n", shrdMem.m_szErrDesc);
	}

	WAIT_SEC(3);
	for(i=0; i<10; i++)    // 10 recipes for auto running
	{
		for(j=0; j<5; j++) // Process Module
		{
			nPRO_PostRunDM = PM1_PRO_PostRunDM +i*5 +j;
			nPRO_PostCmptDM = PM1_PRO_PostCmptDM +i*5 +j;
			nPREVENTIVE_F = PREVENTIVE_PM1 +j;
			if(IOValid(nPRO_PostRunDM) && IOValid(nPRO_PostCmptDM))
			{
				pbPostMon[i][j] = TRUE;
			}
			else
			{
				printf("-----> ITEM[%d]PM[%d] IO NOT Valid: \n", i+1,j+1);
				pbPostMon[i][j] = FALSE;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	while(WAIT_SEC(0.5))
	{
		for(i=0; i<10; i++)	// 10 recipes for auto running
		{
			for(j=0; j<5; j++)	// PM1 ~ PM5
			{
				if(pbPostMon[i][j])
				{
					nPRO_ComptDM = PM1_PRO_ComptDM +i*5 +j;		// 2006.02.24
					nPRO_PostRunDM = PM1_PRO_PostRunDM +i*5 +j;
					nPRO_PostCmptDM = PM1_PRO_PostCmptDM +i*5 +j;
					nPREVENTIVE_F = PREVENTIVE_PM1 +j;

					if(dREAD_DIGITAL(nPRO_PostCmptDM, &bIOStatus) == 1)
					{
						//-------------------------------------------------
						// 2006.02.24
						int nCount = 0;
						while(WAIT_SEC(0.5))
						{
							if(dREAD_DIGITAL(nPRO_ComptDM, &bIOStatus) == 0) break;
							if(++nCount >= 10) break;
						}
						dWRITE_DIGITAL(nPRO_PostCmptDM, 0, &bIOStatus);

						switch(i)  // reset wafer count
						{
							case 0:	// dry cleaning in dry cleaning
								RUN_FUNCTION(nPREVENTIVE_F, "RESET 00 02 01");
								//SetPMModeEnable(j);   //... 2018.11.09 Protect Auto Enable after Clean Recipe is completed
								//if (dREAD_DIGITAL(PM1_ModeChangeIgnore + j, &bIOStatus) != OFF) dWRITE_DIGITAL(PM1_ModeChangeIgnore + j, OFF, &bIOStatus);
								break;
							case 1:	// leak check  in dry cleaning
								break;
							case 2:	// dry post  in dry cleaning
								break;
							case 3:	// purge 1
								RUN_FUNCTION(nPREVENTIVE_F, "RESET 00 08 01");
								break;
							case 4:	// purge 2
								break;
							case 5:	// purge 3
								break;
							case 6:	// auto leak check
								break;
							case 7:	// partial check 1
								break;
							case 8:	// partial check 2
								break;
							case 9:	// partial check 3
								break;
							default:
								break;
						}
						//-------------------------------------------------------------------------
						dWRITE_DIGITAL(nPRO_PostRunDM, 0, &bIOStatus);
						printf("-----> ITEM[%d]PM[%d] Item Complete and Prev Reseted\n",i+1,j+1);

					}

					switch(i)  // set recipe name to the memory
					{
						case 0:	// dry cleaning in dry cleaning
							nEnableCh = PRMD_PM1_Leaf_AuEn_01 +j;
							nRcpNameCh = PRMS_PM1_Leaf_Rcp_01 +j;
							m_RcpData[i][j].nEnable = dREAD_DIGITAL(nEnableCh, &bIOStatus);
							dREAD_STRING(nRcpNameCh, m_RcpData[i][j].pRcpName, &bIOStatus);
							//m_RcpData[i][j].nEnable = 1; // For Test
							//strcpy(m_RcpData[i][j].pRcpName,"drycl1.cln:(CLEAN)"); // For Test
							break;
						case 1:	// leak check  in dry cleaning
							break;
						case 2:	// dry post  in dry cleaning
							break;
						case 3:	// purge 1
							/*
							2015.09.07
							nEnableCh = PRMD_PM1_Leaf_AuEn_07 +j;
							nRcpNameCh = PRMS_PM1_Leaf_Rcp_07 +j;
							m_RcpData[i][j].nEnable = dREAD_DIGITAL(nEnableCh, &bIOStatus);
							dREAD_STRING(nRcpNameCh, m_RcpData[i][j].pRcpName, &bIOStatus);
							2015.09.07
							*/
							//m_RcpData[i][j].nEnable = 1;	// For Test
							//strcpy(m_RcpData[i][j].pRcpName,"pug1.pug:(PURGE)");	// For Test
							break;
						case 4:	// purge 2
							break;
						case 5:	// purge 3
							break;
						case 6:	// auto leak check
							break;
						case 7:	// partial check 1
							break;
						case 8:	// partial check 2
							break;
						case 9:	// partial check 3
							break;

						default:
							break;
					}

					#ifdef _DEBUG
					printf("-----> Write ITEM[%d]PM[%d] Rcp[Name:%s,En:%d] to SharedMemory\n",
										i+1,j+1, m_RcpData[i][j].pRcpName, m_RcpData[i][j].nEnable);
					#endif
				}
			}
		}

		if(! shrdMem.WriteToSM(0, (char*) m_RcpData, sizeof(m_RcpData)))
		{
			printf("-----> SharedMemory Writing Failed !(%s)\n", shrdMem.m_szErrDesc);
		}
	} // end while

	shrdMem.FreeSM();
}

//--------------------------------------------------------------------
// 2005.12.20
void CTmcMonitor::CTC_AbortControlThread(int nThreadNo)
{
	BOOL bIOStatus;
	int nAbCtrl;
	enum { ABC_IDLE, ABC_ABORT, ABC_ABORTING };

	if(IOValid(CTC_AbortControl)) while(WAIT_SEC(1))
	{
		nAbCtrl = dREAD_DIGITAL(CTC_AbortControl, &bIOStatus);
		if(nAbCtrl == ABC_ABORT)
		{
			_LOG("Abort Started in AbortControlThread");
			dWRITE_DIGITAL(CTC_AbortControl, ABC_ABORTING, &bIOStatus);
			//2007.07.30
			dRUN_FUNCTION_ABORT(CTC_MANUAL_TRANSFER);
			//2007.07.30
			dRUN_FUNCTION_ABORT(TMC_RBLOW_CONTROL);
			dRUN_FUNCTION_ABORT(TMC_FMRBLOW_CONTROL);
			dRUN_FUNCTION_ABORT(TMC_FMALIGNER_CONTROL);
			//2008.04.04
			dRUN_FUNCTION(TMC_AUTO_LEAK_CHECK , "ABORT");
			_LOG("-----> Robot Function Abort Event Send and Waiting ...");
			while(WAIT_SEC(1))
			{
				if(SYS_RUNNING != dREAD_FUNCTION(TMC_RBLOW_CONTROL)
					&& SYS_RUNNING != dREAD_FUNCTION(TMC_FMRBLOW_CONTROL)
					&& SYS_RUNNING != dREAD_FUNCTION(TMC_FMALIGNER_CONTROL)) break;
			}
			_LOG("-----> Robot Function Aborted");
			RUN_FUNCTION_ABORT(SCHEDULER);
			while(dREAD_DIGITAL(CTC_RUN_CONTROL, &bIOStatus) != 0)
			{
				if(! WAIT_SEC(1)) break;
			}
			_LOG("-----> Scheduler Aborted");
			dWRITE_DIGITAL(CTC_AbortControl , ABC_IDLE, &bIOStatus);
			//2007.07.30
			dWRITE_DIGITAL(CTC_ManualControl, ABC_IDLE, &bIOStatus);
			//2007.07.30
			_LOG("Abort Finished in AbortControlThread");
		}
	} // end while
}

// WPD Counting
void CTmcMonitor::WPD_Count(int nThreadNo)
{

	BOOL bRet = FALSE;
	SYSTEMTIME systime;
	int    nIOStatus      = 0;
	int    nDay           = 0;
	DWORD  dwTargetSec    = 0;
	DWORD  dwCurSec       = 0;

	int  nPM1ProcessCount  = 0;
	int  nDMinusPM1ProcessCount = 0;
	int  nPM1WPDCount = 0;

	int  nPM2ProcessCount  = 0;
	int  nDMinusPM2ProcessCount = 0;
	int  nPM2WPDCount = 0;

	int  nPM3ProcessCount  = 0;
	int  nDMinusPM3ProcessCount = 0;
	int  nPM3WPDCount = 0;


	if(! WAIT_SEC(10)) return;

	_PWD_LOG(pszFtnName, ELT_File, "WPD Count Start...");

	do{


		GetLocalTime(&systime);

		dwCurSec    = (systime.wHour * 3600) + (systime.wMinute * 60);
		dwTargetSec = (PRMD_PWD_SHOUR * 3600) + (PRMD_PWD_SMIN * 60);

		if (abs(dwCurSec - dwTargetSec) > IN_TIME_SECOND) {

				//printf("WPD Time Checking...\n");

		}else {

			// PM1 Counting...
			nDMinusPM1ProcessCount = dREAD_DIGITAL(PM1_DMinusCount, &nIOStatus);
			nPM1ProcessCount = dREAD_DIGITAL(PM1_PROCESS_COUNT , &nIOStatus);
			nPM1WPDCount = nPM1ProcessCount - nDMinusPM1ProcessCount;

			dWRITE_DIGITAL(PM1_WPDCount, nPM1WPDCount, &nIOStatus);
			printf("PM1 WPD Count : %d\n\n", nPM1WPDCount);
			dWRITE_DIGITAL(PM1_DMinusCount, nPM1ProcessCount, &nIOStatus);

			_PWD_LOG(pszFtnName, ELT_File, "PM1 WPD Count : %d\t Today : %d\t Yday : %d", nPM1WPDCount, nPM1ProcessCount, nDMinusPM1ProcessCount);

			// PM2 Counting...
			nDMinusPM2ProcessCount = dREAD_DIGITAL(PM2_DMinusCount, &nIOStatus);
			nPM2ProcessCount = dREAD_DIGITAL(PM2_PROCESS_COUNT , &nIOStatus);
			nPM2WPDCount = nPM2ProcessCount - nDMinusPM2ProcessCount;

			dWRITE_DIGITAL(PM2_WPDCount, nPM2WPDCount, &nIOStatus);
			printf("PM2 WPD Count : %d\n\n", nPM2WPDCount);
			dWRITE_DIGITAL(PM2_DMinusCount, nPM2ProcessCount, &nIOStatus);

			_PWD_LOG(pszFtnName, ELT_File, "PM2 WPD Count : %d\t Today : %d\t Yday : %d", nPM2WPDCount, nPM2ProcessCount, nDMinusPM2ProcessCount);

			// PM3 Counting...
			nDMinusPM3ProcessCount = dREAD_DIGITAL(PM3_DMinusCount, &nIOStatus);
			nPM3ProcessCount = dREAD_DIGITAL(PM3_PROCESS_COUNT , &nIOStatus);
			nPM3WPDCount = nPM3ProcessCount - nDMinusPM3ProcessCount;

			dWRITE_DIGITAL(PM3_WPDCount, nPM3WPDCount, &nIOStatus);
			printf("PM3 WPD Count : %d\n\n", nPM3WPDCount);
			dWRITE_DIGITAL(PM3_DMinusCount, nPM3ProcessCount, &nIOStatus);

			_PWD_LOG(pszFtnName, ELT_File, "PM3 WPD Count : %d\t Today : %d\t Yday : %d", nPM3WPDCount, nPM3ProcessCount, nDMinusPM3ProcessCount);
			_PWD_LOG(pszFtnName, ELT_File, "All Chamber WPD Count : %d\n", nPM1WPDCount + nPM2WPDCount + nPM3WPDCount);

			Sleep(60000);

		}

	}while(WAIT_SEC(1));
}
