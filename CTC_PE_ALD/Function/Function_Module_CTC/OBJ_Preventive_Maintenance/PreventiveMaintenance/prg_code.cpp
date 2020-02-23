// Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "CPreventiveMaintenance_Client.h"
#include "PreventiveMaintenance_Client_Type2.h"		// 2006.10.18
#include "PrevMaintMonitor.h"					// 2006.02.20
#include "TextParser.h"

#ifndef _DEBUG
#define _DEBUG
#endif

CPreventiveMaintenance_Client* g_pCPreventiveMaintenance_Client = NULL;
CPrevMaintMonitor* g_pPrevMaintMonitor = NULL;	// 2006.02.20

int g_nALARM_BASE = 9999;
BOOL g_bRFMonitoring = FALSE;

// 2006.10.18
enum { CType1_MAINT_CLEAN, CType2_ONLINE_CLEAN };
int g_nCleanType = CType1_MAINT_CLEAN;

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "e:Scheduler"				, _K_S_IO  ,	SCHEDULER				,	0	} ,
	{ "CTC.$0_PREVENTIVE_SAVE"	, _K_D_IO  ,	PREVENTIVE_SAVE			,	0	} ,

	////////
	{ "CTC.$0_PROCESS_COUNT"	, _K_D_IO  ,	Process_Wafer_Count		,	0	} ,

	//2007.12.06 for online dry cleaning
	{ "CLEAN_ENABLE_TYPE"		, _K_D_IO  ,	Clean_Enable_Type   	,	0	} ,
	{ "PM1.PRO_ChamberStatus"	, _K_D_IO  ,	PM1_ChamberStatus   	,	0	} ,
	{ "PM2.PRO_ChamberStatus"	, _K_D_IO  ,	PM2_ChamberStatus   	,	0	} ,
	{ "PM3.PRO_ChamberStatus"	, _K_D_IO  ,	PM3_ChamberStatus   	,	0	} ,

	{ "CTC.FA_Wafer_Status"	    , _K_D_IO  ,	ATMAWaferSts           	,	0	} ,
	{ "CTC.FA_Wafer_result"	    , _K_D_IO  ,	ATMAWaferResult        	,	0	} ,
	{ "CTC.FB_Wafer_Status"	    , _K_D_IO  ,	ATMBWaferSts           	,	0	} ,
	{ "CTC.FB_Wafer_result"	    , _K_D_IO  ,	ATMBWaferResult         ,	0	} ,
	
	{ "CTC.FM_AL_Wafer_Status"	, _K_D_IO  ,	AlignerWaferSts        	,	0	} ,
	

	{ "$0.PRO_ComptDM"			, _K_D_IO  ,	PROCESS_Completed		,	0	} ,
	// 2007.01.11 for online dry cleaning /////////////////////////////////////////
	{ "$0.PRO_ComptDM1"			, _K_D_IO  ,	PROCESS_Completed1		,	0	} ,
	{ "$0.PRO_ComptDM2"			, _K_D_IO  ,	PROCESS_Completed2		,	0	} ,
	{ "$0.PRO_ComptDM3"			, _K_D_IO  ,	PROCESS_Completed3		,	0	} ,
	{ "$0.PRO_ComptDM4"			, _K_D_IO  ,	PROCESS_Completed4		,	0	} ,
	{ "$0.PRO_ComptDM5"			, _K_D_IO  ,	PROCESS_Completed5		,	0	} ,
	{ "$0.PRO_ComptDM6"			, _K_D_IO  ,	PROCESS_Completed6		,	0	} ,
	{ "$0.PRO_ComptDM7"			, _K_D_IO  ,	PROCESS_Completed7		,	0	} ,
	{ "$0.PRO_ComptDM8"			, _K_D_IO  ,	PROCESS_Completed8		,	0	} ,
	{ "$0.PRO_ComptDM9"			, _K_D_IO  ,	PROCESS_Completed9		,	0	} ,

	{ "$0.PRO_PrvVlvCycAM"		, _K_A_IO  ,	VLV_Total_Count			,	0	} ,
	{ "$0.PRO_PrvSrcFdAM"		, _K_A_IO  ,	SRC_Feeding_Amount		,	0	} ,
	// 2006.02.20
	{ "$0.PRO_PrvThickAM"		, _K_A_IO  ,	Thickness_Amount		,	0	} ,

