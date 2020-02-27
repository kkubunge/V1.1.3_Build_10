#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <KutlAnal.h>
#include <kutlgui.h>
#include <kutlStr.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
#include "..\resource.h"
#include "IPS_Log.h"
//------------------------------------------------------------------------------------------
enum	{ ARRIVED , REMOVED		};
enum	{ CLAMPED , UNCLAMPED	};
enum	{ DOCKED  , UNDOCKED	};
enum	{ OPENED  , CLOSED		};
enum	{ PORT1	  , PORT2		, PORT3		, PORT4		};
enum    { Unuse   , Local, DisConn,  Conn,  Disable, DisableHW, EnablePM};
enum	{ NO	  , YES};
enum	{ Manual  , Auto };
enum	{ ON2	  , OFF2 };
enum	{ GEM_Disable, GEM_Enable};	// GEM Change Event Enum
enum	{ eNotCheck, eCheck};
//------------------------------------------------------------------------------------------
#define		MAX_PORT_NUMS		3
#define		MAX_SLOT_NUMS		25
//------------------------------------------------------------------------------------------
#define		REJECT_Run_Recipe_File_Select_Error			1 // Error 6
#define		REJECT_Select_Job_is_Run_Already			2 // Error 8
#define		REJECT_Select_Job_Parameter_Error			3 // Error 9
#define		ABORT_Lot_Recipe_Data_Reading_Error			4 // Error 11
#define		ABORT_Cluster_Recipe_Data_Reading_Error		5 // Error 12
#define		ABORT_PM_Recipe_Not_Exist_Error				6 // Error 18
#define		ABORT_Run_Chamber_is_Not_Enable				7 // Error 14
#define		ABORT_Run_Chamber_is_Not_Manual				8 // Error 15
//------------------------------------------------------------------------------------------
int		ALARM_START_POINT	= 290;
//------------------------------------------------------------------------------------------
typedef struct {
	int nProcessState;
	int nWaferID;
	char szModuleID[255];
	char szPJID[255];
}SPRJobInfo;

typedef struct {
	char szCJID[255];
}SCTRJobInfo;

typedef struct {
	BOOL bUse;
	BOOL bPMUse[6];
}SPRJobPMUseInfo;

typedef struct {
	int nC1_ProCheck;
	int nC2_ProCheck;
	int nC1_WaferID;
	int nC2_WaferID;
	int nC1_PortInfo;
	int nC2_PortInfo;
}SProEventInfo;

SPRJobInfo g_sLPMA_PRJobInfo[25], g_sLPMB_PRJobInfo[25], g_sLPMC_PRJobInfo[25];
SCTRJobInfo g_sLPMA_CTRJobInfo[25], g_sLPMB_CTRJobInfo[25], g_sLPMC_CTRJobInfo[25];
SPRJobPMUseInfo g_sLPMA_PMUse, g_sLPMB_PMUse, g_sLPMC_PMUse;
SProEventInfo g_sPM1EventInfo, g_sPM2EventInfo, g_sPM3EventInfo;

