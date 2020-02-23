#ifndef IODEFINE_H
#define IODEFINE_H

#include <STDIO.H>
extern "C"
{
#include "k:\h\Kutlstr.h"
#include "k:\h\Kutltime.h"
#include "k:\h\kutletc.h"
#include "k:\h\kutlanal.h"
#include "k:\h\cimseqnc.h"
}

// global variable
extern string g_strModuleName;
extern int g_nCommStatus;
extern int g_nALARM_BASE;

// message
#define MSG_RUN_FUNCTION		WM_USER+1

//------------------------------------------------------------------------------------------
// Component type enumeration
enum
{
	// Composite type
	INVALID_TYPE = -1,

	DRY_CLEANING_COMPOSITE_TYPE = 0,
	WET_CLEANING_COMPOSITE_TYPE  ,
	IDLE_CONTROL_COMPOSITE_TYPE  ,
	PRE_TREATMENT_COMPOSITE_TYPE ,
	WET_BACKUP_COMPOSITE_TYPE    ,
	ERROR_BACKUP_COMPOSITE_TYPE  ,
	ERROR_BACKUP2_COMPOSITE_TYPE ,

	// Leaf type
	DRY_CLEANING_CLEANING_LEAF_TYPE ,
	DRY_CLEANING_LEAKCHECK_LEAF_TYPE ,
	DRY_CLEANING_DRYPOST_LEAF_TYPE ,
	WET_CLEANING_LEAF_TYPE ,
	IDLE_CONTROL_LEAF_TYPE ,
	PRE_TREATMENT_LEAF_TYPE ,
	WET_CLEANING_POST_LEAF_TYPE,
	WET_BACKUP_LEAF_TYPE,
	WET_BACKUP_POST_LEAF_TYPE,
	ERROR_BACKUP_LEAF_TYPE,
	ERROR_BACKUP_POST_LEAF_TYPE,
	ERROR_BACKUP2_LEAF_TYPE
};

//------------------------------------------------------------------------------------------
// Enumeration list
enum { __SAVE = 1};
enum {__WAFER_NOT_PRESENT = 0};
enum {__NOT_NEED = 0, __NEED};
enum {__OFF = 0, __ON};
enum {__SLOT_OPENED = 1, __SLOT_CLOSED};
enum {__SEQUENCE_IDLE = 0, __SEQUENCE_RUNNING, __SEQUENCE_ABORTED};
enum {__NORWARALA_NORMAL = 0, __NORWARALA_WARNING, __NORWARALA_ALARM};

//------------------------------------------------------------------------------------------

// Alarm list
#define ALARM__CHAMBER_NOT_ENABLED					g_nALARM_BASE+0
#define ALARM__CHAMBER_DISCONNECTED					g_nALARM_BASE+1
#define ALARM__SLOT_NOT_CLOSED						g_nALARM_BASE+2
#define ALARM__WAFER_PRESENT_IN_CHAMBER				g_nALARM_BASE+3
#define ALARM__SEQUENCE_RUNNING						g_nALARM_BASE+4
#define ALARM__LOT_RUNNING							g_nALARM_BASE+5
#define ALARM__RECIPE_RUN_FAILED					g_nALARM_BASE+6
#define ALARM__LEAK_CHECK_FAILED					g_nALARM_BASE+7



//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------