//	{ "$0.RFPwOnDO"				, _K_D_IO  ,	RFPwOnDO				,	0	} ,

	// WET cleaning
	{ "PRMD_$0_WETEn_01"		, _K_D_IO  ,	PRMD_WETEn_01			,	0	} ,
	{ "PRMD_$0_WETEn_02"		, _K_D_IO  ,	PRMD_WETEn_02			,	0	} ,
	{ "PRMD_$0_WETEn_03"		, _K_D_IO  ,	PRMD_WETEn_03			,	0	} ,
	// 2018.06.17
	{ "PRMD_$0_WETEn_04"		, _K_D_IO  ,	PRMD_WETEn_04			,	0	} ,
	
	{ "PRMD_$0_WETAlarm"		, _K_D_IO  ,	PRMD_WETAlarm			,	0	} ,

	{ "PRMD_$0_WETCWfrCnt"		, _K_D_IO  ,	PRMD_WETCWfrCnt			,	0	} ,
	{ "PRMD_$0_WETWWfrCnt"		, _K_D_IO  ,	PRMD_WETWWfrCnt			,	0	} ,
	{ "PRMD_$0_WETAWfrCnt"		, _K_D_IO  ,	PRMD_WETAWfrCnt			,	0	} ,

	{ "PRMD_$0_WETCVlvCyc"		, _K_D_IO  ,	PRMD_WETCVlvCyc			,	0	} ,
	{ "PRMD_$0_WETWVlvCyc"		, _K_D_IO  ,	PRMD_WETWVlvCyc			,	0	} ,
	{ "PRMD_$0_WETAVlvCyc"		, _K_D_IO  ,	PRMD_WETAVlvCyc			,	0	} ,

	{ "PRMD_$0_WETCSrcFeed"		, _K_A_IO  ,	PRMD_WETCSrcFeed		,	0	} ,
	{ "PRMD_$0_WETWSrcFeed"		, _K_A_IO  ,	PRMD_WETWSrcFeed		,	0	} ,
	{ "PRMD_$0_WETASrcFeed"		, _K_A_IO  ,	PRMD_WETASrcFeed		,	0	} ,
	// 2018.06.17
	{ "PRMA_$0_WETCThick"		, _K_A_IO  ,	PRMA_WETCThick			,	0	} ,
	{ "PRMA_$0_WETWThick"		, _K_A_IO  ,	PRMA_WETWThick			,	0	} ,
	{ "PRMA_$0_WETAThick"		, _K_A_IO  ,	PRMA_WETAThick			,	0	} ,

	// Added by jonghyuk  2009.08.24
	{ "PRMD_$0_WET2CWfrCnt"		, _K_D_IO  ,	PRMD_WET2CWfrCnt		,	0	} ,

	{ "PRMD_$0_WETRstCnt"		, _K_D_IO  ,	PRMD_WETRstCnt			,	0	} ,
	{ "PRMS_$0_WETRstTime"		, _K_S_IO  ,	PRMS_WETRstTime			,	0	} ,
	{ "PRMS_$0_WETRstTime01"	, _K_S_IO  ,	PRMS_WETRstTime01		,	0	} ,
	{ "PRMS_$0_WETRstTime02"	, _K_S_IO  ,	PRMS_WETRstTime02		,	0	} ,
	{ "PRMS_$0_WETRstTime03"	, _K_S_IO  ,	PRMS_WETRstTime03		,	0	} ,
	{ "PRMS_$0_WETRstTime04"	, _K_S_IO  ,	PRMS_WETRstTime04		,	0	} ,
	{ "PRMS_$0_WETRstTime05"	, _K_S_IO  ,	PRMS_WETRstTime05		,	0	} ,
	{ "PRMS_$0_WETRstTime06"	, _K_S_IO  ,	PRMS_WETRstTime06		,	0	} ,
	{ "PRMS_$0_WETRstTime07"	, _K_S_IO  ,	PRMS_WETRstTime07		,	0	} ,
	{ "PRMS_$0_WETRstTime08"	, _K_S_IO  ,	PRMS_WETRstTime08		,	0	} ,
	{ "PRMS_$0_WETRstTime09"	, _K_S_IO  ,	PRMS_WETRstTime09		,	0	} ,
	{ "PRMS_$0_WETRstTime10"	, _K_S_IO  ,	PRMS_WETRstTime10		,	0	} ,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	{ "PRMS_$0_WETRstCount01"	, _K_D_IO  ,	PRMS_WETRstCount01		,	0	} ,
	{ "PRMS_$0_WETRstCount02"	, _K_D_IO  ,	PRMS_WETRstCount02		,	0	} ,
	{ "PRMS_$0_WETRstCount03"	, _K_D_IO  ,	PRMS_WETRstCount03		,	0	} ,
	{ "PRMS_$0_WETRstCount04"	, _K_D_IO  ,	PRMS_WETRstCount04		,	0	} ,
	{ "PRMS_$0_WETRstCount05"	, _K_D_IO  ,	PRMS_WETRstCount05		,	0	} ,
	{ "PRMS_$0_WETRstCount06"	, _K_D_IO  ,	PRMS_WETRstCount06		,	0	} ,
	{ "PRMS_$0_WETRstCount07"	, _K_D_IO  ,	PRMS_WETRstCount07		,	0	} ,
	{ "PRMS_$0_WETRstCount08"	, _K_D_IO  ,	PRMS_WETRstCount08		,	0	} ,
	{ "PRMS_$0_WETRstCount09"	, _K_D_IO  ,	PRMS_WETRstCount09		,	0	} ,
	{ "PRMS_$0_WETRstCount10"	, _K_D_IO  ,	PRMS_WETRstCount10		,	0	} ,

	// DRY cleaning
	{ "PRMD_$0_DRYEn_01"		, _K_D_IO  ,	PRMD_DRYEn_01			,	0	} ,
	{ "PRMD_$0_DRYEn_02"		, _K_D_IO  ,	PRMD_DRYEn_02			,	0	} ,
	{ "PRMD_$0_DRYEn_03"		, _K_D_IO  ,	PRMD_DRYEn_03			,	0	} ,
	{ "PRMD_$0_DRYAlarm"		, _K_D_IO  ,	PRMD_DRYAlarm			,	0	} ,

	{ "PRMD_$0_DRYCWfrCnt"		, _K_D_IO  ,	PRMD_DRYCWfrCnt			,	0	} ,
	{ "PRMD_$0_DRYWWfrCnt"		, _K_D_IO  ,	PRMD_DRYWWfrCnt			,	0	} ,
	{ "PRMD_$0_DRYAWfrCnt"		, _K_D_IO  ,	PRMD_DRYAWfrCnt			,	0	} ,

	{ "PRMD_$0_DRYCVlvCyc"		, _K_D_IO  ,	PRMD_DRYCVlvCyc			,	0	} ,
	{ "PRMD_$0_DRYWVlvCyc"		, _K_D_IO  ,	PRMD_DRYWVlvCyc			,	0	} ,
	{ "PRMD_$0_DRYAVlvCyc"		, _K_D_IO  ,	PRMD_DRYAVlvCyc			,	0	} ,

	{ "PRMD_$0_DRYCSrcFeed"		, _K_A_IO  ,	PRMD_DRYCSrcFeed		,	0	} ,
	{ "PRMD_$0_DRYWSrcFeed"		, _K_A_IO  ,	PRMD_DRYWSrcFeed		,	0	} ,
	{ "PRMD_$0_DRYASrcFeed"		, _K_A_IO  ,	PRMD_DRYASrcFeed		,	0	} ,

	// Added by jonghyuk  2009.08.24
	{ "PRMD_$0_DRY2CWfrCnt"		, _K_D_IO  ,	PRMD_DRY2CWfrCnt		,	0	} ,

	{ "PRMD_$0_DRYRstCnt"		, _K_D_IO  ,	PRMD_DRYRstCnt			,	0	} ,
	{ "PRMS_$0_DRYRstTime"		, _K_S_IO  ,	PRMS_DRYRstTime			,	0	} ,
	{ "PRMS_$0_DRYRstTime01"	, _K_S_IO  ,	PRMS_DRYRstTime01		,	0	} ,
	{ "PRMS_$0_DRYRstTime02"	, _K_S_IO  ,	PRMS_DRYRstTime02		,	0	} ,
	{ "PRMS_$0_DRYRstTime03"	, _K_S_IO  ,	PRMS_DRYRstTime03		,	0	} ,
	{ "PRMS_$0_DRYRstTime04"	, _K_S_IO  ,	PRMS_DRYRstTime04		,	0	} ,
	{ "PRMS_$0_DRYRstTime05"	, _K_S_IO  ,	PRMS_DRYRstTime05		,	0	} ,
	{ "PRMS_$0_DRYRstTime06"	, _K_S_IO  ,	PRMS_DRYRstTime06		,	0	} ,
	{ "PRMS_$0_DRYRstTime07"	, _K_S_IO  ,	PRMS_DRYRstTime07		,	0	} ,
	{ "PRMS_$0_DRYRstTime08"	, _K_S_IO  ,	PRMS_DRYRstTime08		,	0	} ,
	{ "PRMS_$0_DRYRstTime09"	, _K_S_IO  ,	PRMS_DRYRstTime09		,	0	} ,
	{ "PRMS_$0_DRYRstTime10"	, _K_S_IO  ,	PRMS_DRYRstTime10		,	0	} ,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	{ "PRMS_$0_DRYRstCount01"	, _K_D_IO  ,	PRMS_DRYRstCount01		,	0	} ,
	{ "PRMS_$0_DRYRstCount02"	, _K_D_IO  ,	PRMS_DRYRstCount02		,	0	} ,
	{ "PRMS_$0_DRYRstCount03"	, _K_D_IO  ,	PRMS_DRYRstCount03		,	0	} ,
	{ "PRMS_$0_DRYRstCount04"	, _K_D_IO  ,	PRMS_DRYRstCount04		,	0	} ,
	{ "PRMS_$0_DRYRstCount05"	, _K_D_IO  ,	PRMS_DRYRstCount05		,	0	} ,
	{ "PRMS_$0_DRYRstCount06"	, _K_D_IO  ,	PRMS_DRYRstCount06		,	0	} ,
	{ "PRMS_$0_DRYRstCount07"	, _K_D_IO  ,	PRMS_DRYRstCount07		,	0	} ,
	{ "PRMS_$0_DRYRstCount08"	, _K_D_IO  ,	PRMS_DRYRstCount08		,	0	} ,
	{ "PRMS_$0_DRYRstCount09"	, _K_D_IO  ,	PRMS_DRYRstCount09		,	0	} ,
	{ "PRMS_$0_DRYRstCount10"	, _K_D_IO  ,	PRMS_DRYRstCount10		,	0	} ,

	// 2006.02.20
	{ "PRMD_$0_DRYEn_04"		, _K_D_IO  ,	PRMD_DRYEn_04			,	0	} ,
	{ "PRMA_$0_DRYCThick"		, _K_A_IO  ,	PRMA_DRYCThick			,	0	} ,
	{ "PRMA_$0_DRYWThick"		, _K_A_IO  ,	PRMA_DRYWThick			,	0	} ,
	{ "PRMA_$0_DRYAThick"		, _K_A_IO  ,	PRMA_DRYAThick			,	0	} ,

	// Source Exchange
	{ "PRMD_$0_SRCEn_01"		, _K_D_IO  ,	PRMD_SRCEn_01			,	0	} ,
	{ "PRMD_$0_SRCEn_02"		, _K_D_IO  ,	PRMD_SRCEn_02			,	0	} ,
	{ "PRMD_$0_SRCEn_03"		, _K_D_IO  ,	PRMD_SRCEn_03			,	0	} ,
	{ "PRMD_$0_SRCEn_04"		, _K_D_IO  ,	PRMD_SRCEn_04			,	0	} ,
	{ "PRMD_$0_SRCAlarm"		, _K_D_IO  ,	PRMD_SRCAlarm			,	0	} ,

	{ "PRMD_$0_SRCCWfrCnt"		, _K_D_IO  ,	PRMD_SRCCWfrCnt			,	0	} ,
	{ "PRMD_$0_SRCWWfrCnt"		, _K_D_IO  ,	PRMD_SRCWWfrCnt			,	0	} ,
	{ "PRMD_$0_SRCAWfrCnt"		, _K_D_IO  ,	PRMD_SRCAWfrCnt			,	0	} ,

	{ "PRMD_$0_SRCCVlvCyc"		, _K_D_IO  ,	PRMD_SRCCVlvCyc			,	0	} ,
	{ "PRMD_$0_SRCWVlvCyc"		, _K_D_IO  ,	PRMD_SRCWVlvCyc			,	0	} ,
	{ "PRMD_$0_SRCAVlvCyc"		, _K_D_IO  ,	PRMD_SRCAVlvCyc			,	0	} ,

	{ "PRMD_$0_SRCCSrcFeed"		, _K_A_IO  ,	PRMD_SRCCSrcFeed		,	0	} ,
	{ "PRMD_$0_SRCWSrcFeed"		, _K_A_IO  ,	PRMD_SRCWSrcFeed		,	0	} ,
	{ "PRMD_$0_SRCASrcFeed"		, _K_A_IO  ,	PRMD_SRCASrcFeed		,	0	} ,

	{ "PRMD_$0_SRCCLTDay"		, _K_D_IO  ,	PRMD_SRCCLTDay			,	0	} ,
	{ "PRMD_$0_SRCCLTHour"		, _K_D_IO  ,	PRMD_SRCCLTHour			,	0	} ,
	{ "PRMD_$0_SRCCLTMin"		, _K_D_IO  ,	PRMD_SRCCLTMin			,	0	} ,
	{ "PRMD_$0_SRCCLTSec"		, _K_D_IO  ,	PRMD_SRCCLTSecond		,	0	} ,
	{ "PRMD_$0_SRCWLTDay"		, _K_D_IO  ,	PRMD_SRCWLTDay			,	0	} ,
	{ "PRMD_$0_SRCWLTHour"		, _K_D_IO  ,	PRMD_SRCWLTHour			,	0	} ,
	{ "PRMD_$0_SRCALTDay"		, _K_D_IO  ,	PRMD_SRCALTDay			,	0	} ,
	{ "PRMD_$0_SRCALTHour"		, _K_D_IO  ,	PRMD_SRCALTHour			,	0	} ,

	// Added by jonghyuk  2009.08.24
	{ "PRMD_$0_SRC2CWfrCnt"		, _K_D_IO  ,	PRMD_SRC2CWfrCnt		,	0	} ,

	{ "PRMD_$0_SRCRstCnt"		, _K_D_IO  ,	PRMD_SRCRstCnt			,	0	} ,
	{ "PRMS_$0_SRCRstTime"		, _K_S_IO  ,	PRMS_SRCRstTime			,	0	} ,
	{ "PRMS_$0_SRCRstTime01"	, _K_S_IO  ,	PRMS_SRCRstTime01		,	0	} ,
	{ "PRMS_$0_SRCRstTime02"	, _K_S_IO  ,	PRMS_SRCRstTime02		,	0	} ,
	{ "PRMS_$0_SRCRstTime03"	, _K_S_IO  ,	PRMS_SRCRstTime03		,	0	} ,
	{ "PRMS_$0_SRCRstTime04"	, _K_S_IO  ,	PRMS_SRCRstTime04		,	0	} ,
	{ "PRMS_$0_SRCRstTime05"	, _K_S_IO  ,	PRMS_SRCRstTime05		,	0	} ,
	{ "PRMS_$0_SRCRstTime06"	, _K_S_IO  ,	PRMS_SRCRstTime06		,	0	} ,
	{ "PRMS_$0_SRCRstTime07"	, _K_S_IO  ,	PRMS_SRCRstTime07		,	0	} ,
	{ "PRMS_$0_SRCRstTime08"	, _K_S_IO  ,	PRMS_SRCRstTime08		,	0	} ,
	{ "PRMS_$0_SRCRstTime09"	, _K_S_IO  ,	PRMS_SRCRstTime09		,	0	} ,
	{ "PRMS_$0_SRCRstTime10"	, _K_S_IO  ,	PRMS_SRCRstTime10		,	0	} ,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	{ "PRMS_$0_SRCRstCount01"	, _K_D_IO  ,	PRMS_SRCRstCount01		,	0	} ,
	{ "PRMS_$0_SRCRstCount02"	, _K_D_IO  ,	PRMS_SRCRstCount02		,	0	} ,
	{ "PRMS_$0_SRCRstCount03"	, _K_D_IO  ,	PRMS_SRCRstCount03		,	0	} ,
	{ "PRMS_$0_SRCRstCount04"	, _K_D_IO  ,	PRMS_SRCRstCount04		,	0	} ,
	{ "PRMS_$0_SRCRstCount05"	, _K_D_IO  ,	PRMS_SRCRstCount05		,	0	} ,
	{ "PRMS_$0_SRCRstCount06"	, _K_D_IO  ,	PRMS_SRCRstCount06		,	0	} ,
	{ "PRMS_$0_SRCRstCount07"	, _K_D_IO  ,	PRMS_SRCRstCount07		,	0	} ,
	{ "PRMS_$0_SRCRstCount08"	, _K_D_IO  ,	PRMS_SRCRstCount08		,	0	} ,
	{ "PRMS_$0_SRCRstCount09"	, _K_D_IO  ,	PRMS_SRCRstCount09		,	0	} ,
	{ "PRMS_$0_SRCRstCount10"	, _K_D_IO  ,	PRMS_SRCRstCount10		,	0	} ,

	// Stage Heater Exchange
	{ "PRMD_$0_STGEn_01"		, _K_D_IO  ,	PRMD_STGEn_01			,	0	} ,
	{ "PRMD_$0_STGEn_02"		, _K_D_IO  ,	PRMD_STGEn_02			,	0	} ,
	{ "PRMD_$0_STGAlarm"		, _K_D_IO  ,	PRMD_STGAlarm			,	0	} ,

	{ "PRMD_$0_STGCWfrCnt"		, _K_D_IO  ,	PRMD_STGCWfrCnt			,	0	} ,
	{ "PRMD_$0_STGWWfrCnt"		, _K_D_IO  ,	PRMD_STGWWfrCnt			,	0	} ,
	{ "PRMD_$0_STGAWfrCnt"		, _K_D_IO  ,	PRMD_STGAWfrCnt			,	0	} ,

	{ "PRMD_$0_STGCLTDay"		, _K_D_IO  ,	PRMD_STGCLTDay			,	0	} ,
	{ "PRMD_$0_STGCLTHour"		, _K_D_IO  ,	PRMD_STGCLTHour			,	0	} ,
	{ "PRMD_$0_STGCLTMin"		, _K_D_IO  ,	PRMD_STGCLTMin			,	0	} ,
	{ "PRMD_$0_STGCLTSec"		, _K_D_IO  ,	PRMD_STGCLTSecond		,	0	} ,
	{ "PRMD_$0_STGWLTDay"		, _K_D_IO  ,	PRMD_STGWLTDay			,	0	} ,
	{ "PRMD_$0_STGWLTHour"		, _K_D_IO  ,	PRMD_STGWLTHour			,	0	} ,
	{ "PRMD_$0_STGALTDay"		, _K_D_IO  ,	PRMD_STGALTDay			,	0	} ,
	{ "PRMD_$0_STGALTHour"		, _K_D_IO  ,	PRMD_STGALTHour			,	0	} ,

	{ "PRMD_$0_STGRstCnt"		, _K_D_IO  ,	PRMD_STGRstCnt			,	0	} ,
	{ "PRMS_$0_STGRstTime"		, _K_S_IO  ,	PRMS_STGRstTime			,	0	} ,
	{ "PRMS_$0_STGRstTime01"	, _K_S_IO  ,	PRMS_STGRstTime01		,	0	} ,
	{ "PRMS_$0_STGRstTime02"	, _K_S_IO  ,	PRMS_STGRstTime02		,	0	} ,
	{ "PRMS_$0_STGRstTime03"	, _K_S_IO  ,	PRMS_STGRstTime03		,	0	} ,
	{ "PRMS_$0_STGRstTime04"	, _K_S_IO  ,	PRMS_STGRstTime04		,	0	} ,
	{ "PRMS_$0_STGRstTime05"	, _K_S_IO  ,	PRMS_STGRstTime05		,	0	} ,
	{ "PRMS_$0_STGRstTime06"	, _K_S_IO  ,	PRMS_STGRstTime06		,	0	} ,
	{ "PRMS_$0_STGRstTime07"	, _K_S_IO  ,	PRMS_STGRstTime07		,	0	} ,
	{ "PRMS_$0_STGRstTime08"	, _K_S_IO  ,	PRMS_STGRstTime08		,	0	} ,
	{ "PRMS_$0_STGRstTime09"	, _K_S_IO  ,	PRMS_STGRstTime09		,	0	} ,
	{ "PRMS_$0_STGRstTime10"	, _K_S_IO  ,	PRMS_STGRstTime10		,	0	} ,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	{ "PRMS_$0_STGRstCount01"	, _K_D_IO  ,	PRMS_STGRstCount01		,	0	} ,
	{ "PRMS_$0_STGRstCount02"	, _K_D_IO  ,	PRMS_STGRstCount02		,	0	} ,
	{ "PRMS_$0_STGRstCount03"	, _K_D_IO  ,	PRMS_STGRstCount03		,	0	} ,
	{ "PRMS_$0_STGRstCount04"	, _K_D_IO  ,	PRMS_STGRstCount04		,	0	} ,
	{ "PRMS_$0_STGRstCount05"	, _K_D_IO  ,	PRMS_STGRstCount05		,	0	} ,
	{ "PRMS_$0_STGRstCount06"	, _K_D_IO  ,	PRMS_STGRstCount06		,	0	} ,
	{ "PRMS_$0_STGRstCount07"	, _K_D_IO  ,	PRMS_STGRstCount07		,	0	} ,
	{ "PRMS_$0_STGRstCount08"	, _K_D_IO  ,	PRMS_STGRstCount08		,	0	} ,
	{ "PRMS_$0_STGRstCount09"	, _K_D_IO  ,	PRMS_STGRstCount09		,	0	} ,
	{ "PRMS_$0_STGRstCount10"	, _K_D_IO  ,	PRMS_STGRstCount10		,	0	} ,


	// Stage Heater 2 Exchange
	{ "PRMD_$0_STG2En_01"		, _K_D_IO  ,	PRMD_STG2En_01			,	0	} ,
	{ "PRMD_$0_STG2En_02"		, _K_D_IO  ,	PRMD_STG2En_02			,	0	} ,
	{ "PRMD_$0_STG2Alarm"		, _K_D_IO  ,	PRMD_STG2Alarm			,	0	} ,

	{ "PRMD_$0_STG2CWfrCnt"		, _K_D_IO  ,	PRMD_STG2CWfrCnt		,	0	} ,
	{ "PRMD_$0_STG2WWfrCnt"		, _K_D_IO  ,	PRMD_STG2WWfrCnt		,	0	} ,
	{ "PRMD_$0_STG2AWfrCnt"		, _K_D_IO  ,	PRMD_STG2AWfrCnt		,	0	} ,

	{ "PRMD_$0_STG2CLTDay"		, _K_D_IO  ,	PRMD_STG2CLTDay			,	0	} ,
	{ "PRMD_$0_STG2CLTHour"		, _K_D_IO  ,	PRMD_STG2CLTHour		,	0	} ,
	{ "PRMD_$0_STG2CLTMin"		, _K_D_IO  ,	PRMD_STG2CLTMin			,	0	} ,
	{ "PRMD_$0_STG2CLTSec"		, _K_D_IO  ,	PRMD_STG2CLTSecond		,	0	} ,
	{ "PRMD_$0_STG2WLTDay"		, _K_D_IO  ,	PRMD_STG2WLTDay			,	0	} ,
	{ "PRMD_$0_STG2WLTHour"		, _K_D_IO  ,	PRMD_STG2WLTHour		,	0	} ,
	{ "PRMD_$0_STG2ALTDay"		, _K_D_IO  ,	PRMD_STG2ALTDay			,	0	} ,
	{ "PRMD_$0_STG2ALTHour"		, _K_D_IO  ,	PRMD_STG2ALTHour		,	0	} ,

	{ "PRMD_$0_STG2RstCnt"		, _K_D_IO  ,	PRMD_STG2RstCnt			,	0	} ,
	{ "PRMS_$0_STG2RstTime"		, _K_S_IO  ,	PRMS_STG2RstTime		,	0	} ,
	{ "PRMS_$0_STG2RstTime01"	, _K_S_IO  ,	PRMS_STG2RstTime01		,	0	} ,
	{ "PRMS_$0_STG2RstTime02"	, _K_S_IO  ,	PRMS_STG2RstTime02		,	0	} ,
	{ "PRMS_$0_STG2RstTime03"	, _K_S_IO  ,	PRMS_STG2RstTime03		,	0	} ,
	{ "PRMS_$0_STG2RstTime04"	, _K_S_IO  ,	PRMS_STG2RstTime04		,	0	} ,
	{ "PRMS_$0_STG2RstTime05"	, _K_S_IO  ,	PRMS_STG2RstTime05		,	0	} ,
	{ "PRMS_$0_STG2RstTime06"	, _K_S_IO  ,	PRMS_STG2RstTime06		,	0	} ,
	{ "PRMS_$0_STG2RstTime07"	, _K_S_IO  ,	PRMS_STG2RstTime07		,	0	} ,
	{ "PRMS_$0_STG2RstTime08"	, _K_S_IO  ,	PRMS_STG2RstTime08		,	0	} ,
	{ "PRMS_$0_STG2RstTime09"	, _K_S_IO  ,	PRMS_STG2RstTime09		,	0	} ,
	{ "PRMS_$0_STG2RstTime10"	, _K_S_IO  ,	PRMS_STG2RstTime10		,	0	} ,

	{ "PRMS_$0_STG2RstCount01"	, _K_D_IO  ,	PRMS_STG2RstCount01		,	0	} ,
	{ "PRMS_$0_STG2RstCount02"	, _K_D_IO  ,	PRMS_STG2RstCount02		,	0	} ,
	{ "PRMS_$0_STG2RstCount03"	, _K_D_IO  ,	PRMS_STG2RstCount03		,	0	} ,
	{ "PRMS_$0_STG2RstCount04"	, _K_D_IO  ,	PRMS_STG2RstCount04		,	0	} ,
	{ "PRMS_$0_STG2RstCount05"	, _K_D_IO  ,	PRMS_STG2RstCount05		,	0	} ,
	{ "PRMS_$0_STG2RstCount06"	, _K_D_IO  ,	PRMS_STG2RstCount06		,	0	} ,
	{ "PRMS_$0_STG2RstCount07"	, _K_D_IO  ,	PRMS_STG2RstCount07		,	0	} ,
	{ "PRMS_$0_STG2RstCount08"	, _K_D_IO  ,	PRMS_STG2RstCount08		,	0	} ,
	{ "PRMS_$0_STG2RstCount09"	, _K_D_IO  ,	PRMS_STG2RstCount09		,	0	} ,
	{ "PRMS_$0_STG2RstCount10"	, _K_D_IO  ,	PRMS_STG2RstCount10		,	0	} ,

	// RF Generator ON Time
	{ "PRMD_$0_RFGEn_01"		, _K_D_IO  ,	PRMD_RFGEn_01			,	0	} ,
	{ "PRMD_$0_RFGAlarm"		, _K_D_IO  ,	PRMD_RFGAlarm			,	0	} ,

	{ "PRMD_$0_RFGCLTDay"		, _K_D_IO  ,	PRMD_RFGCLTDay			,	0	} ,
	{ "PRMD_$0_RFGCLTHour"		, _K_D_IO  ,	PRMD_RFGCLTHour			,	0	} ,
	{ "PRMD_$0_RFGCLTMin"		, _K_D_IO  ,	PRMD_RFGCLTMin			,	0	} ,
	{ "PRMD_$0_RFGCLTSec"		, _K_D_IO  ,	PRMD_RFGCLTSecond		,	0	} ,
	{ "PRMD_$0_RFGWLTDay"		, _K_D_IO  ,	PRMD_RFGWLTDay			,	0	} ,
	{ "PRMD_$0_RFGWLTHour"		, _K_D_IO  ,	PRMD_RFGWLTHour			,	0	} ,
	{ "PRMD_$0_RFGALTDay"		, _K_D_IO  ,	PRMD_RFGALTDay			,	0	} ,
	{ "PRMD_$0_RFGALTHour"		, _K_D_IO  ,	PRMD_RFGALTHour			,	0	} ,

	{ "PRMD_$0_RFGRstCnt"		, _K_D_IO  ,	PRMD_RFGRstCnt			,	0	} ,
	{ "PRMS_$0_RFGRstTime"		, _K_S_IO  ,	PRMS_RFGRstTime			,	0	} ,
	{ "PRMS_$0_RFGRstTime01"	, _K_S_IO  ,	PRMS_RFGRstTime01		,	0	} ,
	{ "PRMS_$0_RFGRstTime02"	, _K_S_IO  ,	PRMS_RFGRstTime02		,	0	} ,
	{ "PRMS_$0_RFGRstTime03"	, _K_S_IO  ,	PRMS_RFGRstTime03		,	0	} ,
	{ "PRMS_$0_RFGRstTime04"	, _K_S_IO  ,	PRMS_RFGRstTime04		,	0	} ,
	{ "PRMS_$0_RFGRstTime05"	, _K_S_IO  ,	PRMS_RFGRstTime05		,	0	} ,
	{ "PRMS_$0_RFGRstTime06"	, _K_S_IO  ,	PRMS_RFGRstTime06		,	0	} ,
	{ "PRMS_$0_RFGRstTime07"	, _K_S_IO  ,	PRMS_RFGRstTime07		,	0	} ,
	{ "PRMS_$0_RFGRstTime08"	, _K_S_IO  ,	PRMS_RFGRstTime08		,	0	} ,
	{ "PRMS_$0_RFGRstTime09"	, _K_S_IO  ,	PRMS_RFGRstTime09		,	0	} ,
	{ "PRMS_$0_RFGRstTime10"	, _K_S_IO  ,	PRMS_RFGRstTime10		,	0	} ,

	// DRY Pump Exchange
	{ "PRMD_$0_DPEn_01"			, _K_D_IO  ,	PRMD_DPEn_01			,	0	} ,
	{ "PRMD_$0_DPEn_02"			, _K_D_IO  ,	PRMD_DPEn_02			,	0	} ,
	{ "PRMD_$0_DPAlarm"			, _K_D_IO  ,	PRMD_DPAlarm			,	0	} ,

	{ "PRMD_$0_DPCWfrCnt"		, _K_D_IO  ,	PRMD_DPCWfrCnt			,	0	} ,
	{ "PRMD_$0_DPWWfrCnt"		, _K_D_IO  ,	PRMD_DPWWfrCnt			,	0	} ,
	{ "PRMD_$0_DPAWfrCnt"		, _K_D_IO  ,	PRMD_DPAWfrCnt			,	0	} ,

	{ "PRMD_$0_DPCLTDay"		, _K_D_IO  ,	PRMD_DPCLTDay			,	0	} ,
	{ "PRMD_$0_DPCLTHour"		, _K_D_IO  ,	PRMD_DPCLTHour			,	0	} ,
	{ "PRMD_$0_DPCLTMin"		, _K_D_IO  ,	PRMD_DPCLTMin			,	0	} ,
	{ "PRMD_$0_DPCLTSec"		, _K_D_IO  ,	PRMD_DPCLTSecond		,	0	} ,
	{ "PRMD_$0_DPWLTDay"		, _K_D_IO  ,	PRMD_DPWLTDay			,	0	} ,
	{ "PRMD_$0_DPWLTHour"		, _K_D_IO  ,	PRMD_DPWLTHour			,	0	} ,
	{ "PRMD_$0_DPALTDay"		, _K_D_IO  ,	PRMD_DPALTDay			,	0	} ,
	{ "PRMD_$0_DPALTHour"		, _K_D_IO  ,	PRMD_DPALTHour			,	0	} ,

	{ "PRMD_$0_DPRstCnt"		, _K_D_IO  ,	PRMD_DPRstCnt			,	0	} ,
	{ "PRMS_$0_DPRstTime"		, _K_S_IO  ,	PRMS_DPRstTime			,	0	} ,
	{ "PRMS_$0_DPRstTime01"		, _K_S_IO  ,	PRMS_DPRstTime01		,	0	} ,
	{ "PRMS_$0_DPRstTime02"		, _K_S_IO  ,	PRMS_DPRstTime02		,	0	} ,
	{ "PRMS_$0_DPRstTime03"		, _K_S_IO  ,	PRMS_DPRstTime03		,	0	} ,
	{ "PRMS_$0_DPRstTime04"		, _K_S_IO  ,	PRMS_DPRstTime04		,	0	} ,
	{ "PRMS_$0_DPRstTime05"		, _K_S_IO  ,	PRMS_DPRstTime05		,	0	} ,
	{ "PRMS_$0_DPRstTime06"		, _K_S_IO  ,	PRMS_DPRstTime06		,	0	} ,
	{ "PRMS_$0_DPRstTime07"		, _K_S_IO  ,	PRMS_DPRstTime07		,	0	} ,
	{ "PRMS_$0_DPRstTime08"		, _K_S_IO  ,	PRMS_DPRstTime08		,	0	} ,
	{ "PRMS_$0_DPRstTime09"		, _K_S_IO  ,	PRMS_DPRstTime09		,	0	} ,
	{ "PRMS_$0_DPRstTime10"		, _K_S_IO  ,	PRMS_DPRstTime10		,	0	} ,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	{ "PRMS_$0_DPRstCount01"		, _K_D_IO  ,	PRMS_DPRstCount01		,	0	} ,
	{ "PRMS_$0_DPRstCount02"		, _K_D_IO  ,	PRMS_DPRstCount02		,	0	} ,
	{ "PRMS_$0_DPRstCount03"		, _K_D_IO  ,	PRMS_DPRstCount03		,	0	} ,
	{ "PRMS_$0_DPRstCount04"		, _K_D_IO  ,	PRMS_DPRstCount04		,	0	} ,
	{ "PRMS_$0_DPRstCount05"		, _K_D_IO  ,	PRMS_DPRstCount05		,	0	} ,
	{ "PRMS_$0_DPRstCount06"		, _K_D_IO  ,	PRMS_DPRstCount06		,	0	} ,
	{ "PRMS_$0_DPRstCount07"		, _K_D_IO  ,	PRMS_DPRstCount07		,	0	} ,
	{ "PRMS_$0_DPRstCount08"		, _K_D_IO  ,	PRMS_DPRstCount08		,	0	} ,
	{ "PRMS_$0_DPRstCount09"		, _K_D_IO  ,	PRMS_DPRstCount09		,	0	} ,
	{ "PRMS_$0_DPRstCount10"		, _K_D_IO  ,	PRMS_DPRstCount10		,	0	} ,

	// TRAP Exchange
	{ "PRMD_$0_TRAPEn_01"		, _K_D_IO  ,	PRMD_TRAPEn_01			,	0	} ,
	{ "PRMD_$0_TRAPEn_02"		, _K_D_IO  ,	PRMD_TRAPEn_02			,	0	} ,
	{ "PRMD_$0_TRAPAlarm"		, _K_D_IO  ,	PRMD_TRAPAlarm			,	0	} ,

	{ "PRMD_$0_TRAPCWfrCnt"		, _K_D_IO  ,	PRMD_TRAPCWfrCnt		,	0	} ,
	{ "PRMD_$0_TRAPWWfrCnt"		, _K_D_IO  ,	PRMD_TRAPWWfrCnt		,	0	} ,
	{ "PRMD_$0_TRAPAWfrCnt"		, _K_D_IO  ,	PRMD_TRAPAWfrCnt		,	0	} ,

	{ "PRMD_$0_TRAPCLTDay"		, _K_D_IO  ,	PRMD_TRAPCLTDay			,	0	} ,
	{ "PRMD_$0_TRAPCLTHour"		, _K_D_IO  ,	PRMD_TRAPCLTHour		,	0	} ,
	{ "PRMD_$0_TRAPCLTMin"		, _K_D_IO  ,	PRMD_TRAPCLTMin			,	0	} ,
	{ "PRMD_$0_TRAPCLTSec"		, _K_D_IO  ,	PRMD_TRAPCLTSecond		,	0	} ,
	{ "PRMD_$0_TRAPWLTDay"		, _K_D_IO  ,	PRMD_TRAPWLTDay			,	0	} ,
	{ "PRMD_$0_TRAPWLTHour"		, _K_D_IO  ,	PRMD_TRAPWLTHour		,	0	} ,
	{ "PRMD_$0_TRAPALTDay"		, _K_D_IO  ,	PRMD_TRAPALTDay			,	0	} ,
	{ "PRMD_$0_TRAPALTHour"		, _K_D_IO  ,	PRMD_TRAPALTHour		,	0	} ,

	{ "PRMD_$0_TRAPRstCnt"		, _K_D_IO  ,	PRMD_TRAPRstCnt			,	0	} ,
	{ "PRMS_$0_TRAPRstTime"		, _K_S_IO  ,	PRMS_TRAPRstTime		,	0	} ,
	{ "PRMS_$0_TRAPRstTime01"	, _K_S_IO  ,	PRMS_TRAPRstTime01		,	0	} ,
	{ "PRMS_$0_TRAPRstTime02"	, _K_S_IO  ,	PRMS_TRAPRstTime02		,	0	} ,
	{ "PRMS_$0_TRAPRstTime03"	, _K_S_IO  ,	PRMS_TRAPRstTime03		,	0	} ,
	{ "PRMS_$0_TRAPRstTime04"	, _K_S_IO  ,	PRMS_TRAPRstTime04		,	0	} ,
	{ "PRMS_$0_TRAPRstTime05"	, _K_S_IO  ,	PRMS_TRAPRstTime05		,	0	} ,
	{ "PRMS_$0_TRAPRstTime06"	, _K_S_IO  ,	PRMS_TRAPRstTime06		,	0	} ,
	{ "PRMS_$0_TRAPRstTime07"	, _K_S_IO  ,	PRMS_TRAPRstTime07		,	0	} ,
	{ "PRMS_$0_TRAPRstTime08"	, _K_S_IO  ,	PRMS_TRAPRstTime08		,	0	} ,
	{ "PRMS_$0_TRAPRstTime09"	, _K_S_IO  ,	PRMS_TRAPRstTime09		,	0	} ,
	{ "PRMS_$0_TRAPRstTime10"	, _K_S_IO  ,	PRMS_TRAPRstTime10		,	0	} ,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	{ "PRMS_$0_TRAPRstCount01"	, _K_D_IO  ,	PRMS_TRAPRstCount01		,	0	} ,
	{ "PRMS_$0_TRAPRstCount02"	, _K_D_IO  ,	PRMS_TRAPRstCount02		,	0	} ,
	{ "PRMS_$0_TRAPRstCount03"	, _K_D_IO  ,	PRMS_TRAPRstCount03		,	0	} ,
	{ "PRMS_$0_TRAPRstCount04"	, _K_D_IO  ,	PRMS_TRAPRstCount04		,	0	} ,
	{ "PRMS_$0_TRAPRstCount05"	, _K_D_IO  ,	PRMS_TRAPRstCount05		,	0	} ,
	{ "PRMS_$0_TRAPRstCount06"	, _K_D_IO  ,	PRMS_TRAPRstCount06		,	0	} ,
	{ "PRMS_$0_TRAPRstCount07"	, _K_D_IO  ,	PRMS_TRAPRstCount07		,	0	} ,
	{ "PRMS_$0_TRAPRstCount08"	, _K_D_IO  ,	PRMS_TRAPRstCount08		,	0	} ,
	{ "PRMS_$0_TRAPRstCount09"	, _K_D_IO  ,	PRMS_TRAPRstCount09		,	0	} ,
	{ "PRMS_$0_TRAPRstCount10"	, _K_D_IO  ,	PRMS_TRAPRstCount10		,	0	} ,

	// Maintenance Period A
	{ "PRMD_$0_MPAEn_01"		, _K_D_IO  ,	PRMD_MPAEn_01			,	0	} ,
	{ "PRMD_$0_MPAEn_02"		, _K_D_IO  ,	PRMD_MPAEn_02			,	0	} ,
	{ "PRMD_$0_MPAAlarm"		, _K_D_IO  ,	PRMD_MPAAlarm			,	0	} ,

	{ "PRMD_$0_MPACWfrCnt"		, _K_D_IO  ,	PRMD_MPACWfrCnt			,	0	} ,
	{ "PRMD_$0_MPAWWfrCnt"		, _K_D_IO  ,	PRMD_MPAWWfrCnt			,	0	} ,
	{ "PRMD_$0_MPAAWfrCnt"		, _K_D_IO  ,	PRMD_MPAAWfrCnt			,	0	} ,

	{ "PRMD_$0_MPACLTDay"		, _K_D_IO  ,	PRMD_MPACLTDay			,	0	} ,
	{ "PRMD_$0_MPACLTHour"		, _K_D_IO  ,	PRMD_MPACLTHour			,	0	} ,
	{ "PRMD_$0_MPACLTMin"		, _K_D_IO  ,	PRMD_MPACLTMin			,	0	} ,
	{ "PRMD_$0_MPACLTSec"		, _K_D_IO  ,	PRMD_MPACLTSecond		,	0	} ,
	{ "PRMD_$0_MPAWLTDay"		, _K_D_IO  ,	PRMD_MPAWLTDay			,	0	} ,
	{ "PRMD_$0_MPAWLTHour"		, _K_D_IO  ,	PRMD_MPAWLTHour			,	0	} ,
	{ "PRMD_$0_MPAALTDay"		, _K_D_IO  ,	PRMD_MPAALTDay			,	0	} ,
	{ "PRMD_$0_MPAALTHour"		, _K_D_IO  ,	PRMD_MPAALTHour			,	0	} ,

	{ "PRMD_$0_MPARstCnt"		, _K_D_IO  ,	PRMD_MPARstCnt			,	0	} ,
	{ "PRMS_$0_MPARstTime"		, _K_S_IO  ,	PRMS_MPARstTime			,	0	} ,
	{ "PRMS_$0_MPARstTime01"	, _K_S_IO  ,	PRMS_MPARstTime01		,	0	} ,
	{ "PRMS_$0_MPARstTime02"	, _K_S_IO  ,	PRMS_MPARstTime02		,	0	} ,
	{ "PRMS_$0_MPARstTime03"	, _K_S_IO  ,	PRMS_MPARstTime03		,	0	} ,
	{ "PRMS_$0_MPARstTime04"	, _K_S_IO  ,	PRMS_MPARstTime04		,	0	} ,
	{ "PRMS_$0_MPARstTime05"	, _K_S_IO  ,	PRMS_MPARstTime05		,	0	} ,
	{ "PRMS_$0_MPARstTime06"	, _K_S_IO  ,	PRMS_MPARstTime06		,	0	} ,
	{ "PRMS_$0_MPARstTime07"	, _K_S_IO  ,	PRMS_MPARstTime07		,	0	} ,
	{ "PRMS_$0_MPARstTime08"	, _K_S_IO  ,	PRMS_MPARstTime08		,	0	} ,
	{ "PRMS_$0_MPARstTime09"	, _K_S_IO  ,	PRMS_MPARstTime09		,	0	} ,
	{ "PRMS_$0_MPARstTime10"	, _K_S_IO  ,	PRMS_MPARstTime10		,	0	} ,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	{ "PRMS_$0_MPARstCount01"	, _K_D_IO  ,	PRMS_MPARstCount01		,	0	} ,
	{ "PRMS_$0_MPARstCount02"	, _K_D_IO  ,	PRMS_MPARstCount02		,	0	} ,
	{ "PRMS_$0_MPARstCount03"	, _K_D_IO  ,	PRMS_MPARstCount03		,	0	} ,
	{ "PRMS_$0_MPARstCount04"	, _K_D_IO  ,	PRMS_MPARstCount04		,	0	} ,
	{ "PRMS_$0_MPARstCount05"	, _K_D_IO  ,	PRMS_MPARstCount05		,	0	} ,
	{ "PRMS_$0_MPARstCount06"	, _K_D_IO  ,	PRMS_MPARstCount06		,	0	} ,
	{ "PRMS_$0_MPARstCount07"	, _K_D_IO  ,	PRMS_MPARstCount07		,	0	} ,
	{ "PRMS_$0_MPARstCount08"	, _K_D_IO  ,	PRMS_MPARstCount08		,	0	} ,
	{ "PRMS_$0_MPARstCount09"	, _K_D_IO  ,	PRMS_MPARstCount09		,	0	} ,
	{ "PRMS_$0_MPARstCount10"	, _K_D_IO  ,	PRMS_MPARstCount10		,	0	} ,

	// Maintenance Period B
	{ "PRMD_$0_MPBEn_01"		, _K_D_IO  ,	PRMD_MPBEn_01			,	0	} ,
	{ "PRMD_$0_MPBEn_02"		, _K_D_IO  ,	PRMD_MPBEn_02			,	0	} ,
	{ "PRMD_$0_MPBAlarm"		, _K_D_IO  ,	PRMD_MPBAlarm			,	0	} ,

	{ "PRMD_$0_MPBCWfrCnt"		, _K_D_IO  ,	PRMD_MPBCWfrCnt			,	0	} ,
	{ "PRMD_$0_MPBWWfrCnt"		, _K_D_IO  ,	PRMD_MPBWWfrCnt			,	0	} ,
	{ "PRMD_$0_MPBAWfrCnt"		, _K_D_IO  ,	PRMD_MPBAWfrCnt			,	0	} ,

	{ "PRMD_$0_MPBCLTDay"		, _K_D_IO  ,	PRMD_MPBCLTDay			,	0	} ,
	{ "PRMD_$0_MPBCLTHour"		, _K_D_IO  ,	PRMD_MPBCLTHour			,	0	} ,
	{ "PRMD_$0_MPBCLTMin"		, _K_D_IO  ,	PRMD_MPBCLTMin			,	0	} ,
	{ "PRMD_$0_MPBCLTSec"		, _K_D_IO  ,	PRMD_MPBCLTSecond		,	0	} ,
	{ "PRMD_$0_MPBWLTDay"		, _K_D_IO  ,	PRMD_MPBWLTDay			,	0	} ,
	{ "PRMD_$0_MPBWLTHour"		, _K_D_IO  ,	PRMD_MPBWLTHour			,	0	} ,
	{ "PRMD_$0_MPBALTDay"		, _K_D_IO  ,	PRMD_MPBALTDay			,	0	} ,
	{ "PRMD_$0_MPBALTHour"		, _K_D_IO  ,	PRMD_MPBALTHour			,	0	} ,

	{ "PRMD_$0_MPBRstCnt"		, _K_D_IO  ,	PRMD_MPBRstCnt			,	0	} ,
	{ "PRMS_$0_MPBRstTime"		, _K_S_IO  ,	PRMS_MPBRstTime			,	0	} ,
	{ "PRMS_$0_MPBRstTime01"	, _K_S_IO  ,	PRMS_MPBRstTime01		,	0	} ,
	{ "PRMS_$0_MPBRstTime02"	, _K_S_IO  ,	PRMS_MPBRstTime02		,	0	} ,
	{ "PRMS_$0_MPBRstTime03"	, _K_S_IO  ,	PRMS_MPBRstTime03		,	0	} ,
	{ "PRMS_$0_MPBRstTime04"	, _K_S_IO  ,	PRMS_MPBRstTime04		,	0	} ,
	{ "PRMS_$0_MPBRstTime05"	, _K_S_IO  ,	PRMS_MPBRstTime05		,	0	} ,
	{ "PRMS_$0_MPBRstTime06"	, _K_S_IO  ,	PRMS_MPBRstTime06		,	0	} ,
	{ "PRMS_$0_MPBRstTime07"	, _K_S_IO  ,	PRMS_MPBRstTime07		,	0	} ,
	{ "PRMS_$0_MPBRstTime08"	, _K_S_IO  ,	PRMS_MPBRstTime08		,	0	} ,
	{ "PRMS_$0_MPBRstTime09"	, _K_S_IO  ,	PRMS_MPBRstTime09		,	0	} ,
	{ "PRMS_$0_MPBRstTime10"	, _K_S_IO  ,	PRMS_MPBRstTime10		,	0	} ,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	{ "PRMS_$0_MPBRstCount01"	, _K_D_IO  ,	PRMS_MPBRstCount01		,	0	} ,
	{ "PRMS_$0_MPBRstCount02"	, _K_D_IO  ,	PRMS_MPBRstCount02		,	0	} ,
	{ "PRMS_$0_MPBRstCount03"	, _K_D_IO  ,	PRMS_MPBRstCount03		,	0	} ,
	{ "PRMS_$0_MPBRstCount04"	, _K_D_IO  ,	PRMS_MPBRstCount04		,	0	} ,
	{ "PRMS_$0_MPBRstCount05"	, _K_D_IO  ,	PRMS_MPBRstCount05		,	0	} ,
	{ "PRMS_$0_MPBRstCount06"	, _K_D_IO  ,	PRMS_MPBRstCount06		,	0	} ,
	{ "PRMS_$0_MPBRstCount07"	, _K_D_IO  ,	PRMS_MPBRstCount07		,	0	} ,
	{ "PRMS_$0_MPBRstCount08"	, _K_D_IO  ,	PRMS_MPBRstCount08		,	0	} ,
	{ "PRMS_$0_MPBRstCount09"	, _K_D_IO  ,	PRMS_MPBRstCount09		,	0	} ,
	{ "PRMS_$0_MPBRstCount10"	, _K_D_IO  ,	PRMS_MPBRstCount10		,	0	} ,

	// Maintenance Period C
	{ "PRMD_$0_MPCEn_01"		, _K_D_IO  ,	PRMD_MPCEn_01			,	0	} ,
	{ "PRMD_$0_MPCEn_02"		, _K_D_IO  ,	PRMD_MPCEn_02			,	0	} ,
	{ "PRMD_$0_MPCAlarm"		, _K_D_IO  ,	PRMD_MPCAlarm			,	0	} ,

	{ "PRMD_$0_MPCCWfrCnt"		, _K_D_IO  ,	PRMD_MPCCWfrCnt			,	0	} ,
	{ "PRMD_$0_MPCWWfrCnt"		, _K_D_IO  ,	PRMD_MPCWWfrCnt			,	0	} ,
	{ "PRMD_$0_MPCAWfrCnt"		, _K_D_IO  ,	PRMD_MPCAWfrCnt			,	0	} ,

	{ "PRMD_$0_MPCCLTDay"		, _K_D_IO  ,	PRMD_MPCCLTDay			,	0	} ,
	{ "PRMD_$0_MPCCLTHour"		, _K_D_IO  ,	PRMD_MPCCLTHour			,	0	} ,
	{ "PRMD_$0_MPCCLTMin"		, _K_D_IO  ,	PRMD_MPCCLTMin			,	0	} ,
	{ "PRMD_$0_MPCCLTSec"		, _K_D_IO  ,	PRMD_MPCCLTSecond		,	0	} ,
	{ "PRMD_$0_MPCWLTDay"		, _K_D_IO  ,	PRMD_MPCWLTDay			,	0	} ,
	{ "PRMD_$0_MPCWLTHour"		, _K_D_IO  ,	PRMD_MPCWLTHour			,	0	} ,
	{ "PRMD_$0_MPCALTDay"		, _K_D_IO  ,	PRMD_MPCALTDay			,	0	} ,
	{ "PRMD_$0_MPCALTHour"		, _K_D_IO  ,	PRMD_MPCALTHour			,	0	} ,

	{ "PRMD_$0_MPCRstCnt"		, _K_D_IO  ,	PRMD_MPCRstCnt			,	0	} ,
	{ "PRMS_$0_MPCRstTime"		, _K_S_IO  ,	PRMS_MPCRstTime			,	0	} ,
	{ "PRMS_$0_MPCRstTime01"	, _K_S_IO  ,	PRMS_MPCRstTime01		,	0	} ,
	{ "PRMS_$0_MPCRstTime02"	, _K_S_IO  ,	PRMS_MPCRstTime02		,	0	} ,
	{ "PRMS_$0_MPCRstTime03"	, _K_S_IO  ,	PRMS_MPCRstTime03		,	0	} ,
	{ "PRMS_$0_MPCRstTime04"	, _K_S_IO  ,	PRMS_MPCRstTime04		,	0	} ,
	{ "PRMS_$0_MPCRstTime05"	, _K_S_IO  ,	PRMS_MPCRstTime05		,	0	} ,
	{ "PRMS_$0_MPCRstTime06"	, _K_S_IO  ,	PRMS_MPCRstTime06		,	0	} ,
	{ "PRMS_$0_MPCRstTime07"	, _K_S_IO  ,	PRMS_MPCRstTime07		,	0	} ,
	{ "PRMS_$0_MPCRstTime08"	, _K_S_IO  ,	PRMS_MPCRstTime08		,	0	} ,
	{ "PRMS_$0_MPCRstTime09"	, _K_S_IO  ,	PRMS_MPCRstTime09		,	0	} ,
	{ "PRMS_$0_MPCRstTime10"	, _K_S_IO  ,	PRMS_MPCRstTime10		,	0	} ,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	{ "PRMS_$0_MPCRstCount01"	, _K_D_IO  ,	PRMS_MPCRstCount01		,	0	} ,
	{ "PRMS_$0_MPCRstCount02"	, _K_D_IO  ,	PRMS_MPCRstCount02		,	0	} ,
	{ "PRMS_$0_MPCRstCount03"	, _K_D_IO  ,	PRMS_MPCRstCount03		,	0	} ,
	{ "PRMS_$0_MPCRstCount04"	, _K_D_IO  ,	PRMS_MPCRstCount04		,	0	} ,
	{ "PRMS_$0_MPCRstCount05"	, _K_D_IO  ,	PRMS_MPCRstCount05		,	0	} ,
	{ "PRMS_$0_MPCRstCount06"	, _K_D_IO  ,	PRMS_MPCRstCount06		,	0	} ,
	{ "PRMS_$0_MPCRstCount07"	, _K_D_IO  ,	PRMS_MPCRstCount07		,	0	} ,
	{ "PRMS_$0_MPCRstCount08"	, _K_D_IO  ,	PRMS_MPCRstCount08		,	0	} ,
	{ "PRMS_$0_MPCRstCount09"	, _K_D_IO  ,	PRMS_MPCRstCount09		,	0	} ,
	{ "PRMS_$0_MPCRstCount10"	, _K_D_IO  ,	PRMS_MPCRstCount10		,	0	} ,
	{ "$0.PRO_RcpTypeDM"	    , _K_D_IO  ,	PRO_RcpTypeDM		    ,	0	} ,
	""
};

