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
//------------------------------------------------------------------------------------------
enum	{ OFF , ON };
enum	{ ABSENT , PRESENT };
enum	{ OFFLINE , ONLINE , LOCAL , REMOTE  };
enum	{ NOTRESERVE , RESERVATION  };
enum	{ MANUALACCESS , AUTOACCESS };
enum	{ INSERVICE	, OUTOFSERVICE };
enum	{ CANCELCARRIER , CANCELCARRIERATPORT };
enum	{ PORT1 , PORT2 , PORT3 , PORT4 };
enum	{ S_EQ_OFFLINE , S_ATTEMPT_ONLINE , S_HOST_OFFLINE , S_LOCAL , S_REMOTE };
//------------------------------------------------------------------------------------------
#define		MAX_PORT_NUMS		4
#define		MAX_SLOT_NUMS		25
//------------------------------------------------------------------------------------------
#define		ALARM_PORT1_ID_VERIFICATION_FAIL	271
#define		ALARM_PORT2_ID_VERIFICATION_FAIL	272
#define		ALARM_PORT3_ID_VERIFICATION_FAIL	273
#define		ALARM_PORT4_ID_VERIFICATION_FAIL	274
//------------------------------------------------------------------------------------------
#define		ALARM_PORT1_MAP_VERIFICATION_FAIL	281
#define		ALARM_PORT2_MAP_VERIFICATION_FAIL	282
#define		ALARM_PORT3_MAP_VERIFICATION_FAIL	283
#define		ALARM_PORT4_MAP_VERIFICATION_FAIL	284
//------------------------------------------------------------------------------------------
#define MAX_ALARM_OBJECTS 1000
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{	"PRMD_PM1_Leaf_AuEn_01"			,	_K_D_IO	,	PM1_DryClean_EDAbleOption	,	0		} ,
	{	"PRMD_PM1_Leaf_AuEn_02"			,	_K_D_IO	,	PM1_DryLeak_EDAbleOption	,	0		} ,
	{	"PRMD_PM1_Leaf_AuEn_03"			,	_K_D_IO	,	PM1_DryPost_EDAbleOption	,	0		} ,
	{	"PRMD_PM1_Leaf_AuEn_04"			,	_K_D_IO	,	PM1_WetClean_EDAbleOption	,	0		} ,
	{	"PRMD_PM1_Leaf_AuEn_05"			,	_K_D_IO	,	PM1_Idle_EDAbleOption		,	0		} ,
	{	"PRMD_PM2_Leaf_AuEn_01"			,	_K_D_IO	,	PM2_DryClean_EDAbleOption	,	0		} ,
	{	"PRMD_PM2_Leaf_AuEn_02"			,	_K_D_IO	,	PM2_DryLeak_EDAbleOption	,	0		} ,
	{	"PRMD_PM2_Leaf_AuEn_03"			,	_K_D_IO	,	PM2_DryPost_EDAbleOption	,	0		} ,
	{	"PRMD_PM2_Leaf_AuEn_04"			,	_K_D_IO	,	PM2_WetClean_EDAbleOption	,	0		} ,
	{	"PRMD_PM2_Leaf_AuEn_05"			,	_K_D_IO	,	PM2_Idle_EDAbleOption		,	0		} ,
	{	"PRMD_PM3_Leaf_AuEn_01"			,	_K_D_IO	,	PM3_DryClean_EDAbleOption	,	0		} ,
	{	"PRMD_PM3_Leaf_AuEn_02"			,	_K_D_IO	,	PM3_DryLeak_EDAbleOption	,	0		} ,
	{	"PRMD_PM3_Leaf_AuEn_03"			,	_K_D_IO	,	PM3_DryPost_EDAbleOption	,	0		} ,
	{	"PRMD_PM3_Leaf_AuEn_04"			,	_K_D_IO	,	PM3_WetClean_EDAbleOption	,	0		} ,
	{	"PRMD_PM3_Leaf_AuEn_05"			,	_K_D_IO	,	PM3_Idle_EDAbleOption		,	0		} ,
	{	"PRMD_PM4_Leaf_AuEn_01"			,	_K_D_IO	,	PM4_DryClean_EDAbleOption	,	0		} ,
	{	"PRMD_PM4_Leaf_AuEn_02"			,	_K_D_IO	,	PM4_DryLeak_EDAbleOption	,	0		} ,
	{	"PRMD_PM4_Leaf_AuEn_03"			,	_K_D_IO	,	PM4_DryPost_EDAbleOption	,	0		} ,
	{	"PRMD_PM4_Leaf_AuEn_04"			,	_K_D_IO	,	PM4_WetClean_EDAbleOption	,	0		} ,
	{	"PRMD_PM4_Leaf_AuEn_05"			,	_K_D_IO	,	PM4_Idle_EDAbleOption		,	0		} ,
	{	"PRMD_PM5_Leaf_AuEn_01"			,	_K_D_IO	,	PM5_DryClean_EDAbleOption	,	0		} ,
	{	"PRMD_PM5_Leaf_AuEn_02"			,	_K_D_IO	,	PM5_DryLeak_EDAbleOption	,	0		} ,
	{	"PRMD_PM5_Leaf_AuEn_03"			,	_K_D_IO	,	PM5_DryPost_EDAbleOption	,	0		} ,
	{	"PRMD_PM5_Leaf_AuEn_04"			,	_K_D_IO	,	PM5_WetClean_EDAbleOption	,	0		} ,
	{	"PRMD_PM5_Leaf_AuEn_05"			,	_K_D_IO	,	PM5_Idle_EDAbleOption		,	0		} ,
	
	{	"PRMD_PM1_Leaf_RunSt_01"		,	_K_D_IO	,	PM1_DryClean_RunState		,	0		} ,
	{	"PRMD_PM1_Leaf_RunSt_02"		,	_K_D_IO	,	PM1_DryLeak_RunState		,	0		} ,
	{	"PRMD_PM1_Leaf_RunSt_03"		,	_K_D_IO	,	PM1_DryPost_RunState		,	0		} ,
	{	"PRMD_PM1_Leaf_RunSt_04"		,	_K_D_IO	,	PM1_WetClean_RunState		,	0		} ,
	{	"PRMD_PM1_Leaf_RunSt_05"		,	_K_D_IO	,	PM1_Idle_RunState			,	0		} ,
	{	"PRMD_PM2_Leaf_RunSt_01"		,	_K_D_IO	,	PM2_DryClean_RunState		,	0		} ,
	{	"PRMD_PM2_Leaf_RunSt_02"		,	_K_D_IO	,	PM2_DryLeak_RunState		,	0		} ,
	{	"PRMD_PM2_Leaf_RunSt_03"		,	_K_D_IO	,	PM2_DryPost_RunState		,	0		} ,
	{	"PRMD_PM2_Leaf_RunSt_04"		,	_K_D_IO	,	PM2_WetClean_RunState		,	0		} ,
	{	"PRMD_PM2_Leaf_RunSt_05"		,	_K_D_IO	,	PM2_Idle_RunState			,	0		} ,
	{	"PRMD_PM3_Leaf_RunSt_01"		,	_K_D_IO	,	PM3_DryClean_RunState		,	0		} ,
	{	"PRMD_PM3_Leaf_RunSt_02"		,	_K_D_IO	,	PM3_DryLeak_RunState		,	0		} ,
	{	"PRMD_PM3_Leaf_RunSt_03"		,	_K_D_IO	,	PM3_DryPost_RunState		,	0		} ,
	{	"PRMD_PM3_Leaf_RunSt_04"		,	_K_D_IO	,	PM3_WetClean_RunState		,	0		} ,
	{	"PRMD_PM3_Leaf_RunSt_05"		,	_K_D_IO	,	PM3_Idle_RunState			,	0		} ,
	{	"PRMD_PM4_Leaf_RunSt_01"		,	_K_D_IO	,	PM4_DryClean_RunState		,	0		} ,
	{	"PRMD_PM4_Leaf_RunSt_02"		,	_K_D_IO	,	PM4_DryLeak_RunState		,	0		} ,
	{	"PRMD_PM4_Leaf_RunSt_03"		,	_K_D_IO	,	PM4_DryPost_RunState		,	0		} ,
	{	"PRMD_PM4_Leaf_RunSt_04"		,	_K_D_IO	,	PM4_WetClean_RunState		,	0		} ,
	{	"PRMD_PM4_Leaf_RunSt_05"		,	_K_D_IO	,	PM4_Idle_RunState			,	0		} ,
	{	"PRMD_PM5_Leaf_RunSt_01"		,	_K_D_IO	,	PM5_DryClean_RunState		,	0		} ,
	{	"PRMD_PM5_Leaf_RunSt_02"		,	_K_D_IO	,	PM5_DryLeak_RunState		,	0		} ,
	{	"PRMD_PM5_Leaf_RunSt_03"		,	_K_D_IO	,	PM5_DryPost_RunState		,	0		} ,
	{	"PRMD_PM5_Leaf_RunSt_04"		,	_K_D_IO	,	PM5_WetClean_RunState		,	0		} ,
	{	"PRMD_PM5_Leaf_RunSt_05"		,	_K_D_IO	,	PM5_Idle_RunState			,	0		} ,

	{	"PRMS_PM1_Leaf_Rcp_01"			,	_K_S_IO	,	PM1_DryCleaning_Recipe		,	0		} ,
	{	"PRMS_PM1_Leaf_Rcp_03"			,	_K_S_IO	,	PM1_PostCleaning_Recipe		,	0		} ,
	{	"PRMS_PM1_Leaf_Rcp_04"			,	_K_S_IO	,	PM1_WetCleaning_Recipe		,	0		} ,
	{	"PRMS_PM1_Leaf_Rcp_05"			,	_K_S_IO	,	PM1_Idle_Recipe				,	0		} ,
	{	"PRMS_PM2_Leaf_Rcp_01"			,	_K_S_IO	,	PM2_DryCleaning_Recipe		,	0		} ,
	{	"PRMS_PM2_Leaf_Rcp_03"			,	_K_S_IO	,	PM2_PostCleaning_Recipe		,	0		} ,
	{	"PRMS_PM2_Leaf_Rcp_04"			,	_K_S_IO	,	PM2_WetCleaning_Recipe		,	0		} ,
	{	"PRMS_PM2_Leaf_Rcp_05"			,	_K_S_IO	,	PM2_Idle_Recipe				,	0		} ,
	{	"PRMS_PM3_Leaf_Rcp_01"			,	_K_S_IO	,	PM3_DryCleaning_Recipe		,	0		} ,
	{	"PRMS_PM3_Leaf_Rcp_03"			,	_K_S_IO	,	PM3_PostCleaning_Recipe		,	0		} ,
	{	"PRMS_PM3_Leaf_Rcp_04"			,	_K_S_IO	,	PM3_WetCleaning_Recipe		,	0		} ,
	{	"PRMS_PM3_Leaf_Rcp_05"			,	_K_S_IO	,	PM3_Idle_Recipe				,	0		} ,
	{	"PRMS_PM4_Leaf_Rcp_01"			,	_K_S_IO	,	PM4_DryCleaning_Recipe		,	0		} ,
	{	"PRMS_PM4_Leaf_Rcp_03"			,	_K_S_IO	,	PM4_PostCleaning_Recipe		,	0		} ,
	{	"PRMS_PM4_Leaf_Rcp_04"			,	_K_S_IO	,	PM4_WetCleaning_Recipe		,	0		} ,
	{	"PRMS_PM4_Leaf_Rcp_05"			,	_K_S_IO	,	PM4_Idle_Recipe				,	0		} ,
	{	"PRMS_PM5_Leaf_Rcp_01"			,	_K_S_IO	,	PM5_DryCleaning_Recipe		,	0		} ,
	{	"PRMS_PM5_Leaf_Rcp_03"			,	_K_S_IO	,	PM5_PostCleaning_Recipe		,	0		} ,
	{	"PRMS_PM5_Leaf_Rcp_04"			,	_K_S_IO	,	PM5_WetCleaning_Recipe		,	0		} ,
	{	"PRMS_PM5_Leaf_Rcp_05"			,	_K_S_IO	,	PM5_Idle_Recipe				,	0		} ,


	{	"CTC.PM1_CommStatus"			,	_K_D_IO	,	CTC_PM1_CommStatus			,	0		} ,
	{	"CTC.PM2_CommStatus"			,	_K_D_IO	,	CTC_PM2_CommStatus			,	0		} ,
	{	"CTC.PM3_CommStatus"			,	_K_D_IO	,	CTC_PM3_CommStatus			,	0		} ,
	{	"CTC.PM4_CommStatus"			,	_K_D_IO	,	CTC_PM4_CommStatus			,	0		} ,
	{	"CTC.PM5_CommStatus"			,	_K_D_IO	,	CTC_PM5_CommStatus			,	0		} ,
	
	{	"CTC.MAIN_CONTROL"			,	_K_D_IO	,	PORT1_CONTROL		,	0		} , 
	{	"CTC.MAIN_CONTROL2"			,	_K_D_IO	,	PORT2_CONTROL		,	0		} , 
	{	"CTC.MAIN_CONTROL3"			,	_K_D_IO	,	PORT3_CONTROL		,	0		} , 
	
	{	"CTC.MAIN_STATUS"			,	_K_D_IO	,	PORT1_STATUS		,	0		} , 
	{	"CTC.MAIN_STATUS2"			,	_K_D_IO	,	PORT2_STATUS		,	0		} , 
	{	"CTC.MAIN_STATUS3"			,	_K_D_IO	,	PORT3_STATUS		,	0		} , 

	{	"CTC.FA_STATUS_AGV_CM1"		,	_K_D_IO	,	PORT1_AMHS_MODE		,	0		} , 
	{	"CTC.FA_STATUS_AGV_CM2"		,	_K_D_IO	,	PORT2_AMHS_MODE		,	0		} , 
	{	"CTC.FA_STATUS_AGV_CM3"		,	_K_D_IO	,	PORT3_AMHS_MODE		,	0		} , 
	
	{	"GEM.Control_State"			,	_K_D_IO	,	CONTROL_STATE		,	0		} , 
	
	{	"GEM.CM1_LPReserveStatus"	,	_K_D_IO	,	PORT1_RESERVE		,	0		} , 
	{	"GEM.CM2_LPReserveStatus"	,	_K_D_IO	,	PORT2_RESERVE		,	0		} , 
	{	"GEM.CM3_LPReserveStatus"	,	_K_D_IO	,	PORT3_RESERVE		,	0		} , 
	
	{	"LPMA.F_ProperlyPlaceDI"	,	_K_D_IO	,	FOUP1_STATUS		,	0		} , 
	{	"LPMB.F_ProperlyPlaceDI"	,	_K_D_IO	,	FOUP2_STATUS		,	0		} , 
	{	"LPMC.F_ProperlyPlaceDI"	,	_K_D_IO	,	FOUP3_STATUS		,	0		} ,

	{	"CM1.AMHS_MOVING"			,	_K_D_IO	,	PORT1_AMHS_HO		,	0		} , 
	{	"CM2.AMHS_MOVING"			,	_K_D_IO	,	PORT2_AMHS_HO		,	0		} , 
	{	"CM3.AMHS_MOVING"			,	_K_D_IO	,	PORT3_AMHS_HO		,	0		} , 
	