typedef enum {
	CCONDITION_SAVE			,

	PRMD_IDLE_TimeD			,
	PRMD_IDLE_TimeH			,
	PRMD_IDLE_TimeM			,
	PRMD_IDLE_TimeS			,

	//2007.12.06
	PMChamberStatus	    	,


	PRMD_Comp_RunSt_01		,
	PRMD_Comp_ModeEn_01		,
	PRMD_Comp_TimeD_01		,
	PRMD_Comp_TimeH_01		,
	PRMD_Comp_TimeM_01		,
	PRMD_Comp_TimeS_01		,

	PRMD_Comp_RunSt_02		,
	PRMD_Comp_ModeEn_02		,
	PRMD_Comp_TimeD_02		,
	PRMD_Comp_TimeH_02		,
	PRMD_Comp_TimeM_02		,
	PRMD_Comp_TimeS_02		,

	PRMD_Comp_RunSt_03		,
	PRMD_Comp_ModeEn_03		,
	PRMD_Comp_TimeD_03		,
	PRMD_Comp_TimeH_03		,
	PRMD_Comp_TimeM_03		,
	PRMD_Comp_TimeS_03		,

	PRMD_Comp_RunSt_04		,
	PRMD_Comp_ModeEn_04		,
	PRMD_Comp_TimeD_04		,
	PRMD_Comp_TimeH_04		,
	PRMD_Comp_TimeM_04		,
	PRMD_Comp_TimeS_04		,
	//2007.7.4 Added
	//WET Backup Component + Error Backup Component
	// WET Backup
	PRMD_Comp_RunSt_05		,
	PRMD_Comp_ModeEn_05		,
	PRMD_Comp_TimeD_05		,
	PRMD_Comp_TimeH_05		,
	PRMD_Comp_TimeM_05		,
	PRMD_Comp_TimeS_05		,
    // Error Backup
	PRMD_Comp_RunSt_06		,
	PRMD_Comp_ModeEn_06		,
	PRMD_Comp_TimeD_06		,
	PRMD_Comp_TimeH_06		,
	PRMD_Comp_TimeM_06		,
	PRMD_Comp_TimeS_06		,
	// Error Backup2
	PRMD_Comp_RunSt_07		,
	PRMD_Comp_ModeEn_07		,
	PRMD_Comp_TimeD_07		,
	PRMD_Comp_TimeH_07		,
	PRMD_Comp_TimeM_07		,
	PRMD_Comp_TimeS_07		,


	PRMD_Leaf_AuEn_01		,
	PRMS_Leaf_Rcp_01		,
	PRMD_Leaf_RunSt_01		,

	PRMD_Leaf_AuEn_02		,
	PRMS_Leaf_Rcp_02		,
	PRMD_Leaf_RunSt_02		,

	PRMD_Leaf_AuEn_03		,
	PRMS_Leaf_Rcp_03		,
	PRMD_Leaf_RunSt_03		,

	PRMD_Leaf_AuEn_04		,
	PRMS_Leaf_Rcp_04		,
	PRMD_Leaf_RunSt_04		,
	//Add WET Post Leaf
	PRMD_Leaf_AuEn_07		,
	PRMS_Leaf_Rcp_07		,
	PRMD_Leaf_RunSt_07		,

	PRMD_Leaf_AuEn_05		,
	PRMS_Leaf_Rcp_05		,
	PRMD_Leaf_RunSt_05		,

	PRMD_Leaf_AuEn_06		,
	PRMS_Leaf_Rcp_06		,
	PRMD_Leaf_RunSt_06		,
	//2007.07.04 Added Leaf (WET Backup Leaf + Error Backup Leaf
	//WET Backup
	PRMD_Leaf_AuEn_08		,
	PRMS_Leaf_Rcp_08		,
	PRMD_Leaf_RunSt_08		,
	//WET Backup Post
	PRMD_Leaf_AuEn_09		,
	PRMS_Leaf_Rcp_09		,
	PRMD_Leaf_RunSt_09		,
    //Error Backup
	PRMD_Leaf_AuEn_10		,
	PRMS_Leaf_Rcp_10		,
	PRMD_Leaf_RunSt_10		,
	//Error Backup Post
	PRMD_Leaf_AuEn_11		,
	PRMS_Leaf_Rcp_11		,
	PRMD_Leaf_RunSt_11		,
	//Error Backup2
	PRMD_Leaf_AuEn_12		,
	PRMS_Leaf_Rcp_12		,
	PRMD_Leaf_RunSt_12		,

	SCHEDULER				,
	SCHEDULER2				,
	SCHEDULER_MAINT			,
	RECIPE_FILE				,

	MAINT_CONTROL			,
	COMM_STATUS				,
	FA_STATUS				,
	PROCESS_RUN_STATUS		,
	ABORT_PROCESS			,
	PRO_IdleRcpStsDM		,

	DRY_CLEANING_ALARM		,
	WET_CLEANING_ALARM		,
	PRE_TREATMENT_NEED		,
	SLOT_VALVE1_XI			,
	SLOT_VALVE2_XI			,
	WAFER_STATUS			,

	PREVENTIVE				,

	PRO_PreTreatDM			,

	SigTwrBuzzerDM			,
	//20081021
	DRY_CONTROL_OPTION      ,
	MANUAL_LOG				,

	IDLE_MAINT_DRY_OPTION	,

	LeakCheckSts			,
	
	PRMD_PM_AutoCycRun		,	//... 2015.04.18 Add Idle Timer Stop for Auto Cycle Clean
	ModeChangeIgnore		,	//... 2018.08.27 Add Wet Cleaning Run with Ignore Mode Change

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