//--------------------------------------------------------------------
// 2006.02.20
int g_nPMNumber = 0;

//====================================================================
extern "C"
{

Module_Status Program_Main()
{
	g_pCPreventiveMaintenance_Client->MsgParser(PROGRAM_PARAMETER_READ());
	return SYS_SUCCESS;
}

void Screen_Nav_Info( int no )
{
	if(no == -1)
	{
		// Deinitialize
		// 2006.02.20
		if (NULL != g_pCPreventiveMaintenance_Client) delete g_pCPreventiveMaintenance_Client;
		if(NULL != g_pPrevMaintMonitor) delete g_pPrevMaintMonitor;
	}
}

BOOL Program_Init_Code()
{
	// Initialize
	#ifdef _DEBUG
	printf("-----> Init Started in Program_Init_Code\n");
	#endif

	// 2006.02.20
	if(NULL == g_pPrevMaintMonitor)
	{
		g_pPrevMaintMonitor = new CPrevMaintMonitor;
		g_pPrevMaintMonitor->Initialize(g_nPMNumber);
	}

	if(NULL == g_pCPreventiveMaintenance_Client)
	{
		// 2006.10.18
		switch(g_nCleanType)
		{
		case CType1_MAINT_CLEAN:
			g_pCPreventiveMaintenance_Client = new CPreventiveMaintenance_Client(g_bRFMonitoring); break;
		case CType2_ONLINE_CLEAN:
			g_pCPreventiveMaintenance_Client = new CPreventiveMaintenance_Client_Type2(g_bRFMonitoring); break;
		}
	}

	if(NULL == g_pCPreventiveMaintenance_Client || NULL == g_pPrevMaintMonitor)
	{
		#ifdef _DEBUG
		printf("-----> Init Failed in Program_Init_Code\n");
		#endif
		return FALSE;
	}


	#ifdef _DEBUG
	printf("-----> Init Finished in Program_Init_Code\n");
	#endif

	return TRUE;
}

void Event_Message_Received()
{
}

void Program_Enter_Code()
{
	char szTemp[40];
	CTextParser txtP;

	// 2006.10.18
	txtP.SetText(PROGRAM_ARGUMENT_READ());
	txtP.GetWord(szTemp);
	if(strncmp(szTemp, "TYPE", 4) == 0)
	{
		if(szTemp[4] == '1') g_nCleanType = CType1_MAINT_CLEAN;
		else if(szTemp[4] == '2') g_nCleanType = CType2_ONLINE_CLEAN;
		txtP.GetWord(szTemp);
	}
	REPLACE_CHAR_SET(0, szTemp);
	g_nPMNumber = szTemp[2]-'0';
	txtP.GetInt(&g_nALARM_BASE);
	if(txtP.GetWord(szTemp))
	{
		if (string(szTemp) == "RF") g_bRFMonitoring = TRUE;
	}

	#ifdef _DEBUG
	printf("-----> CleanType:%d, PM:%d, AlarmBase:%d\n", g_nCleanType, g_nPMNumber, g_nALARM_BASE);
	#endif

	/*
	char szTemp1[100], szTemp2[100];
	string strArgument = PROGRAM_ARGUMENT_READ();
	int nCountResult = count(strArgument.begin(), strArgument.end(), ' ');
	if (1 == nCountResult)
	{
		sscanf(strArgument.c_str(), "%s %d", szTemp1, &g_nALARM_BASE);
		REPLACE_CHAR_SET(0, (char*)string(szTemp1).c_str());
	}
	else if (2 == nCountResult)
	{
		sscanf(strArgument.c_str(), "%s %d %s", szTemp1, &g_nALARM_BASE, szTemp2);
		REPLACE_CHAR_SET(0, (char*)string(szTemp1).c_str());
		if (string(szTemp2) == "RF")
			g_bRFMonitoring = TRUE;
	}

	// 2006.02.20
	g_nPMNumber = szTemp[2]-'0';
	#ifdef _DEBUG
	printf("PM Number = %d\n", g_nPMNumber);
	#endif
	*/
}

} // End of [extern "C"]
//====================================================================