/*	{	"CM1.PORT_SERVICE"			,	_K_D_IO	,	PORT1_SERVICE		,	0		} , 
	{	"CM2.PORT_SERVICE"			,	_K_D_IO	,	PORT2_SERVICE		,	0		} , 
	{	"CM3.PORT_SERVICE"			,	_K_D_IO	,	PORT3_SERVICE		,	0		} , 

	{	"GEM.Port1_WaferID1"		,	_K_S_IO	,	PORT1_WAFERID_01	,	0		} , 
	{	"GEM.Port1_WaferID2"		,	_K_S_IO	,	PORT1_WAFERID_02	,	0		} , 
	{	"GEM.Port1_WaferID3"		,	_K_S_IO	,	PORT1_WAFERID_03	,	0		} , 
	{	"GEM.Port1_WaferID4"		,	_K_S_IO	,	PORT1_WAFERID_04	,	0		} , 
	{	"GEM.Port1_WaferID5"		,	_K_S_IO	,	PORT1_WAFERID_05	,	0		} , 
	{	"GEM.Port1_WaferID6"		,	_K_S_IO	,	PORT1_WAFERID_06	,	0		} , 
	{	"GEM.Port1_WaferID7"		,	_K_S_IO	,	PORT1_WAFERID_07	,	0		} , 
	{	"GEM.Port1_WaferID8"		,	_K_S_IO	,	PORT1_WAFERID_08	,	0		} , 
	{	"GEM.Port1_WaferID9"		,	_K_S_IO	,	PORT1_WAFERID_09	,	0		} , 
	{	"GEM.Port1_WaferID10"		,	_K_S_IO	,	PORT1_WAFERID_10	,	0		} , 
	{	"GEM.Port1_WaferID11"		,	_K_S_IO	,	PORT1_WAFERID_11	,	0		} , 
	{	"GEM.Port1_WaferID12"		,	_K_S_IO	,	PORT1_WAFERID_12	,	0		} , 
	{	"GEM.Port1_WaferID13"		,	_K_S_IO	,	PORT1_WAFERID_13	,	0		} , 
	{	"GEM.Port1_WaferID14"		,	_K_S_IO	,	PORT1_WAFERID_14	,	0		} , 
	{	"GEM.Port1_WaferID15"		,	_K_S_IO	,	PORT1_WAFERID_15	,	0		} , 
	{	"GEM.Port1_WaferID16"		,	_K_S_IO	,	PORT1_WAFERID_16	,	0		} , 
	{	"GEM.Port1_WaferID17"		,	_K_S_IO	,	PORT1_WAFERID_17	,	0		} , 
	{	"GEM.Port1_WaferID18"		,	_K_S_IO	,	PORT1_WAFERID_18	,	0		} , 
	{	"GEM.Port1_WaferID19"		,	_K_S_IO	,	PORT1_WAFERID_19	,	0		} , 
	{	"GEM.Port1_WaferID20"		,	_K_S_IO	,	PORT1_WAFERID_20	,	0		} , 
	{	"GEM.Port1_WaferID21"		,	_K_S_IO	,	PORT1_WAFERID_21	,	0		} , 
	{	"GEM.Port1_WaferID22"		,	_K_S_IO	,	PORT1_WAFERID_22	,	0		} , 
	{	"GEM.Port1_WaferID23"		,	_K_S_IO	,	PORT1_WAFERID_23	,	0		} , 
	{	"GEM.Port1_WaferID24"		,	_K_S_IO	,	PORT1_WAFERID_24	,	0		} , 
	{	"GEM.Port1_WaferID25"		,	_K_S_IO	,	PORT1_WAFERID_25	,	0		} , 

	{	"GEM.Port2_WaferID1"		,	_K_S_IO	,	PORT2_WAFERID_01	,	0		} , 
	{	"GEM.Port2_WaferID2"		,	_K_S_IO	,	PORT2_WAFERID_02	,	0		} , 
	{	"GEM.Port2_WaferID3"		,	_K_S_IO	,	PORT2_WAFERID_03	,	0		} , 
	{	"GEM.Port2_WaferID4"		,	_K_S_IO	,	PORT2_WAFERID_04	,	0		} , 
	{	"GEM.Port2_WaferID5"		,	_K_S_IO	,	PORT2_WAFERID_05	,	0		} , 
	{	"GEM.Port2_WaferID6"		,	_K_S_IO	,	PORT2_WAFERID_06	,	0		} , 
	{	"GEM.Port2_WaferID7"		,	_K_S_IO	,	PORT2_WAFERID_07	,	0		} , 
	{	"GEM.Port2_WaferID8"		,	_K_S_IO	,	PORT2_WAFERID_08	,	0		} , 
	{	"GEM.Port2_WaferID9"		,	_K_S_IO	,	PORT2_WAFERID_09	,	0		} , 
	{	"GEM.Port2_WaferID10"		,	_K_S_IO	,	PORT2_WAFERID_10	,	0		} , 
	{	"GEM.Port2_WaferID11"		,	_K_S_IO	,	PORT2_WAFERID_11	,	0		} , 
	{	"GEM.Port2_WaferID12"		,	_K_S_IO	,	PORT2_WAFERID_12	,	0		} , 
	{	"GEM.Port2_WaferID13"		,	_K_S_IO	,	PORT2_WAFERID_13	,	0		} , 
	{	"GEM.Port2_WaferID14"		,	_K_S_IO	,	PORT2_WAFERID_14	,	0		} , 
	{	"GEM.Port2_WaferID15"		,	_K_S_IO	,	PORT2_WAFERID_15	,	0		} , 
	{	"GEM.Port2_WaferID16"		,	_K_S_IO	,	PORT2_WAFERID_16	,	0		} , 
	{	"GEM.Port2_WaferID17"		,	_K_S_IO	,	PORT2_WAFERID_17	,	0		} , 
	{	"GEM.Port2_WaferID18"		,	_K_S_IO	,	PORT2_WAFERID_18	,	0		} , 
	{	"GEM.Port2_WaferID19"		,	_K_S_IO	,	PORT2_WAFERID_19	,	0		} , 
	{	"GEM.Port2_WaferID20"		,	_K_S_IO	,	PORT2_WAFERID_20	,	0		} , 
	{	"GEM.Port2_WaferID21"		,	_K_S_IO	,	PORT2_WAFERID_21	,	0		} , 
	{	"GEM.Port2_WaferID22"		,	_K_S_IO	,	PORT2_WAFERID_22	,	0		} , 
	{	"GEM.Port2_WaferID23"		,	_K_S_IO	,	PORT2_WAFERID_23	,	0		} , 
	{	"GEM.Port2_WaferID24"		,	_K_S_IO	,	PORT2_WAFERID_24	,	0		} , 
	{	"GEM.Port2_WaferID25"		,	_K_S_IO	,	PORT2_WAFERID_25	,	0		} , 

	{	"GEM.Port3_WaferID1"		,	_K_S_IO	,	PORT3_WAFERID_01	,	0		} , 
	{	"GEM.Port3_WaferID2"		,	_K_S_IO	,	PORT3_WAFERID_02	,	0		} , 
	{	"GEM.Port3_WaferID3"		,	_K_S_IO	,	PORT3_WAFERID_03	,	0		} , 
	{	"GEM.Port3_WaferID4"		,	_K_S_IO	,	PORT3_WAFERID_04	,	0		} , 
	{	"GEM.Port3_WaferID5"		,	_K_S_IO	,	PORT3_WAFERID_05	,	0		} , 
	{	"GEM.Port3_WaferID6"		,	_K_S_IO	,	PORT3_WAFERID_06	,	0		} , 
	{	"GEM.Port3_WaferID7"		,	_K_S_IO	,	PORT3_WAFERID_07	,	0		} , 
	{	"GEM.Port3_WaferID8"		,	_K_S_IO	,	PORT3_WAFERID_08	,	0		} , 
	{	"GEM.Port3_WaferID9"		,	_K_S_IO	,	PORT3_WAFERID_09	,	0		} , 
	{	"GEM.Port3_WaferID10"		,	_K_S_IO	,	PORT3_WAFERID_10	,	0		} , 
	{	"GEM.Port3_WaferID11"		,	_K_S_IO	,	PORT3_WAFERID_11	,	0		} , 
	{	"GEM.Port3_WaferID12"		,	_K_S_IO	,	PORT3_WAFERID_12	,	0		} , 
	{	"GEM.Port3_WaferID13"		,	_K_S_IO	,	PORT3_WAFERID_13	,	0		} , 
	{	"GEM.Port3_WaferID14"		,	_K_S_IO	,	PORT3_WAFERID_14	,	0		} , 
	{	"GEM.Port3_WaferID15"		,	_K_S_IO	,	PORT3_WAFERID_15	,	0		} , 
	{	"GEM.Port3_WaferID16"		,	_K_S_IO	,	PORT3_WAFERID_16	,	0		} , 
	{	"GEM.Port3_WaferID17"		,	_K_S_IO	,	PORT3_WAFERID_17	,	0		} , 
	{	"GEM.Port3_WaferID18"		,	_K_S_IO	,	PORT3_WAFERID_18	,	0		} , 
	{	"GEM.Port3_WaferID19"		,	_K_S_IO	,	PORT3_WAFERID_19	,	0		} , 
	{	"GEM.Port3_WaferID20"		,	_K_S_IO	,	PORT3_WAFERID_20	,	0		} , 
	{	"GEM.Port3_WaferID21"		,	_K_S_IO	,	PORT3_WAFERID_21	,	0		} , 
	{	"GEM.Port3_WaferID22"		,	_K_S_IO	,	PORT3_WAFERID_22	,	0		} , 
	{	"GEM.Port3_WaferID23"		,	_K_S_IO	,	PORT3_WAFERID_23	,	0		} , 
	{	"GEM.Port3_WaferID24"		,	_K_S_IO	,	PORT3_WAFERID_24	,	0		} , 
	{	"GEM.Port3_WaferID25"		,	_K_S_IO	,	PORT3_WAFERID_25	,	0		} , 

	{	"CM1.C01_Wafer"				,	_K_D_IO	,	PORT1_WAFER_01		,	0		} , 
	{	"CM1.C02_Wafer"				,	_K_D_IO	,	PORT1_WAFER_02		,	0		} , 
	{	"CM1.C03_Wafer"				,	_K_D_IO	,	PORT1_WAFER_03		,	0		} , 
	{	"CM1.C04_Wafer"				,	_K_D_IO	,	PORT1_WAFER_04		,	0		} , 
	{	"CM1.C05_Wafer"				,	_K_D_IO	,	PORT1_WAFER_05		,	0		} , 
	{	"CM1.C06_Wafer"				,	_K_D_IO	,	PORT1_WAFER_06		,	0		} , 
	{	"CM1.C07_Wafer"				,	_K_D_IO	,	PORT1_WAFER_07		,	0		} , 
	{	"CM1.C08_Wafer"				,	_K_D_IO	,	PORT1_WAFER_08		,	0		} , 
	{	"CM1.C09_Wafer"				,	_K_D_IO	,	PORT1_WAFER_09		,	0		} , 
	{	"CM1.C10_Wafer"				,	_K_D_IO	,	PORT1_WAFER_10		,	0		} , 
	{	"CM1.C11_Wafer"				,	_K_D_IO	,	PORT1_WAFER_11		,	0		} , 
	{	"CM1.C12_Wafer"				,	_K_D_IO	,	PORT1_WAFER_12		,	0		} , 
	{	"CM1.C13_Wafer"				,	_K_D_IO	,	PORT1_WAFER_13		,	0		} , 
	{	"CM1.C14_Wafer"				,	_K_D_IO	,	PORT1_WAFER_14		,	0		} , 
	{	"CM1.C15_Wafer"				,	_K_D_IO	,	PORT1_WAFER_15		,	0		} , 
	{	"CM1.C16_Wafer"				,	_K_D_IO	,	PORT1_WAFER_16		,	0		} , 
	{	"CM1.C17_Wafer"				,	_K_D_IO	,	PORT1_WAFER_17		,	0		} , 
	{	"CM1.C18_Wafer"				,	_K_D_IO	,	PORT1_WAFER_18		,	0		} , 
	{	"CM1.C19_Wafer"				,	_K_D_IO	,	PORT1_WAFER_19		,	0		} , 
	{	"CM1.C20_Wafer"				,	_K_D_IO	,	PORT1_WAFER_20		,	0		} , 
	{	"CM1.C21_Wafer"				,	_K_D_IO	,	PORT1_WAFER_21		,	0		} , 
	{	"CM1.C22_Wafer"				,	_K_D_IO	,	PORT1_WAFER_22		,	0		} , 
	{	"CM1.C23_Wafer"				,	_K_D_IO	,	PORT1_WAFER_23		,	0		} , 
	{	"CM1.C24_Wafer"				,	_K_D_IO	,	PORT1_WAFER_24		,	0		} , 
	{	"CM1.C25_Wafer"				,	_K_D_IO	,	PORT1_WAFER_25		,	0		} , 

	{	"CM2.C01_Wafer"				,	_K_D_IO	,	PORT2_WAFER_01		,	0		} , 
	{	"CM2.C02_Wafer"				,	_K_D_IO	,	PORT2_WAFER_02		,	0		} , 
	{	"CM2.C03_Wafer"				,	_K_D_IO	,	PORT2_WAFER_03		,	0		} , 
	{	"CM2.C04_Wafer"				,	_K_D_IO	,	PORT2_WAFER_04		,	0		} , 
	{	"CM2.C05_Wafer"				,	_K_D_IO	,	PORT2_WAFER_05		,	0		} , 
	{	"CM2.C06_Wafer"				,	_K_D_IO	,	PORT2_WAFER_06		,	0		} , 
	{	"CM2.C07_Wafer"				,	_K_D_IO	,	PORT2_WAFER_07		,	0		} , 
	{	"CM2.C08_Wafer"				,	_K_D_IO	,	PORT2_WAFER_08		,	0		} , 
	{	"CM2.C09_Wafer"				,	_K_D_IO	,	PORT2_WAFER_09		,	0		} , 
	{	"CM2.C10_Wafer"				,	_K_D_IO	,	PORT2_WAFER_10		,	0		} , 
	{	"CM2.C11_Wafer"				,	_K_D_IO	,	PORT2_WAFER_11		,	0		} , 
	{	"CM2.C12_Wafer"				,	_K_D_IO	,	PORT2_WAFER_12		,	0		} , 
	{	"CM2.C13_Wafer"				,	_K_D_IO	,	PORT2_WAFER_13		,	0		} , 
	{	"CM2.C14_Wafer"				,	_K_D_IO	,	PORT2_WAFER_14		,	0		} , 
	{	"CM2.C15_Wafer"				,	_K_D_IO	,	PORT2_WAFER_15		,	0		} , 
	{	"CM2.C16_Wafer"				,	_K_D_IO	,	PORT2_WAFER_16		,	0		} , 
	{	"CM2.C17_Wafer"				,	_K_D_IO	,	PORT2_WAFER_17		,	0		} , 
	{	"CM2.C18_Wafer"				,	_K_D_IO	,	PORT2_WAFER_18		,	0		} , 
	{	"CM2.C19_Wafer"				,	_K_D_IO	,	PORT2_WAFER_19		,	0		} , 
	{	"CM2.C20_Wafer"				,	_K_D_IO	,	PORT2_WAFER_20		,	0		} , 
	{	"CM2.C21_Wafer"				,	_K_D_IO	,	PORT2_WAFER_21		,	0		} , 
	{	"CM2.C22_Wafer"				,	_K_D_IO	,	PORT2_WAFER_22		,	0		} , 
	{	"CM2.C23_Wafer"				,	_K_D_IO	,	PORT2_WAFER_23		,	0		} , 
	{	"CM2.C24_Wafer"				,	_K_D_IO	,	PORT2_WAFER_24		,	0		} , 
	{	"CM2.C25_Wafer"				,	_K_D_IO	,	PORT2_WAFER_25		,	0		} , 

	{	"CM3.C01_Wafer"				,	_K_D_IO	,	PORT3_WAFER_01		,	0		} , 
	{	"CM3.C02_Wafer"				,	_K_D_IO	,	PORT3_WAFER_02		,	0		} , 
	{	"CM3.C03_Wafer"				,	_K_D_IO	,	PORT3_WAFER_03		,	0		} , 
	{	"CM3.C04_Wafer"				,	_K_D_IO	,	PORT3_WAFER_04		,	0		} , 
	{	"CM3.C05_Wafer"				,	_K_D_IO	,	PORT3_WAFER_05		,	0		} , 
	{	"CM3.C06_Wafer"				,	_K_D_IO	,	PORT3_WAFER_06		,	0		} , 
	{	"CM3.C07_Wafer"				,	_K_D_IO	,	PORT3_WAFER_07		,	0		} , 
	{	"CM3.C08_Wafer"				,	_K_D_IO	,	PORT3_WAFER_08		,	0		} , 
	{	"CM3.C09_Wafer"				,	_K_D_IO	,	PORT3_WAFER_09		,	0		} , 
	{	"CM3.C10_Wafer"				,	_K_D_IO	,	PORT3_WAFER_10		,	0		} , 
	{	"CM3.C11_Wafer"				,	_K_D_IO	,	PORT3_WAFER_11		,	0		} , 
	{	"CM3.C12_Wafer"				,	_K_D_IO	,	PORT3_WAFER_12		,	0		} , 
	{	"CM3.C13_Wafer"				,	_K_D_IO	,	PORT3_WAFER_13		,	0		} , 
	{	"CM3.C14_Wafer"				,	_K_D_IO	,	PORT3_WAFER_14		,	0		} , 
	{	"CM3.C15_Wafer"				,	_K_D_IO	,	PORT3_WAFER_15		,	0		} , 
	{	"CM3.C16_Wafer"				,	_K_D_IO	,	PORT3_WAFER_16		,	0		} , 
	{	"CM3.C17_Wafer"				,	_K_D_IO	,	PORT3_WAFER_17		,	0		} , 
	{	"CM3.C18_Wafer"				,	_K_D_IO	,	PORT3_WAFER_18		,	0		} , 
	{	"CM3.C19_Wafer"				,	_K_D_IO	,	PORT3_WAFER_19		,	0		} , 
	{	"CM3.C20_Wafer"				,	_K_D_IO	,	PORT3_WAFER_20		,	0		} , 
	{	"CM3.C21_Wafer"				,	_K_D_IO	,	PORT3_WAFER_21		,	0		} , 
	{	"CM3.C22_Wafer"				,	_K_D_IO	,	PORT3_WAFER_22		,	0		} , 
	{	"CM3.C23_Wafer"				,	_K_D_IO	,	PORT3_WAFER_23		,	0		} , 
	{	"CM3.C24_Wafer"				,	_K_D_IO	,	PORT3_WAFER_24		,	0		} , 
	{	"CM3.C25_Wafer"				,	_K_D_IO	,	PORT3_WAFER_25		,	0		} , 
*/	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Chamber_Condition_Control( char *Data ) {
	char szProcessModule[256] , szControlOption[256] , szExecutionOption[256], szRemainStr1[256];
	char szDryCleaningRecipe[256], szPostCleaningRecipe[256], szWetCleaningRecipe[256], szIdleRecipe[256];
	int nCommStatus1, nPMID;
	int nData1, nData2, nData3;
	int nRunSt1, nRunSt2, nRunSt3, nCommData1;
	
	memset(szProcessModule		,		0	,	sizeof(szProcessModule)		);
	memset(szControlOption		,		0	,	sizeof(szControlOption)		);
	memset(szExecutionOption	,		0	,	sizeof(szExecutionOption)	);
	memset(szRemainStr1			,		0	,	sizeof(szRemainStr1)		);
	memset(szDryCleaningRecipe	,		0	,   sizeof(szDryCleaningRecipe)	);
	memset(szPostCleaningRecipe	,		0	,	sizeof(szPostCleaningRecipe));
	memset(szWetCleaningRecipe	,		0	,   sizeof(szWetCleaningRecipe)	);
	memset(szIdleRecipe			,		0	,   sizeof(szIdleRecipe)		);

	STR_SEPERATE_CHAR( Data, '|', szProcessModule, szRemainStr1, 255 );
	STR_SEPERATE_CHAR( szRemainStr1, '|', szControlOption, szExecutionOption, 255 );

	if( STRCMP_L( szProcessModule, "PM1"))
	{
		nPMID = 1;
	}
	else if( STRCMP_L( szProcessModule, "PM2"))
	{
		nPMID = 2;
	}
	else if(STRCMP_L( szProcessModule, "PM3"))
	{		
		nPMID = 3;
	}
	else if( STRCMP_L( szProcessModule, "PM4"))
	{		
		nPMID = 4;
	}
	else if( STRCMP_L( szProcessModule, "PM5"))
	{		
		nPMID = 5;
	}
	else 
	{	
		return SYS_ERROR;
	}
	
	switch(nPMID)
	{
	case 1 :
		{
			nCommData1 = READ_DIGITAL(CTC_PM1_CommStatus, &nCommStatus1);
			if( 7 != nCommData1 )
			{
				if(STRCMP_L(szControlOption, "DryCleaning"))
				{
					nData1 = READ_DIGITAL(PM1_DryClean_EDAbleOption, &nCommStatus1);
					nData2 = READ_DIGITAL(PM1_DryLeak_EDAbleOption, &nCommStatus1);
					nData3 = READ_DIGITAL(PM1_DryPost_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM1_DryClean_RunState, &nCommStatus1);
					nRunSt2 = READ_DIGITAL(PM1_DryLeak_RunState, &nCommStatus1);
					nRunSt3 = READ_DIGITAL(PM1_DryPost_RunState, &nCommStatus1);
					READ_STRING(PM1_DryCleaning_Recipe, szDryCleaningRecipe, &nCommStatus1);
					READ_STRING(PM1_PostCleaning_Recipe, szPostCleaningRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run"))	)
					{
						if( ((1 == nData1) && (0 == STRCMP_L(szDryCleaningRecipe, "")) && ( 1 != nRunSt1)) || ((1 == nData2) && ( 1 != nRunSt2)) || ((1 == nData3) && (0 == STRCMP_L(szPostCleaningRecipe, "")) && ( 1 != nRunSt3)) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Abort"))	)
					{
						if( ((1 == nData1) && ( 2 != nRunSt1)) || ((1 == nData2) && ( 2 != nRunSt2)) || ((1 == nData3) && ( 2 != nRunSt3)) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else if(STRCMP_L(szControlOption, "WetCleaning"))
				{
					nData1 = READ_DIGITAL(PM1_WetClean_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM1_WetClean_RunState, &nCommStatus1);
					READ_STRING(PM1_WetCleaning_Recipe, szWetCleaningRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run")) )
					{
						if( (1 == nData1) && (0 == STRCMP_L(szWetCleaningRecipe, "")) && (1 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Abort")) )
					{
						if( (1 == nData1) && (2 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else if(STRCMP_L(szControlOption, "IdleControl"))
				{
					nData1 = READ_DIGITAL(PM1_Idle_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM1_Idle_RunState, &nCommStatus1);
					READ_STRING(PM1_Idle_Recipe, szIdleRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run")) )
					{
						if( (1 == nData1) && (0 == STRCMP_L(szIdleRecipe, "")) && (1 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Stop")) )
					{
						if( (1 == nData1) && (2 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else
				{
					return SYS_ERROR;
				}
			}
			else
			{
				return SYS_ERROR;
			}
		}
		break;
	case 2 :
		{
			nCommData1 = READ_DIGITAL(CTC_PM2_CommStatus, &nCommStatus1);
			if( 7 != nCommData1 )
			{
				if(STRCMP_L(szControlOption, "DryCleaning"))
				{
					nData1 = READ_DIGITAL(PM2_DryClean_EDAbleOption, &nCommStatus1);
					nData2 = READ_DIGITAL(PM2_DryLeak_EDAbleOption, &nCommStatus1);
					nData3 = READ_DIGITAL(PM2_DryPost_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM2_DryClean_RunState, &nCommStatus1);
					nRunSt2 = READ_DIGITAL(PM2_DryLeak_RunState, &nCommStatus1);
					nRunSt3 = READ_DIGITAL(PM2_DryPost_RunState, &nCommStatus1);
					READ_STRING(PM2_DryCleaning_Recipe, szDryCleaningRecipe, &nCommStatus1);
					READ_STRING(PM2_PostCleaning_Recipe, szPostCleaningRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run"))	)
					{
						if( ((1 == nData1) && (0 == STRCMP_L(szDryCleaningRecipe, "")) && ( 1 != nRunSt1)) || ((1 == nData2) && ( 1 != nRunSt2)) || ((1 == nData3) && (0 == STRCMP_L(szPostCleaningRecipe, "")) && ( 1 != nRunSt3)) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Abort"))	)
					{
						if( ((1 == nData1) && ( 2 != nRunSt1)) || ((1 == nData2) && ( 2 != nRunSt2)) || ((1 == nData3) && ( 2 != nRunSt3)) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else if(STRCMP_L(szControlOption, "WetCleaning"))
				{
					nData1 = READ_DIGITAL(PM2_WetClean_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM2_WetClean_RunState, &nCommStatus1);
					READ_STRING(PM2_WetCleaning_Recipe, szWetCleaningRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run")) )
					{
						if( (1 == nData1) && (0 == STRCMP_L(szWetCleaningRecipe, "")) && (1 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Abort")) )
					{
						if( (1 == nData1) && (2 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else if(STRCMP_L(szControlOption, "IdleControl"))
				{
					nData1 = READ_DIGITAL(PM2_Idle_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM2_Idle_RunState, &nCommStatus1);
					READ_STRING(PM2_Idle_Recipe, szIdleRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run")) )
					{
						if( (1 == nData1) && (0 == STRCMP_L(szIdleRecipe, "")) && (1 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Stop")) )
					{
						if( (1 == nData1) && (2 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else
				{
					return SYS_ERROR;
				}
			}
			else
			{
				return SYS_ERROR;
			}
		}
		break;
	case 3 :
		{
			nCommData1 = READ_DIGITAL(CTC_PM3_CommStatus, &nCommStatus1);
			if( 7 != nCommData1 )
			{
				if(STRCMP_L(szControlOption, "DryCleaning"))
				{
					nData1 = READ_DIGITAL(PM3_DryClean_EDAbleOption, &nCommStatus1);
					nData2 = READ_DIGITAL(PM3_DryLeak_EDAbleOption, &nCommStatus1);
					nData3 = READ_DIGITAL(PM3_DryPost_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM3_DryClean_RunState, &nCommStatus1);
					nRunSt2 = READ_DIGITAL(PM3_DryLeak_RunState, &nCommStatus1);
					nRunSt3 = READ_DIGITAL(PM3_DryPost_RunState, &nCommStatus1);
					READ_STRING(PM3_DryCleaning_Recipe, szDryCleaningRecipe, &nCommStatus1);
					READ_STRING(PM3_PostCleaning_Recipe, szPostCleaningRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run"))	)
					{
						if( ((1 == nData1) && (0 == STRCMP_L(szDryCleaningRecipe, "")) && ( 1 != nRunSt1)) || ((1 == nData2) && ( 1 != nRunSt2)) || ((1 == nData3) && (0 == STRCMP_L(szPostCleaningRecipe, "")) && ( 1 != nRunSt3)) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Abort"))	)
					{
						if( ((1 == nData1) && ( 2 != nRunSt1)) || ((1 == nData2) && ( 2 != nRunSt2)) || ((1 == nData3) && ( 2 != nRunSt3)) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else if(STRCMP_L(szControlOption, "WetCleaning"))
				{
					nData1 = READ_DIGITAL(PM3_WetClean_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM3_WetClean_RunState, &nCommStatus1);
					READ_STRING(PM3_WetCleaning_Recipe, szWetCleaningRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run")) )
					{
						if( (1 == nData1) && (0 == STRCMP_L(szWetCleaningRecipe, "")) && (1 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Abort")) )
					{
						if( (1 == nData1) && (2 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else if(STRCMP_L(szControlOption, "IdleControl"))
				{
					nData1 = READ_DIGITAL(PM3_Idle_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM3_Idle_RunState, &nCommStatus1);
					READ_STRING(PM3_Idle_Recipe, szIdleRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run")) )
					{
						if( (1 == nData1) && (0 == STRCMP_L(szIdleRecipe, "")) && (1 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Stop")) )
					{
						if( (1 == nData1) && (2 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else
				{
					return SYS_ERROR;
				}
			}
			else
			{
				return SYS_ERROR;
			}
		}
		break;
	case 4 :
		{
			nCommData1 = READ_DIGITAL(CTC_PM4_CommStatus, &nCommStatus1);
			if( 7 != nCommData1 )
			{
				if(STRCMP_L(szControlOption, "DryCleaning"))
				{
					nData1 = READ_DIGITAL(PM4_DryClean_EDAbleOption, &nCommStatus1);
					nData2 = READ_DIGITAL(PM4_DryLeak_EDAbleOption, &nCommStatus1);
					nData3 = READ_DIGITAL(PM4_DryPost_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM4_DryClean_RunState, &nCommStatus1);
					nRunSt2 = READ_DIGITAL(PM4_DryLeak_RunState, &nCommStatus1);
					nRunSt3 = READ_DIGITAL(PM4_DryPost_RunState, &nCommStatus1);
					READ_STRING(PM4_DryCleaning_Recipe, szDryCleaningRecipe, &nCommStatus1);
					READ_STRING(PM4_PostCleaning_Recipe, szPostCleaningRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run"))	)
					{
						if( ((1 == nData1) && (0 == STRCMP_L(szDryCleaningRecipe, "")) && ( 1 != nRunSt1)) || ((1 == nData2) && ( 1 != nRunSt2)) || ((1 == nData3) && (0 == STRCMP_L(szPostCleaningRecipe, "")) && ( 1 != nRunSt3)) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Abort"))	)
					{
						if( ((1 == nData1) && ( 2 != nRunSt1)) || ((1 == nData2) && ( 2 != nRunSt2)) || ((1 == nData3) && ( 2 != nRunSt3)) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else if(STRCMP_L(szControlOption, "WetCleaning"))
				{
					nData1 = READ_DIGITAL(PM4_WetClean_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM4_WetClean_RunState, &nCommStatus1);
					READ_STRING(PM4_WetCleaning_Recipe, szWetCleaningRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run")) )
					{
						if( (1 == nData1) && (0 == STRCMP_L(szWetCleaningRecipe, "")) && (1 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Abort")) )
					{
						if( (1 == nData1) && (2 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else if(STRCMP_L(szControlOption, "IdleControl"))
				{
					nData1 = READ_DIGITAL(PM4_Idle_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM4_Idle_RunState, &nCommStatus1);
					READ_STRING(PM4_Idle_Recipe, szIdleRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run")) )
					{
						if( (1 == nData1) && (0 == STRCMP_L(szIdleRecipe, "")) && (1 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Stop")) )
					{
						if( (1 == nData1) && (2 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else
				{
					return SYS_ERROR;
				}
			}
			else
			{
				return SYS_ERROR;
			}
		}
		break;
	case 5 :
		{
			nCommData1 = READ_DIGITAL(CTC_PM5_CommStatus, &nCommStatus1);
			if( 7 != nCommData1 )
			{
				if(STRCMP_L(szControlOption, "DryCleaning"))
				{
					nData1 = READ_DIGITAL(PM5_DryClean_EDAbleOption, &nCommStatus1);
					nData2 = READ_DIGITAL(PM5_DryLeak_EDAbleOption, &nCommStatus1);
					nData3 = READ_DIGITAL(PM5_DryPost_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM5_DryClean_RunState, &nCommStatus1);
					nRunSt2 = READ_DIGITAL(PM5_DryLeak_RunState, &nCommStatus1);
					nRunSt3 = READ_DIGITAL(PM5_DryPost_RunState, &nCommStatus1);
					READ_STRING(PM5_DryCleaning_Recipe, szDryCleaningRecipe, &nCommStatus1);
					READ_STRING(PM5_PostCleaning_Recipe, szPostCleaningRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run"))	)
					{
						if( ((1 == nData1) && (0 == STRCMP_L(szDryCleaningRecipe, "")) && ( 1 != nRunSt1)) || ((1 == nData2) && ( 1 != nRunSt2)) || ((1 == nData3) && (0 == STRCMP_L(szPostCleaningRecipe, "")) && ( 1 != nRunSt3)) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Abort"))	)
					{
						if( ((1 == nData1) && ( 2 != nRunSt1)) || ((1 == nData2) && ( 2 != nRunSt2)) || ((1 == nData3) && ( 2 != nRunSt3)) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else if(STRCMP_L(szControlOption, "WetCleaning"))
				{
					nData1 = READ_DIGITAL(PM5_WetClean_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM5_WetClean_RunState, &nCommStatus1);
					READ_STRING(PM5_WetCleaning_Recipe, szWetCleaningRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run")) )
					{
						if( (1 == nData1) && (0 == STRCMP_L(szWetCleaningRecipe, "")) && (1 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Abort")) )
					{
						if( (1 == nData1) && (2 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else if(STRCMP_L(szControlOption, "IdleControl"))
				{
					nData1 = READ_DIGITAL(PM5_Idle_EDAbleOption, &nCommStatus1);
					nRunSt1 = READ_DIGITAL(PM5_Idle_RunState, &nCommStatus1);
					READ_STRING(PM5_Idle_Recipe, szIdleRecipe, &nCommStatus1);
					if( (STRCMP_L(szExecutionOption, "Run")) )
					{
						if( (1 == nData1) && (0 == STRCMP_L(szIdleRecipe, "")) && (1 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else if( (STRCMP_L(szExecutionOption, "Stop")) )
					{
						if( (1 == nData1) && (2 != nRunSt1) )
						{
							return SYS_SUCCESS;
						}
						else
						{
							return SYS_ERROR;
						}
					}
					else
					{
						return SYS_ERROR;
					}
				}
				else
				{
					return SYS_ERROR;
				}
			}
			else
			{
				return SYS_ERROR;
			}
		}
		break;
	default :
		{
			return SYS_ERROR;
		}
		break;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Alarm_Retry( char *Data ) {
	int nAlarm_Count, nCount, nCount_Module[ MAX_ALARM_OBJECTS ], nCount_Buffer[ MAX_ALARM_OBJECTS ] , i;

	nAlarm_Count = ALARMQUEUECOUNT();
	for ( i = 0 ; i < nAlarm_Count ; i++ )
	{
		nCount_Module[ i ] = ALARMQUEUEMODULE( i , &nCount );
		nCount_Buffer[ i ] = ALARMQUEUEDATA( i , &nCount );
	}
	if( 0 < nAlarm_Count )
	{
		return SYS_SUCCESS;
	}
	else
	{
		return SYS_ERROR;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Alarm_Clear( char *Data ) {
	int nALID, nAlarm_Count, nCount, nCount_Module[ MAX_ALARM_OBJECTS ], nCount_Buffer[ MAX_ALARM_OBJECTS ] , i;
	char szModuleName[256] , szALID[256];

	memset(szModuleName,	0,	sizeof(szModuleName));
	memset(szALID,			0,	sizeof(szALID));

	nAlarm_Count = ALARMQUEUECOUNT();
	for ( i = 0 ; i < nAlarm_Count ; i++ )
	{
		nCount_Module[ i ] = ALARMQUEUEMODULE( i , &nCount );
		nCount_Buffer[ i ] = ALARMQUEUEDATA( i , &nCount );
	}
	STR_SEPERATE_CHAR( Data , '|' , szModuleName , szALID, 255 );
	if ( STRCMP_L(szModuleName, "CTC") || STRCMP_L(szModuleName, "PM1") || STRCMP_L(szModuleName, "PM2") || STRCMP_L(szModuleName, "PM3") || STRCMP_L(szModuleName, "PM4") || STRCMP_L(szModuleName, "PM5"))
	{	 
		 nALID = atoi(szALID);
		 if( 0 < nAlarm_Count )
		 {
			 for ( i = 1 ; i < nAlarm_Count ; i++ )
			 {
				 if( nCount_Buffer[ i ] == nALID)
				 {
					 return SYS_SUCCESS;
				 }
			 }
		 }
		 else
		 {
			 return SYS_ERROR;
		 }
	}
	else
	{
		return SYS_ERROR;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Port_Name_Check( char *Module , int *PortID ) {
	*PortID = -1;
	if		( STRNCMP_L( Module , "PORT", 4 ) ) *PortID = atoi(Module+4)-1;
	else if ( STRNCMP_L( Module , "CM"	, 2 ) ) *PortID = atoi(Module+2)-1;
	else if ( STRNCMP_L( Module , "1"	, 2 ) ) *PortID = 0;
	else if ( STRNCMP_L( Module , "2"	, 2 ) ) *PortID = 1;
	else if ( STRNCMP_L( Module , "3"	, 2 ) ) *PortID = 2;
	else if ( STRNCMP_L( Module , "4"	, 2 ) ) *PortID = 3;
	else return FALSE;
	if ( *PortID < PORT1 || *PortID > PORT4 || *PortID >= MAX_PORT_NUMS ) return FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int All_Foup_Check() {
	int nCommStatus , nFoup1 , nFoup2;
	nFoup1 = READ_DIGITAL( FOUP1_STATUS , &nCommStatus );
	nFoup2 = READ_DIGITAL( FOUP2_STATUS , &nCommStatus );
	if ( nFoup1 == PRESENT || nFoup2 == PRESENT ) return PRESENT;
	return ABSENT;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status GEM_Control_Mode( int Mode ) {
	switch( Mode ) {
		case OFFLINE	:
		case ONLINE		:
		case LOCAL		:
		case REMOTE		:
//			if ( All_Foup_Check() == PRESENT ) return SYS_ERROR;
// Host Online Req(S1F1/S1F17) is only capable in HOST OFFLINE Mode,
// Operator can change to Online at EQUIP OFFLINE Mode
// Open for CCS( Infinion/Inotera )
//			if ( READ_DIGITAL( CONTROL_STATE , &CommStatus ) < S_HOST_OFFLINE ) return SYS_ERROR;
			break;
		default			:
			return SYS_ERROR;
			break;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Port_Control_Service( int Service , char *Port ) {
	int nPortID , nCommStatus , nStatus;
	if ( !Port_Name_Check( Port , &nPortID ) ) return SYS_ERROR;
	switch( Service ) {
		case INSERVICE		:
			if ( READ_DIGITAL( FOUP1_STATUS + nPortID , &nCommStatus ) == PRESENT ) {
				return SYS_ERROR;
			}
			break;
		case OUTOFSERVICE	:
			if ( READ_DIGITAL( FOUP1_STATUS + nPortID , &nCommStatus ) == PRESENT ) {
				return SYS_ERROR;
			}
			nStatus = READ_DIGITAL( PORT1_CONTROL + nPortID , &nCommStatus );
			if ( nStatus >= 1 && nStatus <= 4 ) { // Running
				return SYS_ERROR;
			}
			if ( READ_DIGITAL( PORT1_AMHS_HO + nPortID , &nCommStatus ) == ON ) return SYS_ERROR;
			break;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Port_Control_Access( int Mode , char *Port ) {
	int nPortID , nCommStatus , i;
	if ( STRCMP_L( Port , "ALL" ) ) {
		for ( i = 0 ; i < MAX_PORT_NUMS ; i++ ) {
			if		( READ_DIGITAL( PORT1_RESERVE + i , &nCommStatus ) == RESERVATION ) return SYS_ERROR;
			else if ( READ_DIGITAL( PORT1_AMHS_HO + i , &nCommStatus ) == ON ) return SYS_ERROR;
// In Group, this may not need
		// 20071102, cnlee add
			else if ( READ_DIGITAL( PORT1_AMHS_MODE + i , &nCommStatus ) == Mode ) return SYS_ERROR;
		}
	}
	else {
		if		( !Port_Name_Check( Port , &nPortID ) ) return SYS_ERROR;
		else if ( READ_DIGITAL( PORT1_RESERVE   + nPortID , &nCommStatus ) == RESERVATION ) return SYS_ERROR;
		else if ( READ_DIGITAL( PORT1_AMHS_HO   + nPortID , &nCommStatus ) == ON   ) return SYS_ERROR;
// when already mode, if want to reject, under code release
		// 20071102, cnlee add
		else if ( READ_DIGITAL( PORT1_AMHS_MODE + nPortID , &nCommStatus ) == Mode ) return SYS_ERROR;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*Module_Status Port_Control_Reserve( int Service , char *Port ) {
	int nPortID , nCommStatus;
	if ( !Port_Name_Check( Port , &nPortID ) ) return SYS_ERROR;
	switch( Service ) {
		case NOTRESERVE		:
			break;
		case RESERVATION	:
			if ( READ_DIGITAL( PORT1_SERVICE + nPortID , &nCommStatus ) == OUTOFSERVICE ) {
				return SYS_ERROR;
			}
			if ( READ_DIGITAL( FOUP1_STATUS + nPortID , &nCommStatus ) == PRESENT ) {
				return SYS_ERROR;
			}
			break;
	}
	return SYS_SUCCESS;
}*/
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*Module_Status ContentMap_Analysis( char *ContentMap ) { 
	// Format : CONTENTMAP TYPE:PORTNAME:MAPCOUNT:CARRIERID|SLOTNUM:LOTID00:WAFERID00|SLOTNUMB:LOTID01:WAFERID01|.....~"
	// TYPE => "ID" , "MAP", "BIND" , "CANCELBIND" , PORTNAME => "PORT1", "PORT2", "PORT3" , "PORT4", MAPCOUNT => 0 ~ 25, SLOTNUM => 0 ~ 25 , CARRIERID is MATERIAL ID
	// Bind/CancelBind is used normally Full Auto( OHT&REMOTE )
	BOOL bCheck = FALSE;
//	int		nPortID , nCount , nIO , nIO2 , nIO3 /*, nIO4*/ //, nSlotNum , i , j , nCommStatus , nStatus;
/*	char	szBuffer[1023] , szBuffer2[1023] , szSubBuffer[1023] , szSubBuffer2[1023] , szWaferID[255];

	memset(szBuffer,		0,	sizeof(szBuffer));
	memset(szBuffer2,		0,	sizeof(szBuffer2));
	memset(szSubBuffer,		0,	sizeof(szSubBuffer));
	memset(szSubBuffer2,	0,	sizeof(szSubBuffer2));
	memset(szWaferID,		0,	sizeof(szWaferID));

// printf( "=====>> ContentMap : [%s]\n" , ContentMap );

	STR_SEPERATE_CHAR( ContentMap , '|' , szBuffer , szBuffer2 , 1023 ); // Buffer => TYPE:PORTNAME:MAPCOUNT
	STR_SEPERATE_CHAR( szBuffer , ':' , szSubBuffer , szSubBuffer2 , 1023 ); // SubBuffer => ID or MAP or BIND or CANCELBIND
//	if ( STRCMP_L( SubBuffer , "MAP" ) ) return SYS_SUCCESS; // if data is used only in ID Vrfication, close this line
	if		( STRCMP_L( szSubBuffer , "BIND"		 ) ) return SYS_SUCCESS; // if it is used bind data, comment
	else if ( STRCMP_L( szSubBuffer , "CANCELBIND" ) ) return SYS_SUCCESS;

	if ( STRCMP_L( szSubBuffer , "MAP" ) ) bCheck = TRUE;

	STR_SEPERATE_CHAR( szSubBuffer2 , ':' , szSubBuffer , szSubBuffer2 , 1023 ); // SubBuffer => PORT1 or PORT2 or PORT3 or PORT4
	if ( !Port_Name_Check( szSubBuffer , &nPortID ) ) return SYS_ERROR;
	if		( nPortID == PORT1 ) { nIO = PORT2_WAFERID_01; nIO2 = PORT3_WAFERID_01; nIO3 = PORT1_WAFER_01; }
	else if ( nPortID == PORT2 ) { nIO = PORT1_WAFERID_01; nIO2 = PORT3_WAFERID_01; nIO3 = PORT2_WAFER_01; }
	else if ( nPortID == PORT3 ) { nIO = PORT1_WAFERID_01; nIO2 = PORT2_WAFERID_01; nIO3 = PORT3_WAFER_01; }
	else return SYS_ERROR; // Now Port is used only PORT1 and PORT2

	STR_SEPERATE_CHAR( szSubBuffer2 , ':' , szSubBuffer , szSubBuffer2 , 1023 ); // SubBuffer => MAPCOUNT
	nCount = atoi( szSubBuffer );
	if ( nCount > MAX_SLOT_NUMS ) return SYS_ERROR;

	STR_SEPERATE_CHAR( szSubBuffer2 , ':' , szSubBuffer , szSubBuffer2 , 1023 ); // SubBuffer => CARRIERID
// 	for ( i = 0 ; i < MAX_SLOT_NUMS ; i++ ) WRITE_STRING( IO + i , "" , &CommStatus );
	for ( i = 0 ; i < nCount ; i++ ) {
		STR_SEPERATE_CHAR( szBuffer2 , '|' , szBuffer , szBuffer2 , 1023 ); // Buffer => SLOTNUM:LOTID:WAFERID
		STR_SEPERATE_CHAR( szBuffer , ':' , szSubBuffer , szSubBuffer2 , 1023 ); // SubBuffer => SLOTNUM
		nSlotNum = atoi( szSubBuffer );
		if ( strlen( szSubBuffer ) <= 0 || nSlotNum > MAX_SLOT_NUMS || nSlotNum < 1 ) continue;
		STR_SEPERATE_CHAR( szSubBuffer2 , ':' , szSubBuffer , szSubBuffer2 , 1023 ); // SubBuffer => LOTID
		STR_SEPERATE_CHAR( szSubBuffer2 , ':' , szSubBuffer , szSubBuffer2 , 1023 ); // SubBuffer => WAFERID
// 		WRITE_STRING( IO + SlotNum - 1 , SubBuffer , &CommStatus );
		if ( strlen( szSubBuffer ) > 0 ) {
			if ( bCheck ) {
				nStatus = READ_DIGITAL( nIO3+nSlotNum-1 , &nCommStatus );
				if ( nStatus != 2 && nStatus != 3 ) return SYS_ERROR; // 2 = Present, 3 = Processed // when real wafer is none, if WaferID exist
			}
			for ( j = 0 ; j < MAX_SLOT_NUMS ; j++ ) {
				READ_STRING( nIO+j , szWaferID , &nCommStatus );
				if ( STRCMP_L( szSubBuffer , szWaferID ) ) {
					printf( "=====> ContentMap WaferID Duplication Error : %s\n" , szSubBuffer );
					return SYS_ERROR;
				} 
			}
			for ( j = 0 ; j < MAX_SLOT_NUMS ; j++ ) {
				READ_STRING( nIO2+j , szWaferID , &nCommStatus );
				if ( STRCMP_L( szSubBuffer , szWaferID ) ) {
					printf( "=====> ContentMap WaferID Duplication Error : %s\n" , szSubBuffer );
					return SYS_ERROR;
				} 
			}
			for ( j = 0 ; j < MAX_SLOT_NUMS ; j++ ) {
				READ_STRING( nIO3+j , szWaferID , &nCommStatus );
				if ( STRCMP_L( szSubBuffer , szWaferID ) ) {
					printf( "=====> ContentMap WaferID Duplication Error : %s\n" , szSubBuffer );
					return SYS_ERROR;
				} 
			}
		}
	}
	// if need, this io must be clear when foup is removed
	return SYS_SUCCESS;
}*/
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*Module_Status SlotMap_Analysis( char *SlotMap ) {
	BOOL Check = FALSE;
	int i , nPortID , nIO , nStatus , nCommStatus;
	char szBuffer[256] , szBuffer2[256] , szSubBuffer[256] , szSubBuffer2[256];;

	memset(szBuffer,		0,  sizeof(szBuffer));
	memset(szBuffer2,		0,	sizeof(szBuffer2));
	memset(szSubBuffer,		0,  sizeof(szSubBuffer));
	memset(szSubBuffer2,	0,	sizeof(szSubBuffer2));

	STR_SEPERATE_CHAR( SlotMap , '|' , szBuffer , szBuffer2	, 255 ); // szBuffer = SlotMap Info
	STR_SEPERATE_CHAR( szBuffer  , ':' , szSubBuffer , szSubBuffer2 , 255 ); // szSubBuffer = ID/MAP/BIND/CARRIERNOTIFY/CARRIERRECREATE
	if ( STRCMP_L( szSubBuffer , "MAP" ) ) {
		STR_SEPERATE_CHAR( szSubBuffer2  , ':' , szSubBuffer , szSubBuffer2 , 255 ); // szSubBuffer = Port Number
		if ( !Port_Name_Check( szSubBuffer , &nPortID ) ) return SYS_ERROR;
		if		( nPortID == PORT1 ) nIO = PORT1_WAFER_01;
		else if ( nPortID == PORT2 ) nIO = PORT2_WAFER_01;
		else if ( nPortID == PORT3 ) nIO = PORT3_WAFER_01;
		else return SYS_ERROR;
		Check = TRUE;
	}
	STR_SEPERATE_CHAR( szBuffer2 , '|' , szBuffer , szBuffer2	, 255 ); // szBuffer = SlotID
	strcpy( szSubBuffer2 , szBuffer ); // szSubBuffer2 = 3:3:3:3...
	i = 0;
	while ( TRUE ) {
		STR_SEPERATE_CHAR( szSubBuffer2 , ':' , szSubBuffer , szSubBuffer2 , 255 ); // szSubBuffer = SlotID
		if ( strlen( szSubBuffer ) <= 0 ) break;
		if ( szSubBuffer[0] != '1' && szSubBuffer[0] != '2' && szSubBuffer[0] != '3' ) return SYS_ERROR; // 1 = EMPTY, 2 = NOTEMPTY, 3 = OCCUPIED
		if ( i < MAX_SLOT_NUMS && Check ) {
			nStatus = READ_DIGITAL( nIO+i , &nCommStatus );
			if ( nStatus != 2 && nStatus != 3 ) { // 2 = Present, 3 = Processed
				if ( szSubBuffer[0] == '2' || szSubBuffer[0] == '3' ) return SYS_ERROR; // when real wafer is none, if SlotID is not 1
			}
		}
		i++;
	}
	return SYS_SUCCESS;
}*/
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// CTCMainStatus Idle Loading Loaded LoadFail Mapping Mapped MapFail Start HandOffIn Begin MapIn Waiting Running MapOut HandOffOut Complete Cancel Aborted Unloading Unloading(C) Unloaded UnloadFail Unloaded(C)
Module_Status CancelCarrier_Analysis( int CancelMode , char *CancelData ) {
	int nPortID , nCommStatus , nStatus;
	char szBuffer[256] , szBuffer2[256];

	memset(szBuffer,	0,  sizeof(szBuffer));
	memset(szBuffer2,	0,	sizeof(szBuffer2));
	printf( "=====> Host Command CancelCarrier_Analysis\n" );
	STR_SEPERATE_CHAR( CancelData , '|' , szBuffer , szBuffer2	, 255 ); // Buffer = PORT Number
	if ( !Port_Name_Check( szBuffer , &nPortID ) ) return SYS_ERROR;
	nStatus = READ_DIGITAL( PORT1_STATUS+nPortID , &nCommStatus );
	//============================================================================================================================================
	// 0=Idle, 1=Loading, 2=Loaded, 3=LoadFail, 4=Mapping, 5=Mapped, 6=MapFail, 7=Start, 8=HandOffIn, 9=Begin, 10=MapIn, 11=Waiting, 12=Running, 
	// 13=MapOut, 14=HandOffOut, 15=Complete, 16=Cancel, 17=Aborted, 18=Unloading, 19=Unloading(C), 20=Unloaded, 21=UnloadFail, 22=Unloaded(C)
	//============================================================================================================================================
	if ( nStatus == 1 || nStatus == 12 || nStatus == 14 || nStatus == 18 || nStatus == 19 ) return SYS_ERROR;
	switch( CancelMode ) {
		case CANCELCARRIER		:
			break;
		case CANCELCARRIERATPORT:
			break;
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
/*void Alarm_IDVerify_Fail( char *Data ) {
	int nPortID;
	char szBuffer[256] , szBuffer2[256];

	memset(szBuffer,	0,  sizeof(szBuffer));
	memset(szBuffer2,	0,	sizeof(szBuffer2));

	STR_SEPERATE_CHAR( Data , '|' , szBuffer , szBuffer2	, 255 ); // Buffer = PORT Number : PORT1 , PORT2 ... , Buffer2 = Message Happen Program Point in EasyGEM
	if ( !Port_Name_Check( szBuffer , &nPortID ) ) return;
	if ( ALARM_STATUS( ALARM_PORT1_ID_VERIFICATION_FAIL + nPortID ) != ALM_PAUSED ) ALARM_POST( ALARM_PORT1_ID_VERIFICATION_FAIL + nPortID );
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Alarm_MapVerify_Fail( char *Data ) {
	int nPortID;
	char szBuffer[256] , szBuffer2[256];

	memset(szBuffer,	0,  sizeof(szBuffer));
	memset(szBuffer2,	0,	sizeof(szBuffer2));

	STR_SEPERATE_CHAR( Data , '|' , szBuffer , szBuffer2	, 255 ); // Buffer = PORT Number : PORT1 , PORT2 ... , Buffer2 = Message Happen Program Point in EasyGEM
	if ( !Port_Name_Check( szBuffer , &nPortID ) ) return;
//	printf( "=====> Alarm_MapVerify_Fail : [%s]\n", Data );
	if ( ALARM_STATUS( ALARM_PORT1_MAP_VERIFICATION_FAIL + nPortID ) != ALM_PAUSED ) ALARM_POST( ALARM_PORT1_MAP_VERIFICATION_FAIL + nPortID );
}*/
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int	nDl, nDl2;
	char *pszEd;
	char szBuffer[1024] , szBuffer2[1024], szBuffer3[1024];

	memset(szBuffer,	0,  sizeof(szBuffer));
	memset(szBuffer2,	0,	sizeof(szBuffer2));
	memset(szBuffer3,	0,	sizeof(szBuffer3));

	pszEd = PROGRAM_PARAMETER_READ();
	nDl = strlen( pszEd );
	pszEd[ nDl ] = 0x00;
	
	if ( nDl < 1 ) {
		printf( "=====> Host Command Manager Parameter Error\n" );
		return SYS_ERROR;
	}
	printf("===###&&& Host Command : [%s]\n", pszEd);
	//	STR_SEPERATE_CHAR( pszEd, '|', szBuffer, szBuffer2, 1023 );	
	STR_SEPERATE( pszEd, szBuffer, szBuffer2, 1023 );	
	
	nDl2 = strlen(szBuffer2);
	if ( nDl2 < 1 ) {
		STR_SEPERATE_CHAR( szBuffer, '|', szBuffer, szBuffer2, 1023 );
		if( STRCMP_L( szBuffer , "Alarm_Clear") )
		{
			return Alarm_Clear( szBuffer2 );
		}			
		else if ( STRCMP_L( szBuffer , "Alarm_Retry") ) 
		{
			return Alarm_Retry( szBuffer2 );
		}
		else if ( STRCMP_L( szBuffer , "PM1") || STRCMP_L( szBuffer , "PM2") || STRCMP_L( szBuffer , "PM3") || STRCMP_L( szBuffer , "PM4") || STRCMP_L( szBuffer , "PM5") ) 
		{
			STR_SEPERATE_CHAR( szBuffer2, '|', szBuffer2, szBuffer3, 1023 );
			if (( STRCMP_L( szBuffer2 , "DryCleaning") || STRCMP_L( szBuffer2 , "WetCleaning") || STRCMP_L( szBuffer2 , "IdleControl")))
			{
				return Chamber_Condition_Control( pszEd );
			}
		}
	}
	else {
		if	( STRCMP_L( szBuffer , "OFFLINE" ) )
			return GEM_Control_Mode( OFFLINE );
		else if	( STRCMP_L( szBuffer , "ONLINE" ) )
			return GEM_Control_Mode( ONLINE  );
		else if ( STRCMP_L( szBuffer , "LOCAL" ) )
			return GEM_Control_Mode( LOCAL	 );
		else if ( STRCMP_L( szBuffer , "REMOTE"	) )
			return GEM_Control_Mode( REMOTE  );
		else if ( STRCMP_L( szBuffer , "INSERVICE" ) )
			return Port_Control_Service( INSERVICE	  , szBuffer2 );
		else if ( STRCMP_L( szBuffer , "OUTOFSERVICE" ) )
			return Port_Control_Service( OUTOFSERVICE , szBuffer2 );
		else if ( STRCMP_L( szBuffer , "AUTOACCESS"	) )
			return Port_Control_Access ( AUTOACCESS   , szBuffer2 );
		else if ( STRCMP_L( szBuffer , "MANUALACCESS" ) )
			return Port_Control_Access ( MANUALACCESS , szBuffer2 );
/*		else if ( STRCMP_L( szBuffer , "RESERVATION" ) )
			return Port_Control_Reserve( RESERVATION  , szBuffer2 );
		else if ( STRCMP_L( szBuffer , "CONTENTMAP"	) )
			return ContentMap_Analysis( szBuffer2 );
		else if ( STRCMP_L( szBuffer , "SLOTMAP" ) )
			return SlotMap_Analysis( szBuffer2 );
*/		else if ( STRCMP_L( szBuffer , "CANCELCARRIER" ) )
			return CancelCarrier_Analysis( CANCELCARRIER , szBuffer2 );
		else if ( STRCMP_L( szBuffer , "CANCELCARRIERATPORT" ) )
			return CancelCarrier_Analysis( CANCELCARRIERATPORT , szBuffer2 );
	}
	return SYS_SUCCESS;
}
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//---------------------------------------------------------------------------------------
void Event_Message_Received() {
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