int nCM1_PJ_WfrPrcdCnt, nCM2_PJ_WfrPrcdCnt, nCM3_PJ_WfrPrcdCnt;
int nCM1_CJ_WfrPrcdCnt, nCM2_CJ_WfrPrcdCnt, nCM3_CJ_WfrPrcdCnt;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "GEM.LotRecipe_Manager"			, _K_F_IO	,	LOT_RECIPE_MANAGER			,	0	} ,
	{ "GEM.RECIPE_HISTORY_MANAGER"		, _K_F_IO	,	Recipe_Histroy_Manager		,	0	} ,
	{ "OBJ_TM_Remaining_Time"	        , _K_F_IO	,	Control_Remaining_Time		,	0	} ,
	{ "GEM.JobHistory_Manager"			, _K_F_IO	,	JobHistory_Manager			,	0	} ,
	{ "GEM.WaferPrcCnt_Manager"			, _K_F_IO	,	WaferPrcCnt_Manager			,	0	} ,

	//2011.09.21
	{ "Wafer_ZoneID_Monitor"			, _K_F_IO	,	Wafer_ZoneID_Monitor		,	0	} ,
	//

	{ "CM1.MAPWAFER_COUNT"		, _K_D_IO		,	CM1_MAPWAFER_COUNT			,	0	} ,
	{ "CM2.MAPWAFER_COUNT"		, _K_D_IO		,	CM2_MAPWAFER_COUNT			,	0	} ,
	{ "CM3.MAPWAFER_COUNT"		, _K_D_IO		,	CM3_MAPWAFER_COUNT			,	0	} ,

	{ "CM1.MAPWAFER_INFO"		, _K_S_IO		,	CM1_MAPWAFER_INFO			,	0	} ,
	{ "CM2.MAPWAFER_INFO"		, _K_S_IO		,	CM2_MAPWAFER_INFO			,	0	} ,
	{ "CM3.MAPWAFER_INFO"		, _K_S_IO		,	CM3_MAPWAFER_INFO			,	0	} ,

	{ "CM1.C01_Wafer"			, _K_D_IO		,	CM1_C01_Wafer				,	0	} ,
	{ "CM1.C02_Wafer"			, _K_D_IO		,	CM1_C02_Wafer				,	0	} ,
	{ "CM1.C03_Wafer"			, _K_D_IO		,	CM1_C03_Wafer				,	0	} ,
	{ "CM1.C04_Wafer"			, _K_D_IO		,	CM1_C04_Wafer				,	0	} ,
	{ "CM1.C05_Wafer"			, _K_D_IO		,	CM1_C05_Wafer				,	0	} ,
	{ "CM1.C06_Wafer"			, _K_D_IO		,	CM1_C06_Wafer				,	0	} ,
	{ "CM1.C07_Wafer"			, _K_D_IO		,	CM1_C07_Wafer				,	0	} ,
	{ "CM1.C08_Wafer"			, _K_D_IO		,	CM1_C08_Wafer				,	0	} ,
	{ "CM1.C09_Wafer"			, _K_D_IO		,	CM1_C09_Wafer				,	0	} ,
	{ "CM1.C10_Wafer"			, _K_D_IO		,	CM1_C10_Wafer				,	0	} ,
	{ "CM1.C11_Wafer"			, _K_D_IO		,	CM1_C11_Wafer				,	0	} ,
	{ "CM1.C12_Wafer"			, _K_D_IO		,	CM1_C12_Wafer				,	0	} ,
	{ "CM1.C13_Wafer"			, _K_D_IO		,	CM1_C13_Wafer				,	0	} ,
	{ "CM1.C14_Wafer"			, _K_D_IO		,	CM1_C14_Wafer				,	0	} ,
	{ "CM1.C15_Wafer"			, _K_D_IO		,	CM1_C15_Wafer				,	0	} ,
	{ "CM1.C16_Wafer"			, _K_D_IO		,	CM1_C16_Wafer				,	0	} ,
	{ "CM1.C17_Wafer"			, _K_D_IO		,	CM1_C17_Wafer				,	0	} ,
	{ "CM1.C18_Wafer"			, _K_D_IO		,	CM1_C18_Wafer				,	0	} ,
	{ "CM1.C19_Wafer"			, _K_D_IO		,	CM1_C19_Wafer				,	0	} ,
	{ "CM1.C20_Wafer"			, _K_D_IO		,	CM1_C20_Wafer				,	0	} ,
	{ "CM1.C21_Wafer"			, _K_D_IO		,	CM1_C21_Wafer				,	0	} ,
	{ "CM1.C22_Wafer"			, _K_D_IO		,	CM1_C22_Wafer				,	0	} ,
	{ "CM1.C23_Wafer"			, _K_D_IO		,	CM1_C23_Wafer				,	0	} ,
	{ "CM1.C24_Wafer"			, _K_D_IO		,	CM1_C24_Wafer				,	0	} ,
	{ "CM1.C25_Wafer"			, _K_D_IO		,	CM1_C25_Wafer				,	0	} ,

	{ "CM2.C01_Wafer"			, _K_D_IO		,	CM2_C01_Wafer				,	0	} ,
	{ "CM2.C02_Wafer"			, _K_D_IO		,	CM2_C02_Wafer				,	0	} ,
	{ "CM2.C03_Wafer"			, _K_D_IO		,	CM2_C03_Wafer				,	0	} ,
	{ "CM2.C04_Wafer"			, _K_D_IO		,	CM2_C04_Wafer				,	0	} ,
	{ "CM2.C05_Wafer"			, _K_D_IO		,	CM2_C05_Wafer				,	0	} ,
	{ "CM2.C06_Wafer"			, _K_D_IO		,	CM2_C06_Wafer				,	0	} ,
	{ "CM2.C07_Wafer"			, _K_D_IO		,	CM2_C07_Wafer				,	0	} ,
	{ "CM2.C08_Wafer"			, _K_D_IO		,	CM2_C08_Wafer				,	0	} ,
	{ "CM2.C09_Wafer"			, _K_D_IO		,	CM2_C09_Wafer				,	0	} ,
	{ "CM2.C10_Wafer"			, _K_D_IO		,	CM2_C10_Wafer				,	0	} ,
	{ "CM2.C11_Wafer"			, _K_D_IO		,	CM2_C11_Wafer				,	0	} ,
	{ "CM2.C12_Wafer"			, _K_D_IO		,	CM2_C12_Wafer				,	0	} ,
	{ "CM2.C13_Wafer"			, _K_D_IO		,	CM2_C13_Wafer				,	0	} ,
	{ "CM2.C14_Wafer"			, _K_D_IO		,	CM2_C14_Wafer				,	0	} ,
	{ "CM2.C15_Wafer"			, _K_D_IO		,	CM2_C15_Wafer				,	0	} ,
	{ "CM2.C16_Wafer"			, _K_D_IO		,	CM2_C16_Wafer				,	0	} ,
	{ "CM2.C17_Wafer"			, _K_D_IO		,	CM2_C17_Wafer				,	0	} ,
	{ "CM2.C18_Wafer"			, _K_D_IO		,	CM2_C18_Wafer				,	0	} ,
	{ "CM2.C19_Wafer"			, _K_D_IO		,	CM2_C19_Wafer				,	0	} ,
	{ "CM2.C20_Wafer"			, _K_D_IO		,	CM2_C20_Wafer				,	0	} ,
	{ "CM2.C21_Wafer"			, _K_D_IO		,	CM2_C21_Wafer				,	0	} ,
	{ "CM2.C22_Wafer"			, _K_D_IO		,	CM2_C22_Wafer				,	0	} ,
	{ "CM2.C23_Wafer"			, _K_D_IO		,	CM2_C23_Wafer				,	0	} ,
	{ "CM2.C24_Wafer"			, _K_D_IO		,	CM2_C24_Wafer				,	0	} ,
	{ "CM2.C25_Wafer"			, _K_D_IO		,	CM2_C25_Wafer				,	0	} ,

	{ "CM3.C01_Wafer"			, _K_D_IO		,	CM3_C01_Wafer				,	0	} ,
	{ "CM3.C02_Wafer"			, _K_D_IO		,	CM3_C02_Wafer				,	0	} ,
	{ "CM3.C03_Wafer"			, _K_D_IO		,	CM3_C03_Wafer				,	0	} ,
	{ "CM3.C04_Wafer"			, _K_D_IO		,	CM3_C04_Wafer				,	0	} ,
	{ "CM3.C05_Wafer"			, _K_D_IO		,	CM3_C05_Wafer				,	0	} ,
	{ "CM3.C06_Wafer"			, _K_D_IO		,	CM3_C06_Wafer				,	0	} ,
	{ "CM3.C07_Wafer"			, _K_D_IO		,	CM3_C07_Wafer				,	0	} ,
	{ "CM3.C08_Wafer"			, _K_D_IO		,	CM3_C08_Wafer				,	0	} ,
	{ "CM3.C09_Wafer"			, _K_D_IO		,	CM3_C09_Wafer				,	0	} ,
	{ "CM3.C10_Wafer"			, _K_D_IO		,	CM3_C10_Wafer				,	0	} ,
	{ "CM3.C11_Wafer"			, _K_D_IO		,	CM3_C11_Wafer				,	0	} ,
	{ "CM3.C12_Wafer"			, _K_D_IO		,	CM3_C12_Wafer				,	0	} ,
	{ "CM3.C13_Wafer"			, _K_D_IO		,	CM3_C13_Wafer				,	0	} ,
	{ "CM3.C14_Wafer"			, _K_D_IO		,	CM3_C14_Wafer				,	0	} ,
	{ "CM3.C15_Wafer"			, _K_D_IO		,	CM3_C15_Wafer				,	0	} ,
	{ "CM3.C16_Wafer"			, _K_D_IO		,	CM3_C16_Wafer				,	0	} ,
	{ "CM3.C17_Wafer"			, _K_D_IO		,	CM3_C17_Wafer				,	0	} ,
	{ "CM3.C18_Wafer"			, _K_D_IO		,	CM3_C18_Wafer				,	0	} ,
	{ "CM3.C19_Wafer"			, _K_D_IO		,	CM3_C19_Wafer				,	0	} ,
	{ "CM3.C20_Wafer"			, _K_D_IO		,	CM3_C20_Wafer				,	0	} ,
	{ "CM3.C21_Wafer"			, _K_D_IO		,	CM3_C21_Wafer				,	0	} ,
	{ "CM3.C22_Wafer"			, _K_D_IO		,	CM3_C22_Wafer				,	0	} ,
	{ "CM3.C23_Wafer"			, _K_D_IO		,	CM3_C23_Wafer				,	0	} ,
	{ "CM3.C24_Wafer"			, _K_D_IO		,	CM3_C24_Wafer				,	0	} ,
	{ "CM3.C25_Wafer"			, _K_D_IO		,	CM3_C25_Wafer				,	0	} ,

	{ "CM1.RFID_MID_PAGE"		, _K_S_IO		,	CM1_RFID_MID_PAGE			,	0	} ,
	{ "CM1.RFID_PAGE1"			, _K_S_IO		,	CM1_RFID_PAGE1				,	0	} ,
	{ "CM1.RFID_PAGE2"			, _K_S_IO		,	CM1_RFID_PAGE2				,	0	} ,
	{ "CM1.RFID_PAGE3"			, _K_S_IO		,	CM1_RFID_PAGE3				,	0	} ,
	{ "CM1.RFID_PAGE4"			, _K_S_IO		,	CM1_RFID_PAGE4				,	0	} ,
	{ "CM1.RFID_PAGE5"			, _K_S_IO		,	CM1_RFID_PAGE5				,	0	} ,
	{ "CM1.RFID_PAGE6"			, _K_S_IO		,	CM1_RFID_PAGE6				,	0	} ,
	{ "CM1.RFID_PAGE7"			, _K_S_IO		,	CM1_RFID_PAGE7				,	0	} ,
	{ "CM1.RFID_PAGE8"			, _K_S_IO		,	CM1_RFID_PAGE8				,	0	} ,
	{ "CM1.RFID_PAGE9"			, _K_S_IO		,	CM1_RFID_PAGE9				,	0	} ,
	{ "CM1.RFID_PAGE10"			, _K_S_IO		,	CM1_RFID_PAGE10				,	0	} ,
	{ "CM1.RFID_PAGE11"			, _K_S_IO		,	CM1_RFID_PAGE11				,	0	} ,
	{ "CM1.RFID_PAGE12"			, _K_S_IO		,	CM1_RFID_PAGE12				,	0	} ,
	{ "CM1.RFID_PAGE13"			, _K_S_IO		,	CM1_RFID_PAGE13				,	0	} ,
	{ "CM1.RFID_PAGE14"			, _K_S_IO		,	CM1_RFID_PAGE14				,	0	} ,
	{ "CM1.RFID_PAGE15"			, _K_S_IO		,	CM1_RFID_PAGE15				,	0	} ,
	{ "CM1.RFID_PAGE16"			, _K_S_IO		,	CM1_RFID_PAGE16				,	0	} ,
	{ "CM1.RFID_PAGE17"			, _K_S_IO		,	CM1_RFID_PAGE17				,	0	} ,

	{ "CM2.RFID_MID_PAGE"		, _K_S_IO		,	CM2_RFID_MID_PAGE			,	0	} ,
	{ "CM2.RFID_PAGE1"			, _K_S_IO		,	CM2_RFID_PAGE1				,	0	} ,
	{ "CM2.RFID_PAGE2"			, _K_S_IO		,	CM2_RFID_PAGE2				,	0	} ,
	{ "CM2.RFID_PAGE3"			, _K_S_IO		,	CM2_RFID_PAGE3				,	0	} ,
	{ "CM2.RFID_PAGE4"			, _K_S_IO		,	CM2_RFID_PAGE4				,	0	} ,
	{ "CM2.RFID_PAGE5"			, _K_S_IO		,	CM2_RFID_PAGE5				,	0	} ,
	{ "CM2.RFID_PAGE6"			, _K_S_IO		,	CM2_RFID_PAGE6				,	0	} ,
	{ "CM2.RFID_PAGE7"			, _K_S_IO		,	CM2_RFID_PAGE7				,	0	} ,
	{ "CM2.RFID_PAGE8"			, _K_S_IO		,	CM2_RFID_PAGE8				,	0	} ,
	{ "CM2.RFID_PAGE9"			, _K_S_IO		,	CM2_RFID_PAGE9				,	0	} ,
	{ "CM2.RFID_PAGE10"			, _K_S_IO		,	CM2_RFID_PAGE10				,	0	} ,
	{ "CM2.RFID_PAGE11"			, _K_S_IO		,	CM2_RFID_PAGE11				,	0	} ,
	{ "CM2.RFID_PAGE12"			, _K_S_IO		,	CM2_RFID_PAGE12				,	0	} ,
	{ "CM2.RFID_PAGE13"			, _K_S_IO		,	CM2_RFID_PAGE13				,	0	} ,
	{ "CM2.RFID_PAGE14"			, _K_S_IO		,	CM2_RFID_PAGE14				,	0	} ,
	{ "CM2.RFID_PAGE15"			, _K_S_IO		,	CM2_RFID_PAGE15				,	0	} ,
	{ "CM2.RFID_PAGE16"			, _K_S_IO		,	CM2_RFID_PAGE16				,	0	} ,
	{ "CM2.RFID_PAGE17"			, _K_S_IO		,	CM2_RFID_PAGE17				,	0	} ,

	{ "CM3.RFID_MID_PAGE"		, _K_S_IO		,	CM3_RFID_MID_PAGE			,	0	} ,
	{ "CM3.RFID_PAGE1"			, _K_S_IO		,	CM3_RFID_PAGE1				,	0	} ,
	{ "CM3.RFID_PAGE2"			, _K_S_IO		,	CM3_RFID_PAGE2				,	0	} ,
	{ "CM3.RFID_PAGE3"			, _K_S_IO		,	CM3_RFID_PAGE3				,	0	} ,
	{ "CM3.RFID_PAGE4"			, _K_S_IO		,	CM3_RFID_PAGE4				,	0	} ,
	{ "CM3.RFID_PAGE5"			, _K_S_IO		,	CM3_RFID_PAGE5				,	0	} ,
	{ "CM3.RFID_PAGE6"			, _K_S_IO		,	CM3_RFID_PAGE6				,	0	} ,
	{ "CM3.RFID_PAGE7"			, _K_S_IO		,	CM3_RFID_PAGE7				,	0	} ,
	{ "CM3.RFID_PAGE8"			, _K_S_IO		,	CM3_RFID_PAGE8				,	0	} ,
	{ "CM3.RFID_PAGE9"			, _K_S_IO		,	CM3_RFID_PAGE9				,	0	} ,
	{ "CM3.RFID_PAGE10"			, _K_S_IO		,	CM3_RFID_PAGE10				,	0	} ,
	{ "CM3.RFID_PAGE11"			, _K_S_IO		,	CM3_RFID_PAGE11				,	0	} ,
	{ "CM3.RFID_PAGE12"			, _K_S_IO		,	CM3_RFID_PAGE12				,	0	} ,
	{ "CM3.RFID_PAGE13"			, _K_S_IO		,	CM3_RFID_PAGE13				,	0	} ,
	{ "CM3.RFID_PAGE14"			, _K_S_IO		,	CM3_RFID_PAGE14				,	0	} ,
	{ "CM3.RFID_PAGE15"			, _K_S_IO		,	CM3_RFID_PAGE15				,	0	} ,
	{ "CM3.RFID_PAGE16"			, _K_S_IO		,	CM3_RFID_PAGE16				,	0	} ,
	{ "CM3.RFID_PAGE17"			, _K_S_IO		,	CM3_RFID_PAGE17				,	0	} ,

	{ "CM.MAPWAFER_COUNT"		, _K_D_IO		,	CM_MAPWAFER_COUNT			,	0	} ,
	{ "CM.MAPWAFER_INFO"		, _K_S_IO		,	CM_MAPWAFER_INFO			,	0	} ,

	{ "CM.C01_Wafer"			, _K_D_IO		,	CM_C01_Wafer				,	0	} ,
	{ "CM.C02_Wafer"			, _K_D_IO		,	CM_C02_Wafer				,	0	} ,
	{ "CM.C03_Wafer"			, _K_D_IO		,	CM_C03_Wafer				,	0	} ,
	{ "CM.C04_Wafer"			, _K_D_IO		,	CM_C04_Wafer				,	0	} ,
	{ "CM.C05_Wafer"			, _K_D_IO		,	CM_C05_Wafer				,	0	} ,
	{ "CM.C06_Wafer"			, _K_D_IO		,	CM_C06_Wafer				,	0	} ,
	{ "CM.C07_Wafer"			, _K_D_IO		,	CM_C07_Wafer				,	0	} ,
	{ "CM.C08_Wafer"			, _K_D_IO		,	CM_C08_Wafer				,	0	} ,
	{ "CM.C09_Wafer"			, _K_D_IO		,	CM_C09_Wafer				,	0	} ,
	{ "CM.C10_Wafer"			, _K_D_IO		,	CM_C10_Wafer				,	0	} ,
	{ "CM.C11_Wafer"			, _K_D_IO		,	CM_C11_Wafer				,	0	} ,
	{ "CM.C12_Wafer"			, _K_D_IO		,	CM_C12_Wafer				,	0	} ,
	{ "CM.C13_Wafer"			, _K_D_IO		,	CM_C13_Wafer				,	0	} ,
	{ "CM.C14_Wafer"			, _K_D_IO		,	CM_C14_Wafer				,	0	} ,
	{ "CM.C15_Wafer"			, _K_D_IO		,	CM_C15_Wafer				,	0	} ,
	{ "CM.C16_Wafer"			, _K_D_IO		,	CM_C16_Wafer				,	0	} ,
	{ "CM.C17_Wafer"			, _K_D_IO		,	CM_C17_Wafer				,	0	} ,
	{ "CM.C18_Wafer"			, _K_D_IO		,	CM_C18_Wafer				,	0	} ,
	{ "CM.C19_Wafer"			, _K_D_IO		,	CM_C19_Wafer				,	0	} ,
	{ "CM.C20_Wafer"			, _K_D_IO		,	CM_C20_Wafer				,	0	} ,
	{ "CM.C21_Wafer"			, _K_D_IO		,	CM_C21_Wafer				,	0	} ,
	{ "CM.C22_Wafer"			, _K_D_IO		,	CM_C22_Wafer				,	0	} ,
	{ "CM.C23_Wafer"			, _K_D_IO		,	CM_C23_Wafer				,	0	} ,
	{ "CM.C24_Wafer"			, _K_D_IO		,	CM_C24_Wafer				,	0	} ,
	{ "CM.C25_Wafer"			, _K_D_IO		,	CM_C25_Wafer				,	0	} ,

	{ "CM.RFID_MID_PAGE"		, _K_S_IO		,	CM_RFID_MID_PAGE			,	0	} ,
	{ "CM.RFID_PAGE1"			, _K_S_IO		,	CM_RFID_PAGE1				,	0	} ,
	{ "CM.RFID_PAGE2"			, _K_S_IO		,	CM_RFID_PAGE2				,	0	} ,
	{ "CM.RFID_PAGE3"			, _K_S_IO		,	CM_RFID_PAGE3				,	0	} ,
	{ "CM.RFID_PAGE4"			, _K_S_IO		,	CM_RFID_PAGE4				,	0	} ,
	{ "CM.RFID_PAGE5"			, _K_S_IO		,	CM_RFID_PAGE5				,	0	} ,
	{ "CM.RFID_PAGE6"			, _K_S_IO		,	CM_RFID_PAGE6				,	0	} ,
	{ "CM.RFID_PAGE7"			, _K_S_IO		,	CM_RFID_PAGE7				,	0	} ,
	{ "CM.RFID_PAGE8"			, _K_S_IO		,	CM_RFID_PAGE8				,	0	} ,
	{ "CM.RFID_PAGE9"			, _K_S_IO		,	CM_RFID_PAGE9				,	0	} ,
	{ "CM.RFID_PAGE10"			, _K_S_IO		,	CM_RFID_PAGE10				,	0	} ,
	{ "CM.RFID_PAGE11"			, _K_S_IO		,	CM_RFID_PAGE11				,	0	} ,
	{ "CM.RFID_PAGE12"			, _K_S_IO		,	CM_RFID_PAGE12				,	0	} ,
	{ "CM.RFID_PAGE13"			, _K_S_IO		,	CM_RFID_PAGE13				,	0	} ,
	{ "CM.RFID_PAGE14"			, _K_S_IO		,	CM_RFID_PAGE14				,	0	} ,
	{ "CM.RFID_PAGE15"			, _K_S_IO		,	CM_RFID_PAGE15				,	0	} ,
	{ "CM.RFID_PAGE16"			, _K_S_IO		,	CM_RFID_PAGE16				,	0	} ,
	{ "CM.RFID_PAGE17"			, _K_S_IO		,	CM_RFID_PAGE17				,	0	} ,

	//////////////////////////////////////////////////////////////////////////
	// PM1 Analog (DCOP)
	//////////////////////////////////////////////////////////////////////////
	{ "PM1.PRO_CurStepAM_LAvr"		, _K_A_IO	,	PM1_PRO_CurStepAM_LAvr		,	0	} ,
	{ "PM1.PRO_StepTimeAM_LAvr"		, _K_A_IO	,	PM1_PRO_StepTimeAM_LAvr		,	0	} ,
	{ "PM1.ChmPrsAI_LAvr"			, _K_A_IO	,	PM1_ChmPrsAI_LAvr			,	0	} ,
	{ "PM1.PumpPrsAI_LAvr"			, _K_A_IO	,	PM1_PumpPrsAI_LAvr			,	0	} ,
	{ "PM1.M651_Pressure_LAvr"		, _K_A_IO	,	PM1_M651_Pressure_LAvr		,	0	} ,
	{ "PM1.M651_Position_LAvr"		, _K_A_IO	,	PM1_M651_Position_LAvr		,	0	} ,

	{ "PM1.HT01_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT01_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT02_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT02_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT03_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT03_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT04_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT04_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT05_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT05_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT06_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT06_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT07_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT07_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT08_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT08_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT09_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT09_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT10_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT10_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT11_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT11_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT12_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT12_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT13_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT13_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT14_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT14_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT15_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT15_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT16_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT16_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT17_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT17_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT18_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT18_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT19_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT19_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT20_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT20_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT21_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT21_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT22_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT22_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT23_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT23_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT24_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT24_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT25_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT25_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT26_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT26_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT27_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT27_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT28_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT28_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT29_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT29_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT30_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT30_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT31_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT31_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.HT32_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_HT32_TmpRdAI_LAvr		,	0	} ,

	{ "PM1.RF1LoadPowAI_LAvr"		, _K_A_IO	,	PM1_RF1LoadPowAI_LAvr		,	0	} ,
	{ "PM1.RF2LoadPowAI_LAvr"		, _K_A_IO	,	PM1_RF2LoadPowAI_LAvr		,	0	} ,
	{ "PM1.RF1RefPowAI_LAvr"		, _K_A_IO	,	PM1_RF1RefPowAI_LAvr		,	0	} ,
	{ "PM1.RF2RefPowAI_LAvr"		, _K_A_IO	,	PM1_RF2RefPowAI_LAvr		,	0	} ,
	{ "PM1.RPGLoadPowAI_LAvr"		, _K_A_IO	,	PM1_RPGLoadPowAI_LAvr		,	0	} ,
	{ "PM1.RPGRefPowAI_LAvr"		, _K_A_IO	,	PM1_RPGRefPowAI_LAvr		,	0	} ,
	{ "PM1.HF_LOAD_POS_A_LAvr"		, _K_A_IO	,	PM1_HF_LOAD_POS_A_LAvr		,	0	} ,
	{ "PM1.HF_TUNE_POS_A_LAvr"		, _K_A_IO	,	PM1_HF_TUNE_POS_A_LAvr		,	0	} ,
	{ "PM1.HF_LOAD_POS_B_LAvr"		, _K_A_IO	,	PM1_HF_LOAD_POS_B_LAvr		,	0	} ,
	{ "PM1.HF_TUNE_POS_B_LAvr"		, _K_A_IO	,	PM1_HF_TUNE_POS_B_LAvr		,	0	} ,

	{ "PM1.V97VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM1_V97VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM1.V98VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM1_V98VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM1.V100VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM1_V100VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1.V101VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM1_V101VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1.V102VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM1_V102VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1.V105VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM1_V105VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1.V106VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM1_V106VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1.V108VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM1_V108VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1.V109VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM1_V109VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1.V110VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM1_V110VlvOpenTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM1.V11VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM1_V11VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM1.V12VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM1_V12VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM1.V17VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM1_V17VlvOpenTmAI_LAvr		,	0	} ,

	{ "PM1.V97VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V97VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V98VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V98VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V100VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V100VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V101VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V101VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V102VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V102VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V105VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V105VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V106VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V106VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V108VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V108VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V109VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V109VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V110VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V110VlvCloseTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM1.V11VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V11VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V12VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V12VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1.V17VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM1_V17VlvCloseTmAI_LAvr	,	0	} ,

	{ "PM1.MFC01FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC01FlwAI_LAvr			,	0	} ,
	{ "PM1.MFC02FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC02FlwAI_LAvr			,	0	} ,
	{ "PM1.MFC03FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC03FlwAI_LAvr			,	0	} ,
	{ "PM1.MFC04FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC04FlwAI_LAvr			,	0	} ,
	{ "PM1.MFC05FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC05FlwAI_LAvr			,	0	} ,
	{ "PM1.MFC06FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC06FlwAI_LAvr			,	0	} ,
	{ "PM1.MFC07FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC07FlwAI_LAvr			,	0	} ,
	{ "PM1.MFC08FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC08FlwAI_LAvr			,	0	} ,
	{ "PM1.MFC09FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC09FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC10FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC10FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC11FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC11FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC12FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC12FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC13FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC13FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC14FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC14FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC15FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC15FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC16FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC16FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC17FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC17FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC18FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC18FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC19FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC19FlwAI_LAvr			,	0	} ,
// 	{ "PM1.MFC20FlwAI_LAvr"			, _K_A_IO	,	PM1_MFC20FlwAI_LAvr			,	0	} ,
	{ "PM1.HF_Z_Phase_A_LAvr"		, _K_A_IO	,	PM1_HF_Z_Phase_A_LAvr		,	0	},
	{ "PM1.HF_Z_Phase_B_LAvr"		, _K_A_IO	,	PM1_HF_Z_Phase_B_LAvr		,	0	},
	{ "PM1.HF_Z_Power_A_LAvr"		, _K_A_IO	,	PM1_HF_Z_Power_A_LAvr		,	0	},
	{ "PM1.HF_Z_Power_B_LAvr"		, _K_A_IO	,	PM1_HF_Z_Power_B_LAvr		,	0	},
	{ "PM1.HF_Z_R_A_LAvr"			, _K_A_IO	,	PM1_HF_Z_R_A_LAvr			,	0	},
	{ "PM1.HF_Z_R_B_LAvr"			, _K_A_IO	,	PM1_HF_Z_R_B_LAvr			,	0	},
	{ "PM1.HF_Z_X_A_LAvr"			, _K_A_IO	,	PM1_HF_Z_X_A_LAvr			,	0	},
	{ "PM1.HF_Z_X_B_LAvr"			, _K_A_IO	,	PM1_HF_Z_X_B_LAvr			,	0	},
	{ "PM1.HF_Z_Vrms_A_LAvr"		, _K_A_IO	,	PM1_HF_Z_Vrms_A_LAvr		,	0	},
	{ "PM1.HF_Z_Vrms_B_LAvr"		, _K_A_IO	,	PM1_HF_Z_Vrms_B_LAvr		,	0	},
	{ "PM1.HF_Z_Irms_A_LAvr"		, _K_A_IO	,	PM1_HF_Z_Irms_A_LAvr		,	0	},
	{ "PM1.HF_Z_Irms_B_LAvr"		, _K_A_IO	,	PM1_HF_Z_Irms_B_LAvr		,	0	},
	{ "PM1.SHT1_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_SHT1_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.SHT1_OutPwAI_LAvr"		, _K_A_IO	,	PM1_SHT1_OutPwAI_LAvr		,	0	} ,
	{ "PM1.SHT2_TmpRdAI_LAvr"		, _K_A_IO	,	PM1_SHT2_TmpRdAI_LAvr		,	0	} ,
	{ "PM1.SHT2_OutPwAI_LAvr"		, _K_A_IO	,	PM1_SHT2_OutPwAI_LAvr		,	0	} ,
	{ "PM1.StgHtr1CurrAI_LAvr"		, _K_A_IO	,	PM1_StgHtr1CurrAI_LAvr		,	0	} ,
	{ "PM1.StgHtr1VoltAI_LAvr"		, _K_A_IO	,	PM1_StgHtr1VoltAI_LAvr		,	0	} ,
	{ "PM1.StgHtr2CurrAI_LAvr"		, _K_A_IO	,	PM1_StgHtr2CurrAI_LAvr		,	0	} ,
	{ "PM1.StgHtr2VoltAI_LAvr"		, _K_A_IO	,	PM1_StgHtr2VoltAI_LAvr		,	0	} ,

	{ "PM1.PRO_CurStepAM_LAvr2"		, _K_A_IO	,	PM1_PRO_CurStepAM_LAvr2		,	0	} ,
	{ "PM1.PRO_StepTimeAM_LAvr2"	, _K_A_IO	,	PM1_PRO_StepTimeAM_LAvr2	,	0	} ,
	{ "PM1.ChmPrsAI_LAvr2"			, _K_A_IO	,	PM1_ChmPrsAI_LAvr2			,	0	} ,
	{ "PM1.PumpPrsAI_LAvr2"			, _K_A_IO	,	PM1_PumpPrsAI_LAvr2			,	0	} ,
	{ "PM1.M651_Pressure_LAvr2"		, _K_A_IO	,	PM1_M651_Pressure_LAvr2		,	0	} ,
	{ "PM1.M651_Position_LAvr2"		, _K_A_IO	,	PM1_M651_Position_LAvr2		,	0	} ,

	{ "PM1.HT01_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT01_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT02_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT02_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT03_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT03_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT04_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT04_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT05_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT05_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT06_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT06_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT07_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT07_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT08_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT08_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT09_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT09_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT10_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT10_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT11_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT11_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT12_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT12_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT13_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT13_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT14_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT14_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT15_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT15_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT16_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT16_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT17_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT17_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT18_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT18_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT19_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT19_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT20_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT20_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT21_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT21_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT22_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT22_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT23_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT23_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT24_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT24_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT25_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT25_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT26_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT26_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT27_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT27_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT28_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT28_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT29_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT29_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT30_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT30_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT31_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT31_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.HT32_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_HT32_TmpRdAI_LAvr2		,	0	} ,

	{ "PM1.RF1LoadPowAI_LAvr2"		, _K_A_IO	,	PM1_RF1LoadPowAI_LAvr2		,	0	} ,
	{ "PM1.RF2LoadPowAI_LAvr2"   	, _K_A_IO	,	PM1_RF2LoadPowAI_LAvr2		,	0	} ,
	{ "PM1.RF1RefPowAI_LAvr2"     	, _K_A_IO	,	PM1_RF1RefPowAI_LAvr2		,	0	} ,
	{ "PM1.RF2RefPowAI_LAvr2"    	, _K_A_IO	,	PM1_RF2RefPowAI_LAvr2		,	0	} ,
	{ "PM1.RPGLoadPowAI_LAvr2"   	, _K_A_IO	,	PM1_RPGLoadPowAI_LAvr2		,	0	} ,
	{ "PM1.RPGRefPowAI_LAvr2"    	, _K_A_IO	,	PM1_RPGRefPowAI_LAvr2		,	0	} ,
	{ "PM1.HF_LOAD_POS_A_LAvr2"  	, _K_A_IO	,	PM1_HF_LOAD_POS_A_LAvr2		,	0	} ,
	{ "PM1.HF_TUNE_POS_A_LAvr2"  	, _K_A_IO	,	PM1_HF_TUNE_POS_A_LAvr2		,	0	} ,
	{ "PM1.HF_LOAD_POS_B_LAvr2"  	, _K_A_IO	,	PM1_HF_LOAD_POS_B_LAvr2		,	0	} ,
	{ "PM1.HF_TUNE_POS_B_LAvr2"  	, _K_A_IO	,	PM1_HF_TUNE_POS_B_LAvr2		,	0	} ,

	{ "PM1.V97VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V97VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V98VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V98VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V100VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V100VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V101VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V101VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V102VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V102VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V105VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V105VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V106VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V106VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V108VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V108VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V109VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V109VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V110VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V110VlvOpenTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM1.V11VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V11VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V12VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V12VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1.V17VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM1_V17VlvOpenTmAI_LAvr2	,	0	} ,

	{ "PM1.V97VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V97VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V98VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V98VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V100VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V100VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V101VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V101VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V102VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V102VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V105VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V105VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V106VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V106VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V108VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V108VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V109VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V109VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V110VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V110VlvCloseTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM1.V11VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V11VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V12VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V12VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1.V17VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM1_V17VlvCloseTmAI_LAvr2	,	0	} ,

	{ "PM1.MFC01FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC01FlwAI_LAvr2		,	0	} ,
	{ "PM1.MFC02FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC02FlwAI_LAvr2		,	0	} ,
	{ "PM1.MFC03FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC03FlwAI_LAvr2		,	0	} ,
	{ "PM1.MFC04FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC04FlwAI_LAvr2		,	0	} ,
	{ "PM1.MFC05FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC05FlwAI_LAvr2		,	0	} ,
	{ "PM1.MFC06FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC06FlwAI_LAvr2		,	0	} ,
	{ "PM1.MFC07FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC07FlwAI_LAvr2		,	0	} ,
	{ "PM1.MFC08FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC08FlwAI_LAvr2		,	0	} ,
	{ "PM1.MFC09FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC09FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC10FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC10FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC11FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC11FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC12FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC12FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC13FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC13FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC14FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC14FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC15FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC15FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC16FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC16FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC17FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC17FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC18FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC18FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC19FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC19FlwAI_LAvr2		,	0	} ,
// 	{ "PM1.MFC20FlwAI_LAvr2"		, _K_A_IO	,	PM1_MFC20FlwAI_LAvr2		,	0	} ,
	{ "PM1.HF_Z_Phase_A_LAvr2"		, _K_A_IO	,	PM1_HF_Z_Phase_A_LAvr2		,	0	} ,
	{ "PM1.HF_Z_Phase_B_LAvr2"		, _K_A_IO	,	PM1_HF_Z_Phase_B_LAvr2		,	0	} ,
	{ "PM1.HF_Z_Power_A_LAvr2"		, _K_A_IO	,	PM1_HF_Z_Power_A_LAvr2		,	0	} ,
	{ "PM1.HF_Z_Power_B_LAvr2"		, _K_A_IO	,	PM1_HF_Z_Power_B_LAvr2		,	0	} ,
	{ "PM1.HF_Z_R_A_LAvr2"			, _K_A_IO	,	PM1_HF_Z_R_A_LAvr2			,	0	},
	{ "PM1.HF_Z_R_B_LAvr2"			, _K_A_IO	,	PM1_HF_Z_R_B_LAvr2			,	0	},
	{ "PM1.HF_Z_X_A_LAvr2"			, _K_A_IO	,	PM1_HF_Z_X_A_LAvr2			,	0	},
	{ "PM1.HF_Z_X_B_LAvr2"			, _K_A_IO	,	PM1_HF_Z_X_B_LAvr2			,	0	},
	{ "PM1.HF_Z_Vrms_A_LAvr2"		, _K_A_IO	,	PM1_HF_Z_Vrms_A_LAvr2		,	0	} ,
	{ "PM1.HF_Z_Vrms_B_LAvr2"		, _K_A_IO	,	PM1_HF_Z_Vrms_B_LAvr2		,	0	} ,
	{ "PM1.HF_Z_Irms_A_LAvr2"		, _K_A_IO	,	PM1_HF_Z_Irms_A_LAvr2		,	0	} ,
	{ "PM1.HF_Z_Irms_B_LAvr2"		, _K_A_IO	,	PM1_HF_Z_Irms_B_LAvr2		,	0	} ,
	{ "PM1.SHT1_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_SHT1_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.SHT1_OutPwAI_LAvr2"		, _K_A_IO	,	PM1_SHT1_OutPwAI_LAvr2		,	0	} ,
	{ "PM1.SHT2_TmpRdAI_LAvr2"		, _K_A_IO	,	PM1_SHT2_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1.SHT2_OutPwAI_LAvr2"		, _K_A_IO	,	PM1_SHT2_OutPwAI_LAvr2		,	0	} ,
	{ "PM1.StgHtr1CurrAI_LAvr2"		, _K_A_IO	,	PM1_StgHtr1CurrAI_LAvr2		,	0	} ,
	{ "PM1.StgHtr1VoltAI_LAvr2"		, _K_A_IO	,	PM1_StgHtr1VoltAI_LAvr2		,	0	} ,
	{ "PM1.StgHtr2CurrAI_LAvr2"		, _K_A_IO	,	PM1_StgHtr2CurrAI_LAvr2		,	0	} ,
	{ "PM1.StgHtr2VoltAI_LAvr2"		, _K_A_IO	,	PM1_StgHtr2VoltAI_LAvr2		,	0	} ,

	// PM1 String (DCOP-Event Report)
	{ "PM1_PRO_CurStepAM_LAvr"		, _K_S_IO	,	PM1V_PRO_CurStepAM_LAvr		,	0	} ,
	{ "PM1_PRO_StepTimeAM_LAvr"		, _K_S_IO	,	PM1V_PRO_StepTimeAM_LAvr	,	0	} ,
	{ "PM1_ChmPrsAI_LAvr"			, _K_S_IO	,	PM1V_ChmPrsAI_LAvr			,	0	} ,
	{ "PM1_PumpPrsAI_LAvr"			, _K_S_IO	,	PM1V_PumpPrsAI_LAvr			,	0	} ,
	{ "PM1_M651_Pressure_LAvr"		, _K_S_IO	,	PM1V_M651_Pressure_LAvr		,	0	} ,
	{ "PM1_M651_Position_LAvr"		, _K_S_IO	,	PM1V_M651_Position_LAvr		,	0	} ,

	{ "PM1_HT01_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT01_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT02_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT02_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT03_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT03_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT04_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT04_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT05_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT05_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT06_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT06_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT07_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT07_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT08_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT08_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT09_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT09_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT10_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT10_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT11_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT11_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT12_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT12_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT13_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT13_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT14_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT14_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT15_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT15_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT16_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT16_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT17_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT17_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT18_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT18_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT19_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT19_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT20_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT20_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT21_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT21_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT22_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT22_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT23_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT23_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT24_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT24_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT25_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT25_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT26_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT26_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT27_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT27_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT28_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT28_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT29_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT29_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT30_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT30_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT31_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT31_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_HT32_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_HT32_TmpRdAI_LAvr		,	0	} ,

	{ "PM1_RF1LoadPowAI_LAvr"		, _K_S_IO	,	PM1V_RF1LoadPowAI_LAvr		,	0	} ,
	{ "PM1_RF2LoadPowAI_LAvr"    	, _K_S_IO	,	PM1V_RF2LoadPowAI_LAvr		,	0	} ,
	{ "PM1_RF1RefPowAI_LAvr"      	, _K_S_IO	,	PM1V_RF1RefPowAI_LAvr		,	0	} ,
	{ "PM1_RF2RefPowAI_LAvr"     	, _K_S_IO	,	PM1V_RF2RefPowAI_LAvr		,	0	} ,
	{ "PM1_RPGLoadPowAI_LAvr"    	, _K_S_IO	,	PM1V_RPGLoadPowAI_LAvr		,	0	} ,
	{ "PM1_RPGRefPowAI_LAvr"     	, _K_S_IO	,	PM1V_RPGRefPowAI_LAvr		,	0	} ,
	{ "PM1_HF_LOAD_POS_A_LAvr"   	, _K_S_IO	,	PM1V_HF_LOAD_POS_A_LAvr		,	0	} ,
	{ "PM1_HF_TUNE_POS_A_LAvr"   	, _K_S_IO	,	PM1V_HF_TUNE_POS_A_LAvr		,	0	} ,
	{ "PM1_HF_LOAD_POS_B_LAvr"   	, _K_S_IO	,	PM1V_HF_LOAD_POS_B_LAvr		,	0	} ,
	{ "PM1_HF_TUNE_POS_B_LAvr"   	, _K_S_IO	,	PM1V_HF_TUNE_POS_B_LAvr		,	0	} ,

	{ "PM1_V97VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM1V_V97VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V98VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM1V_V98VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V100VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM1V_V100VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V101VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM1V_V101VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V102VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM1V_V102VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V105VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM1V_V105VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V106VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM1V_V106VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V108VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM1V_V108VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V109VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM1V_V109VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V110VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM1V_V110VlvOpenTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM1_V11VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM1V_V11VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V12VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM1V_V12VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM1_V17VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM1V_V17VlvOpenTmAI_LAvr	,	0	} ,

	{ "PM1_V97VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V97VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V98VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V98VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V100VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V100VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V101VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V101VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V102VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V102VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V105VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V105VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V106VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V106VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V108VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V108VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V109VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V109VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V110VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V110VlvCloseTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM1_V11VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V11VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V12VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V12VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM1_V17VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM1V_V17VlvCloseTmAI_LAvr	,	0	} ,

	{ "PM1_MFC01FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC01FlwAI_LAvr		,	0	} ,
	{ "PM1_MFC02FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC02FlwAI_LAvr		,	0	} ,
	{ "PM1_MFC03FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC03FlwAI_LAvr		,	0	} ,
	{ "PM1_MFC04FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC04FlwAI_LAvr		,	0	} ,
	{ "PM1_MFC05FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC05FlwAI_LAvr		,	0	} ,
	{ "PM1_MFC06FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC06FlwAI_LAvr		,	0	} ,
	{ "PM1_MFC07FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC07FlwAI_LAvr		,	0	} ,
	{ "PM1_MFC08FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC08FlwAI_LAvr		,	0	} ,
	{ "PM1_MFC09FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC09FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC10FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC10FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC11FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC11FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC12FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC12FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC13FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC13FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC14FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC14FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC15FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC15FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC16FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC16FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC17FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC17FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC18FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC18FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC19FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC19FlwAI_LAvr		,	0	} ,
// 	{ "PM1_MFC20FlwAI_LAvr"			, _K_S_IO	,	PM1V_MFC20FlwAI_LAvr		,	0	} ,
	{ "PM1_HF_Z_Phase_A_LAvr"		, _K_S_IO	,	PM1V_HF_Z_Phase_A_LAvr		,	0	} ,
	{ "PM1_HF_Z_Phase_B_LAvr"		, _K_S_IO	,	PM1V_HF_Z_Phase_B_LAvr		,	0	} ,
	{ "PM1_HF_Z_Power_A_LAvr"		, _K_S_IO	,	PM1V_HF_Z_Power_A_LAvr		,	0	} ,
	{ "PM1_HF_Z_Power_B_LAvr"		, _K_S_IO	,	PM1V_HF_Z_Power_B_LAvr		,	0	} ,
	{ "PM1_HF_Z_R_A_LAvr"			, _K_S_IO	,	PM1V_HF_Z_R_A_LAvr			,	0	} ,
	{ "PM1_HF_Z_R_B_LAvr"			, _K_S_IO	,	PM1V_HF_Z_R_B_LAvr			,	0	} ,
	{ "PM1_HF_Z_X_A_LAvr"			, _K_S_IO	,	PM1V_HF_Z_X_A_LAvr			,	0	} ,
	{ "PM1_HF_Z_X_B_LAvr"			, _K_S_IO	,	PM1V_HF_Z_X_B_LAvr			,	0	} ,
	{ "PM1_HF_Z_Vrms_A_LAvr"		, _K_S_IO	,	PM1V_HF_Z_Vrms_A_LAvr		,	0	} ,
	{ "PM1_HF_Z_Vrms_B_LAvr"		, _K_S_IO	,	PM1V_HF_Z_Vrms_B_LAvr		,	0	} ,
	{ "PM1_HF_Z_Irms_A_LAvr"		, _K_S_IO	,	PM1V_HF_Z_Irms_A_LAvr		,	0	} ,
	{ "PM1_HF_Z_Irms_B_LAvr"		, _K_S_IO	,	PM1V_HF_Z_Irms_B_LAvr		,	0	} ,
	{ "PM1_SHT1_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_SHT1_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_SHT1_OutPwAI_LAvr"		, _K_S_IO	,	PM1V_SHT1_OutPwAI_LAvr		,	0	} ,
	{ "PM1_SHT2_TmpRdAI_LAvr"		, _K_S_IO	,	PM1V_SHT2_TmpRdAI_LAvr		,	0	} ,
	{ "PM1_SHT2_OutPwAI_LAvr"		, _K_S_IO	,	PM1V_SHT2_OutPwAI_LAvr		,	0	} ,
	{ "PM1_StgHtr1CurrAI_LAvr"		, _K_S_IO	,	PM1V_StgHtr1CurrAI_LAvr		,	0	} ,
	{ "PM1_StgHtr1VoltAI_LAvr"		, _K_S_IO	,	PM1V_StgHtr1VoltAI_LAvr		,	0	} ,
	{ "PM1_StgHtr2CurrAI_LAvr"		, _K_S_IO	,	PM1V_StgHtr2CurrAI_LAvr		,	0	} ,
	{ "PM1_StgHtr2VoltAI_LAvr"		, _K_S_IO	,	PM1V_StgHtr2VoltAI_LAvr		,	0	} ,

	{ "PM1_PRO_CurStepAM_LAvr2"		, _K_S_IO	,	PM1V_PRO_CurStepAM_LAvr2	,	0	} ,
	{ "PM1_PRO_StepTimeAM_LAvr2"	, _K_S_IO	,	PM1V_PRO_StepTimeAM_LAvr2	,	0	} ,
	{ "PM1_ChmPrsAI_LAvr2"			, _K_S_IO	,	PM1V_ChmPrsAI_LAvr2			,	0	} ,
	{ "PM1_PumpPrsAI_LAvr2"			, _K_S_IO	,	PM1V_PumpPrsAI_LAvr2		,	0	} ,
	{ "PM1_M651_Pressure_LAvr2"		, _K_S_IO	,	PM1V_M651_Pressure_LAvr2	,	0	} ,
	{ "PM1_M651_Position_LAvr2"		, _K_S_IO	,	PM1V_M651_Position_LAvr2	,	0	} ,

	{ "PM1_HT01_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT01_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT02_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT02_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT03_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT03_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT04_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT04_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT05_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT05_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT06_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT06_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT07_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT07_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT08_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT08_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT09_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT09_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT10_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT10_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT11_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT11_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT12_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT12_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT13_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT13_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT14_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT14_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT15_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT15_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT16_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT16_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT17_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT17_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT18_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT18_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT19_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT19_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT20_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT20_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT21_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT21_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT22_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT22_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT23_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT23_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT24_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT24_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT25_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT25_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT26_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT26_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT27_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT27_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT28_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT28_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT29_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT29_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT30_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT30_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT31_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT31_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_HT32_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_HT32_TmpRdAI_LAvr2		,	0	} ,

	{ "PM1_RF1LoadPowAI_LAvr2"		, _K_S_IO	,	PM1V_RF1LoadPowAI_LAvr2		,	0	} ,
	{ "PM1_RF2LoadPowAI_LAvr2"		, _K_S_IO	,	PM1V_RF2LoadPowAI_LAvr2		,	0	} ,
	{ "PM1_RF1RefPowAI_LAvr2"  		, _K_S_IO	,	PM1V_RF1RefPowAI_LAvr2		,	0	} ,
	{ "PM1_RF2RefPowAI_LAvr2" 		, _K_S_IO	,	PM1V_RF2RefPowAI_LAvr2		,	0	} ,
	{ "PM1_RPGLoadPowAI_LAvr2"		, _K_S_IO	,	PM1V_RPGLoadPowAI_LAvr2		,	0	} ,
	{ "PM1_RPGRefPowAI_LAvr2" 		, _K_S_IO	,	PM1V_RPGRefPowAI_LAvr2		,	0	} ,
	{ "PM1_HF_LOAD_POS_A_LAvr2"		, _K_S_IO	,	PM1V_HF_LOAD_POS_A_LAvr2	,	0	} ,
	{ "PM1_HF_TUNE_POS_A_LAvr2"		, _K_S_IO	,	PM1V_HF_TUNE_POS_A_LAvr2	,	0	} ,
	{ "PM1_HF_LOAD_POS_B_LAvr2"		, _K_S_IO	,	PM1V_HF_LOAD_POS_B_LAvr2	,	0	} ,
	{ "PM1_HF_TUNE_POS_B_LAvr2"		, _K_S_IO	,	PM1V_HF_TUNE_POS_B_LAvr2	,	0	} ,

	{ "PM1_V97VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V97VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V98VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V98VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V100VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V100VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V101VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V101VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V102VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V102VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V105VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V105VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V106VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V106VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V108VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V108VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V109VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V109VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V110VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V110VlvOpenTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM1_V11VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V11VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V12VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V12VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM1_V17VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM1V_V17VlvOpenTmAI_LAvr2	,	0	} ,

	{ "PM1_V97VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V97VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V98VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V98VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V100VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V100VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V101VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V101VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V102VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V102VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V105VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V105VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V106VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V106VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V108VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V108VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V109VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V109VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V110VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V110VlvCloseTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM1_V11VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V11VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V12VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V12VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM1_V17VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM1V_V17VlvCloseTmAI_LAvr2	,	0	} ,

	{ "PM1_MFC01FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC01FlwAI_LAvr2		,	0	} ,
	{ "PM1_MFC02FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC02FlwAI_LAvr2		,	0	} ,
	{ "PM1_MFC03FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC03FlwAI_LAvr2		,	0	} ,
	{ "PM1_MFC04FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC04FlwAI_LAvr2		,	0	} ,
	{ "PM1_MFC05FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC05FlwAI_LAvr2		,	0	} ,
	{ "PM1_MFC06FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC06FlwAI_LAvr2		,	0	} ,
	{ "PM1_MFC07FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC07FlwAI_LAvr2		,	0	} ,
	{ "PM1_MFC08FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC08FlwAI_LAvr2		,	0	} ,
	{ "PM1_MFC09FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC09FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC10FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC10FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC11FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC11FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC12FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC12FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC13FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC13FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC14FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC14FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC15FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC15FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC16FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC16FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC17FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC17FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC18FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC18FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC19FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC19FlwAI_LAvr2		,	0	} ,
// 	{ "PM1_MFC20FlwAI_LAvr2"		, _K_S_IO	,	PM1V_MFC20FlwAI_LAvr2		,	0	} ,
	{ "PM1_HF_Z_Phase_A_LAvr2"		, _K_S_IO	,	PM1V_HF_Z_Phase_A_LAvr2		,	0	} ,
	{ "PM1_HF_Z_Phase_B_LAvr2"		, _K_S_IO	,	PM1V_HF_Z_Phase_B_LAvr2		,	0	} ,
	{ "PM1_HF_Z_Power_A_LAvr2"		, _K_S_IO	,	PM1V_HF_Z_Power_A_LAvr2		,	0	} ,
	{ "PM1_HF_Z_Power_B_LAvr2"		, _K_S_IO	,	PM1V_HF_Z_Power_B_LAvr2		,	0	} ,
	{ "PM1_HF_Z_R_A_LAvr2"			, _K_S_IO	,	PM1V_HF_Z_R_A_LAvr2			,	0	} ,
	{ "PM1_HF_Z_R_B_LAvr2"			, _K_S_IO	,	PM1V_HF_Z_R_B_LAvr2			,	0	} ,
	{ "PM1_HF_Z_X_A_LAvr2"			, _K_S_IO	,	PM1V_HF_Z_X_A_LAvr2			,	0	} ,
	{ "PM1_HF_Z_X_B_LAvr2"			, _K_S_IO	,	PM1V_HF_Z_X_B_LAvr2			,	0	} ,
	{ "PM1_HF_Z_Vrms_A_LAvr2"		, _K_S_IO	,	PM1V_HF_Z_Vrms_A_LAvr2		,	0	} ,
	{ "PM1_HF_Z_Vrms_B_LAvr2"		, _K_S_IO	,	PM1V_HF_Z_Vrms_B_LAvr2		,	0	} ,
	{ "PM1_HF_Z_Irms_A_LAvr2"		, _K_S_IO	,	PM1V_HF_Z_Irms_A_LAvr2		,	0	} ,
	{ "PM1_HF_Z_Irms_B_LAvr2"		, _K_S_IO	,	PM1V_HF_Z_Irms_B_LAvr2		,	0	} ,
	{ "PM1_SHT1_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_SHT1_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_SHT1_OutPwAI_LAvr2"		, _K_S_IO	,	PM1V_SHT1_OutPwAI_LAvr2		,	0	} ,
	{ "PM1_SHT2_TmpRdAI_LAvr2"		, _K_S_IO	,	PM1V_SHT2_TmpRdAI_LAvr2		,	0	} ,
	{ "PM1_SHT2_OutPwAI_LAvr2"		, _K_S_IO	,	PM1V_SHT2_OutPwAI_LAvr2		,	0	} ,
	{ "PM1_StgHtr1CurrAI_LAvr2"		, _K_S_IO	,	PM1V_StgHtr1CurrAI_LAvr2	,	0	} ,
	{ "PM1_StgHtr1VoltAI_LAvr2"		, _K_S_IO	,	PM1V_StgHtr1VoltAI_LAvr2	,	0	} ,
	{ "PM1_StgHtr2CurrAI_LAvr2"		, _K_S_IO	,	PM1V_StgHtr2CurrAI_LAvr2	,	0	} ,
	{ "PM1_StgHtr2VoltAI_LAvr2"		, _K_S_IO	,	PM1V_StgHtr2VoltAI_LAvr2	,	0	} ,

	//////////////////////////////////////////////////////////////////////////
	// PM2 Analog (DCOP)
	//////////////////////////////////////////////////////////////////////////
	{ "PM2.PRO_CurStepAM_LAvr"		, _K_A_IO	,	PM2_PRO_CurStepAM_LAvr		,	0	} ,
	{ "PM2.PRO_StepTimeAM_LAvr"		, _K_A_IO	,	PM2_PRO_StepTimeAM_LAvr		,	0	} ,
	{ "PM2.ChmPrsAI_LAvr"			, _K_A_IO	,	PM2_ChmPrsAI_LAvr			,	0	} ,
	{ "PM2.PumpPrsAI_LAvr"			, _K_A_IO	,	PM2_PumpPrsAI_LAvr			,	0	} ,
	{ "PM2.M651_Pressure_LAvr"		, _K_A_IO	,	PM2_M651_Pressure_LAvr		,	0	} ,
	{ "PM2.M651_Position_LAvr"		, _K_A_IO	,	PM2_M651_Position_LAvr		,	0	} ,

	{ "PM2.HT01_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT01_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT02_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT02_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT03_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT03_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT04_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT04_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT05_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT05_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT06_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT06_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT07_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT07_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT08_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT08_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT09_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT09_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT10_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT10_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT11_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT11_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT12_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT12_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT13_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT13_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT14_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT14_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT15_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT15_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT16_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT16_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT17_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT17_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT18_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT18_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT19_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT19_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT20_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT20_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT21_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT21_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT22_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT22_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT23_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT23_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT24_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT24_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT25_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT25_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT26_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT26_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT27_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT27_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT28_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT28_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT29_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT29_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT30_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT30_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT31_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT31_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.HT32_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_HT32_TmpRdAI_LAvr		,	0	} ,

	{ "PM2.RF1LoadPowAI_LAvr"		, _K_A_IO	,	PM2_RF1LoadPowAI_LAvr		,	0	} ,
	{ "PM2.RF2LoadPowAI_LAvr"		, _K_A_IO	,	PM2_RF2LoadPowAI_LAvr		,	0	} ,
	{ "PM2.RF1RefPowAI_LAvr"		, _K_A_IO	,	PM2_RF1RefPowAI_LAvr		,	0	} ,
	{ "PM2.RF2RefPowAI_LAvr"		, _K_A_IO	,	PM2_RF2RefPowAI_LAvr		,	0	} ,
	{ "PM2.RPGLoadPowAI_LAvr"		, _K_A_IO	,	PM2_RPGLoadPowAI_LAvr		,	0	} ,
	{ "PM2.RPGRefPowAI_LAvr"		, _K_A_IO	,	PM2_RPGRefPowAI_LAvr		,	0	} ,
	{ "PM2.HF_LOAD_POS_A_LAvr"		, _K_A_IO	,	PM2_HF_LOAD_POS_A_LAvr		,	0	} ,
	{ "PM2.HF_TUNE_POS_A_LAvr"		, _K_A_IO	,	PM2_HF_TUNE_POS_A_LAvr		,	0	} ,
	{ "PM2.HF_LOAD_POS_B_LAvr"		, _K_A_IO	,	PM2_HF_LOAD_POS_B_LAvr		,	0	} ,
	{ "PM2.HF_TUNE_POS_B_LAvr"		, _K_A_IO	,	PM2_HF_TUNE_POS_B_LAvr		,	0	} ,

	{ "PM2.V97VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM2_V97VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM2.V98VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM2_V98VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM2.V100VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM2_V100VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2.V101VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM2_V101VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2.V102VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM2_V102VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2.V105VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM2_V105VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2.V106VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM2_V106VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2.V108VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM2_V108VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2.V109VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM2_V109VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2.V110VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM2_V110VlvOpenTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM2.V11VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM2_V11VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM2.V12VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM2_V12VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM2.V17VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM2_V17VlvOpenTmAI_LAvr		,	0	} ,

	{ "PM2.V97VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V97VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V98VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V98VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V100VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V100VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V101VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V101VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V102VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V102VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V105VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V105VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V106VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V106VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V108VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V108VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V109VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V109VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V110VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V110VlvCloseTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM2.V11VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V11VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V12VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V12VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2.V17VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM2_V17VlvCloseTmAI_LAvr	,	0	} ,

	{ "PM2.MFC01FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC01FlwAI_LAvr			,	0	} ,
	{ "PM2.MFC02FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC02FlwAI_LAvr			,	0	} ,
	{ "PM2.MFC03FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC03FlwAI_LAvr			,	0	} ,
	{ "PM2.MFC04FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC04FlwAI_LAvr			,	0	} ,
	{ "PM2.MFC05FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC05FlwAI_LAvr			,	0	} ,
	{ "PM2.MFC06FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC06FlwAI_LAvr			,	0	} ,
	{ "PM2.MFC07FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC07FlwAI_LAvr			,	0	} ,
	{ "PM2.MFC08FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC08FlwAI_LAvr			,	0	} ,
	{ "PM2.MFC09FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC09FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC10FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC10FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC11FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC11FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC12FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC12FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC13FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC13FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC14FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC14FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC15FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC15FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC16FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC16FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC17FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC17FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC18FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC18FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC19FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC19FlwAI_LAvr			,	0	} ,
// 	{ "PM2.MFC20FlwAI_LAvr"			, _K_A_IO	,	PM2_MFC20FlwAI_LAvr			,	0	} ,
	{ "PM2.HF_Z_Phase_A_LAvr"		, _K_A_IO	,	PM2_HF_Z_Phase_A_LAvr		,	0	},
	{ "PM2.HF_Z_Phase_B_LAvr"		, _K_A_IO	,	PM2_HF_Z_Phase_B_LAvr		,	0	},
	{ "PM2.HF_Z_Power_A_LAvr"		, _K_A_IO	,	PM2_HF_Z_Power_A_LAvr		,	0	},
	{ "PM2.HF_Z_Power_B_LAvr"		, _K_A_IO	,	PM2_HF_Z_Power_B_LAvr		,	0	},
	{ "PM2.HF_Z_R_A_LAvr"			, _K_A_IO	,	PM2_HF_Z_R_A_LAvr			,	0	},
	{ "PM2.HF_Z_R_B_LAvr"			, _K_A_IO	,	PM2_HF_Z_R_B_LAvr			,	0	},
	{ "PM2.HF_Z_X_A_LAvr"			, _K_A_IO	,	PM2_HF_Z_X_A_LAvr			,	0	},
	{ "PM2.HF_Z_X_B_LAvr"			, _K_A_IO	,	PM2_HF_Z_X_B_LAvr			,	0	},
	{ "PM2.HF_Z_Vrms_A_LAvr"		, _K_A_IO	,	PM2_HF_Z_Vrms_A_LAvr		,	0	},
	{ "PM2.HF_Z_Vrms_B_LAvr"		, _K_A_IO	,	PM2_HF_Z_Vrms_B_LAvr		,	0	},
	{ "PM2.HF_Z_Irms_A_LAvr"		, _K_A_IO	,	PM2_HF_Z_Irms_A_LAvr		,	0	},
	{ "PM2.HF_Z_Irms_B_LAvr"		, _K_A_IO	,	PM2_HF_Z_Irms_B_LAvr		,	0	},
	{ "PM2.SHT1_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_SHT1_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.SHT1_OutPwAI_LAvr"		, _K_A_IO	,	PM2_SHT1_OutPwAI_LAvr		,	0	} ,
	{ "PM2.SHT2_TmpRdAI_LAvr"		, _K_A_IO	,	PM2_SHT2_TmpRdAI_LAvr		,	0	} ,
	{ "PM2.SHT2_OutPwAI_LAvr"		, _K_A_IO	,	PM2_SHT2_OutPwAI_LAvr		,	0	} ,
	{ "PM2.StgHtr1CurrAI_LAvr"		, _K_A_IO	,	PM2_StgHtr1CurrAI_LAvr		,	0	} ,
	{ "PM2.StgHtr1VoltAI_LAvr"		, _K_A_IO	,	PM2_StgHtr1VoltAI_LAvr		,	0	} ,
	{ "PM2.StgHtr2CurrAI_LAvr"		, _K_A_IO	,	PM2_StgHtr2CurrAI_LAvr		,	0	} ,
	{ "PM2.StgHtr2VoltAI_LAvr"		, _K_A_IO	,	PM2_StgHtr2VoltAI_LAvr		,	0	} ,

	{ "PM2.PRO_CurStepAM_LAvr2"		, _K_A_IO	,	PM2_PRO_CurStepAM_LAvr2		,	0	} ,
	{ "PM2.PRO_StepTimeAM_LAvr2"	, _K_A_IO	,	PM2_PRO_StepTimeAM_LAvr2	,	0	} ,
	{ "PM2.ChmPrsAI_LAvr2"			, _K_A_IO	,	PM2_ChmPrsAI_LAvr2			,	0	} ,
	{ "PM2.PumpPrsAI_LAvr2"			, _K_A_IO	,	PM2_PumpPrsAI_LAvr2			,	0	} ,
	{ "PM2.M651_Pressure_LAvr2"		, _K_A_IO	,	PM2_M651_Pressure_LAvr2		,	0	} ,
	{ "PM2.M651_Position_LAvr2"		, _K_A_IO	,	PM2_M651_Position_LAvr2		,	0	} ,

	{ "PM2.HT01_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT01_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT02_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT02_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT03_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT03_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT04_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT04_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT05_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT05_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT06_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT06_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT07_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT07_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT08_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT08_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT09_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT09_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT10_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT10_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT11_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT11_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT12_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT12_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT13_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT13_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT14_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT14_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT15_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT15_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT16_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT16_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT17_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT17_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT18_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT18_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT19_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT19_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT20_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT20_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT21_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT21_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT22_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT22_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT23_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT23_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT24_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT24_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT25_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT25_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT26_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT26_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT27_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT27_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT28_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT28_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT29_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT29_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT30_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT30_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT31_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT31_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.HT32_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_HT32_TmpRdAI_LAvr2		,	0	} ,

	{ "PM2.RF1LoadPowAI_LAvr2"		, _K_A_IO	,	PM2_RF1LoadPowAI_LAvr2		,	0	} ,
	{ "PM2.RF2LoadPowAI_LAvr2"   	, _K_A_IO	,	PM2_RF2LoadPowAI_LAvr2		,	0	} ,
	{ "PM2.RF1RefPowAI_LAvr2"     	, _K_A_IO	,	PM2_RF1RefPowAI_LAvr2		,	0	} ,
	{ "PM2.RF2RefPowAI_LAvr2"    	, _K_A_IO	,	PM2_RF2RefPowAI_LAvr2		,	0	} ,
	{ "PM2.RPGLoadPowAI_LAvr2"   	, _K_A_IO	,	PM2_RPGLoadPowAI_LAvr2		,	0	} ,
	{ "PM2.RPGRefPowAI_LAvr2"    	, _K_A_IO	,	PM2_RPGRefPowAI_LAvr2		,	0	} ,
	{ "PM2.HF_LOAD_POS_A_LAvr2"  	, _K_A_IO	,	PM2_HF_LOAD_POS_A_LAvr2		,	0	} ,
	{ "PM2.HF_TUNE_POS_A_LAvr2"  	, _K_A_IO	,	PM2_HF_TUNE_POS_A_LAvr2		,	0	} ,
	{ "PM2.HF_LOAD_POS_B_LAvr2"  	, _K_A_IO	,	PM2_HF_LOAD_POS_B_LAvr2		,	0	} ,
	{ "PM2.HF_TUNE_POS_B_LAvr2"  	, _K_A_IO	,	PM2_HF_TUNE_POS_B_LAvr2		,	0	} ,

	{ "PM2.V97VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V97VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V98VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V98VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V100VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V100VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V101VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V101VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V102VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V102VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V105VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V105VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V106VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V106VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V108VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V108VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V109VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V109VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V110VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V110VlvOpenTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM2.V11VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V11VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V12VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V12VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2.V17VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM2_V17VlvOpenTmAI_LAvr2	,	0	} ,

	{ "PM2.V97VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V97VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V98VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V98VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V100VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V100VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V101VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V101VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V102VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V102VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V105VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V105VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V106VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V106VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V108VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V108VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V109VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V109VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V110VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V110VlvCloseTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM2.V11VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V11VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V12VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V12VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2.V17VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM2_V17VlvCloseTmAI_LAvr2	,	0	} ,

	{ "PM2.MFC01FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC01FlwAI_LAvr2		,	0	} ,
	{ "PM2.MFC02FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC02FlwAI_LAvr2		,	0	} ,
	{ "PM2.MFC03FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC03FlwAI_LAvr2		,	0	} ,
	{ "PM2.MFC04FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC04FlwAI_LAvr2		,	0	} ,
	{ "PM2.MFC05FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC05FlwAI_LAvr2		,	0	} ,
	{ "PM2.MFC06FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC06FlwAI_LAvr2		,	0	} ,
	{ "PM2.MFC07FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC07FlwAI_LAvr2		,	0	} ,
	{ "PM2.MFC08FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC08FlwAI_LAvr2		,	0	} ,
	{ "PM2.MFC09FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC09FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC10FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC10FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC11FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC11FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC12FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC12FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC13FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC13FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC14FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC14FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC15FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC15FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC16FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC16FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC17FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC17FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC18FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC18FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC19FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC19FlwAI_LAvr2		,	0	} ,
// 	{ "PM2.MFC20FlwAI_LAvr2"		, _K_A_IO	,	PM2_MFC20FlwAI_LAvr2		,	0	} ,
	{ "PM2.HF_Z_Phase_A_LAvr2"		, _K_A_IO	,	PM2_HF_Z_Phase_A_LAvr2		,	0	} ,
	{ "PM2.HF_Z_Phase_B_LAvr2"		, _K_A_IO	,	PM2_HF_Z_Phase_B_LAvr2		,	0	} ,
	{ "PM2.HF_Z_Power_A_LAvr2"		, _K_A_IO	,	PM2_HF_Z_Power_A_LAvr2		,	0	} ,
	{ "PM2.HF_Z_Power_B_LAvr2"		, _K_A_IO	,	PM2_HF_Z_Power_B_LAvr2		,	0	} ,
	{ "PM2.HF_Z_R_A_LAvr2"			, _K_A_IO	,	PM2_HF_Z_R_A_LAvr2			,	0	},
	{ "PM2.HF_Z_R_B_LAvr2"			, _K_A_IO	,	PM2_HF_Z_R_B_LAvr2			,	0	},
	{ "PM2.HF_Z_X_A_LAvr2"			, _K_A_IO	,	PM2_HF_Z_X_A_LAvr2			,	0	},
	{ "PM2.HF_Z_X_B_LAvr2"			, _K_A_IO	,	PM2_HF_Z_X_B_LAvr2			,	0	},
	{ "PM2.HF_Z_Vrms_A_LAvr2"		, _K_A_IO	,	PM2_HF_Z_Vrms_A_LAvr2		,	0	} ,
	{ "PM2.HF_Z_Vrms_B_LAvr2"		, _K_A_IO	,	PM2_HF_Z_Vrms_B_LAvr2		,	0	} ,
	{ "PM2.HF_Z_Irms_A_LAvr2"		, _K_A_IO	,	PM2_HF_Z_Irms_A_LAvr2		,	0	} ,
	{ "PM2.HF_Z_Irms_B_LAvr2"		, _K_A_IO	,	PM2_HF_Z_Irms_B_LAvr2		,	0	} ,
	{ "PM2.SHT1_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_SHT1_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.SHT1_OutPwAI_LAvr2"		, _K_A_IO	,	PM2_SHT1_OutPwAI_LAvr2		,	0	} ,
	{ "PM2.SHT2_TmpRdAI_LAvr2"		, _K_A_IO	,	PM2_SHT2_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2.SHT2_OutPwAI_LAvr2"		, _K_A_IO	,	PM2_SHT2_OutPwAI_LAvr2		,	0	} ,
	{ "PM2.StgHtr1CurrAI_LAvr2"		, _K_A_IO	,	PM2_StgHtr1CurrAI_LAvr2		,	0	} ,
	{ "PM2.StgHtr1VoltAI_LAvr2"		, _K_A_IO	,	PM2_StgHtr1VoltAI_LAvr2		,	0	} ,
	{ "PM2.StgHtr2CurrAI_LAvr2"		, _K_A_IO	,	PM2_StgHtr2CurrAI_LAvr2		,	0	} ,
	{ "PM2.StgHtr2VoltAI_LAvr2"		, _K_A_IO	,	PM2_StgHtr2VoltAI_LAvr2		,	0	} ,

	// PM2 String (DCOP-Event Report)
	{ "PM2_PRO_CurStepAM_LAvr"		, _K_S_IO	,	PM2V_PRO_CurStepAM_LAvr		,	0	} ,
	{ "PM2_PRO_StepTimeAM_LAvr"		, _K_S_IO	,	PM2V_PRO_StepTimeAM_LAvr	,	0	} ,
	{ "PM2_ChmPrsAI_LAvr"			, _K_S_IO	,	PM2V_ChmPrsAI_LAvr			,	0	} ,
	{ "PM2_PumpPrsAI_LAvr"			, _K_S_IO	,	PM2V_PumpPrsAI_LAvr			,	0	} ,
	{ "PM2_M651_Pressure_LAvr"		, _K_S_IO	,	PM2V_M651_Pressure_LAvr		,	0	} ,
	{ "PM2_M651_Position_LAvr"		, _K_S_IO	,	PM2V_M651_Position_LAvr		,	0	} ,

	{ "PM2_HT01_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT01_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT02_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT02_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT03_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT03_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT04_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT04_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT05_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT05_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT06_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT06_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT07_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT07_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT08_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT08_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT09_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT09_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT10_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT10_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT11_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT11_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT12_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT12_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT13_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT13_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT14_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT14_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT15_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT15_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT16_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT16_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT17_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT17_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT18_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT18_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT19_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT19_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT20_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT20_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT21_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT21_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT22_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT22_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT23_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT23_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT24_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT24_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT25_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT25_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT26_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT26_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT27_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT27_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT28_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT28_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT29_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT29_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT30_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT30_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT31_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT31_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_HT32_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_HT32_TmpRdAI_LAvr		,	0	} ,

	{ "PM2_RF1LoadPowAI_LAvr"		, _K_S_IO	,	PM2V_RF1LoadPowAI_LAvr		,	0	} ,
	{ "PM2_RF2LoadPowAI_LAvr"    	, _K_S_IO	,	PM2V_RF2LoadPowAI_LAvr		,	0	} ,
	{ "PM2_RF1RefPowAI_LAvr"      	, _K_S_IO	,	PM2V_RF1RefPowAI_LAvr		,	0	} ,
	{ "PM2_RF2RefPowAI_LAvr"     	, _K_S_IO	,	PM2V_RF2RefPowAI_LAvr		,	0	} ,
	{ "PM2_RPGLoadPowAI_LAvr"    	, _K_S_IO	,	PM2V_RPGLoadPowAI_LAvr		,	0	} ,
	{ "PM2_RPGRefPowAI_LAvr"     	, _K_S_IO	,	PM2V_RPGRefPowAI_LAvr		,	0	} ,
	{ "PM2_HF_LOAD_POS_A_LAvr"   	, _K_S_IO	,	PM2V_HF_LOAD_POS_A_LAvr		,	0	} ,
	{ "PM2_HF_TUNE_POS_A_LAvr"   	, _K_S_IO	,	PM2V_HF_TUNE_POS_A_LAvr		,	0	} ,
	{ "PM2_HF_LOAD_POS_B_LAvr"   	, _K_S_IO	,	PM2V_HF_LOAD_POS_B_LAvr		,	0	} ,
	{ "PM2_HF_TUNE_POS_B_LAvr"   	, _K_S_IO	,	PM2V_HF_TUNE_POS_B_LAvr		,	0	} ,

	{ "PM2_V97VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM2V_V97VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V98VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM2V_V98VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V100VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM2V_V100VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V101VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM2V_V101VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V102VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM2V_V102VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V105VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM2V_V105VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V106VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM2V_V106VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V108VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM2V_V108VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V109VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM2V_V109VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V110VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM2V_V110VlvOpenTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM2_V11VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM2V_V11VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V12VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM2V_V12VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM2_V17VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM2V_V17VlvOpenTmAI_LAvr	,	0	} ,

	{ "PM2_V97VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V97VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V98VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V98VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V100VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V100VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V101VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V101VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V102VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V102VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V105VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V105VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V106VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V106VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V108VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V108VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V109VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V109VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V110VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V110VlvCloseTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM2_V11VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V11VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V12VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V12VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM2_V17VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM2V_V17VlvCloseTmAI_LAvr	,	0	} ,

	{ "PM2_MFC01FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC01FlwAI_LAvr		,	0	} ,
	{ "PM2_MFC02FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC02FlwAI_LAvr		,	0	} ,
	{ "PM2_MFC03FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC03FlwAI_LAvr		,	0	} ,
	{ "PM2_MFC04FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC04FlwAI_LAvr		,	0	} ,
	{ "PM2_MFC05FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC05FlwAI_LAvr		,	0	} ,
	{ "PM2_MFC06FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC06FlwAI_LAvr		,	0	} ,
	{ "PM2_MFC07FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC07FlwAI_LAvr		,	0	} ,
	{ "PM2_MFC08FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC08FlwAI_LAvr		,	0	} ,
	{ "PM2_MFC09FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC09FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC10FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC10FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC11FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC11FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC12FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC12FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC13FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC13FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC14FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC14FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC15FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC15FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC16FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC16FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC17FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC17FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC18FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC18FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC19FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC19FlwAI_LAvr		,	0	} ,
// 	{ "PM2_MFC20FlwAI_LAvr"			, _K_S_IO	,	PM2V_MFC20FlwAI_LAvr		,	0	} ,
	{ "PM2_HF_Z_Phase_A_LAvr"		, _K_S_IO	,	PM2V_HF_Z_Phase_A_LAvr		,	0	} ,
	{ "PM2_HF_Z_Phase_B_LAvr"		, _K_S_IO	,	PM2V_HF_Z_Phase_B_LAvr		,	0	} ,
	{ "PM2_HF_Z_Power_A_LAvr"		, _K_S_IO	,	PM2V_HF_Z_Power_A_LAvr		,	0	} ,
	{ "PM2_HF_Z_Power_B_LAvr"		, _K_S_IO	,	PM2V_HF_Z_Power_B_LAvr		,	0	} ,
	{ "PM2_HF_Z_R_A_LAvr"			, _K_S_IO	,	PM2V_HF_Z_R_A_LAvr			,	0	} ,
	{ "PM2_HF_Z_R_B_LAvr"			, _K_S_IO	,	PM2V_HF_Z_R_B_LAvr			,	0	} ,
	{ "PM2_HF_Z_X_A_LAvr"			, _K_S_IO	,	PM2V_HF_Z_X_A_LAvr			,	0	} ,
	{ "PM2_HF_Z_X_B_LAvr"			, _K_S_IO	,	PM2V_HF_Z_X_B_LAvr			,	0	} ,
	{ "PM2_HF_Z_Vrms_A_LAvr"		, _K_S_IO	,	PM2V_HF_Z_Vrms_A_LAvr		,	0	} ,
	{ "PM2_HF_Z_Vrms_B_LAvr"		, _K_S_IO	,	PM2V_HF_Z_Vrms_B_LAvr		,	0	} ,
	{ "PM2_HF_Z_Irms_A_LAvr"		, _K_S_IO	,	PM2V_HF_Z_Irms_A_LAvr		,	0	} ,
	{ "PM2_HF_Z_Irms_B_LAvr"		, _K_S_IO	,	PM2V_HF_Z_Irms_B_LAvr		,	0	} ,
	{ "PM2_SHT1_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_SHT1_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_SHT1_OutPwAI_LAvr"		, _K_S_IO	,	PM2V_SHT1_OutPwAI_LAvr		,	0	} ,
	{ "PM2_SHT2_TmpRdAI_LAvr"		, _K_S_IO	,	PM2V_SHT2_TmpRdAI_LAvr		,	0	} ,
	{ "PM2_SHT2_OutPwAI_LAvr"		, _K_S_IO	,	PM2V_SHT2_OutPwAI_LAvr		,	0	} ,
	{ "PM2_StgHtr1CurrAI_LAvr"		, _K_S_IO	,	PM2V_StgHtr1CurrAI_LAvr		,	0	} ,
	{ "PM2_StgHtr1VoltAI_LAvr"		, _K_S_IO	,	PM2V_StgHtr1VoltAI_LAvr		,	0	} ,
	{ "PM2_StgHtr2CurrAI_LAvr"		, _K_S_IO	,	PM2V_StgHtr2CurrAI_LAvr		,	0	} ,
	{ "PM2_StgHtr2VoltAI_LAvr"		, _K_S_IO	,	PM2V_StgHtr2VoltAI_LAvr		,	0	} ,

	{ "PM2_PRO_CurStepAM_LAvr2"		, _K_S_IO	,	PM2V_PRO_CurStepAM_LAvr2	,	0	} ,
	{ "PM2_PRO_StepTimeAM_LAvr2"	, _K_S_IO	,	PM2V_PRO_StepTimeAM_LAvr2	,	0	} ,
	{ "PM2_ChmPrsAI_LAvr2"			, _K_S_IO	,	PM2V_ChmPrsAI_LAvr2			,	0	} ,
	{ "PM2_PumpPrsAI_LAvr2"			, _K_S_IO	,	PM2V_PumpPrsAI_LAvr2		,	0	} ,
	{ "PM2_M651_Pressure_LAvr2"		, _K_S_IO	,	PM2V_M651_Pressure_LAvr2	,	0	} ,
	{ "PM2_M651_Position_LAvr2"		, _K_S_IO	,	PM2V_M651_Position_LAvr2	,	0	} ,

	{ "PM2_HT01_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT01_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT02_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT02_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT03_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT03_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT04_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT04_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT05_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT05_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT06_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT06_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT07_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT07_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT08_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT08_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT09_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT09_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT10_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT10_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT11_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT11_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT12_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT12_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT13_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT13_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT14_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT14_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT15_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT15_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT16_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT16_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT17_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT17_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT18_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT18_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT19_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT19_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT20_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT20_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT21_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT21_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT22_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT22_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT23_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT23_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT24_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT24_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT25_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT25_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT26_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT26_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT27_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT27_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT28_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT28_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT29_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT29_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT30_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT30_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT31_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT31_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_HT32_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_HT32_TmpRdAI_LAvr2		,	0	} ,

	{ "PM2_RF1LoadPowAI_LAvr2"		, _K_S_IO	,	PM2V_RF1LoadPowAI_LAvr2		,	0	} ,
	{ "PM2_RF2LoadPowAI_LAvr2"		, _K_S_IO	,	PM2V_RF2LoadPowAI_LAvr2		,	0	} ,
	{ "PM2_RF1RefPowAI_LAvr2"  		, _K_S_IO	,	PM2V_RF1RefPowAI_LAvr2		,	0	} ,
	{ "PM2_RF2RefPowAI_LAvr2" 		, _K_S_IO	,	PM2V_RF2RefPowAI_LAvr2		,	0	} ,
	{ "PM2_RPGLoadPowAI_LAvr2"		, _K_S_IO	,	PM2V_RPGLoadPowAI_LAvr2		,	0	} ,
	{ "PM2_RPGRefPowAI_LAvr2" 		, _K_S_IO	,	PM2V_RPGRefPowAI_LAvr2		,	0	} ,
	{ "PM2_HF_LOAD_POS_A_LAvr2"		, _K_S_IO	,	PM2V_HF_LOAD_POS_A_LAvr2	,	0	} ,
	{ "PM2_HF_TUNE_POS_A_LAvr2"		, _K_S_IO	,	PM2V_HF_TUNE_POS_A_LAvr2	,	0	} ,
	{ "PM2_HF_LOAD_POS_B_LAvr2"		, _K_S_IO	,	PM2V_HF_LOAD_POS_B_LAvr2	,	0	} ,
	{ "PM2_HF_TUNE_POS_B_LAvr2"		, _K_S_IO	,	PM2V_HF_TUNE_POS_B_LAvr2	,	0	} ,

	{ "PM2_V97VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V97VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V98VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V98VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V100VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V100VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V101VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V101VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V102VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V102VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V105VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V105VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V106VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V106VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V108VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V108VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V109VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V109VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V110VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V110VlvOpenTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM2_V11VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V11VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V12VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V12VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM2_V17VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM2V_V17VlvOpenTmAI_LAvr2	,	0	} ,

	{ "PM2_V97VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V97VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V98VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V98VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V100VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V100VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V101VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V101VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V102VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V102VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V105VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V105VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V106VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V106VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V108VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V108VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V109VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V109VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V110VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V110VlvCloseTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM2_V11VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V11VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V12VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V12VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM2_V17VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM2V_V17VlvCloseTmAI_LAvr2	,	0	} ,

	{ "PM2_MFC01FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC01FlwAI_LAvr2		,	0	} ,
	{ "PM2_MFC02FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC02FlwAI_LAvr2		,	0	} ,
	{ "PM2_MFC03FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC03FlwAI_LAvr2		,	0	} ,
	{ "PM2_MFC04FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC04FlwAI_LAvr2		,	0	} ,
	{ "PM2_MFC05FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC05FlwAI_LAvr2		,	0	} ,
	{ "PM2_MFC06FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC06FlwAI_LAvr2		,	0	} ,
	{ "PM2_MFC07FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC07FlwAI_LAvr2		,	0	} ,
	{ "PM2_MFC08FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC08FlwAI_LAvr2		,	0	} ,
	{ "PM2_MFC09FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC09FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC10FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC10FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC11FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC11FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC12FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC12FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC13FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC13FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC14FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC14FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC15FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC15FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC16FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC16FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC17FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC17FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC18FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC18FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC19FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC19FlwAI_LAvr2		,	0	} ,
// 	{ "PM2_MFC20FlwAI_LAvr2"		, _K_S_IO	,	PM2V_MFC20FlwAI_LAvr2		,	0	} ,
	{ "PM2_HF_Z_Phase_A_LAvr2"		, _K_S_IO	,	PM2V_HF_Z_Phase_A_LAvr2		,	0	} ,
	{ "PM2_HF_Z_Phase_B_LAvr2"		, _K_S_IO	,	PM2V_HF_Z_Phase_B_LAvr2		,	0	} ,
	{ "PM2_HF_Z_Power_A_LAvr2"		, _K_S_IO	,	PM2V_HF_Z_Power_A_LAvr2		,	0	} ,
	{ "PM2_HF_Z_Power_B_LAvr2"		, _K_S_IO	,	PM2V_HF_Z_Power_B_LAvr2		,	0	} ,
	{ "PM2_HF_Z_R_A_LAvr2"			, _K_S_IO	,	PM2V_HF_Z_R_A_LAvr2			,	0	} ,
	{ "PM2_HF_Z_R_B_LAvr2"			, _K_S_IO	,	PM2V_HF_Z_R_B_LAvr2			,	0	} ,
	{ "PM2_HF_Z_X_A_LAvr2"			, _K_S_IO	,	PM2V_HF_Z_X_A_LAvr2			,	0	} ,
	{ "PM2_HF_Z_X_B_LAvr2"			, _K_S_IO	,	PM2V_HF_Z_X_B_LAvr2			,	0	} ,
	{ "PM2_HF_Z_Vrms_A_LAvr2"		, _K_S_IO	,	PM2V_HF_Z_Vrms_A_LAvr2		,	0	} ,
	{ "PM2_HF_Z_Vrms_B_LAvr2"		, _K_S_IO	,	PM2V_HF_Z_Vrms_B_LAvr2		,	0	} ,
	{ "PM2_HF_Z_Irms_A_LAvr2"		, _K_S_IO	,	PM2V_HF_Z_Irms_A_LAvr2		,	0	} ,
	{ "PM2_HF_Z_Irms_B_LAvr2"		, _K_S_IO	,	PM2V_HF_Z_Irms_B_LAvr2		,	0	} ,
	{ "PM2_SHT1_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_SHT1_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_SHT1_OutPwAI_LAvr2"		, _K_S_IO	,	PM2V_SHT1_OutPwAI_LAvr2		,	0	} ,
	{ "PM2_SHT2_TmpRdAI_LAvr2"		, _K_S_IO	,	PM2V_SHT2_TmpRdAI_LAvr2		,	0	} ,
	{ "PM2_SHT2_OutPwAI_LAvr2"		, _K_S_IO	,	PM2V_SHT2_OutPwAI_LAvr2		,	0	} ,
	{ "PM2_StgHtr1CurrAI_LAvr2"		, _K_S_IO	,	PM2V_StgHtr1CurrAI_LAvr2	,	0	} ,
	{ "PM2_StgHtr1VoltAI_LAvr2"		, _K_S_IO	,	PM2V_StgHtr1VoltAI_LAvr2	,	0	} ,
	{ "PM2_StgHtr2CurrAI_LAvr2"		, _K_S_IO	,	PM2V_StgHtr2CurrAI_LAvr2	,	0	} ,
	{ "PM2_StgHtr2VoltAI_LAvr2"		, _K_S_IO	,	PM2V_StgHtr2VoltAI_LAvr2	,	0	} ,

	//////////////////////////////////////////////////////////////////////////
	// PM3 Analog (DCOP)
	//////////////////////////////////////////////////////////////////////////
	{ "PM3.PRO_CurStepAM_LAvr"		, _K_A_IO	,	PM3_PRO_CurStepAM_LAvr		,	0	} ,
	{ "PM3.PRO_StepTimeAM_LAvr"		, _K_A_IO	,	PM3_PRO_StepTimeAM_LAvr		,	0	} ,
	{ "PM3.ChmPrsAI_LAvr"			, _K_A_IO	,	PM3_ChmPrsAI_LAvr			,	0	} ,
	{ "PM3.PumpPrsAI_LAvr"			, _K_A_IO	,	PM3_PumpPrsAI_LAvr			,	0	} ,
	{ "PM3.M651_Pressure_LAvr"		, _K_A_IO	,	PM3_M651_Pressure_LAvr		,	0	} ,
	{ "PM3.M651_Position_LAvr"		, _K_A_IO	,	PM3_M651_Position_LAvr		,	0	} ,

	{ "PM3.HT01_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT01_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT02_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT02_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT03_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT03_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT04_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT04_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT05_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT05_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT06_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT06_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT07_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT07_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT08_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT08_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT09_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT09_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT10_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT10_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT11_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT11_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT12_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT12_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT13_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT13_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT14_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT14_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT15_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT15_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT16_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT16_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT17_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT17_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT18_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT18_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT19_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT19_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT20_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT20_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT21_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT21_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT22_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT22_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT23_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT23_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT24_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT24_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT25_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT25_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT26_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT26_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT27_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT27_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT28_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT28_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT29_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT29_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT30_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT30_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT31_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT31_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.HT32_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_HT32_TmpRdAI_LAvr		,	0	} ,

	{ "PM3.RF1LoadPowAI_LAvr"		, _K_A_IO	,	PM3_RF1LoadPowAI_LAvr		,	0	} ,
	{ "PM3.RF2LoadPowAI_LAvr"		, _K_A_IO	,	PM3_RF2LoadPowAI_LAvr		,	0	} ,
	{ "PM3.RF1RefPowAI_LAvr"		, _K_A_IO	,	PM3_RF1RefPowAI_LAvr		,	0	} ,
	{ "PM3.RF2RefPowAI_LAvr"		, _K_A_IO	,	PM3_RF2RefPowAI_LAvr		,	0	} ,
	{ "PM3.RPGLoadPowAI_LAvr"		, _K_A_IO	,	PM3_RPGLoadPowAI_LAvr		,	0	} ,
	{ "PM3.RPGRefPowAI_LAvr"		, _K_A_IO	,	PM3_RPGRefPowAI_LAvr		,	0	} ,
	{ "PM3.HF_LOAD_POS_A_LAvr"		, _K_A_IO	,	PM3_HF_LOAD_POS_A_LAvr		,	0	} ,
	{ "PM3.HF_TUNE_POS_A_LAvr"		, _K_A_IO	,	PM3_HF_TUNE_POS_A_LAvr		,	0	} ,
	{ "PM3.HF_LOAD_POS_B_LAvr"		, _K_A_IO	,	PM3_HF_LOAD_POS_B_LAvr		,	0	} ,
	{ "PM3.HF_TUNE_POS_B_LAvr"		, _K_A_IO	,	PM3_HF_TUNE_POS_B_LAvr		,	0	} ,

	{ "PM3.V97VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM3_V97VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM3.V98VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM3_V98VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM3.V100VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM3_V100VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3.V101VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM3_V101VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3.V102VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM3_V102VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3.V105VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM3_V105VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3.V106VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM3_V106VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3.V108VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM3_V108VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3.V109VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM3_V109VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3.V110VlvOpenTmAI_LAvr"	, _K_A_IO	,	PM3_V110VlvOpenTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM3.V11VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM3_V11VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM3.V12VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM3_V12VlvOpenTmAI_LAvr		,	0	} ,
	{ "PM3.V17VlvOpenTmAI_LAvr"		, _K_A_IO	,	PM3_V17VlvOpenTmAI_LAvr		,	0	} ,

	{ "PM3.V97VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V97VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V98VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V98VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V100VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V100VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V101VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V101VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V102VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V102VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V105VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V105VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V106VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V106VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V108VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V108VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V109VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V109VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V110VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V110VlvCloseTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM3.V11VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V11VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V12VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V12VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3.V17VlvCloseTmAI_LAvr"	, _K_A_IO	,	PM3_V17VlvCloseTmAI_LAvr	,	0	} ,

	{ "PM3.MFC01FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC01FlwAI_LAvr			,	0	} ,
	{ "PM3.MFC02FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC02FlwAI_LAvr			,	0	} ,
	{ "PM3.MFC03FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC03FlwAI_LAvr			,	0	} ,
	{ "PM3.MFC04FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC04FlwAI_LAvr			,	0	} ,
	{ "PM3.MFC05FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC05FlwAI_LAvr			,	0	} ,
	{ "PM3.MFC06FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC06FlwAI_LAvr			,	0	} ,
	{ "PM3.MFC07FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC07FlwAI_LAvr			,	0	} ,
	{ "PM3.MFC08FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC08FlwAI_LAvr			,	0	} ,
	{ "PM3.MFC09FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC09FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC10FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC10FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC11FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC11FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC12FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC12FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC13FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC13FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC14FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC14FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC15FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC15FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC16FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC16FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC17FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC17FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC18FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC18FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC19FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC19FlwAI_LAvr			,	0	} ,
// 	{ "PM3.MFC20FlwAI_LAvr"			, _K_A_IO	,	PM3_MFC20FlwAI_LAvr			,	0	} ,
	{ "PM3.HF_Z_Phase_A_LAvr"		, _K_A_IO	,	PM3_HF_Z_Phase_A_LAvr		,	0	},
	{ "PM3.HF_Z_Phase_B_LAvr"		, _K_A_IO	,	PM3_HF_Z_Phase_B_LAvr		,	0	},
	{ "PM3.HF_Z_Power_A_LAvr"		, _K_A_IO	,	PM3_HF_Z_Power_A_LAvr		,	0	},
	{ "PM3.HF_Z_Power_B_LAvr"		, _K_A_IO	,	PM3_HF_Z_Power_B_LAvr		,	0	},
	{ "PM3.HF_Z_R_A_LAvr"			, _K_A_IO	,	PM3_HF_Z_R_A_LAvr			,	0	},
	{ "PM3.HF_Z_R_B_LAvr"			, _K_A_IO	,	PM3_HF_Z_R_B_LAvr			,	0	},
	{ "PM3.HF_Z_X_A_LAvr"			, _K_A_IO	,	PM3_HF_Z_X_A_LAvr			,	0	},
	{ "PM3.HF_Z_X_B_LAvr"			, _K_A_IO	,	PM3_HF_Z_X_B_LAvr			,	0	},
	{ "PM3.HF_Z_Vrms_A_LAvr"		, _K_A_IO	,	PM3_HF_Z_Vrms_A_LAvr		,	0	},
	{ "PM3.HF_Z_Vrms_B_LAvr"		, _K_A_IO	,	PM3_HF_Z_Vrms_B_LAvr		,	0	},
	{ "PM3.HF_Z_Irms_A_LAvr"		, _K_A_IO	,	PM3_HF_Z_Irms_A_LAvr		,	0	},
	{ "PM3.HF_Z_Irms_B_LAvr"		, _K_A_IO	,	PM3_HF_Z_Irms_B_LAvr		,	0	},
	{ "PM3.SHT1_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_SHT1_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.SHT1_OutPwAI_LAvr"		, _K_A_IO	,	PM3_SHT1_OutPwAI_LAvr		,	0	} ,
	{ "PM3.SHT2_TmpRdAI_LAvr"		, _K_A_IO	,	PM3_SHT2_TmpRdAI_LAvr		,	0	} ,
	{ "PM3.SHT2_OutPwAI_LAvr"		, _K_A_IO	,	PM3_SHT2_OutPwAI_LAvr		,	0	} ,
	{ "PM3.StgHtr1CurrAI_LAvr"		, _K_A_IO	,	PM3_StgHtr1CurrAI_LAvr		,	0	} ,
	{ "PM3.StgHtr1VoltAI_LAvr"		, _K_A_IO	,	PM3_StgHtr1VoltAI_LAvr		,	0	} ,
	{ "PM3.StgHtr2CurrAI_LAvr"		, _K_A_IO	,	PM3_StgHtr2CurrAI_LAvr		,	0	} ,
	{ "PM3.StgHtr2VoltAI_LAvr"		, _K_A_IO	,	PM3_StgHtr2VoltAI_LAvr		,	0	} ,

	{ "PM3.PRO_CurStepAM_LAvr2"		, _K_A_IO	,	PM3_PRO_CurStepAM_LAvr2		,	0	} ,
	{ "PM3.PRO_StepTimeAM_LAvr2"	, _K_A_IO	,	PM3_PRO_StepTimeAM_LAvr2	,	0	} ,
	{ "PM3.ChmPrsAI_LAvr2"			, _K_A_IO	,	PM3_ChmPrsAI_LAvr2			,	0	} ,
	{ "PM3.PumpPrsAI_LAvr2"			, _K_A_IO	,	PM3_PumpPrsAI_LAvr2			,	0	} ,
	{ "PM3.M651_Pressure_LAvr2"		, _K_A_IO	,	PM3_M651_Pressure_LAvr2		,	0	} ,
	{ "PM3.M651_Position_LAvr2"		, _K_A_IO	,	PM3_M651_Position_LAvr2		,	0	} ,

	{ "PM3.HT01_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT01_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT02_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT02_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT03_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT03_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT04_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT04_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT05_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT05_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT06_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT06_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT07_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT07_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT08_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT08_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT09_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT09_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT10_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT10_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT11_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT11_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT12_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT12_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT13_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT13_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT14_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT14_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT15_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT15_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT16_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT16_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT17_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT17_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT18_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT18_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT19_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT19_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT20_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT20_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT21_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT21_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT22_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT22_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT23_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT23_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT24_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT24_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT25_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT25_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT26_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT26_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT27_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT27_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT28_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT28_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT29_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT29_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT30_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT30_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT31_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT31_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.HT32_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_HT32_TmpRdAI_LAvr2		,	0	} ,

	{ "PM3.RF1LoadPowAI_LAvr2"		, _K_A_IO	,	PM3_RF1LoadPowAI_LAvr2		,	0	} ,
	{ "PM3.RF2LoadPowAI_LAvr2"   	, _K_A_IO	,	PM3_RF2LoadPowAI_LAvr2		,	0	} ,
	{ "PM3.RF1RefPowAI_LAvr2"     	, _K_A_IO	,	PM3_RF1RefPowAI_LAvr2		,	0	} ,
	{ "PM3.RF2RefPowAI_LAvr2"    	, _K_A_IO	,	PM3_RF2RefPowAI_LAvr2		,	0	} ,
	{ "PM3.RPGLoadPowAI_LAvr2"   	, _K_A_IO	,	PM3_RPGLoadPowAI_LAvr2		,	0	} ,
	{ "PM3.RPGRefPowAI_LAvr2"    	, _K_A_IO	,	PM3_RPGRefPowAI_LAvr2		,	0	} ,
	{ "PM3.HF_LOAD_POS_A_LAvr2"  	, _K_A_IO	,	PM3_HF_LOAD_POS_A_LAvr2		,	0	} ,
	{ "PM3.HF_TUNE_POS_A_LAvr2"  	, _K_A_IO	,	PM3_HF_TUNE_POS_A_LAvr2		,	0	} ,
	{ "PM3.HF_LOAD_POS_B_LAvr2"  	, _K_A_IO	,	PM3_HF_LOAD_POS_B_LAvr2		,	0	} ,
	{ "PM3.HF_TUNE_POS_B_LAvr2"  	, _K_A_IO	,	PM3_HF_TUNE_POS_B_LAvr2		,	0	} ,

	{ "PM3.V97VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V97VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V98VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V98VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V100VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V100VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V101VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V101VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V102VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V102VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V105VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V105VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V106VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V106VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V108VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V108VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V109VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V109VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V110VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V110VlvOpenTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM3.V11VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V11VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V12VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V12VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3.V17VlvOpenTmAI_LAvr2"	, _K_A_IO	,	PM3_V17VlvOpenTmAI_LAvr2	,	0	} ,

	{ "PM3.V97VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V97VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V98VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V98VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V100VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V100VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V101VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V101VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V102VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V102VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V105VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V105VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V106VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V106VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V108VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V108VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V109VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V109VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V110VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V110VlvCloseTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM3.V11VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V11VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V12VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V12VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3.V17VlvCloseTmAI_LAvr2"	, _K_A_IO	,	PM3_V17VlvCloseTmAI_LAvr2	,	0	} ,

	{ "PM3.MFC01FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC01FlwAI_LAvr2		,	0	} ,
	{ "PM3.MFC02FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC02FlwAI_LAvr2		,	0	} ,
	{ "PM3.MFC03FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC03FlwAI_LAvr2		,	0	} ,
	{ "PM3.MFC04FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC04FlwAI_LAvr2		,	0	} ,
	{ "PM3.MFC05FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC05FlwAI_LAvr2		,	0	} ,
	{ "PM3.MFC06FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC06FlwAI_LAvr2		,	0	} ,
	{ "PM3.MFC07FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC07FlwAI_LAvr2		,	0	} ,
	{ "PM3.MFC08FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC08FlwAI_LAvr2		,	0	} ,
	{ "PM3.MFC09FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC09FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC10FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC10FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC11FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC11FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC12FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC12FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC13FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC13FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC14FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC14FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC15FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC15FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC16FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC16FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC17FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC17FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC18FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC18FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC19FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC19FlwAI_LAvr2		,	0	} ,
// 	{ "PM3.MFC20FlwAI_LAvr2"		, _K_A_IO	,	PM3_MFC20FlwAI_LAvr2		,	0	} ,
	{ "PM3.HF_Z_Phase_A_LAvr2"		, _K_A_IO	,	PM3_HF_Z_Phase_A_LAvr2		,	0	} ,
	{ "PM3.HF_Z_Phase_B_LAvr2"		, _K_A_IO	,	PM3_HF_Z_Phase_B_LAvr2		,	0	} ,
	{ "PM3.HF_Z_Power_A_LAvr2"		, _K_A_IO	,	PM3_HF_Z_Power_A_LAvr2		,	0	} ,
	{ "PM3.HF_Z_Power_B_LAvr2"		, _K_A_IO	,	PM3_HF_Z_Power_B_LAvr2		,	0	} ,
	{ "PM3.HF_Z_R_A_LAvr2"			, _K_A_IO	,	PM3_HF_Z_R_A_LAvr2			,	0	},
	{ "PM3.HF_Z_R_B_LAvr2"			, _K_A_IO	,	PM3_HF_Z_R_B_LAvr2			,	0	},
	{ "PM3.HF_Z_X_A_LAvr2"			, _K_A_IO	,	PM3_HF_Z_X_A_LAvr2			,	0	},
	{ "PM3.HF_Z_X_B_LAvr2"			, _K_A_IO	,	PM3_HF_Z_X_B_LAvr2			,	0	},
	{ "PM3.HF_Z_Vrms_A_LAvr2"		, _K_A_IO	,	PM3_HF_Z_Vrms_A_LAvr2		,	0	} ,
	{ "PM3.HF_Z_Vrms_B_LAvr2"		, _K_A_IO	,	PM3_HF_Z_Vrms_B_LAvr2		,	0	} ,
	{ "PM3.HF_Z_Irms_A_LAvr2"		, _K_A_IO	,	PM3_HF_Z_Irms_A_LAvr2		,	0	} ,
	{ "PM3.HF_Z_Irms_B_LAvr2"		, _K_A_IO	,	PM3_HF_Z_Irms_B_LAvr2		,	0	} ,
	{ "PM3.SHT1_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_SHT1_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.SHT1_OutPwAI_LAvr2"		, _K_A_IO	,	PM3_SHT1_OutPwAI_LAvr2		,	0	} ,
	{ "PM3.SHT2_TmpRdAI_LAvr2"		, _K_A_IO	,	PM3_SHT2_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3.SHT2_OutPwAI_LAvr2"		, _K_A_IO	,	PM3_SHT2_OutPwAI_LAvr2		,	0	} ,
	{ "PM3.StgHtr1CurrAI_LAvr2"		, _K_A_IO	,	PM3_StgHtr1CurrAI_LAvr2		,	0	} ,
	{ "PM3.StgHtr1VoltAI_LAvr2"		, _K_A_IO	,	PM3_StgHtr1VoltAI_LAvr2		,	0	} ,
	{ "PM3.StgHtr2CurrAI_LAvr2"		, _K_A_IO	,	PM3_StgHtr2CurrAI_LAvr2		,	0	} ,
	{ "PM3.StgHtr2VoltAI_LAvr2"		, _K_A_IO	,	PM3_StgHtr2VoltAI_LAvr2		,	0	} ,

	// PM3 String (DCOP-Event Report)
	{ "PM3_PRO_CurStepAM_LAvr"		, _K_S_IO	,	PM3V_PRO_CurStepAM_LAvr		,	0	} ,
	{ "PM3_PRO_StepTimeAM_LAvr"		, _K_S_IO	,	PM3V_PRO_StepTimeAM_LAvr	,	0	} ,
	{ "PM3_ChmPrsAI_LAvr"			, _K_S_IO	,	PM3V_ChmPrsAI_LAvr			,	0	} ,
	{ "PM3_PumpPrsAI_LAvr"			, _K_S_IO	,	PM3V_PumpPrsAI_LAvr			,	0	} ,
	{ "PM3_M651_Pressure_LAvr"		, _K_S_IO	,	PM3V_M651_Pressure_LAvr		,	0	} ,
	{ "PM3_M651_Position_LAvr"		, _K_S_IO	,	PM3V_M651_Position_LAvr		,	0	} ,

	{ "PM3_HT01_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT01_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT02_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT02_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT03_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT03_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT04_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT04_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT05_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT05_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT06_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT06_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT07_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT07_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT08_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT08_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT09_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT09_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT10_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT10_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT11_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT11_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT12_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT12_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT13_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT13_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT14_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT14_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT15_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT15_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT16_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT16_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT17_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT17_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT18_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT18_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT19_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT19_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT20_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT20_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT21_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT21_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT22_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT22_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT23_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT23_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT24_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT24_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT25_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT25_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT26_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT26_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT27_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT27_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT28_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT28_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT29_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT29_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT30_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT30_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT31_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT31_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_HT32_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_HT32_TmpRdAI_LAvr		,	0	} ,

	{ "PM3_RF1LoadPowAI_LAvr"		, _K_S_IO	,	PM3V_RF1LoadPowAI_LAvr		,	0	} ,
	{ "PM3_RF2LoadPowAI_LAvr"    	, _K_S_IO	,	PM3V_RF2LoadPowAI_LAvr		,	0	} ,
	{ "PM3_RF1RefPowAI_LAvr"      	, _K_S_IO	,	PM3V_RF1RefPowAI_LAvr		,	0	} ,
	{ "PM3_RF2RefPowAI_LAvr"     	, _K_S_IO	,	PM3V_RF2RefPowAI_LAvr		,	0	} ,
	{ "PM3_RPGLoadPowAI_LAvr"    	, _K_S_IO	,	PM3V_RPGLoadPowAI_LAvr		,	0	} ,
	{ "PM3_RPGRefPowAI_LAvr"     	, _K_S_IO	,	PM3V_RPGRefPowAI_LAvr		,	0	} ,
	{ "PM3_HF_LOAD_POS_A_LAvr"   	, _K_S_IO	,	PM3V_HF_LOAD_POS_A_LAvr		,	0	} ,
	{ "PM3_HF_TUNE_POS_A_LAvr"   	, _K_S_IO	,	PM3V_HF_TUNE_POS_A_LAvr		,	0	} ,
	{ "PM3_HF_LOAD_POS_B_LAvr"   	, _K_S_IO	,	PM3V_HF_LOAD_POS_B_LAvr		,	0	} ,
	{ "PM3_HF_TUNE_POS_B_LAvr"   	, _K_S_IO	,	PM3V_HF_TUNE_POS_B_LAvr		,	0	} ,

	{ "PM3_V97VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM3V_V97VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V98VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM3V_V98VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V100VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM3V_V100VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V101VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM3V_V101VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V102VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM3V_V102VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V105VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM3V_V105VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V106VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM3V_V106VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V108VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM3V_V108VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V109VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM3V_V109VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V110VlvOpenTmAI_LAvr"	, _K_S_IO	,	PM3V_V110VlvOpenTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM3_V11VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM3V_V11VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V12VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM3V_V12VlvOpenTmAI_LAvr	,	0	} ,
	{ "PM3_V17VlvOpenTmAI_LAvr"		, _K_S_IO	,	PM3V_V17VlvOpenTmAI_LAvr	,	0	} ,

	{ "PM3_V97VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V97VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V98VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V98VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V100VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V100VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V101VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V101VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V102VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V102VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V105VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V105VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V106VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V106VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V108VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V108VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V109VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V109VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V110VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V110VlvCloseTmAI_LAvr	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM3_V11VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V11VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V12VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V12VlvCloseTmAI_LAvr	,	0	} ,
	{ "PM3_V17VlvCloseTmAI_LAvr"	, _K_S_IO	,	PM3V_V17VlvCloseTmAI_LAvr	,	0	} ,

	{ "PM3_MFC01FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC01FlwAI_LAvr		,	0	} ,
	{ "PM3_MFC02FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC02FlwAI_LAvr		,	0	} ,
	{ "PM3_MFC03FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC03FlwAI_LAvr		,	0	} ,
	{ "PM3_MFC04FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC04FlwAI_LAvr		,	0	} ,
	{ "PM3_MFC05FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC05FlwAI_LAvr		,	0	} ,
	{ "PM3_MFC06FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC06FlwAI_LAvr		,	0	} ,
	{ "PM3_MFC07FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC07FlwAI_LAvr		,	0	} ,
	{ "PM3_MFC08FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC08FlwAI_LAvr		,	0	} ,
	{ "PM3_MFC09FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC09FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC10FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC10FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC11FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC11FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC12FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC12FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC13FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC13FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC14FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC14FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC15FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC15FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC16FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC16FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC17FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC17FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC18FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC18FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC19FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC19FlwAI_LAvr		,	0	} ,
// 	{ "PM3_MFC20FlwAI_LAvr"			, _K_S_IO	,	PM3V_MFC20FlwAI_LAvr		,	0	} ,
	{ "PM3_HF_Z_Phase_A_LAvr"		, _K_S_IO	,	PM3V_HF_Z_Phase_A_LAvr		,	0	} ,
	{ "PM3_HF_Z_Phase_B_LAvr"		, _K_S_IO	,	PM3V_HF_Z_Phase_B_LAvr		,	0	} ,
	{ "PM3_HF_Z_Power_A_LAvr"		, _K_S_IO	,	PM3V_HF_Z_Power_A_LAvr		,	0	} ,
	{ "PM3_HF_Z_Power_B_LAvr"		, _K_S_IO	,	PM3V_HF_Z_Power_B_LAvr		,	0	} ,
	{ "PM3_HF_Z_R_A_LAvr"			, _K_S_IO	,	PM3V_HF_Z_R_A_LAvr			,	0	} ,
	{ "PM3_HF_Z_R_B_LAvr"			, _K_S_IO	,	PM3V_HF_Z_R_B_LAvr			,	0	} ,
	{ "PM3_HF_Z_X_A_LAvr"			, _K_S_IO	,	PM3V_HF_Z_X_A_LAvr			,	0	} ,
	{ "PM3_HF_Z_X_B_LAvr"			, _K_S_IO	,	PM3V_HF_Z_X_B_LAvr			,	0	} ,
	{ "PM3_HF_Z_Vrms_A_LAvr"		, _K_S_IO	,	PM3V_HF_Z_Vrms_A_LAvr		,	0	} ,
	{ "PM3_HF_Z_Vrms_B_LAvr"		, _K_S_IO	,	PM3V_HF_Z_Vrms_B_LAvr		,	0	} ,
	{ "PM3_HF_Z_Irms_A_LAvr"		, _K_S_IO	,	PM3V_HF_Z_Irms_A_LAvr		,	0	} ,
	{ "PM3_HF_Z_Irms_B_LAvr"		, _K_S_IO	,	PM3V_HF_Z_Irms_B_LAvr		,	0	} ,
	{ "PM3_SHT1_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_SHT1_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_SHT1_OutPwAI_LAvr"		, _K_S_IO	,	PM3V_SHT1_OutPwAI_LAvr		,	0	} ,
	{ "PM3_SHT2_TmpRdAI_LAvr"		, _K_S_IO	,	PM3V_SHT2_TmpRdAI_LAvr		,	0	} ,
	{ "PM3_SHT2_OutPwAI_LAvr"		, _K_S_IO	,	PM3V_SHT2_OutPwAI_LAvr		,	0	} ,
	{ "PM3_StgHtr1CurrAI_LAvr"		, _K_S_IO	,	PM3V_StgHtr1CurrAI_LAvr		,	0	} ,
	{ "PM3_StgHtr1VoltAI_LAvr"		, _K_S_IO	,	PM3V_StgHtr1VoltAI_LAvr		,	0	} ,
	{ "PM3_StgHtr2CurrAI_LAvr"		, _K_S_IO	,	PM3V_StgHtr2CurrAI_LAvr		,	0	} ,
	{ "PM3_StgHtr2VoltAI_LAvr"		, _K_S_IO	,	PM3V_StgHtr2VoltAI_LAvr		,	0	} ,

	{ "PM3_PRO_CurStepAM_LAvr2"		, _K_S_IO	,	PM3V_PRO_CurStepAM_LAvr2	,	0	} ,
	{ "PM3_PRO_StepTimeAM_LAvr2"	, _K_S_IO	,	PM3V_PRO_StepTimeAM_LAvr2	,	0	} ,
	{ "PM3_ChmPrsAI_LAvr2"			, _K_S_IO	,	PM3V_ChmPrsAI_LAvr2			,	0	} ,
	{ "PM3_PumpPrsAI_LAvr2"			, _K_S_IO	,	PM3V_PumpPrsAI_LAvr2		,	0	} ,
	{ "PM3_M651_Pressure_LAvr2"		, _K_S_IO	,	PM3V_M651_Pressure_LAvr2	,	0	} ,
	{ "PM3_M651_Position_LAvr2"		, _K_S_IO	,	PM3V_M651_Position_LAvr2	,	0	} ,

	{ "PM3_HT01_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT01_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT02_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT02_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT03_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT03_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT04_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT04_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT05_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT05_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT06_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT06_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT07_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT07_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT08_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT08_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT09_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT09_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT10_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT10_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT11_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT11_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT12_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT12_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT13_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT13_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT14_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT14_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT15_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT15_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT16_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT16_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT17_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT17_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT18_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT18_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT19_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT19_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT20_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT20_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT21_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT21_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT22_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT22_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT23_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT23_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT24_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT24_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT25_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT25_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT26_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT26_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT27_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT27_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT28_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT28_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT29_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT29_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT30_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT30_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT31_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT31_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_HT32_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_HT32_TmpRdAI_LAvr2		,	0	} ,

	{ "PM3_RF1LoadPowAI_LAvr2"		, _K_S_IO	,	PM3V_RF1LoadPowAI_LAvr2		,	0	} ,
	{ "PM3_RF2LoadPowAI_LAvr2"		, _K_S_IO	,	PM3V_RF2LoadPowAI_LAvr2		,	0	} ,
	{ "PM3_RF1RefPowAI_LAvr2"  		, _K_S_IO	,	PM3V_RF1RefPowAI_LAvr2		,	0	} ,
	{ "PM3_RF2RefPowAI_LAvr2" 		, _K_S_IO	,	PM3V_RF2RefPowAI_LAvr2		,	0	} ,
	{ "PM3_RPGLoadPowAI_LAvr2"		, _K_S_IO	,	PM3V_RPGLoadPowAI_LAvr2		,	0	} ,
	{ "PM3_RPGRefPowAI_LAvr2" 		, _K_S_IO	,	PM3V_RPGRefPowAI_LAvr2		,	0	} ,
	{ "PM3_HF_LOAD_POS_A_LAvr2"		, _K_S_IO	,	PM3V_HF_LOAD_POS_A_LAvr2	,	0	} ,
	{ "PM3_HF_TUNE_POS_A_LAvr2"		, _K_S_IO	,	PM3V_HF_TUNE_POS_A_LAvr2	,	0	} ,
	{ "PM3_HF_LOAD_POS_B_LAvr2"		, _K_S_IO	,	PM3V_HF_LOAD_POS_B_LAvr2	,	0	} ,
	{ "PM3_HF_TUNE_POS_B_LAvr2"		, _K_S_IO	,	PM3V_HF_TUNE_POS_B_LAvr2	,	0	} ,

	{ "PM3_V97VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V97VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V98VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V98VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V100VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V100VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V101VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V101VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V102VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V102VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V105VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V105VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V106VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V106VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V108VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V108VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V109VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V109VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V110VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V110VlvOpenTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM3_V11VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V11VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V12VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V12VlvOpenTmAI_LAvr2	,	0	} ,
	{ "PM3_V17VlvOpenTmAI_LAvr2"	, _K_S_IO	,	PM3V_V17VlvOpenTmAI_LAvr2	,	0	} ,

	{ "PM3_V97VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V97VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V98VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V98VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V100VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V100VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V101VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V101VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V102VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V102VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V105VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V105VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V106VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V106VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V108VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V108VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V109VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V109VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V110VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V110VlvCloseTmAI_LAvr2	,	0	} ,

	//... 2015.07.09 Add ALD Valve V11, V12, V17
	{ "PM3_V11VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V11VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V12VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V12VlvCloseTmAI_LAvr2	,	0	} ,
	{ "PM3_V17VlvCloseTmAI_LAvr2"	, _K_S_IO	,	PM3V_V17VlvCloseTmAI_LAvr2	,	0	} ,

	{ "PM3_MFC01FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC01FlwAI_LAvr2		,	0	} ,
	{ "PM3_MFC02FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC02FlwAI_LAvr2		,	0	} ,
	{ "PM3_MFC03FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC03FlwAI_LAvr2		,	0	} ,
	{ "PM3_MFC04FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC04FlwAI_LAvr2		,	0	} ,
	{ "PM3_MFC05FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC05FlwAI_LAvr2		,	0	} ,
	{ "PM3_MFC06FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC06FlwAI_LAvr2		,	0	} ,
	{ "PM3_MFC07FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC07FlwAI_LAvr2		,	0	} ,
	{ "PM3_MFC08FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC08FlwAI_LAvr2		,	0	} ,
	{ "PM3_MFC09FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC09FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC10FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC10FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC11FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC11FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC12FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC12FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC13FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC13FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC14FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC14FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC15FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC15FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC16FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC16FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC17FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC17FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC18FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC18FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC19FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC19FlwAI_LAvr2		,	0	} ,
// 	{ "PM3_MFC20FlwAI_LAvr2"		, _K_S_IO	,	PM3V_MFC20FlwAI_LAvr2		,	0	} ,
	{ "PM3_HF_Z_Phase_A_LAvr2"		, _K_S_IO	,	PM3V_HF_Z_Phase_A_LAvr2		,	0	} ,
	{ "PM3_HF_Z_Phase_B_LAvr2"		, _K_S_IO	,	PM3V_HF_Z_Phase_B_LAvr2		,	0	} ,
	{ "PM3_HF_Z_Power_A_LAvr2"		, _K_S_IO	,	PM3V_HF_Z_Power_A_LAvr2		,	0	} ,
	{ "PM3_HF_Z_Power_B_LAvr2"		, _K_S_IO	,	PM3V_HF_Z_Power_B_LAvr2		,	0	} ,
	{ "PM3_HF_Z_R_A_LAvr2"			, _K_S_IO	,	PM3V_HF_Z_R_A_LAvr2			,	0	} ,
	{ "PM3_HF_Z_R_B_LAvr2"			, _K_S_IO	,	PM3V_HF_Z_R_B_LAvr2			,	0	} ,
	{ "PM3_HF_Z_X_A_LAvr2"			, _K_S_IO	,	PM3V_HF_Z_X_A_LAvr2			,	0	} ,
	{ "PM3_HF_Z_X_B_LAvr2"			, _K_S_IO	,	PM3V_HF_Z_X_B_LAvr2			,	0	} ,
	{ "PM3_HF_Z_Vrms_A_LAvr2"		, _K_S_IO	,	PM3V_HF_Z_Vrms_A_LAvr2		,	0	} ,
	{ "PM3_HF_Z_Vrms_B_LAvr2"		, _K_S_IO	,	PM3V_HF_Z_Vrms_B_LAvr2		,	0	} ,
	{ "PM3_HF_Z_Irms_A_LAvr2"		, _K_S_IO	,	PM3V_HF_Z_Irms_A_LAvr2		,	0	} ,
	{ "PM3_HF_Z_Irms_B_LAvr2"		, _K_S_IO	,	PM3V_HF_Z_Irms_B_LAvr2		,	0	} ,
	{ "PM3_SHT1_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_SHT1_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_SHT1_OutPwAI_LAvr2"		, _K_S_IO	,	PM3V_SHT1_OutPwAI_LAvr2		,	0	} ,
	{ "PM3_SHT2_TmpRdAI_LAvr2"		, _K_S_IO	,	PM3V_SHT2_TmpRdAI_LAvr2		,	0	} ,
	{ "PM3_SHT2_OutPwAI_LAvr2"		, _K_S_IO	,	PM3V_SHT2_OutPwAI_LAvr2		,	0	} ,
	{ "PM3_StgHtr1CurrAI_LAvr2"		, _K_S_IO	,	PM3V_StgHtr1CurrAI_LAvr2	,	0	} ,
	{ "PM3_StgHtr1VoltAI_LAvr2"		, _K_S_IO	,	PM3V_StgHtr1VoltAI_LAvr2	,	0	} ,
	{ "PM3_StgHtr2CurrAI_LAvr2"		, _K_S_IO	,	PM3V_StgHtr2CurrAI_LAvr2	,	0	} ,
	{ "PM3_StgHtr2VoltAI_LAvr2"		, _K_S_IO	,	PM3V_StgHtr2VoltAI_LAvr2	,	0	} ,

	{ "CTC.PM1_CommStatus"		, _K_D_IO		,	PM1_CommStatus				,	0	} ,
	{ "CTC.PM2_CommStatus"		, _K_D_IO		,	PM2_CommStatus				,	0	} ,
	{ "CTC.PM3_CommStatus"		, _K_D_IO		,	PM3_CommStatus				,	0	} ,

	{ "LPMA.PM1_Use"			, _K_D_IO		,	LPMA_PM1_Use				,	0	} ,
	{ "LPMA.PM2_Use"			, _K_D_IO		,	LPMA_PM2_Use				,	0	} ,
	{ "LPMA.PM3_Use"			, _K_D_IO		,	LPMA_PM3_Use				,	0	} ,

	{ "LPMB.PM1_Use"			, _K_D_IO		,	LPMB_PM1_Use				,	0	} ,
	{ "LPMB.PM2_Use"			, _K_D_IO		,	LPMB_PM2_Use				,	0	} ,
	{ "LPMB.PM3_Use"			, _K_D_IO		,	LPMB_PM3_Use				,	0	} ,

	{ "LPMC.PM1_Use"			, _K_D_IO		,	LPMC_PM1_Use				,	0	} ,
	{ "LPMC.PM2_Use"			, _K_D_IO		,	LPMC_PM2_Use				,	0	} ,
	{ "LPMC.PM3_Use"			, _K_D_IO		,	LPMC_PM3_Use				,	0	} ,

	{ "CTC.PM1_MIDName"			, _K_S_IO		,	PM1_MID						,	0   } ,
	{ "CTC.PM1_MIDName2"		, _K_S_IO		,	PM1_MID2					,	0   } ,
	{ "CTC.PM2_MIDName"			, _K_S_IO		,	PM2_MID						,	0   } ,
	{ "CTC.PM2_MIDName2"		, _K_S_IO		,	PM2_MID2					,	0   } ,
	{ "CTC.PM3_MIDName"			, _K_S_IO		,	PM3_MID						,	0   } ,
	{ "CTC.PM3_MIDName2"		, _K_S_IO		,	PM3_MID2					,	0   } ,

	{ "CTC.MID_NAME"			, _K_S_IO		,	PORT1_MID					,	0	} ,
	{ "CTC.MID_NAME2"			, _K_S_IO		,	PORT2_MID					,	0	} ,
	{ "CTC.MID_NAME3"			, _K_S_IO		,	PORT3_MID					,	0	} ,

	{ "PRMD_PM1_WETCWfrCnt"		, _K_D_IO		,	PRMD_PM1_WETCWfrCnt			,	0	} ,
	{ "PRMD_PM1_DRYCWfrCnt"		, _K_D_IO		,	PRMD_PM1_DRYCWfrCnt			,	0	} ,
	{ "PRMD_PM1_TRAPCWfrCnt"	, _K_D_IO		,	PRMD_PM1_TRAPCWfrCnt		,	0	} ,
	{ "PRMD_PM2_WETCWfrCnt"		, _K_D_IO		,	PRMD_PM2_WETCWfrCnt			,	0	} ,
	{ "PRMD_PM2_DRYCWfrCnt"		, _K_D_IO		,	PRMD_PM2_DRYCWfrCnt			,	0	} ,
	{ "PRMD_PM2_TRAPCWfrCnt"	, _K_D_IO		,	PRMD_PM2_TRAPCWfrCnt		,	0	} ,
	{ "PRMD_PM3_WETCWfrCnt"		, _K_D_IO		,	PRMD_PM3_WETCWfrCnt			,	0	} ,
	{ "PRMD_PM3_DRYCWfrCnt"		, _K_D_IO		,	PRMD_PM3_DRYCWfrCnt			,	0	} ,
	{ "PRMD_PM3_TRAPCWfrCnt"	, _K_D_IO		,	PRMD_PM3_TRAPCWfrCnt		,	0	} ,

	{ "GEM.PRMD_PM1_WETCWfrCnt"		, _K_D_IO	,	GEM_PM1_WETCWfrCnt			,	0	} ,
	{ "GEM.PRMD_PM1_DRYCWfrCnt"		, _K_D_IO	,	GEM_PM1_DRYCWfrCnt			,	0	} ,
	{ "GEM.PRMD_PM1_TRAPCWfrCnt"	, _K_D_IO	,	GEM_PM1_TRAPCWfrCnt			,	0	} ,
	{ "GEM.PRMD_PM2_WETCWfrCnt"		, _K_D_IO	,	GEM_PM2_WETCWfrCnt			,	0	} ,
	{ "GEM.PRMD_PM2_DRYCWfrCnt"		, _K_D_IO	,	GEM_PM2_DRYCWfrCnt			,	0	} ,
	{ "GEM.PRMD_PM2_TRAPCWfrCnt"	, _K_D_IO	,	GEM_PM2_TRAPCWfrCnt			,	0	} ,
	{ "GEM.PRMD_PM3_WETCWfrCnt"		, _K_D_IO	,	GEM_PM3_WETCWfrCnt			,	0	} ,
	{ "GEM.PRMD_PM3_DRYCWfrCnt"		, _K_D_IO	,	GEM_PM3_DRYCWfrCnt			,	0	} ,
	{ "GEM.PRMD_PM3_TRAPCWfrCnt"	, _K_D_IO	,	GEM_PM3_TRAPCWfrCnt			,	0	} ,

	{ "GEM.Port1_LotID"				, _K_S_IO	,	GEM_Port1_LotID				,	0	} ,
	{ "GEM.Port2_LotID"				, _K_S_IO	,	GEM_Port2_LotID				,	0	} ,
	{ "GEM.Port3_LotID"				, _K_S_IO	,	GEM_Port3_LotID				,	0	} ,

	{ "CTC.MID_READ"				, _K_S_IO	,	MID_READ					,	0	} ,
	{ "CTC.MID_READ2"				, _K_S_IO	,	MID_READ2					,	0	} ,
	{ "CTC.MID_READ3"				, _K_S_IO	,	MID_READ3					,	0	} ,

	{ "CTC.JOB_NAME"				, _K_S_IO	,	JID_NAME					,	0	} ,
	{ "CTC.JOB_NAME2"				, _K_S_IO	,	JID_NAME2					,	0	} ,
	{ "CTC.JOB_NAME3"				, _K_S_IO	,	JID_NAME3					,	0	} ,

	{ "CTC.JID_READ"				, _K_S_IO	,	JID_READ					,	0	} ,
	{ "CTC.JID_READ2"				, _K_S_IO	,	JID_READ2					,	0	} ,
	{ "CTC.JID_READ3"				, _K_S_IO	,	JID_READ3					,	0	} ,

	{ "CTC.RECIPE_FILE"				, _K_S_IO	,	CM1_RECIPE_FILE					,	0	} ,
	{ "CTC.RECIPE_FILE2"			, _K_S_IO	,	CM2_RECIPE_FILE				,	0	} ,
	{ "CTC.RECIPE_FILE3"			, _K_S_IO	,	CM3_RECIPE_FILE				,	0	} ,

	{ "CM1.PROCESS_END"				, _K_D_IO	,	CM1_PROCESS_END				,	0	} ,
	{ "CM2.PROCESS_END"				, _K_D_IO	,	CM2_PROCESS_END				,	0	} ,
	{ "CM3.PROCESS_END"				, _K_D_IO	,	CM3_PROCESS_END				,	0	} ,

	{ "CTC.START_SLOT"				, _K_D_IO	,	START_SLOT					,	0	} ,
	{ "CTC.START_SLOT2"				, _K_D_IO	,	START_SLOT2					,	0	} ,
	{ "CTC.START_SLOT3"				, _K_D_IO	,	START_SLOT3					,	0	} ,
	{ "CTC.END_SLOT"				, _K_D_IO	,	END_SLOT					,	0	} ,
	{ "CTC.END_SLOT2"				, _K_D_IO	,	END_SLOT2					,	0	} ,
	{ "CTC.END_SLOT3"				, _K_D_IO	,	END_SLOT3					,	0	} ,

	{ "CTC.LOOP_COUNT"				, _K_D_IO	,	LOOP_COUNT					,	0	} ,
	{ "CTC.LOOP_COUNT2"				, _K_D_IO	,	LOOP_COUNT2					,	0	} ,
	{ "CTC.LOOP_COUNT3"				, _K_D_IO	,	LOOP_COUNT3					,	0	} ,

    { "CTC.LOTPRE_USE"              , _K_D_IO	,	CTC_LOTPRE_USE				,   0   } ,
	{ "CTC.LOTPRE_USE2"             , _K_D_IO	,   CTC_LOTPRE_USE2				,   0   } ,
	{ "CTC.LOTPRE_USE3"             , _K_D_IO	,   CTC_LOTPRE_USE3				,   0   } ,

	{ "CTC.PM1_PPIDName"			, _K_S_IO	,	PM1_PPIDName				,	0	} ,
	{ "CTC.PM2_PPIDName"			, _K_S_IO	,	PM2_PPIDName				,	0	} ,
	{ "CTC.PM3_PPIDName"			, _K_S_IO	,	PM3_PPIDName				,	0	} ,

	{ "CTC.FA_STATUS_AGV_CM1"		, _K_D_IO	,	AGV_MODE_CM1				,	0   } ,
    { "CTC.FA_STATUS_AGV_CM2" 		, _K_D_IO	,	AGV_MODE_CM2				,	0   } ,
    { "CTC.FA_STATUS_AGV_CM3"		, _K_D_IO	,	AGV_MODE_CM3				,	0   } ,

	{ "GEM.PJ_WfrPrcCnt"			, _K_D_IO	,	GEM_PJ_WfrPrcCnt    		,	0	} ,
	{ "GEM.CJ_WfrPrcCnt"			, _K_D_IO	,	GEM_CJ_WfrPrcCnt    		,	0	} ,
	{ "GEM.CM_WfrPrcCnt"			, _K_D_IO	,	GEM_CM_WfrPrcCnt    		,	0	} ,

	{ "GEM.CM1_WfrPrcCnt"			, _K_D_IO	,	GEM_CM1_WfrPrcCnt    		,	0	} ,
	{ "GEM.CM2_WfrPrcCnt"			, _K_D_IO	,	GEM_CM2_WfrPrcCnt    		,	0	} ,
	{ "GEM.CM3_WfrPrcCnt"			, _K_D_IO	,	GEM_CM3_WfrPrcCnt    		,	0	} ,

    { "CTC.OPERATOR_START"			, _K_D_IO	,	OPERATOR_START				,	0   } ,
    { "CTC.OPERATOR_START2"			, _K_D_IO	,	OPERATOR_START2				,	0   } ,
    { "CTC.OPERATOR_START3"			, _K_D_IO	,	OPERATOR_START3				,	0   } ,

	{ "GEM.Subst1_PMZoneHistory"		, _K_S_IO	,   GEM_Subst01_PMZoneHistory		,	0   } ,
	{ "GEM.Subst2_PMZoneHistory"		, _K_S_IO	,   GEM_Subst02_PMZoneHistory		,	0   } ,
	{ "GEM.Subst3_PMZoneHistory"		, _K_S_IO	,   GEM_Subst03_PMZoneHistory		,	0   } ,
	{ "GEM.Subst4_PMZoneHistory"		, _K_S_IO	,   GEM_Subst04_PMZoneHistory		,	0   } ,
	{ "GEM.Subst5_PMZoneHistory"		, _K_S_IO	,   GEM_Subst05_PMZoneHistory		,	0   } ,
	{ "GEM.Subst6_PMZoneHistory"		, _K_S_IO	,   GEM_Subst06_PMZoneHistory		,	0   } ,
	{ "GEM.Subst7_PMZoneHistory"		, _K_S_IO	,   GEM_Subst07_PMZoneHistory		,	0   } ,
	{ "GEM.Subst8_PMZoneHistory"		, _K_S_IO	,   GEM_Subst08_PMZoneHistory		,	0   } ,
	{ "GEM.Subst9_PMZoneHistory"		, _K_S_IO	,   GEM_Subst09_PMZoneHistory		,	0   } ,
	{ "GEM.Subst10_PMZoneHistory"		, _K_S_IO	,   GEM_Subst10_PMZoneHistory		,	0   } ,
	{ "GEM.Subst11_PMZoneHistory"		, _K_S_IO	,   GEM_Subst11_PMZoneHistory		,	0   } ,
	{ "GEM.Subst12_PMZoneHistory"		, _K_S_IO	,   GEM_Subst12_PMZoneHistory		,	0   } ,
	{ "GEM.Subst13_PMZoneHistory"		, _K_S_IO	,   GEM_Subst13_PMZoneHistory		,	0   } ,
	{ "GEM.Subst14_PMZoneHistory"		, _K_S_IO	,   GEM_Subst14_PMZoneHistory		,	0   } ,
	{ "GEM.Subst15_PMZoneHistory"		, _K_S_IO	,   GEM_Subst15_PMZoneHistory		,	0   } ,
	{ "GEM.Subst16_PMZoneHistory"		, _K_S_IO	,   GEM_Subst16_PMZoneHistory		,	0   } ,
	{ "GEM.Subst17_PMZoneHistory"		, _K_S_IO	,   GEM_Subst17_PMZoneHistory		,	0   } ,
	{ "GEM.Subst18_PMZoneHistory"		, _K_S_IO	,   GEM_Subst18_PMZoneHistory		,	0   } ,
	{ "GEM.Subst19_PMZoneHistory"		, _K_S_IO	,   GEM_Subst19_PMZoneHistory		,	0   } ,
	{ "GEM.Subst20_PMZoneHistory"		, _K_S_IO	,   GEM_Subst20_PMZoneHistory		,	0   } ,
	{ "GEM.Subst21_PMZoneHistory"		, _K_S_IO	,   GEM_Subst21_PMZoneHistory		,	0   } ,
	{ "GEM.Subst22_PMZoneHistory"		, _K_S_IO	,   GEM_Subst22_PMZoneHistory		,	0   } ,
	{ "GEM.Subst23_PMZoneHistory"		, _K_S_IO	,   GEM_Subst23_PMZoneHistory		,	0   } ,
	{ "GEM.Subst24_PMZoneHistory"		, _K_S_IO	,   GEM_Subst24_PMZoneHistory		,	0   } ,
	{ "GEM.Subst25_PMZoneHistory"		, _K_S_IO	,   GEM_Subst25_PMZoneHistory		,	0   } ,

	{ "GEM.Port1_Subst1_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst01_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst2_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst02_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst3_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst03_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst4_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst04_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst5_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst05_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst6_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst06_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst7_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst07_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst8_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst08_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst9_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst09_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst10_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst10_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst11_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst11_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst12_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst12_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst13_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst13_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst14_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst14_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst15_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst15_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst16_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst16_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst17_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst17_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst18_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst18_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst19_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst19_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst20_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst20_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst21_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst21_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst22_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst22_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst23_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst23_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst24_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst24_PMZoneHistory		,	0   } ,
	{ "GEM.Port1_Subst25_PMZoneID"	, _K_S_IO	,   GEM_Port1_Subst25_PMZoneHistory		,	0   } ,

	{ "GEM.Port2_Subst1_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst01_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst2_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst02_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst3_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst03_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst4_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst04_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst5_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst05_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst6_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst06_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst7_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst07_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst8_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst08_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst9_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst09_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst10_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst10_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst11_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst11_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst12_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst12_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst13_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst13_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst14_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst14_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst15_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst15_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst16_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst16_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst17_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst17_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst18_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst18_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst19_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst19_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst20_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst20_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst21_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst21_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst22_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst22_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst23_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst23_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst24_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst24_PMZoneHistory		,	0   } ,
	{ "GEM.Port2_Subst25_PMZoneID"	, _K_S_IO	,   GEM_Port2_Subst25_PMZoneHistory		,	0   } ,

	{ "GEM.Port3_Subst1_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst01_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst2_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst02_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst3_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst03_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst4_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst04_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst5_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst05_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst6_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst06_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst7_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst07_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst8_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst08_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst9_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst09_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst10_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst10_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst11_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst11_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst12_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst12_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst13_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst13_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst14_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst14_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst15_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst15_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst16_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst16_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst17_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst17_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst18_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst18_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst19_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst19_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst20_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst20_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst21_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst21_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst22_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst22_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst23_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst23_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst24_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst24_PMZoneHistory		,	0   } ,
	{ "GEM.Port3_Subst25_PMZoneID"	, _K_S_IO	,   GEM_Port3_Subst25_PMZoneHistory		,	0   } ,

	//2012.08.29 C1,C2 PM_SUBST_PROCESSED Seperated
	{ "CTC.PM1_Wafer_Status"		, _K_D_IO	,   C1_PM1_Wafer_Status					,	0   } ,
	{ "CTC.PM1_Wafer_Status2"		, _K_D_IO	,   C2_PM1_Wafer_Status					,	0   } ,
	{ "CTC.PM2_Wafer_Status"		, _K_D_IO	,   C1_PM2_Wafer_Status					,	0   } ,
	{ "CTC.PM2_Wafer_Status2"		, _K_D_IO	,   C2_PM2_Wafer_Status					,	0   } ,
	{ "CTC.PM3_Wafer_Status"		, _K_D_IO	,   C1_PM3_Wafer_Status					,	0   } ,
	{ "CTC.PM3_Wafer_Status2"		, _K_D_IO	,   C2_PM3_Wafer_Status					,	0   } ,
	// 2015.08.04
	{ "CTC.PM1_Wafer_Source"		, _K_D_IO	,   C1_PM1_Wafer_Source					,	0   } ,
	{ "CTC.PM1_Wafer_Source2"		, _K_D_IO	,   C2_PM1_Wafer_Source					,	0   } ,
	{ "CTC.PM2_Wafer_Source"		, _K_D_IO	,   C1_PM2_Wafer_Source					,	0   } ,
	{ "CTC.PM2_Wafer_Source2"		, _K_D_IO	,   C2_PM2_Wafer_Source					,	0   } ,
	{ "CTC.PM3_Wafer_Source"		, _K_D_IO	,   C1_PM3_Wafer_Source					,	0   } ,
	{ "CTC.PM3_Wafer_Source2"		, _K_D_IO	,   C2_PM3_Wafer_Source					,	0   } ,

	// 2013.07.23
	{ "CTC_EVENT_LOG_SVR"			, _K_F_IO	,		CTC_EVENT_LOG_SVR	,	0	} ,
	{ "CTC.LPM1_LotPMPath"			, _K_S_IO	,		CTC_LPM1_LOT_PMPATH	,	0	} ,
	{ "CTC.LPM2_LotPMPath"			, _K_S_IO	,		CTC_LPM2_LOT_PMPATH	,	0	} ,
	{ "CTC.LPM3_LotPMPath"			, _K_S_IO	,		CTC_LPM3_LOT_PMPATH	,	0	} ,
	// 2015.06.17
	{ "OBJ_KeySensorMon"			, _K_F_IO	,		OBJ_KeySensorMon	,	0	} ,
	""
};

//////////////////////////////////////////////////////////////////////
// Global Function
// 2013.07.23
// LOG_TYPE
enum { LTYPE_PRC, LTYPE_XFR, LTYPE_FNC, LTYPE_LEH, LTYPE_USER };

// 2013.07.23
extern void _MLOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);
	WRITE_FUNCTION_EVENT(CTC_EVENT_LOG_SVR, _szString);
}

// 2013.07.23
void LogEvent(char *szModule, char *szRunStr, char *szStatus, char *szData)
{
	char szEventID[20];
	int nLogType = LTYPE_LEH;

	do {
		if(STRCMP_L(szRunStr , "LOAD")) strcpy(szEventID, "CarrierLoad");
		else if(STRCMP_L(szRunStr , "UNLOAD")) strcpy(szEventID, "CarrierUnload");
		else if(STRCMP_L(szRunStr , "JOB_START")) strcpy(szEventID, "ProcessJobStart");
		else if(STRCMP_L(szRunStr , "JOB_END")) strcpy(szEventID, "ProcessJobEnd");
		else if(STRCMP_L(szRunStr , "MAP")) { strcpy(szEventID, "Map"); nLogType = LTYPE_FNC; }
		else break;

		switch(nLogType)
		{
		case LTYPE_LEH: _MLOG("LEH %s %s %s %s", szModule, szEventID, szStatus, szData); break;
		case LTYPE_FNC: _MLOG("FNC %s %s %s %s", szModule, szEventID, szStatus, szData); break;
		}

	} while(0);
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Port_Name_Check( char *Module , int *nPortID ) {
	*nPortID = -1;
	if		( STRNCMP_L( Module , "PORT", 4 ) ) *nPortID = atoi(Module+4)-1;
	else if ( STRNCMP_L( Module , "CM"	, 2 ) ) *nPortID = atoi(Module+2)-1;
	else return FALSE;
	if ( *nPortID < PORT1 || *nPortID > PORT4 || *nPortID >= MAX_PORT_NUMS ) return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Clear_PortData( int Port ) {
	int Index , CommStatus , i;
    int MAX_SLOT, MAX_PAGE;

    MAX_SLOT = 25;
    MAX_PAGE = 18;

	if      ( Port == 0 ) Index = CM1_C01_Wafer;
	else if ( Port == 1 ) Index = CM2_C01_Wafer;
	else if ( Port == 2 ) Index = CM3_C01_Wafer;
//	else return;
	for ( i = 0 ; i < MAX_SLOT ; i++ ) WRITE_DIGITAL( Index + i , 0 , &CommStatus ); // unknown
	if      ( Port == 0 ) Index = CM1_RFID_MID_PAGE;
	else if ( Port == 1 ) Index = CM2_RFID_MID_PAGE;
	else if ( Port == 2 ) Index = CM3_RFID_MID_PAGE;
	else return;
	for ( i = 0 ; i < MAX_PAGE ; i++ ) WRITE_STRING( Index + i , "" , &CommStatus );
	WRITE_STRING ( MID_READ		+ Port , "" , &CommStatus );
	WRITE_STRING ( PORT1_MID		+ Port , "" , &CommStatus );
	WRITE_STRING ( JID_READ		+ Port , "" , &CommStatus );
	WRITE_STRING ( JID_NAME		+ Port , "" , &CommStatus );
	WRITE_STRING ( CM1_RECIPE_FILE	+ Port , "" , &CommStatus );
	WRITE_DIGITAL( CM1_PROCESS_END	  + Port , 0 , &CommStatus );  //OFF
	WRITE_DIGITAL( CM1_MAPWAFER_COUNT + Port , 0   , &CommStatus );
	WRITE_STRING ( CM1_MAPWAFER_INFO  + Port , ""  , &CommStatus );

	// add Default Wafer & Loop Conunt setting by jelee 2006.06.01
	WRITE_DIGITAL ( START_SLOT  + Port , 1  , &CommStatus );
	WRITE_DIGITAL ( END_SLOT  + Port , 25  , &CommStatus );
	WRITE_DIGITAL ( LOOP_COUNT  + Port , 1  , &CommStatus );

	// 2004.07.26 by Kornic Inform, for FA
	WRITE_STRING ( GEM_Port1_LotID	  + Port , ""  , &CommStatus );
	WRITE_DIGITAL( OPERATOR_START + Port , OFF2, &CommStatus);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Event_Analysis( char *Event , char *Data ) { // example for 300mm event
// DCOP for each Process Job Start
	int nCommStatus , nPortID , i , nCount , nIndex , nData2, j=0, k=0;
	char szBuffer[255] , szBuffer2[255] , szBuffer3[255], szBuffer4[255], szBuffer5[255], szBuffer6[255], szPJID[255], szCJID[255];
	char szTimeInfo[255], szModuleName[255], szWaferNum[255],/* szWaferNum1[255], szWaferNum2[255],*/ szRecipeName[255], szCJName[255], szPJName[255], szPRJobSts[255], szCTRJobSts[255];
	char szPortNum[255], szPRJobCompleteSts[255], szCTRJobCompleteSts[255];
	char szModuleCompareName[255];
	int  nModuleIndex = 0;
	int  nStartIOEnum = 0, nEndIOEnum = 0;
	int  nFirstStringIOEnum = 0, nLastStringIOEnum = 0;
	int nWETCWfrCnt, nDRYCWfrCnt, nTRAPCWfrCnt;
	double DcopValue ;
	int nCM1_WfrPrcdCnt = 0, nCM2_WfrPrcdCnt = 0, nCM3_WfrPrcdCnt = 0, nCM_WfrPrcdCnt = 0;
	char szPortMID[255], szPort1MID[255], szPort2MID[255], szPort3MID[255];
	char szTitle[255], szMsg[255], szRejectCommand[255], szRejectID[255];
	char szPort1PPID[255], szPort2PPID[255], szPort3PPID[255];
	char szPM1PPID[255], szPM2PPID[255], szPM3PPID[255], szPM4PPID[255], szPM5PPID[255];
	int  nWaferNumStrSize = 0, nStringLen = 0;
	char pszFtnName[256] = "";

	//2012.08.29 C1,C2 PM_SUBST_PROCESSED Seperated
	int addr;
	int  nPM1WfrSts_C1 = 0, nPM1WfrSts_C2 = 0;
	char szSubst1[255], szSubst2[255], szSubst3[255], szSubst4[255], szSubst5[255], szSubst6[255];
	char szSubstPortID[255], szSubstTime[255], szSubstPM[255], szSubstWfrID[255];

	//2011.09.21
	char szPort1_WaferZoneID[255], szPort2_WaferZoneID[255], szPort3_WaferZoneID[255];
	//

	// 2013.07.23
	char szFlowInfo[255];
	char szCM_PMPathTemp[255];
	char szCM_PMPath[255];
	//2014.10.09
	char szPath1[32];
	char szPath2[32];
	char szPath3[32];
	char szPath4[32];
	char szPathTemp1[255];
	char szPathTemp2[255];
	char szPathTemp3[255];
	char szPathTemp4[255];

	int nLPMIdx = 0;
	int nPMPathLenth;
	//2014.10.09
	memset(szPath1			 , 0, sizeof(szPath1));
	memset(szPath2			 , 0, sizeof(szPath2));
	memset(szPath3			 , 0, sizeof(szPath3));
	memset(szPath4			 , 0, sizeof(szPath4));
	memset(szPathTemp1		 , 0, sizeof(szPathTemp1));
	memset(szPathTemp2		 , 0, sizeof(szPathTemp2));
	memset(szPathTemp3		 , 0, sizeof(szPathTemp3));
	memset(szPathTemp4		 , 0, sizeof(szPathTemp4));
	memset(szFlowInfo		 , 0, sizeof(szFlowInfo));
	memset(szCM_PMPathTemp	 , 0, sizeof(szCM_PMPathTemp));
	memset(szCM_PMPath		 , 0, sizeof(szCM_PMPath));

	memset(szBuffer			 , 0, sizeof(szBuffer));
	memset(szBuffer2		 , 0, sizeof(szBuffer2));
	memset(szBuffer3		 , 0, sizeof(szBuffer3));
	memset(szBuffer4		 , 0, sizeof(szBuffer4));
	memset(szBuffer5		 , 0, sizeof(szBuffer5));
	memset(szBuffer6		 , 0, sizeof(szBuffer6));
	memset(szPJID			 , 0, sizeof(szPJID));
	memset(szCJID			 , 0, sizeof(szCJID));
	memset(szTimeInfo		 , 0, sizeof(szTimeInfo));
	memset(szModuleName		 , 0, sizeof(szModuleName));
	memset(szWaferNum		 , 0, sizeof(szWaferNum));
//	memset(szWaferNum1		 , 0, sizeof(szWaferNum1));
//	memset(szWaferNum2		 , 0, sizeof(szWaferNum2));
	memset(szRecipeName		 , 0, sizeof(szRecipeName));
	memset(szPRJobSts		 , 0, sizeof(szPRJobSts));
	memset(szCTRJobSts		 , 0, sizeof(szCTRJobSts));
	memset(szCJName			 , 0, sizeof(szCJName));
	memset(szPJName			 , 0, sizeof(szPJName));
	memset(szPortNum		 , 0, sizeof(szPortNum));
	memset(szPRJobCompleteSts, 0, sizeof(szPRJobCompleteSts));
	memset(szCTRJobCompleteSts, 0, sizeof(szCTRJobCompleteSts));
	memset(szModuleCompareName, 0, sizeof(szModuleCompareName));
	memset(szPortMID		 , 0, sizeof(szPortMID));
	memset(szPort1MID		 , 0, sizeof(szPort1MID));
	memset(szPort2MID		 , 0, sizeof(szPort2MID));
	memset(szPort3MID		 , 0, sizeof(szPort3MID));
	memset(szTitle		     , 0, sizeof(szTitle));
	memset(szMsg			 , 0, sizeof(szMsg));
	memset(szPort1PPID		 , 0, sizeof(szPort1PPID));
	memset(szPort2PPID		 , 0, sizeof(szPort2PPID));
	memset(szPort3PPID		 , 0, sizeof(szPort3PPID));
	memset(szPM1PPID		 , 0, sizeof(szPM1PPID));
	memset(szPM2PPID		 , 0, sizeof(szPM2PPID));
	memset(szPM3PPID		 , 0, sizeof(szPM3PPID));
	memset(szPM4PPID		 , 0, sizeof(szPM4PPID));
	memset(szPM5PPID		 , 0, sizeof(szPM5PPID));

	memset(szPort1_WaferZoneID		 , 0, sizeof(szPort1_WaferZoneID));
	memset(szPort2_WaferZoneID		 , 0, sizeof(szPort2_WaferZoneID));
	memset(szPort3_WaferZoneID		 , 0, sizeof(szPort3_WaferZoneID));

	//2012.08.29 C1,C2 PM_SUBST_PROCESSED Seperated
	memset(szSubst1			, 0, sizeof(szSubst1));
	memset(szSubst2			, 0, sizeof(szSubst2));
	memset(szSubst3			, 0, sizeof(szSubst3));
	memset(szSubst4		    , 0, sizeof(szSubst4));
	memset(szSubst5		    , 0, sizeof(szSubst5));
	memset(szSubst6		    , 0, sizeof(szSubst6));
	memset(szSubstPortID    , 0, sizeof(szSubstPortID));
	memset(szSubstPM	    , 0, sizeof(szSubstPM));
	memset(szSubstWfrID	    , 0, sizeof(szSubstWfrID));
	memset(szSubstTime	    , 0, sizeof(szSubstTime));



	STR_SEPERATE_CHAR( Data , '|' , szBuffer , szBuffer2	, 255 );
	if		( STRCMP_L( Event , "MAPPING_COMPLETE_F"		) ) { // Buffer = Port Name
		if ( !Port_Name_Check( szBuffer , &nPortID ) ) {
			WRITE_DIGITAL( CM_MAPWAFER_COUNT , 0 , &nCommStatus );
			WRITE_STRING( CM_MAPWAFER_INFO , "" , &nCommStatus );
			for ( i = 0 ; i < MAX_SLOT_NUMS ; i++ ) {
				WRITE_DIGITAL( CM_C01_Wafer + i , 0 , &nCommStatus ); // Unknown
			}
		}
		else {
			if      ( nPortID == PORT1 ) nIndex = CM1_C01_Wafer;
			else if ( nPortID == PORT2 ) nIndex = CM2_C01_Wafer;
			else if ( nPortID == PORT3 ) nIndex = CM3_C01_Wafer;
			else return SYS_SUCCESS;
			nCount = READ_DIGITAL( CM1_MAPWAFER_COUNT + nPortID , &nCommStatus );
			READ_STRING( CM1_MAPWAFER_INFO + nPortID , szBuffer3 , &nCommStatus );
			WRITE_DIGITAL( CM_MAPWAFER_COUNT , nCount , &nCommStatus );
			WRITE_STRING( CM_MAPWAFER_INFO , szBuffer3 , &nCommStatus );
			for ( i = 0 ; i < MAX_SLOT_NUMS ; i++ ) {
				nData2 = READ_DIGITAL( nIndex + i , &nCommStatus );
				WRITE_DIGITAL( CM_C01_Wafer + i , nData2 , &nCommStatus );
			}
		}
	}
	else if	( STRCMP_L( Event , "MID_AUTO_READ_COMPLETE"		) ) { // szBuffer = Port Name
		_sleep (4500);
		if ( !Port_Name_Check( szBuffer , &nPortID ) ) {
			for ( i = 0 ; i < 18 ; i++ ) {
				WRITE_STRING( CM_RFID_MID_PAGE + i , "" , &nCommStatus );
			}
		}
		else {
			if      ( nPortID == PORT1 ) nIndex = CM1_RFID_MID_PAGE;
			else if ( nPortID == PORT2 ) nIndex = CM2_RFID_MID_PAGE;
			else if ( nPortID == PORT3 ) nIndex = CM3_RFID_MID_PAGE;
			else return SYS_SUCCESS;
			for ( i = 0 ; i < 18 ; i++ ) {
				READ_STRING( nIndex + i , szBuffer3 , &nCommStatus );
				WRITE_STRING( CM_RFID_MID_PAGE + i , szBuffer3 , &nCommStatus );
			}
		}
	}

	// --------------------------------------------------------------------------
	// Lot Information Clear when Manual Unload
	// --------------------------------------------------------------------------
	else if	( STRCMP_L( Event  , "CARR_DELETED_F" ) )
	{
		//2011.09.21
		RUN_FUNCTION( Wafer_ZoneID_Monitor, PROGRAM_PARAMETER_READ() );
 		//
        if( STRCMP_L( Data, "PORT1" ) )
		{
	     	i = 0;
		}
        else if( STRCMP_L( Data, "PORT2" ) )
		{
	     	i = 1;
		}
        else if( STRCMP_L( Data, "PORT3" ) )
		{
	     	i = 2;
		}
		else
		{
			//
		}
		if ( READ_DIGITAL( AGV_MODE_CM1 + i, &nCommStatus ) == Manual )
		{
			Clear_PortData( i );
		}
		else {
			if ( 0 == i ) {
				READ_STRING(JID_NAME, szCJName, &nCommStatus );
				READ_STRING(PORT1_MID, szPortMID, &nCommStatus );
				if ( (!STRCMP_L(szCJName, "")) || (!STRCMP_L(szPortMID, "")) )
				{
					Clear_PortData( i );
				}
			}
			else if ( 1 == i ) {
				READ_STRING(JID_NAME2, szCJName, &nCommStatus );
				READ_STRING(PORT2_MID, szPortMID, &nCommStatus );
				if ( (!STRCMP_L(szCJName, "")) || (!STRCMP_L(szPortMID, "")) )
				{
					Clear_PortData( i );
				}
			}
			else if ( 2 == i ) {
				READ_STRING(JID_NAME3, szCJName, &nCommStatus );
				READ_STRING(PORT3_MID, szPortMID, &nCommStatus );
				if ( (!STRCMP_L(szCJName, "")) || (!STRCMP_L(szPortMID, "")) )
				{
					Clear_PortData( i );
				}
			}
			else {
				//
			}
		}
	}
	else if	( STRCMP_L( Event , "RECIPE_EDIT" ) ) {
		RUN_FUNCTION( Recipe_Histroy_Manager, PROGRAM_PARAMETER_READ() );
	}
	// --------------------------------------------------------------------------
	// Post Run Start Fail Alarm when ProcessReicpe is not exist
	// --------------------------------------------------------------------------
	else if	( STRCMP_L(Event , "REJECT_F") )
	{
		STR_SEPERATE_CHAR( Data      , '|' , szPortNum		   ,  szBuffer2 	, 255 );
		STR_SEPERATE_CHAR( szBuffer2 , '|' , szRejectCommand   ,  szRejectID	, 255 );
		if( STRCMP_L(szRejectCommand, "START_F"))
		{
			if (STRCMP_L(szRejectID, "18"))
			{
				if(STRCMP_L(szPortNum, "PORT1"))
				{
					ALARM_POST(ALARM_START_POINT + ABORT_PM_Recipe_Not_Exist_Error);
				}
				else if(STRCMP_L(szPortNum, "PORT2"))
				{
					ALARM_POST(ALARM_START_POINT + 300 + ABORT_PM_Recipe_Not_Exist_Error);
				}
				else if(STRCMP_L(szPortNum, "PORT3"))
				{
					ALARM_POST(ALARM_START_POINT + 600 + ABORT_PM_Recipe_Not_Exist_Error);
				}
			}
			else if (STRCMP_L(szRejectID, "8"))
			{
				if(STRCMP_L(szPortNum, "PORT1"))
				{
					ALARM_POST(ALARM_START_POINT + REJECT_Select_Job_is_Run_Already);
				}
				else if(STRCMP_L(szPortNum, "PORT2"))
				{
					ALARM_POST(ALARM_START_POINT + 300 + REJECT_Select_Job_is_Run_Already);
				}
				else if(STRCMP_L(szPortNum, "PORT3"))
				{
					ALARM_POST(ALARM_START_POINT + 600 + REJECT_Select_Job_is_Run_Already);
				}
			}
			else if (STRCMP_L(szRejectID, "11"))
			{
				if(STRCMP_L(szPortNum, "PORT1"))
				{
					ALARM_POST(ALARM_START_POINT + ABORT_Lot_Recipe_Data_Reading_Error);
				}
				else if(STRCMP_L(szPortNum, "PORT2"))
				{
					ALARM_POST(ALARM_START_POINT + 300 + ABORT_Lot_Recipe_Data_Reading_Error);
				}
				else if(STRCMP_L(szPortNum, "PORT3"))
				{
					ALARM_POST(ALARM_START_POINT + 600 + ABORT_Lot_Recipe_Data_Reading_Error);
				}
			}
			else if (STRCMP_L(szRejectID, "12"))
			{
				if(STRCMP_L(szPortNum, "PORT1"))
				{
					ALARM_POST(ALARM_START_POINT + ABORT_Cluster_Recipe_Data_Reading_Error);
				}
				else if(STRCMP_L(szPortNum, "PORT2"))
				{
					ALARM_POST(ALARM_START_POINT + 300 + ABORT_Cluster_Recipe_Data_Reading_Error);
				}
				else if(STRCMP_L(szPortNum, "PORT3"))
				{
					ALARM_POST(ALARM_START_POINT + 600 + ABORT_Cluster_Recipe_Data_Reading_Error);
				}
			}
			else if (STRCMP_L(szRejectID, "6"))
			{
				if(STRCMP_L(szPortNum, "PORT1"))
				{
					ALARM_POST(ALARM_START_POINT + REJECT_Run_Recipe_File_Select_Error);
				}
				else if(STRCMP_L(szPortNum, "PORT2"))
				{
					ALARM_POST(ALARM_START_POINT + 300 + REJECT_Run_Recipe_File_Select_Error);
				}
				else if(STRCMP_L(szPortNum, "PORT3"))
				{
					ALARM_POST(ALARM_START_POINT + 600 + REJECT_Run_Recipe_File_Select_Error);
				}
			}
			else if (STRCMP_L(szRejectID, "9"))
			{
				if(STRCMP_L(szPortNum, "PORT1"))
				{
					ALARM_POST(ALARM_START_POINT + REJECT_Select_Job_Parameter_Error);
				}
				else if(STRCMP_L(szPortNum, "PORT2"))
				{
					ALARM_POST(ALARM_START_POINT + 300 + REJECT_Select_Job_Parameter_Error);
				}
				else if(STRCMP_L(szPortNum, "PORT3"))
				{
					ALARM_POST(ALARM_START_POINT + 600 + REJECT_Select_Job_Parameter_Error);
				}
			}
			else if (STRCMP_L(szRejectID, "14"))
			{
				if(STRCMP_L(szPortNum, "PORT1"))
				{
					ALARM_POST(ALARM_START_POINT + ABORT_Run_Chamber_is_Not_Enable);
				}
				else if(STRCMP_L(szPortNum, "PORT2"))
				{
					ALARM_POST(ALARM_START_POINT + 300 + ABORT_Run_Chamber_is_Not_Enable);
				}
				else if(STRCMP_L(szPortNum, "PORT3"))
				{
					ALARM_POST(ALARM_START_POINT + 600 + ABORT_Run_Chamber_is_Not_Enable);
				}
			}
			else if (STRCMP_L(szRejectID, "15"))
			{
				if(STRCMP_L(szPortNum, "PORT1"))
				{
					ALARM_POST(ALARM_START_POINT + ABORT_Run_Chamber_is_Not_Manual);
				}
				else if(STRCMP_L(szPortNum, "PORT2"))
				{
					ALARM_POST(ALARM_START_POINT + 300 + ABORT_Run_Chamber_is_Not_Manual);
				}
				else if(STRCMP_L(szPortNum, "PORT3"))
				{
					ALARM_POST(ALARM_START_POINT + 600 + ABORT_Run_Chamber_is_Not_Manual);
				}
			}
		}
	}
	// --------------------------------------------------------------------------
	// DCOP DATA ANALYSIS : ANALOG IO => STRING IO
	// --------------------------------------------------------------------------
	else if	( STRCMP_L( Event , "START_F" ) )
	{
		int i;
		//2014.10.09
		//int nRetry = 0;
		//BOOL bPass = FALSE;

		RUN_FUNCTION( LOT_RECIPE_MANAGER, PROGRAM_PARAMETER_READ() ); // add by jelee 2008.02.27 for Lot Recipe Interlock.
		//2011.09.21
		RUN_FUNCTION( Wafer_ZoneID_Monitor, PROGRAM_PARAMETER_READ() );
		//
		if( STRCMP_L( Data, "PORT1" ) )
		{
			nLPMIdx = 0;
			memset(&g_sLPMA_PMUse, 0, sizeof(g_sLPMA_PMUse));
			for( i = 0; i < 25; i++)
			{
				memset(&g_sLPMA_PRJobInfo[i], 0, sizeof(g_sLPMA_PRJobInfo[i]));
				memset(&g_sLPMA_CTRJobInfo[i], 0, sizeof(g_sLPMA_CTRJobInfo[i]));
			}
		}
		else if( STRCMP_L( Data, "PORT2") )
		{
			nLPMIdx = 1;
			memset(&g_sLPMB_PMUse, 0, sizeof(g_sLPMB_PMUse));
			for( i = 0; i < 25; i++)
			{
				memset(&g_sLPMB_PRJobInfo[i], 0, sizeof(g_sLPMB_PRJobInfo[i]));
				memset(&g_sLPMB_CTRJobInfo[i], 0, sizeof(g_sLPMB_CTRJobInfo[i]));
			}
		}
		else if( STRCMP_L( Data, "PORT3" ) )
		{
			nLPMIdx = 2;
			memset(&g_sLPMC_PMUse, 0, sizeof(g_sLPMC_PMUse));
			for( i = 0; i < 25; i++)
			{
				memset(&g_sLPMC_PRJobInfo[i], 0, sizeof(g_sLPMC_PRJobInfo[i]));
				memset(&g_sLPMC_CTRJobInfo[i], 0, sizeof(g_sLPMC_CTRJobInfo[i]));
			}
		}
/*
		// 2014.10.09
		if(nLPMIdx >= 0 && nLPMIdx <= 2 )
		{

			do{
				READ_STRING(CTC_LPM1_LOT_PMPATH+nLPMIdx, szCM_PMPathTemp, &nCommStatus);

				printf("szCM_PMPathTemp : %s\n", szCM_PMPathTemp);

				nPMPathLenth = strlen(szCM_PMPathTemp);

				if(nPMPathLenth < 1){
					WAIT_SECONDS(0.5);
					nRetry++;
					bPass = FALSE;
				}else{
					nRetry = 0;
					bPass = TRUE;
					break;
				}
				printf("nRetry : %d\n",nRetry);

			}while(nRetry<60);

			// 2014.03.16 Pass Length Error Case Added by hyuk
			if(bPass){
				strncpy(szCM_PMPath, szCM_PMPathTemp, nPMPathLenth-1);
				sprintf(szFlowInfo, "[2,'LL1|LL2','%s']", szCM_PMPath);
				printf("szCM_PMPath : %s szFlowInfo : %s\n", szCM_PMPath, szFlowInfo);
			}
			else{
				sprintf(szFlowInfo, "[2,'LL1|LL2','PM1|PM2|PM3']");
			}
		}

		LogEvent(Data, "JOB_START", szFlowInfo, "");
		*/
	}

/*	else if	( STRCMP_L( Event , "PROCESSJOB") )
	{
		STR_SEPERATE_CHAR( Data      , '|' , szPRJobSts			,  szBuffer2	, 255 );
		STR_SEPERATE_CHAR( szBuffer2 , '|' , szPJID				,  szBuffer3	, 255 );
		if( STRCMP_L( szPRJobSts, "START") )
		{
			i = 0;
			for( i = 0; i < 25; i++)
			{
				if(0 == strcmp(g_sLPMA_PRJobInfo[i].szPJID, szPJID) )
				{
					nModuleIndex = 2 ;
					for( nModuleIndex = 2; nModuleIndex <= 5; nModuleIndex++ )
					{
						g_sLPMA_PMUse.bUse = FALSE;
						g_sLPMA_PMUse.bPMUse[nModuleIndex] = FALSE;
					}
				}

				if(0 == strcmp(g_sLPMB_PRJobInfo[i].szPJID, szPJID) )
				{
					nModuleIndex = 2;
					for( nModuleIndex = 2; nModuleIndex <= 5; nModuleIndex++ )
					{
						g_sLPMB_PMUse.bUse = FALSE;
						g_sLPMB_PMUse.bPMUse[nModuleIndex] = FALSE;
					}
				}

				if(0 == strcmp(g_sLPMC_PRJobInfo[i].szPJID, szPJID) )
				{
					nModuleIndex = 2;
					for( nModuleIndex = 2; nModuleIndex <= 5; nModuleIndex++ )
					{
						g_sLPMC_PMUse.bUse = FALSE;
						g_sLPMC_PMUse.bPMUse[nModuleIndex] = FALSE;
					}
				}
			}
		}
	}
*/
	else if ( STRCMP_L( Event , "TIME_START" ) )
    {
	    //2014.10.09
		int nRetry = 0;
		int nPass = 0;

		RUN_FUNCTION( Control_Remaining_Time, PROGRAM_PARAMETER_READ() );

		if(strcmp(szBuffer, "PORT1") == 0) nLPMIdx = 0;
		else if(strcmp(szBuffer, "PORT2") == 0) nLPMIdx = 1;
		else if(strcmp(szBuffer, "PORT3") == 0) nLPMIdx = 2;
		else nLPMIdx = 99;


		if(nLPMIdx >= 0 && nLPMIdx <= 2 )
		{
			do{
				READ_STRING(CTC_LPM1_LOT_PMPATH+nLPMIdx, szCM_PMPathTemp, &nCommStatus);
				nPMPathLenth = strlen(szCM_PMPathTemp);

				if(nPMPathLenth < 1)
				{
					WAIT_SECONDS(0.5);
					nRetry++;
					nPass = 0;
				}
				else
				{
					nRetry = 0;
					nPass = 1;
					break;
				}
				printf("nRetry : %d\n",nRetry);
			}while(nRetry<30);

			//
			STR_SEPERATE_CHAR( szCM_PMPathTemp , '|' , szPath1 , szPathTemp1	, 255 );
			STR_SEPERATE_CHAR( szPathTemp1	   , '|' , szPath2 , szPathTemp2	, 255 );
			STR_SEPERATE_CHAR( szPathTemp2	   , '|' , szPath3 , szPathTemp3	, 255 );
			STR_SEPERATE_CHAR( szPathTemp3	   , '|' , szPath4 , szPathTemp4	, 255 );

			printf("Path :: %s -> %s %s %s %s \n", szCM_PMPathTemp, szPath1, szPath2, szPath3, szPath4);

			if(nPass == 1){
				strncpy(szCM_PMPath, szCM_PMPathTemp, nPMPathLenth-1);
				sprintf(szFlowInfo, "[4,'Aligner','LL1|LL2','%s','LL1|LL2']", szCM_PMPath);
			}else{
				sprintf(szFlowInfo, "[4,'Aligner','LL1|LL2','PM1|PM2|PM3','LL1|LL2']");
			}
		}

		//printf("szCM_PMPath : %s szFlowInfo : %s\n", szCM_PMPath, szFlowInfo);
		LogEvent(szBuffer, "JOB_START", szFlowInfo, "");
		// 2014.10.09
	}
	else if ( STRCMP_L( Event , "TIME_FIRSTRUN" ) )
    {
		RUN_FUNCTION( Control_Remaining_Time, PROGRAM_PARAMETER_READ() );
	}

	else if	( STRCMP_L( Event , "TIME_PROCESS_START" ) )
	{
		//2011.09.21
		RUN_FUNCTION( Wafer_ZoneID_Monitor, PROGRAM_PARAMETER_READ() );
		//

		STR_SEPERATE_CHAR( Data      , '|' , szPortNum    ,  szBuffer2	, 255 );
		STR_SEPERATE_CHAR( szBuffer2 , '|' , szTimeInfo   ,  szBuffer3	, 255 );
		STR_SEPERATE_CHAR( szBuffer3 , '|' , szModuleName ,  szBuffer4	, 255 );
		STR_SEPERATE_CHAR( szBuffer4 , '|' , szWaferNum   ,  szBuffer5	, 255 );
		STR_SEPERATE_CHAR( szBuffer5 , '|' , szRecipeName ,  szBuffer6	, 255 );
		STR_SEPERATE_CHAR( szBuffer6 , '|' , szCJName     ,  szPJName	, 255 );

		i = atoi(szWaferNum) -1 ;  // Edit Wafer Number Assgin 0 ~ 24 By chs 2006.12.26
		if( STRCMP_L( szPortNum, "PORT1" ) )
		{
			strcat(g_sLPMA_PRJobInfo[i].szModuleID, szModuleName);
			strcpy(g_sLPMA_PRJobInfo[i].szPJID,	szPJName);
			strcpy(g_sLPMA_CTRJobInfo[i].szCJID, szCJName);
			READ_STRING(PORT1_MID, szPort1MID, &nCommStatus);
			READ_STRING(CM1_RECIPE_FILE, szPort1PPID, &nCommStatus);
			if( STRCMP_L( szModuleName, "PM1"))
			{
			//	WRITE_STRING(PM1_MID, szPort1MID, &nCommStatus);
				WRITE_STRING(PM1_PPIDName, szPort1PPID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM2"))
			{
			//	WRITE_STRING(PM2_MID, szPort1MID, &nCommStatus);
				WRITE_STRING(PM2_PPIDName, szPort1PPID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM3"))
			{
			//	WRITE_STRING(PM3_MID, szPort1MID, &nCommStatus);
				WRITE_STRING(PM3_PPIDName, szPort1PPID, &nCommStatus);
			}
		}
		else if( STRCMP_L( szPortNum, "PORT2") )
		{
			strcat(g_sLPMB_PRJobInfo[i].szModuleID, szModuleName);
			strcpy(g_sLPMB_PRJobInfo[i].szPJID,	szPJName);
			strcpy(g_sLPMB_CTRJobInfo[i].szCJID,	szCJName);
			READ_STRING(PORT2_MID, szPort2MID, &nCommStatus);
			READ_STRING(CM2_RECIPE_FILE, szPort2PPID, &nCommStatus);
			if( STRCMP_L( szModuleName, "PM1"))
			{
			//	WRITE_STRING(PM1_MID, szPort2MID, &nCommStatus);
				WRITE_STRING(PM1_PPIDName, szPort2PPID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM2"))
			{
			//	WRITE_STRING(PM2_MID, szPort2MID, &nCommStatus);
				WRITE_STRING(PM2_PPIDName, szPort2PPID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM3"))
			{
			//	WRITE_STRING(PM3_MID, szPort2MID, &nCommStatus);
				WRITE_STRING(PM3_PPIDName, szPort2PPID, &nCommStatus);
			}
		}
		else if( STRCMP_L( szPortNum, "PORT3" ) )
		{
			strcat(g_sLPMC_PRJobInfo[i].szModuleID, szModuleName);
			strcpy(g_sLPMC_PRJobInfo[i].szPJID,	szPJName);
			strcpy(g_sLPMC_CTRJobInfo[i].szCJID,	szCJName);
			READ_STRING(PORT3_MID, szPort3MID, &nCommStatus);
			READ_STRING(CM3_RECIPE_FILE, szPort3PPID, &nCommStatus);
			if( STRCMP_L( szModuleName, "PM1"))
			{
			//	WRITE_STRING(PM1_MID, szPort3MID, &nCommStatus);
				WRITE_STRING(PM1_PPIDName, szPort3PPID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM2"))
			{
			//	WRITE_STRING(PM2_MID, szPort3MID, &nCommStatus);
				WRITE_STRING(PM2_PPIDName, szPort3PPID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM3"))
			{
			//	WRITE_STRING(PM3_MID, szPort3MID, &nCommStatus);
				WRITE_STRING(PM3_PPIDName, szPort3PPID, &nCommStatus);
			}
		}
	}
	else if	( STRCMP_L( Event , "TIME_PROCESS_END" ) )
	{
		//2011.09.21
		RUN_FUNCTION( Wafer_ZoneID_Monitor, PROGRAM_PARAMETER_READ() );
		//

		//2015.06.26
		//RUN_SET_FUNCTION( OBJ_KeySensorMon, PROGRAM_PARAMETER_READ() );   //2015.09.07
		//

		STR_SEPERATE_CHAR( Data      , '|' , szPortNum    ,  szBuffer2	, 255 );
		STR_SEPERATE_CHAR( szBuffer2 , '|' , szTimeInfo   ,  szBuffer3	, 255 );
		STR_SEPERATE_CHAR( szBuffer3 , '|' , szModuleName ,  szBuffer4	, 255 );
		STR_SEPERATE_CHAR( szBuffer4 , '|' , szWaferNum   ,  szBuffer5	, 255 );
		STR_SEPERATE_CHAR( szBuffer5 , '|' , szRecipeName ,  szBuffer6	, 255 );
		STR_SEPERATE_CHAR( szBuffer6 , '|' , szCJName     ,  szPJName	, 255 );

		if( STRCMP_L( szModuleName, "PM1"))
		{
		//	WRITE_STRING(PM1_MID, "", &nCommStatus);
			WRITE_STRING(PM1_PPIDName, "", &nCommStatus);
		}
		else if( STRCMP_L( szModuleName, "PM2"))
		{
		//	WRITE_STRING(PM2_MID, "", &nCommStatus);
			WRITE_STRING(PM2_PPIDName, "", &nCommStatus);
		}
		else if( STRCMP_L( szModuleName, "PM3"))
		{
		//	WRITE_STRING(PM3_MID, "", &nCommStatus);
			WRITE_STRING(PM3_PPIDName, "", &nCommStatus);
		}
	}

	else if	( STRCMP_L( Event , "PROCESSJOB") )	// PROCESSJOB FINISH|JOB2|0|4
	{

		STR_SEPERATE_CHAR( Data      , '|' , szPRJobSts			,  szBuffer2	, 255 );
		STR_SEPERATE_CHAR( szBuffer2 , '|' , szPJID				,  szBuffer3	, 255 );
		STR_SEPERATE_CHAR( szBuffer3 , '|' , szPRJobCompleteSts ,  szBuffer4	, 255 );
		if( STRCMP_L( szPRJobSts, "FINISH") )
		{
			//2015.06.26
			//RUN_FUNCTION( OBJ_KeySensorMon, PROGRAM_PARAMETER_READ() );  //2015.09.07
			//
            S_TIMER_READY();

			if( 0 == atoi(szPRJobCompleteSts) )
			{
				i = 0;
				nCM1_WfrPrcdCnt = 0;
				nCM2_WfrPrcdCnt = 0;
				nCM3_WfrPrcdCnt = 0;
				for( i = 0; i < 25; i++)
				{
					if(0 == strcmp(g_sLPMA_PRJobInfo[i].szPJID, szPJID) )
					{
						//2011.09.21
						READ_STRING(GEM_Port1_Subst01_PMZoneHistory+i, szPort1_WaferZoneID, &nCommStatus);
						//
						WRITE_STRING(GEM_Subst01_PMZoneHistory+i, szPort1_WaferZoneID, &nCommStatus);
						nModuleIndex = 1 ;
						for( nModuleIndex = 1; nModuleIndex <= 3; nModuleIndex++ )
						{
//							memset(pszFtnName, 0, sizeof(pszFtnName));
							sprintf(szModuleCompareName, "PM%d", nModuleIndex);
							if(NULL != strstr(g_sLPMA_PRJobInfo[i].szModuleID, szModuleCompareName))
							{
//								LOG(pszFtnName, ELT_File, "LPMA [PJID = %s], PM List : '%s', PM Name : '%s'", g_sLPMA_PRJobInfo[i].szPJID, g_sLPMA_PRJobInfo[i].szModuleID, szModuleCompareName);
								g_sLPMA_PMUse.bUse = TRUE;
								g_sLPMA_PMUse.bPMUse[nModuleIndex] = TRUE;
							}
						}
/*					    nCM1_WfrPrcdCnt = 0;
						for( j = CM1_C01_Wafer ; j <= CM1_C25_Wafer ; j++ )
						{
							if ( 3 == READ_DIGITAL( j , &nCommStatus) )
							{
								nCM1_WfrPrcdCnt = nCM1_WfrPrcdCnt + 1;
							}
						}
						nCM1_PJ_WfrPrcdCnt = nCM1_WfrPrcdCnt - nCM1_CJ_WfrPrcdCnt;
						nCM1_CJ_WfrPrcdCnt = nCM1_WfrPrcdCnt;
						WRITE_DIGITAL( GEM_PJ_WfrPrcCnt, nCM1_PJ_WfrPrcdCnt, &nCommStatus);
						WRITE_DIGITAL( GEM_CJ_WfrPrcCnt, nCM1_WfrPrcdCnt, &nCommStatus);
					    nCM1_WfrPrcdCnt = 0;
					    break;
*/
						j = CM1_C01_Wafer + i;
						if ( 3 == READ_DIGITAL( j , &nCommStatus) )
						{
							nCM1_WfrPrcdCnt = nCM1_WfrPrcdCnt + 1;
						}
						nCM1_PJ_WfrPrcdCnt = nCM1_WfrPrcdCnt;
//						WRITE_DIGITAL( GEM_PJ_WfrPrcCnt, nCM1_PJ_WfrPrcdCnt, &nCommStatus);
						goto next;
					}
					else
					{
						WRITE_STRING(GEM_Subst01_PMZoneHistory+i, "", &nCommStatus);
					}


					if(0 == strcmp(g_sLPMB_PRJobInfo[i].szPJID, szPJID) )
					{
						//2011.09.21
						READ_STRING(GEM_Port2_Subst01_PMZoneHistory+i, szPort2_WaferZoneID, &nCommStatus);
						WRITE_STRING(GEM_Subst01_PMZoneHistory+i, szPort2_WaferZoneID, &nCommStatus);
						//
						nModuleIndex = 1;
						for( nModuleIndex = 1; nModuleIndex <= 3; nModuleIndex++ )
						{
							sprintf(szModuleCompareName, "PM%d", nModuleIndex);
							if(NULL != strstr(g_sLPMB_PRJobInfo[i].szModuleID, szModuleCompareName))
							{
								g_sLPMB_PMUse.bUse = TRUE;
								g_sLPMB_PMUse.bPMUse[nModuleIndex] = TRUE;
							}
						}
/*						nCM2_WfrPrcdCnt = 0;
						for( j = CM2_C01_Wafer ; j <= CM2_C25_Wafer ; j++ )
						{
							if ( 3 == READ_DIGITAL( j , &nCommStatus) )
							{
								nCM2_WfrPrcdCnt = nCM2_WfrPrcdCnt + 1;
							}
						}
						nCM2_PJ_WfrPrcdCnt = nCM2_WfrPrcdCnt - nCM2_CJ_WfrPrcdCnt;
						nCM2_CJ_WfrPrcdCnt = nCM2_WfrPrcdCnt;
						WRITE_DIGITAL( GEM_PJ_WfrPrcCnt, nCM2_PJ_WfrPrcdCnt, &nCommStatus);
						WRITE_DIGITAL( GEM_CJ_WfrPrcCnt, nCM2_WfrPrcdCnt, &nCommStatus);
					    nCM2_WfrPrcdCnt = 0;
					    break;
*/
						j = CM2_C01_Wafer + i;
						if ( 3 == READ_DIGITAL( j , &nCommStatus) )
						{
							nCM2_WfrPrcdCnt = nCM2_WfrPrcdCnt + 1;
						}
						nCM2_PJ_WfrPrcdCnt = nCM2_WfrPrcdCnt;
//						WRITE_DIGITAL( GEM_PJ_WfrPrcCnt, nCM2_PJ_WfrPrcdCnt, &nCommStatus);
						goto next;
					}
					else
					{
						WRITE_STRING(GEM_Subst01_PMZoneHistory+i, "", &nCommStatus);
					}

					if(0 == strcmp(g_sLPMC_PRJobInfo[i].szPJID, szPJID) )
					{
						//2011.09.21
						READ_STRING(GEM_Port3_Subst01_PMZoneHistory+i, szPort3_WaferZoneID, &nCommStatus);
						WRITE_STRING(GEM_Subst01_PMZoneHistory+i, szPort3_WaferZoneID, &nCommStatus);
						//
						nModuleIndex = 1;
						for( nModuleIndex = 1; nModuleIndex <= 3; nModuleIndex++ )
						{
							sprintf(szModuleCompareName, "PM%d", nModuleIndex);
							if(NULL != strstr(g_sLPMC_PRJobInfo[i].szModuleID, szModuleCompareName))
							{
								g_sLPMC_PMUse.bUse = TRUE;
								g_sLPMC_PMUse.bPMUse[nModuleIndex] = TRUE;
							}
						}
/*						nCM3_WfrPrcdCnt = 0;
						for( j = CM3_C01_Wafer ; j <= CM3_C25_Wafer ; j++ )
						{
							if ( 3 == READ_DIGITAL( j , &nCommStatus) )
							{
								nCM3_WfrPrcdCnt = nCM3_WfrPrcdCnt + 1;
							}
						}
						nCM3_PJ_WfrPrcdCnt = nCM3_WfrPrcdCnt - nCM3_CJ_WfrPrcdCnt;
						nCM3_CJ_WfrPrcdCnt = nCM3_WfrPrcdCnt;
						WRITE_DIGITAL( GEM_PJ_WfrPrcCnt, nCM3_PJ_WfrPrcdCnt, &nCommStatus);
						WRITE_DIGITAL( GEM_CJ_WfrPrcCnt, nCM3_WfrPrcdCnt, &nCommStatus);
					    nCM3_WfrPrcdCnt = 0;
					    break;
*/
						j = CM3_C01_Wafer + i;
						if ( 3 == READ_DIGITAL( j , &nCommStatus) )
						{
							nCM3_WfrPrcdCnt = nCM3_WfrPrcdCnt + 1;
						}
						nCM3_PJ_WfrPrcdCnt = nCM3_WfrPrcdCnt;
//						WRITE_DIGITAL( GEM_PJ_WfrPrcCnt, nCM3_PJ_WfrPrcdCnt, &nCommStatus);
						goto next;
					}
					else
					{
						WRITE_STRING(GEM_Subst01_PMZoneHistory+i, "", &nCommStatus);
					}
					next:;

				}

				for( i = 0; i < 25; i++)
				{
					if(0 == strcmp(g_sLPMA_PRJobInfo[i].szPJID, szPJID) )
					{
						WRITE_DIGITAL( GEM_PJ_WfrPrcCnt, nCM1_PJ_WfrPrcdCnt, &nCommStatus);
						break;
					}
					if(0 == strcmp(g_sLPMB_PRJobInfo[i].szPJID, szPJID) )
					{
						WRITE_DIGITAL( GEM_PJ_WfrPrcCnt, nCM2_PJ_WfrPrcdCnt, &nCommStatus);
						break;
					}
					if(0 == strcmp(g_sLPMC_PRJobInfo[i].szPJID, szPJID) )
					{
						WRITE_DIGITAL( GEM_PJ_WfrPrcCnt, nCM3_PJ_WfrPrcdCnt, &nCommStatus);
						break;
					}
				}

				if(TRUE == g_sLPMA_PMUse.bUse)
				{

					for(nModuleIndex = 0; nModuleIndex < 3; nModuleIndex++)
					{
					    switch(nModuleIndex)
					    {
					        case 0 :
					            nStartIOEnum = PM1_PRO_CurStepAM_LAvr;
					            nEndIOEnum = PM1_StgHtr2VoltAI_LAvr2;
					            nFirstStringIOEnum = PM1V_PRO_CurStepAM_LAvr;
					            nLastStringIOEnum = PM1V_StgHtr2VoltAI_LAvr2;
					            break;
					        case 1 :
					            nStartIOEnum = PM2_PRO_CurStepAM_LAvr;
					            nEndIOEnum = PM2_StgHtr2VoltAI_LAvr2;
					            nFirstStringIOEnum = PM2V_PRO_CurStepAM_LAvr;
					            nLastStringIOEnum = PM2V_StgHtr2VoltAI_LAvr2;
					            break;
					        case 2 :
					            nStartIOEnum = PM3_PRO_CurStepAM_LAvr;
					            nEndIOEnum = PM3_StgHtr2VoltAI_LAvr2;
					            nFirstStringIOEnum = PM3V_PRO_CurStepAM_LAvr;
					            nLastStringIOEnum = PM3V_StgHtr2VoltAI_LAvr2;
					            break;
					        default:
					            break;
					    }

					    nWETCWfrCnt = READ_DIGITAL (PRMD_PM1_WETCWfrCnt + (nModuleIndex*3), &nCommStatus);
					    nDRYCWfrCnt = READ_DIGITAL (PRMD_PM1_DRYCWfrCnt + (nModuleIndex*3), &nCommStatus);
					    nTRAPCWfrCnt = READ_DIGITAL (PRMD_PM1_TRAPCWfrCnt + (nModuleIndex*3), &nCommStatus);

						j = 0;
					    for (i = nStartIOEnum ; i <= nEndIOEnum ; i++, j++)
					    {
					        DcopValue = READ_ANALOG (i ,&nCommStatus);
					        if (DcopValue == -99)
					        {
					            WRITE_STRING(nFirstStringIOEnum + j, "", &nCommStatus);
					        }
					        else
					        {
					            sprintf(szBuffer, "%0.1f", DcopValue);
					            WRITE_STRING(nFirstStringIOEnum + j, szBuffer, &nCommStatus);
					        }
					    }
					    WRITE_DIGITAL (GEM_PM1_WETCWfrCnt + (nModuleIndex*3), nWETCWfrCnt, &nCommStatus);
					    WRITE_DIGITAL (GEM_PM1_DRYCWfrCnt + (nModuleIndex*3), nDRYCWfrCnt, &nCommStatus);
					    WRITE_DIGITAL (GEM_PM1_TRAPCWfrCnt + (nModuleIndex*3), nTRAPCWfrCnt, &nCommStatus);
					}
				}
				if(TRUE == g_sLPMB_PMUse.bUse)
				{

					for(nModuleIndex = 0; nModuleIndex < 3; nModuleIndex++)
					{
						switch(nModuleIndex)
						{
							case 0 :
								nStartIOEnum = PM1_PRO_CurStepAM_LAvr;
								nEndIOEnum = PM1_StgHtr2VoltAI_LAvr2;
								nFirstStringIOEnum = PM1V_PRO_CurStepAM_LAvr;
								nLastStringIOEnum = PM1V_StgHtr2VoltAI_LAvr2;
								break;
							case 1 :
								nStartIOEnum = PM2_PRO_CurStepAM_LAvr;
								nEndIOEnum = PM2_StgHtr2VoltAI_LAvr2;
								nFirstStringIOEnum = PM2V_PRO_CurStepAM_LAvr;
								nLastStringIOEnum = PM2V_StgHtr2VoltAI_LAvr2;
								break;
							case 2 :
								nStartIOEnum = PM3_PRO_CurStepAM_LAvr;
								nEndIOEnum = PM3_StgHtr2VoltAI_LAvr2;
								nFirstStringIOEnum = PM3V_PRO_CurStepAM_LAvr;
								nLastStringIOEnum = PM3V_StgHtr2VoltAI_LAvr2;
								break;
							default:
								break;
						}

						nWETCWfrCnt = READ_DIGITAL (PRMD_PM1_WETCWfrCnt + (nModuleIndex*3), &nCommStatus);
						nDRYCWfrCnt = READ_DIGITAL (PRMD_PM1_DRYCWfrCnt + (nModuleIndex*3), &nCommStatus);
						nTRAPCWfrCnt = READ_DIGITAL (PRMD_PM1_TRAPCWfrCnt + (nModuleIndex*3), &nCommStatus);

						j = 0;
						for (i = nStartIOEnum ; i <= nEndIOEnum ; i++, j++)
						{
							DcopValue = READ_ANALOG (i ,&nCommStatus);
							if (DcopValue == -99)
							{
								WRITE_STRING(nFirstStringIOEnum + j, "", &nCommStatus);
							}
							else
							{
								sprintf(szBuffer, "%0.1f", DcopValue);
								WRITE_STRING(nFirstStringIOEnum + j, szBuffer, &nCommStatus);
							}
						}
						WRITE_DIGITAL (GEM_PM1_WETCWfrCnt + (nModuleIndex*3), nWETCWfrCnt, &nCommStatus);
						WRITE_DIGITAL (GEM_PM1_DRYCWfrCnt + (nModuleIndex*3), nDRYCWfrCnt, &nCommStatus);
						WRITE_DIGITAL (GEM_PM1_TRAPCWfrCnt + (nModuleIndex*3), nTRAPCWfrCnt, &nCommStatus);
					}
				}
				if(TRUE == g_sLPMC_PMUse.bUse)
				{

					for(nModuleIndex = 0; nModuleIndex < 3; nModuleIndex++)
					{
						switch(nModuleIndex)
						{
							case 0 :
								nStartIOEnum = PM1_PRO_CurStepAM_LAvr;
								nEndIOEnum = PM1_StgHtr2VoltAI_LAvr2;
								nFirstStringIOEnum = PM1V_PRO_CurStepAM_LAvr;
								nLastStringIOEnum = PM1V_StgHtr2VoltAI_LAvr2;
								break;
							case 1 :
								nStartIOEnum = PM2_PRO_CurStepAM_LAvr;
								nEndIOEnum = PM2_StgHtr2VoltAI_LAvr2;
								nFirstStringIOEnum = PM2V_PRO_CurStepAM_LAvr;
								nLastStringIOEnum = PM2V_StgHtr2VoltAI_LAvr2;
								break;
							case 2 :
								nStartIOEnum = PM3_PRO_CurStepAM_LAvr;
								nEndIOEnum = PM3_StgHtr2VoltAI_LAvr2;
								nFirstStringIOEnum = PM3V_PRO_CurStepAM_LAvr;
								nLastStringIOEnum = PM3V_StgHtr2VoltAI_LAvr2;
								break;
							default:
								break;
						}

						nWETCWfrCnt = READ_DIGITAL (PRMD_PM1_WETCWfrCnt + (nModuleIndex*3), &nCommStatus);
						nDRYCWfrCnt = READ_DIGITAL (PRMD_PM1_DRYCWfrCnt + (nModuleIndex*3), &nCommStatus);
						nTRAPCWfrCnt = READ_DIGITAL (PRMD_PM1_TRAPCWfrCnt + (nModuleIndex*3), &nCommStatus);

						j = 0;
						for (i = nStartIOEnum ; i <= nEndIOEnum ; i++, j++)
						{
							DcopValue = READ_ANALOG (i ,&nCommStatus);
							if (DcopValue == -99)
							{
								WRITE_STRING(nFirstStringIOEnum + j, "", &nCommStatus);
							}
							else
							{
								sprintf(szBuffer, "%0.1f", DcopValue);
								WRITE_STRING(nFirstStringIOEnum + j, szBuffer, &nCommStatus);
							}
						}
						WRITE_DIGITAL (GEM_PM1_WETCWfrCnt + (nModuleIndex*3), nWETCWfrCnt, &nCommStatus);
						WRITE_DIGITAL (GEM_PM1_DRYCWfrCnt + (nModuleIndex*3), nDRYCWfrCnt, &nCommStatus);
						WRITE_DIGITAL (GEM_PM1_TRAPCWfrCnt + (nModuleIndex*3), nTRAPCWfrCnt, &nCommStatus);
					}
				}
				// Reset
				memset(&g_sLPMA_PMUse, 0, sizeof(g_sLPMA_PMUse));
				memset(&g_sLPMB_PMUse, 0, sizeof(g_sLPMB_PMUse));
				memset(&g_sLPMC_PMUse, 0, sizeof(g_sLPMC_PMUse));
			}
		}
		else if ( STRCMP_L( szPRJobSts, "INSERT" ))
		{
		  RUN_FUNCTION( JobHistory_Manager, PROGRAM_PARAMETER_READ() );
		}
    }

	else if	( STRCMP_L( Event , "CONTROLJOB") )
	{
		STR_SEPERATE_CHAR( Data      , '|' , szCTRJobSts		,  szBuffer2	, 255 );
		STR_SEPERATE_CHAR( szBuffer2 , '|' , szCJID				,  szBuffer3	, 255 );
		STR_SEPERATE_CHAR( szBuffer3 , '|' , szCTRJobCompleteSts ,  szBuffer4	, 255 );
		if( STRCMP_L( szCTRJobSts, "FINISH") )
		{
            S_TIMER_READY();
			if( 0 == atoi(szCTRJobCompleteSts) )
			{
				i = 0;
				for( i = 0; i < 25; i++)
				{
					if(0 == strcmp(g_sLPMA_CTRJobInfo[i].szCJID, szCJID) )
					{
						nCM1_WfrPrcdCnt = 0;
						for( j = CM1_C01_Wafer ; j <= CM1_C25_Wafer ; j++ )
						{
							if ( 3 == READ_DIGITAL( j , &nCommStatus) )
							{
								nCM1_WfrPrcdCnt = nCM1_WfrPrcdCnt + 1;
							}
						}
						nCM1_CJ_WfrPrcdCnt = nCM1_WfrPrcdCnt;
						WRITE_DIGITAL( GEM_CJ_WfrPrcCnt, nCM1_WfrPrcdCnt, &nCommStatus);
						nCM1_WfrPrcdCnt = 0;
						break;
					}

					if(0 == strcmp(g_sLPMB_PRJobInfo[i].szPJID, szPJID) )
					{
						nCM2_WfrPrcdCnt = 0;
						for( j = CM2_C01_Wafer ; j <= CM2_C25_Wafer ; j++ )
						{
							if ( 3 == READ_DIGITAL( j , &nCommStatus) )
							{
								nCM2_WfrPrcdCnt = nCM2_WfrPrcdCnt + 1;
							}
						}
						nCM2_CJ_WfrPrcdCnt = nCM2_WfrPrcdCnt;
						WRITE_DIGITAL( GEM_CJ_WfrPrcCnt, nCM2_WfrPrcdCnt, &nCommStatus);
						nCM2_WfrPrcdCnt = 0;
						break;
					}

					if(0 == strcmp(g_sLPMC_PRJobInfo[i].szPJID, szPJID) )
					{
						nCM3_WfrPrcdCnt = 0;
						for( j = CM3_C01_Wafer ; j <= CM3_C25_Wafer ; j++ )
						{
							if ( 3 == READ_DIGITAL( j , &nCommStatus) )
							{
								nCM3_WfrPrcdCnt = nCM3_WfrPrcdCnt + 1;
							}
						}
						nCM3_CJ_WfrPrcdCnt = nCM3_WfrPrcdCnt;
						WRITE_DIGITAL( GEM_CJ_WfrPrcCnt, nCM3_WfrPrcdCnt, &nCommStatus);
						break;
					}
				}
			}
        }
		else if ( STRCMP_L( szCTRJobSts, "INSERT" ))
		{
			RUN_FUNCTION( JobHistory_Manager, PROGRAM_PARAMETER_READ() );
		}
    }
	else if	( STRCMP_L( Event , "TIME_COMPLETE"		) ) { // Buffer = Port Name
//		RUN_FUNCTION( LOT_RECIPE_AUTOSAVE_MANAGER, PROGRAM_PARAMETER_READ() ); // add by jelee 2008.03.20 for Lot Recipe AutoSave.
		RUN_FUNCTION( Control_Remaining_Time, PROGRAM_PARAMETER_READ() );
		if(FALSE == Port_Name_Check( szBuffer, &nPortID) )
		{
			printf("---> *** Invalid Port ID [%s|%s]", Event, szBuffer);
			return SYS_SUCCESS;
		}

		//LogEvent(szBuffer, "JOB_END", "", "");		// 2014.10.09

		if( PORT1 == nPortID )
		{
			nCM1_PJ_WfrPrcdCnt = 0;
			nCM1_CJ_WfrPrcdCnt = 0;
			for( i = CM1_C01_Wafer ; i <= CM1_C25_Wafer ; i++ )
			{
				if ( 3 == READ_DIGITAL( i , &nCommStatus) )
				{
					nCM1_WfrPrcdCnt = nCM1_WfrPrcdCnt + 1;
				}
				strcpy(g_sLPMA_PRJobInfo[i - CM1_C01_Wafer ].szPJID, "");
				strcpy(g_sLPMA_CTRJobInfo[i - CM1_C01_Wafer ].szCJID, "");
			    strcpy(g_sLPMA_PRJobInfo[i - CM1_C01_Wafer ].szModuleID, "");
			    g_sLPMA_PRJobInfo[i - CM1_C01_Wafer ].nProcessState = 0;
                g_sLPMA_PRJobInfo[i - CM1_C01_Wafer ].nWaferID = 0;
			}
			WRITE_DIGITAL( GEM_CM_WfrPrcCnt, nCM1_WfrPrcdCnt, &nCommStatus);
			WRITE_DIGITAL( GEM_CM1_WfrPrcCnt, nCM1_WfrPrcdCnt, &nCommStatus);
		}
		else if( PORT2 == nPortID )
		{
			nCM2_PJ_WfrPrcdCnt = 0;
			nCM2_CJ_WfrPrcdCnt = 0;
			for( i = CM2_C01_Wafer ; i <= CM2_C25_Wafer ; i++ )
			{
				if ( 3 == READ_DIGITAL( i , &nCommStatus) )
				{
					nCM2_WfrPrcdCnt = nCM2_WfrPrcdCnt + 1;
				}
				strcpy(g_sLPMB_PRJobInfo[i - CM2_C01_Wafer ].szPJID, "");
				strcpy(g_sLPMB_CTRJobInfo[i - CM2_C01_Wafer ].szCJID, "");
			    strcpy(g_sLPMB_PRJobInfo[i - CM2_C01_Wafer ].szModuleID, "");
		        g_sLPMB_PRJobInfo[i - CM2_C01_Wafer ].nProcessState = 0;
                g_sLPMB_PRJobInfo[i - CM2_C01_Wafer ].nWaferID = 0;
			}
			WRITE_DIGITAL( GEM_CM_WfrPrcCnt, nCM2_WfrPrcdCnt, &nCommStatus);
			WRITE_DIGITAL( GEM_CM2_WfrPrcCnt, nCM2_WfrPrcdCnt, &nCommStatus);
		}
		else if( PORT3 == nPortID )
		{
			nCM3_PJ_WfrPrcdCnt = 0;
			nCM3_CJ_WfrPrcdCnt = 0;
			for( i = CM3_C01_Wafer ; i <= CM3_C25_Wafer ; i++ )
			{
				if ( 3 == READ_DIGITAL( i , &nCommStatus) )
				{
					nCM3_WfrPrcdCnt = nCM3_WfrPrcdCnt + 1;
				}
				strcpy(g_sLPMC_PRJobInfo[i - CM3_C01_Wafer ].szPJID, "");
				strcpy(g_sLPMC_CTRJobInfo[i - CM3_C01_Wafer ].szCJID, "");
		   	    strcpy(g_sLPMC_PRJobInfo[i - CM3_C01_Wafer ].szModuleID, "");
		        g_sLPMC_PRJobInfo[i - CM3_C01_Wafer ].nProcessState = 0;
                g_sLPMC_PRJobInfo[i - CM3_C01_Wafer ].nWaferID = 0;
			}
			WRITE_DIGITAL( GEM_CM_WfrPrcCnt, nCM3_WfrPrcdCnt, &nCommStatus);
			WRITE_DIGITAL( GEM_CM3_WfrPrcCnt, nCM3_WfrPrcdCnt, &nCommStatus);
		}
		else
		{
			printf("---> *** Invalid Port ID [%s|%s, %d]", Event, szBuffer, nPortID);
			return SYS_SUCCESS;
		}
    }
	else if ( STRCMP_L( Event , "ABORTED_F" ) )
    {
		RUN_FUNCTION( Control_Remaining_Time, PROGRAM_PARAMETER_READ() );
	}
	else if ( STRCMP_L( Event , "END_F" ) )
    {
		RUN_FUNCTION( Control_Remaining_Time, PROGRAM_PARAMETER_READ() );
	}

/*	else if	( STRCMP_L( Event , "ABORTED_F"	) ) { // Buffer = Port Name
		RUN_FUNCTION( LOT_RECIPE_AUTOSAVE_MANAGER, PROGRAM_PARAMETER_READ() ); // add by jelee 2008.03.20 for Lot Recipe AutoSave.
	}
*/
	else if	( STRCMP_L( Event , "COMPLETE_F"	) ) { // Buffer = Port Name
		RUN_FUNCTION( WaferPrcCnt_Manager, PROGRAM_PARAMETER_READ() ); // add by jelee 2008.07.28 for Lot Recipe AutoSave.
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	//2012.08.29 C1,C2 PM_SUBST_IN_PROCESS and PM_SUBST_PROCESSED Event Seperated. Start
    //////////////////////////////////////////////////////////////////////////////////////////////

	STR_SEPERATE( Data , szSubst1 , szSubst2 , 255 );

	STR_SEPERATE_CHAR( szSubst2  , '|' , szSubstPortID,  szSubst3	, 255 );
	STR_SEPERATE_CHAR( szSubst3  , '|' , szSubstTime  ,  szSubst4	, 255 );
	STR_SEPERATE_CHAR( szSubst4  , '|' , szSubstPM	  ,  szSubst5	, 255 );
	STR_SEPERATE_CHAR( szSubst5  , '|' , szSubstWfrID ,  szSubst6	, 255 );

	if(STRCMP_L(Event, "$IN_GEM_EVENT$")&&STRCMP_L(szSubst1, "PM1_SUBST_IN_PROCESS")||STRCMP_L(szSubst1, "PM2_SUBST_IN_PROCESS")||STRCMP_L(szSubst1, "PM3_SUBST_IN_PROCESS"))
	{
		if (STRCMP_L(szSubst1, "PM1_SUBST_IN_PROCESS"))
		{
			memset(szSubst1, 0, 255);
			if (READ_DIGITAL(C1_PM1_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
				&& READ_DIGITAL(C1_PM1_Wafer_Source, &nCommStatus) == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C1_PM1_SUBST_IN_PROCESS");   //2014.07.14
				g_sPM1EventInfo.nC1_ProCheck = eCheck;
				g_sPM1EventInfo.nC1_WaferID = atoi(szSubstWfrID);
				g_sPM1EventInfo.nC1_PortInfo = (atoi(&szSubstPortID[4]) - 1);
			}
			else if (READ_DIGITAL(C2_PM1_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
					 && READ_DIGITAL(C2_PM1_Wafer_Source, &nCommStatus) == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C2_PM1_SUBST_IN_PROCESS");   //2014.07.14
				g_sPM1EventInfo.nC2_ProCheck = eCheck;
				g_sPM1EventInfo.nC2_WaferID = atoi(szSubstWfrID);
				g_sPM1EventInfo.nC2_PortInfo = (atoi(&szSubstPortID[4]) - 1);
			}

		}
		else if(STRCMP_L(szSubst1, "PM2_SUBST_IN_PROCESS"))
		{
			memset(szSubst1, 0, 255);
			if (READ_DIGITAL(C1_PM2_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
				&& READ_DIGITAL(C1_PM2_Wafer_Source, &nCommStatus) == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C1_PM2_SUBST_IN_PROCESS");    //2014.07.14
				g_sPM2EventInfo.nC1_ProCheck = eCheck;
				g_sPM2EventInfo.nC1_WaferID = atoi(szSubstWfrID);
				g_sPM2EventInfo.nC1_PortInfo = (atoi(&szSubstPortID[4]) - 1);
			}
			else if (READ_DIGITAL(C2_PM2_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
				     && READ_DIGITAL(C2_PM2_Wafer_Source, &nCommStatus) == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C2_PM2_SUBST_IN_PROCESS");    //2014.07.14
				g_sPM2EventInfo.nC2_ProCheck = eCheck;
				g_sPM2EventInfo.nC2_WaferID = atoi(szSubstWfrID);
				g_sPM2EventInfo.nC2_PortInfo = (atoi(&szSubstPortID[4]) - 1);
			}
		}
		else if(STRCMP_L(szSubst1, "PM3_SUBST_IN_PROCESS"))
		{
			memset(szSubst1, 0, 255);
			if (READ_DIGITAL(C1_PM3_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
				&& READ_DIGITAL(C1_PM3_Wafer_Source, &nCommStatus) == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C1_PM3_SUBST_IN_PROCESS");    //2014.07.14
				g_sPM3EventInfo.nC1_ProCheck = eCheck;
				g_sPM3EventInfo.nC1_WaferID = atoi(szSubstWfrID);
				g_sPM3EventInfo.nC1_PortInfo = (atoi(&szSubstPortID[4]) - 1);
			}
			else if (READ_DIGITAL(C2_PM3_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
				     && READ_DIGITAL(C1_PM3_Wafer_Source, &nCommStatus) == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C2_PM3_SUBST_IN_PROCESS");    //2014.07.14
				g_sPM3EventInfo.nC2_ProCheck = eCheck;
				g_sPM3EventInfo.nC2_WaferID = atoi(szSubstWfrID);
				g_sPM3EventInfo.nC2_PortInfo = (atoi(&szSubstPortID[4]) - 1);
			}
		}

		szSubst1[strlen(szSubst1)]=0x00;

		addr = _FIND_FROM_STRING(_K_D_IO,"GEM.CHANGE_EVENT");

        _dWRITE_DIGITAL(addr,GEM_Enable,&nCommStatus);	// GEM Change Event Enable

        _dWRITE_UPLOAD_MESSAGE(szSubst1);
	}
	else if(STRCMP_L(Event, "$IN_GEM_EVENT$")&&STRCMP_L(szSubst1, "PM1_SUBST_PROCESSED")||STRCMP_L(szSubst1, "PM2_SUBST_PROCESSED")||STRCMP_L(szSubst1, "PM3_SUBST_PROCESSED"))
	{
		if (STRCMP_L(szSubst1, "PM1_SUBST_PROCESSED"))
		{
			memset(szSubst1, 0, 255);
			if (READ_DIGITAL(C1_PM1_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
				&& READ_DIGITAL(C1_PM1_Wafer_Source, &nCommStatus) == g_sPM1EventInfo.nC1_PortInfo)
			{
				strcpy(szSubst1,"C1_PM1_SUBST_PROCESSED");       //2014.07.14
				g_sPM1EventInfo.nC1_ProCheck = eNotCheck;
				g_sPM1EventInfo.nC1_WaferID = 0;
				g_sPM1EventInfo.nC1_PortInfo = -1;
			}
			else if (READ_DIGITAL(C2_PM1_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
				&& READ_DIGITAL(C2_PM1_Wafer_Source, &nCommStatus) == g_sPM1EventInfo.nC2_PortInfo)
			{
				strcpy(szSubst1,"C2_PM1_SUBST_PROCESSED");       //2014.07.14
				g_sPM1EventInfo.nC2_ProCheck = eNotCheck;
				g_sPM1EventInfo.nC2_WaferID = 0;
				g_sPM1EventInfo.nC2_PortInfo = -1;
			}
			else if (g_sPM1EventInfo.nC1_ProCheck == eCheck
				&& g_sPM1EventInfo.nC1_WaferID == atoi(szSubstWfrID)
				&& g_sPM1EventInfo.nC1_PortInfo == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C1_PM1_SUBST_PROCESSED");       //2014.07.14
				g_sPM1EventInfo.nC1_ProCheck = eNotCheck;
				g_sPM1EventInfo.nC1_WaferID = 0;
				g_sPM1EventInfo.nC1_PortInfo = -1;
				printf("[EventInterface] == PM1 C1 delay Event\n");
			}
			else if (g_sPM1EventInfo.nC2_ProCheck == eCheck
				&& g_sPM1EventInfo.nC2_WaferID == atoi(szSubstWfrID)
				&& g_sPM1EventInfo.nC2_PortInfo == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C2_PM1_SUBST_PROCESSED");       //2014.07.14
				g_sPM1EventInfo.nC2_ProCheck = eNotCheck;
				g_sPM1EventInfo.nC2_WaferID = 0;
				g_sPM1EventInfo.nC2_PortInfo = -1;
				printf("[EventInterface] == PM1 C2 delay Event\n");
			}
		}
		else if(STRCMP_L(szSubst1, "PM2_SUBST_PROCESSED"))
		{
			memset(szSubst1, 0, 255);
			if (READ_DIGITAL(C1_PM2_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
				&& READ_DIGITAL(C1_PM2_Wafer_Source, &nCommStatus) == g_sPM2EventInfo.nC1_PortInfo)
			{
				strcpy(szSubst1,"C1_PM2_SUBST_PROCESSED");       //2014.07.14
				g_sPM2EventInfo.nC1_ProCheck = eNotCheck;
				g_sPM2EventInfo.nC1_WaferID = 0;
				g_sPM2EventInfo.nC1_PortInfo = -1;
			}
			else if (READ_DIGITAL(C2_PM2_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
				     && READ_DIGITAL(C2_PM2_Wafer_Source, &nCommStatus) == g_sPM2EventInfo.nC2_PortInfo)
			{
				strcpy(szSubst1,"C2_PM2_SUBST_PROCESSED");       //2014.07.14
				g_sPM2EventInfo.nC2_ProCheck = eNotCheck;
				g_sPM2EventInfo.nC2_WaferID = 0;
				g_sPM2EventInfo.nC2_PortInfo = -1;
			}
			else if (g_sPM2EventInfo.nC1_ProCheck == eCheck
					&& g_sPM2EventInfo.nC1_WaferID == atoi(szSubstWfrID)
					&& g_sPM2EventInfo.nC1_PortInfo == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C1_PM2_SUBST_PROCESSED");       //2014.07.14
				g_sPM2EventInfo.nC1_ProCheck = eNotCheck;
				g_sPM2EventInfo.nC1_WaferID = 0;
				g_sPM2EventInfo.nC1_PortInfo = -1;
				printf("[EventInterface] == PM2 C1 Delay Event\n");
			}
			else if (g_sPM2EventInfo.nC2_ProCheck == eCheck
					&& g_sPM2EventInfo.nC2_WaferID == atoi(szSubstWfrID)
					&& g_sPM2EventInfo.nC2_PortInfo == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C2_PM2_SUBST_PROCESSED");       //2014.07.14
				g_sPM2EventInfo.nC2_ProCheck = eNotCheck;
				g_sPM2EventInfo.nC2_WaferID = 0;
				g_sPM2EventInfo.nC2_PortInfo = -1;
				printf("[EventInterface] == PM2 C2 Delay Event\n");
			}
		}
		else if(STRCMP_L(szSubst1, "PM3_SUBST_PROCESSED"))
		{
			memset(szSubst1, 0, 255);
			if (READ_DIGITAL(C1_PM3_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
				&& READ_DIGITAL(C1_PM3_Wafer_Source, &nCommStatus) == g_sPM3EventInfo.nC1_PortInfo)
			{
				strcpy(szSubst1,"C1_PM3_SUBST_PROCESSED");      //2014.07.14
				g_sPM3EventInfo.nC1_ProCheck = eNotCheck;
				g_sPM3EventInfo.nC1_WaferID = 0;
				g_sPM3EventInfo.nC1_PortInfo = -1;
			}
			else if (READ_DIGITAL(C2_PM3_Wafer_Status, &nCommStatus) == atoi(szSubstWfrID)
					&& READ_DIGITAL(C2_PM3_Wafer_Source, &nCommStatus) == g_sPM3EventInfo.nC2_PortInfo)
			{
				strcpy(szSubst1,"C2_PM3_SUBST_PROCESSED");		//2014.07.14
				g_sPM3EventInfo.nC2_ProCheck = eNotCheck;
				g_sPM3EventInfo.nC2_WaferID = 0;
				g_sPM3EventInfo.nC2_PortInfo = -1;
			}
			else if (g_sPM3EventInfo.nC1_ProCheck == eCheck
				&& g_sPM3EventInfo.nC1_WaferID == atoi(szSubstWfrID)
				&& g_sPM3EventInfo.nC1_PortInfo == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C1_PM3_SUBST_PROCESSED");       //2014.07.14
				g_sPM3EventInfo.nC1_ProCheck = eNotCheck;
				g_sPM3EventInfo.nC1_WaferID = 0;
				g_sPM3EventInfo.nC1_PortInfo = -1;
				printf("[EventInterface] == PM3 C1 Delay Event\n");
			}
			else if (g_sPM3EventInfo.nC2_ProCheck == eCheck
					&& g_sPM3EventInfo.nC2_WaferID == atoi(szSubstWfrID)
					&& g_sPM3EventInfo.nC2_PortInfo == (atoi(&szSubstPortID[4]) - 1))
			{
				strcpy(szSubst1,"C2_PM3_SUBST_PROCESSED");       //2014.07.14
				g_sPM3EventInfo.nC2_ProCheck = eNotCheck;
				g_sPM3EventInfo.nC2_WaferID = 0;
				g_sPM3EventInfo.nC2_PortInfo = -1;
				printf("[EventInterface] == PM3 C2 Delay Event\n");
			}
		}
		//...End 2015.07.20

		szSubst1[strlen(szSubst1)]=0x00;

		addr = _FIND_FROM_STRING(_K_D_IO,"GEM.CHANGE_EVENT");

        _dWRITE_DIGITAL(addr,GEM_Enable,&nCommStatus);	// GEM Change Event Enable

        _dWRITE_UPLOAD_MESSAGE(szSubst1);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	//2012.08.29 C1,C2 PM_SUBST_IN_PROCESS and PM_SUBST_PROCESSED Event Seperated. End
    //////////////////////////////////////////////////////////////////////////////////////////////
	else if ( STRCMP_L( Event , "STS_LASTOUT_F" ) ){
		LogEvent(szBuffer, "JOB_END", "", "");		//2014.07.18 For MARS
	}
// DCOP for each Process Job End.

// DCOP for each Lot Start.
/*
	int nCommStatus , nPortID , i , nCount , nIndex , nData2, j=0;
	int LP_PM2_UseIO, LP_PM3_UseIO, LP_PM4_UseIO, LP_PM5_UseIO;
	int PM2_WETCWfrCnt, PM2_DRYCWfrCnt, PM2_TRAPCWfrCnt, PM3_WETCWfrCnt, PM3_DRYCWfrCnt, PM3_TRAPCWfrCnt;
    int PM4_WETCWfrCnt, PM4_DRYCWfrCnt, PM4_TRAPCWfrCnt, PM5_WETCWfrCnt, PM5_DRYCWfrCnt, PM5_TRAPCWfrCnt;
	char szBuffer[255] , szBuffer2[255] , szBuffer3[255], szBuffer4[255], szBuffer5[255], szBuffer6[255], szPJID[255];
	char szTimeInfo[255], szModuleName[255], szWaferNum[255], szRecipeName[255], szCJName[255], szPJName[255], szPRJobSts[255];
	char szPortNum[255], szPRJobCompleteSts[255];
	char szModuleCompareName[255];
	int  nModuleIndex = 0;
	int  nStartIOEnum = 0, nEndIOEnum = 0;
	int  nFirstStringIOEnum = 0, nLastStringIOEnum = 0;
	double DcopValue ;
	char szPort1MID[255], szPort2MID[255], szPort3MID[255];
	memset(szBuffer			 , 0, sizeof(szBuffer));
	memset(szBuffer2		 , 0, sizeof(szBuffer2));
	memset(szBuffer3		 , 0, sizeof(szBuffer3));
	memset(szBuffer4		 , 0, sizeof(szBuffer4));
	memset(szBuffer5		 , 0, sizeof(szBuffer5));
	memset(szBuffer6		 , 0, sizeof(szBuffer6));
	memset(szPJID			 , 0, sizeof(szPJID));
	memset(szTimeInfo		 , 0, sizeof(szTimeInfo));
	memset(szModuleName		 , 0, sizeof(szModuleName));
	memset(szWaferNum		 , 0, sizeof(szWaferNum));
	memset(szRecipeName		 , 0, sizeof(szRecipeName));
	memset(szPRJobSts		 , 0, sizeof(szPRJobSts));
	memset(szPJName			 , 0, sizeof(szPJName));
	memset(szPortNum		 , 0, sizeof(szPortNum));
	memset(szPRJobCompleteSts, 0, sizeof(szPRJobCompleteSts));
	memset(szModuleCompareName, 0, sizeof(szModuleCompareName));
	memset(szPort1MID		 , 0, sizeof(szPort1MID));
	memset(szPort2MID		 , 0, sizeof(szPort2MID));
	memset(szPort3MID		 , 0, sizeof(szPort3MID));
	STR_SEPERATE_CHAR( Data , '|' , szBuffer , szBuffer2	, 255 );
	if		( STRCMP_L( Event , "MAPPING_COMPLETE_F"		) ) { // Buffer = Port Name
		if ( !Port_Name_Check( szBuffer , &nPortID ) ) {
			WRITE_DIGITAL( CM_MAPWAFER_COUNT , 0 , &nCommStatus );
			WRITE_STRING( CM_MAPWAFER_INFO , "" , &nCommStatus );
			for ( i = 0 ; i < MAX_SLOT_NUMS ; i++ ) {
				WRITE_DIGITAL( CM_C01_Wafer + i , 0 , &nCommStatus ); // Unknown
			}
		}
		else {
			if      ( nPortID == PORT1 ) nIndex = CM1_C01_Wafer;
			else if ( nPortID == PORT2 ) nIndex = CM2_C01_Wafer;
			else if ( nPortID == PORT3 ) nIndex = CM3_C01_Wafer;
			else return SYS_SUCCESS;
			nCount = READ_DIGITAL( CM1_MAPWAFER_COUNT + nPortID , &nCommStatus );
			READ_STRING( CM1_MAPWAFER_INFO + nPortID , szBuffer3 , &nCommStatus );
			WRITE_DIGITAL( CM_MAPWAFER_COUNT , nCount , &nCommStatus );
			WRITE_STRING( CM_MAPWAFER_INFO , szBuffer3 , &nCommStatus );
			for ( i = 0 ; i < MAX_SLOT_NUMS ; i++ ) {
				nData2 = READ_DIGITAL( nIndex + i , &nCommStatus );
				WRITE_DIGITAL( CM_C01_Wafer + i , nData2 , &nCommStatus );
			}
		}
	}
	else if	( STRCMP_L( Event , "MID_AUTO_READ_COMPLETE"		) ) { // Buffer = Port Name
		_sleep (4500);
		if ( !Port_Name_Check( szBuffer , &nPortID ) ) {
			for ( i = 0 ; i < 18 ; i++ ) {
				WRITE_STRING( CM_RFID_MID_PAGE + i , "" , &nCommStatus );
			}
		}
		else {
			if      ( nPortID == PORT1 ) nIndex = CM1_RFID_MID_PAGE;
			else if ( nPortID == PORT2 ) nIndex = CM2_RFID_MID_PAGE;
			else if ( nPortID == PORT3 ) nIndex = CM3_RFID_MID_PAGE;
			else return SYS_SUCCESS;
			for ( i = 0 ; i < 18 ; i++ ) {
				READ_STRING( nIndex + i , szBuffer3 , &nCommStatus );
				WRITE_STRING( CM_RFID_MID_PAGE + i , szBuffer3 , &nCommStatus );
			}
		}
	}
	// --------------------------------------------------------------------------
	// DCOP DATA ANALYSIS : ANALOG IO => STRING IO
	// --------------------------------------------------------------------------
	else if	( STRCMP_L( Event , "TIME_PROCESS_START" ) )
	{

		STR_SEPERATE_CHAR( Data      , '|' , szPortNum    ,  szBuffer2	, 255 );
		STR_SEPERATE_CHAR( szBuffer2 , '|' , szTimeInfo   ,  szBuffer3	, 255 );
		STR_SEPERATE_CHAR( szBuffer3 , '|' , szModuleName ,  szBuffer4	, 255 );
		STR_SEPERATE_CHAR( szBuffer4 , '|' , szWaferNum   ,  szBuffer5	, 255 );
		STR_SEPERATE_CHAR( szBuffer5 , '|' , szRecipeName ,  szBuffer6	, 255 );
		STR_SEPERATE_CHAR( szBuffer6 , '|' , szCJName     ,  szPJName	, 255 );

		if( STRCMP_L( szPortNum, "PORT1" ) )
		{
			READ_STRING(PORT1_MID, szPort1MID, &nCommStatus);
			if( STRCMP_L( szModuleName, "PM1"))
			{
				WRITE_STRING(PM1_MID, szPort1MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM2"))
			{
				WRITE_STRING(PM2_MID, szPort1MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM3"))
			{
				WRITE_STRING(PM3_MID, szPort1MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM4"))
			{
				WRITE_STRING(PM4_MID, szPort1MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM5"))
			{
				WRITE_STRING(PM5_MID, szPort1MID, &nCommStatus);
			}
		}
		else if( STRCMP_L( szPortNum, "PORT2") )
		{
			READ_STRING(PORT2_MID, szPort2MID, &nCommStatus);
			if( STRCMP_L( szModuleName, "PM1"))
			{
				WRITE_STRING(PM1_MID, szPort2MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM2"))
			{
				WRITE_STRING(PM2_MID, szPort2MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM3"))
			{
				WRITE_STRING(PM3_MID, szPort2MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM4"))
			{
				WRITE_STRING(PM4_MID, szPort2MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM5"))
			{
				WRITE_STRING(PM5_MID, szPort2MID, &nCommStatus);
			}
		}
		else if( STRCMP_L( szPortNum, "PORT3" ) )
		{
			READ_STRING(PORT3_MID, szPort3MID, &nCommStatus);
			if( STRCMP_L( szModuleName, "PM1"))
			{
				WRITE_STRING(PM1_MID, szPort3MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM2"))
			{
				WRITE_STRING(PM2_MID, szPort3MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM3"))
			{
				WRITE_STRING(PM3_MID, szPort3MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM4"))
			{
				WRITE_STRING(PM4_MID, szPort3MID, &nCommStatus);
			}
			else if( STRCMP_L( szModuleName, "PM5"))
			{
				WRITE_STRING(PM5_MID, szPort3MID, &nCommStatus);
			}
		}
	}

	else if	( STRCMP_L( Event , "TIME_PROCESS_END" ) )
	{

		STR_SEPERATE_CHAR( Data      , '|' , szPortNum    ,  szBuffer2	, 255 );
		STR_SEPERATE_CHAR( szBuffer2 , '|' , szTimeInfo   ,  szBuffer3	, 255 );
		STR_SEPERATE_CHAR( szBuffer3 , '|' , szModuleName ,  szBuffer4	, 255 );
		STR_SEPERATE_CHAR( szBuffer4 , '|' , szWaferNum   ,  szBuffer5	, 255 );
		STR_SEPERATE_CHAR( szBuffer5 , '|' , szRecipeName ,  szBuffer6	, 255 );
		STR_SEPERATE_CHAR( szBuffer6 , '|' , szCJName     ,  szPJName	, 255 );

		if( STRCMP_L( szModuleName, "PM1"))
		{
			WRITE_STRING(PM1_MID, "", &nCommStatus);
		}
		else if( STRCMP_L( szModuleName, "PM2"))
		{
			WRITE_STRING(PM2_MID, "", &nCommStatus);
		}
		else if( STRCMP_L( szModuleName, "PM3"))
		{
			WRITE_STRING(PM3_MID, "", &nCommStatus);
		}
		else if( STRCMP_L( szModuleName, "PM4"))
		{
			WRITE_STRING(PM4_MID, "", &nCommStatus);
		}
		else if( STRCMP_L( szModuleName, "PM5"))
		{
			WRITE_STRING(PM5_MID, "", &nCommStatus);
		}
	}

	else if	( STRCMP_L( Event , "TIME_COMPLETE"		) ) { // Buffer = Port Name

		if(FALSE == Port_Name_Check( szBuffer, &nPortID) )
		{
			printf("---> *** Invalid Port ID [%s|%s]", Event, szBuffer);
			return SYS_SUCCESS;
		}

		if( PORT1 == nPortID )
		{
			LP_PM2_UseIO = LPMA_PM2_Use;
			LP_PM3_UseIO = LPMA_PM3_Use;
			LP_PM4_UseIO = LPMA_PM4_Use;
			LP_PM5_UseIO = LPMA_PM5_Use;
		}
		else if( PORT2 == nPortID )
		{
			LP_PM2_UseIO = LPMB_PM2_Use;
			LP_PM3_UseIO = LPMB_PM3_Use;
			LP_PM4_UseIO = LPMB_PM4_Use;
			LP_PM5_UseIO = LPMB_PM5_Use;
		}
		else if( PORT3 == nPortID )
		{
			LP_PM2_UseIO = LPMC_PM2_Use;
			LP_PM3_UseIO = LPMC_PM3_Use;
			LP_PM4_UseIO = LPMC_PM4_Use;
			LP_PM5_UseIO = LPMC_PM5_Use;
		}
		else
		{
			printf("---> *** Invalid Port ID [%s|%s, %d]", Event, szBuffer, nPortID);
			return SYS_SUCCESS;
		}

		j=0;
		// DCOP DATA for PM2
		if ( READ_DIGITAL ( PM2_CommStatus , &nCommStatus) < 3 ||
			NO == READ_DIGITAL ( LP_PM2_UseIO, &nCommStatus) ) {
			for ( i=PM2V_PRO_CurStepAM_LAvr ; i<= PM2V_SHT_OutPwAI_LAvr2  ; i++ )
				WRITE_STRING ( i, "", &nCommStatus ) ;
			WRITE_DIGITAL (GEM_PM2_WETCWfrCnt, 0, &nCommStatus);
			WRITE_DIGITAL (GEM_PM2_DRYCWfrCnt, 0, &nCommStatus);
			WRITE_DIGITAL (GEM_PM2_TRAPCWfrCnt, 0, &nCommStatus);
		}
		else {
			PM2_WETCWfrCnt = READ_DIGITAL (PRMD_PM2_WETCWfrCnt, &nCommStatus);
			PM2_DRYCWfrCnt = READ_DIGITAL (PRMD_PM2_DRYCWfrCnt, &nCommStatus);
			PM2_TRAPCWfrCnt = READ_DIGITAL (PRMD_PM2_TRAPCWfrCnt, &nCommStatus);
			for ( i = PM2_PRO_CurStepAM_LAvr ; i <= PM2_SHT_OutPwAI_LAvr2 ; i++, j++ ) {
				DcopValue = READ_ANALOG ( i ,&nCommStatus);
				if (DcopValue == -99 ) WRITE_STRING ( PM2V_PRO_CurStepAM_LAvr+j, "", &nCommStatus );
				else {
					sprintf(szBuffer, "%0.1f", DcopValue);
					WRITE_STRING ( PM2V_PRO_CurStepAM_LAvr+j, szBuffer, &nCommStatus );
				}

			}
			WRITE_DIGITAL (GEM_PM2_WETCWfrCnt, PM2_WETCWfrCnt, &nCommStatus);
			WRITE_DIGITAL (GEM_PM2_DRYCWfrCnt, PM2_DRYCWfrCnt, &nCommStatus);
			WRITE_DIGITAL (GEM_PM2_TRAPCWfrCnt, PM2_TRAPCWfrCnt, &nCommStatus);
		}

		j=0;
		// DCOP DATA for PM3
		if ( READ_DIGITAL ( PM3_CommStatus , &nCommStatus) < 3 ||
			NO == READ_DIGITAL ( LP_PM3_UseIO, &nCommStatus) ) {
			for ( i=PM3V_PRO_CurStepAM_LAvr ; i<= PM3V_SHT_OutPwAI_LAvr2  ; i++ )
				WRITE_STRING ( i, "", &nCommStatus ) ;
			WRITE_DIGITAL (GEM_PM3_WETCWfrCnt, 0, &nCommStatus);
			WRITE_DIGITAL (GEM_PM3_DRYCWfrCnt, 0, &nCommStatus);
			WRITE_DIGITAL (GEM_PM3_TRAPCWfrCnt, 0, &nCommStatus);
		}
		else {
			PM3_WETCWfrCnt = READ_DIGITAL (PRMD_PM3_WETCWfrCnt, &nCommStatus);
			PM3_DRYCWfrCnt = READ_DIGITAL (PRMD_PM3_DRYCWfrCnt, &nCommStatus);
			PM3_TRAPCWfrCnt = READ_DIGITAL (PRMD_PM3_TRAPCWfrCnt, &nCommStatus);
			for ( i = PM3_PRO_CurStepAM_LAvr ; i <= PM3_SHT_OutPwAI_LAvr2 ; i++, j++ ) {
				DcopValue = READ_ANALOG ( i ,&nCommStatus);
				if (DcopValue == -99 ) WRITE_STRING ( PM3V_PRO_CurStepAM_LAvr+j, "", &nCommStatus );
				else {
					sprintf(szBuffer, "%0.1f", DcopValue);
					WRITE_STRING ( PM3V_PRO_CurStepAM_LAvr+j, szBuffer, &nCommStatus );
				}

			}
			WRITE_DIGITAL (GEM_PM3_WETCWfrCnt, PM3_WETCWfrCnt, &nCommStatus);
			WRITE_DIGITAL (GEM_PM3_DRYCWfrCnt, PM3_DRYCWfrCnt, &nCommStatus);
			WRITE_DIGITAL (GEM_PM3_TRAPCWfrCnt, PM3_TRAPCWfrCnt, &nCommStatus);
		}

		j=0;
		// DCOP DATA for PM4
		if ( READ_DIGITAL ( PM4_CommStatus , &nCommStatus) < 3 ||
			NO == READ_DIGITAL ( LP_PM4_UseIO, &nCommStatus) ) {
			for ( i=PM4V_PRO_CurStepAM_LAvr ; i<= PM4V_SHT_OutPwAI_LAvr2  ; i++ )
				WRITE_STRING ( i, "", &nCommStatus ) ;
			WRITE_DIGITAL (GEM_PM4_WETCWfrCnt, 0, &nCommStatus);
			WRITE_DIGITAL (GEM_PM4_DRYCWfrCnt, 0, &nCommStatus);
			WRITE_DIGITAL (GEM_PM4_TRAPCWfrCnt, 0, &nCommStatus);
		}
		else {
			PM4_WETCWfrCnt = READ_DIGITAL (PRMD_PM4_WETCWfrCnt, &nCommStatus);
			PM4_DRYCWfrCnt = READ_DIGITAL (PRMD_PM4_DRYCWfrCnt, &nCommStatus);
			PM4_TRAPCWfrCnt = READ_DIGITAL (PRMD_PM4_TRAPCWfrCnt, &nCommStatus);
			for ( i = PM4_PRO_CurStepAM_LAvr ; i <= PM4_SHT_OutPwAI_LAvr2 ; i++, j++ ) {
				DcopValue = READ_ANALOG ( i ,&nCommStatus);
				if (DcopValue == -99 ) WRITE_STRING ( PM4V_PRO_CurStepAM_LAvr+j, "", &nCommStatus );
				else {
					sprintf(szBuffer, "%0.1f", DcopValue);
					WRITE_STRING ( PM4V_PRO_CurStepAM_LAvr+j, szBuffer, &nCommStatus );
				}
			}
			WRITE_DIGITAL (GEM_PM4_WETCWfrCnt, PM4_WETCWfrCnt, &nCommStatus);
			WRITE_DIGITAL (GEM_PM4_DRYCWfrCnt, PM4_DRYCWfrCnt, &nCommStatus);
			WRITE_DIGITAL (GEM_PM4_TRAPCWfrCnt, PM4_TRAPCWfrCnt, &nCommStatus);
		}

		j=0;
		// DCOP DATA for PM5
		if ( READ_DIGITAL ( PM5_CommStatus , &nCommStatus) < 3 ||
			NO == READ_DIGITAL ( LP_PM5_UseIO, &nCommStatus) ) {
			for ( i=PM5V_PRO_CurStepAM_LAvr ; i<= PM5V_SHT_OutPwAI_LAvr2  ; i++ )
				WRITE_STRING ( i, "", &nCommStatus ) ;
			WRITE_DIGITAL (GEM_PM5_WETCWfrCnt, 0, &nCommStatus);
			WRITE_DIGITAL (GEM_PM5_DRYCWfrCnt, 0, &nCommStatus);
			WRITE_DIGITAL (GEM_PM5_TRAPCWfrCnt, 0, &nCommStatus);
		}
		else {
			PM5_WETCWfrCnt = READ_DIGITAL (PRMD_PM5_WETCWfrCnt, &nCommStatus);
			PM5_DRYCWfrCnt = READ_DIGITAL (PRMD_PM5_DRYCWfrCnt, &nCommStatus);
			PM5_TRAPCWfrCnt = READ_DIGITAL (PRMD_PM5_TRAPCWfrCnt, &nCommStatus);
			for ( i = PM5_PRO_CurStepAM_LAvr ; i <= PM5_SHT_OutPwAI_LAvr2 ; i++, j++ ) {
				DcopValue = READ_ANALOG ( i ,&nCommStatus);
				if (DcopValue == -99 ) WRITE_STRING ( PM5V_PRO_CurStepAM_LAvr+j, "", &nCommStatus );
				else {
					sprintf(szBuffer, "%0.1f", DcopValue);
					WRITE_STRING ( PM5V_PRO_CurStepAM_LAvr+j, szBuffer, &nCommStatus );
				}

			}
			WRITE_DIGITAL (GEM_PM5_WETCWfrCnt, PM5_WETCWfrCnt, &nCommStatus);
			WRITE_DIGITAL (GEM_PM5_DRYCWfrCnt, PM5_DRYCWfrCnt, &nCommStatus);
			WRITE_DIGITAL (GEM_PM5_TRAPCWfrCnt, PM5_TRAPCWfrCnt, &nCommStatus);
		}

	}
*/
// DCOP for each Lot Start.
	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int	Dl;
	unsigned char *Ed;
	char szBuffer[5000] , szBuffer2[5000];

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	if ( Dl < 1 ) return SYS_SUCCESS;

	STR_SEPERATE( Ed , szBuffer , szBuffer2	, 255 );

	Event_Analysis( szBuffer , szBuffer2 );

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Program_Enter_Code() {
/*	int i;
	for( i= 0; i<26; i++)
	{
		memset(&g_sLPMA_PRJobInfo[i], 0, sizeof(g_sLPMA_PRJobInfo[i]));
		memset(&g_sLPMB_PRJobInfo[i], 0, sizeof(g_sLPMB_PRJobInfo[i]));
		memset(&g_sLPMC_PRJobInfo[i], 0, sizeof(g_sLPMC_PRJobInfo[i]));
	}
	memset(&g_sLPMA_PMUse, 0, sizeof(g_sLPMA_PMUse));
	memset(&g_sLPMB_PMUse, 0, sizeof(g_sLPMB_PMUse));
	memset(&g_sLPMC_PMUse, 0, sizeof(g_sLPMC_PMUse));
*/
	g_sPM1EventInfo.nC1_ProCheck = eNotCheck;
	g_sPM1EventInfo.nC2_ProCheck = eNotCheck;
	g_sPM2EventInfo.nC1_ProCheck = eNotCheck;
	g_sPM2EventInfo.nC2_ProCheck = eNotCheck;
	g_sPM3EventInfo.nC1_ProCheck = eNotCheck;
	g_sPM3EventInfo.nC2_ProCheck = eNotCheck;

	g_sPM1EventInfo.nC1_WaferID = 0;
	g_sPM1EventInfo.nC2_WaferID = 0;
	g_sPM2EventInfo.nC1_WaferID = 0;
	g_sPM2EventInfo.nC2_WaferID = 0;
	g_sPM3EventInfo.nC1_WaferID = 0;
	g_sPM3EventInfo.nC2_WaferID = 0;

	g_sPM1EventInfo.nC1_PortInfo = -1;
	g_sPM1EventInfo.nC2_PortInfo = -1;
	g_sPM2EventInfo.nC1_PortInfo = -1;
	g_sPM2EventInfo.nC2_PortInfo = -1;
    g_sPM3EventInfo.nC1_PortInfo = -1;
	g_sPM3EventInfo.nC2_PortInfo = -1;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
