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

//------------------------------------------------------------------------------------------
// Component type enumeration
enum
{
	// Composite type
	INVALID_TYPE = 0	,
	WET_CLEANING_COMPOSITE_TYPE,
	DRY_CLEANING_COMPOSITE_TYPE	,
	SOURCE_EXCHANGE_COMPOSITE_TYPE	,
	STAGE_HEATER_EXCHANGE_COMPOSITE_TYPE	,
	RF_GENERATOR_COMPOSITE_TYPE	,
	DRY_PUMP_EXCHANGE_COMPOSITE_TYPE	,
	TRAP_EXCHANGE_COMPOSITE_TYPE	,
	MAINTENANCE_PERIOD_A_EXCHANGE_COMPOSITE_TYPE	,
	MAINTENANCE_PERIOD_B_EXCHANGE_COMPOSITE_TYPE	,
	MAINTENANCE_PERIOD_C_EXCHANGE_COMPOSITE_TYPE	,
	STAGE_HEATER2_EXCHANGE_COMPOSITE_TYPE
};
enum
{
	// Leaf type
	WAFER_COUNT_LEAF_TYPE = 1	,
	VALVE_CYCLE_COUNT_LEAF_TYPE	,
	SOURCE_FEEDING_AMOUNT_LEAF_TYPE	,
	LIFE_TIME_LEAF_TYPE	,
	RF_GENERATOR_ON_LEAF_TYPE,
	THICKNESS_AMOUNT_LEAF_TYPE,				// 2006.02.20
};

//------------------------------------------------------------------------------------------
// Enumeration list
enum {__INITIAL_STATE = -1};
enum {__PROCESS_READY = 0, __PROCESS_COMPLETED};
enum {__RF_OFF = 0, __RF_ON};
enum { NORWARALA_NORMAL, NORWARALA_WARNING, NORWARALA_ALARM };

//------------------------------------------------------------------------------------------

extern int g_nALARM_BASE;
// WET cleaning
#define WARNING__WET_CLEANING__WAFER_COUNT__OVER					g_nALARM_BASE+0
#define ALARM__WET_CLEANING__WAFER_COUNT__OVER						g_nALARM_BASE+1
#define WARNING__WET_CLEANING__VALVE_CYCLE_COUNT__OVER				g_nALARM_BASE+2
#define ALARM__WET_CLEANING__VALVE_CYCLE_COUNT__OVER				g_nALARM_BASE+3
#define WARNING__WET_CLEANING__SOURCE_FEEDING_AMOUNT__OVER			g_nALARM_BASE+4
#define ALARM__WET_CLEANING__SOURCE_FEEDING_AMOUNT__OVER			g_nALARM_BASE+5

// DRY cleaning
#define WARNING__DRY_CLEANING__WAFER_COUNT__OVER					g_nALARM_BASE+6
#define ALARM__DRY_CLEANING__WAFER_COUNT__OVER						g_nALARM_BASE+7
#define WARNING__DRY_CLEANING__VALVE_CYCLE_COUNT__OVER				g_nALARM_BASE+8
#define ALARM__DRY_CLEANING__VALVE_CYCLE_COUNT__OVER				g_nALARM_BASE+9
#define WARNING__DRY_CLEANING__SOURCE_FEEDING_AMOUNT__OVER			g_nALARM_BASE+10
#define ALARM__DRY_CLEANING__SOURCE_FEEDING_AMOUNT__OVER			g_nALARM_BASE+11
// 2006.02.20
#define WARNING__DRY_CLEANING__THICKNESS_AMOUNT__OVER				g_nALARM_BASE+51
#define ALARM__DRY_CLEANING__THICKNESS_AMOUNT__OVER					g_nALARM_BASE+52

#define WARNING__WET_CLEANING__THICKNESS_AMOUNT__OVER				g_nALARM_BASE+60
#define ALARM__WET_CLEANING__THICKNESS_AMOUNT__OVER					g_nALARM_BASE+61

// Source exchange
#define WARNING__SOURCE_EXCHANGE__WAFER_COUNT__OVER					g_nALARM_BASE+12
#define ALARM__SOURCE_EXCHANGE__WAFER_COUNT__OVER					g_nALARM_BASE+13
#define WARNING__SOURCE_EXCHANGE__VALVE_CYCLE_COUNT__OVER			g_nALARM_BASE+14
#define ALARM__SOURCE_EXCHANGE__VALVE_CYCLE_COUNT__OVER				g_nALARM_BASE+15
#define WARNING__SOURCE_EXCHANGE__SOURCE_FEEDING_AMOUNT__OVER		g_nALARM_BASE+16
#define ALARM__SOURCE_EXCHANGE__SOURCE_FEEDING_AMOUNT__OVER			g_nALARM_BASE+17
#define WARNING__SOURCE_EXCHANGE__LIFE_TIME__OVER					g_nALARM_BASE+18
#define ALARM__SOURCE_EXCHANGE__LIFE_TIME__OVER						g_nALARM_BASE+19

// Stage Heater exchange
#define WARNING__STG_HEATER_EXCHANGE__WAFER_COUNT__OVER				g_nALARM_BASE+20
#define ALARM__STG_HEATER_EXCHANGE__WAFER_COUNT__OVER				g_nALARM_BASE+21
#define WARNING__STG_HEATER_EXCHANGE__LIFE_TIME__OVER				g_nALARM_BASE+22
#define ALARM__STG_HEATER_EXCHANGE__LIFE_TIME__OVER					g_nALARM_BASE+23

// RF generator ON time
#define WARNING__RF_GENERATOR__ON_TIME__OVER						g_nALARM_BASE+24
#define ALARM__RF_GENERATOR__ON_TIME__OVER							g_nALARM_BASE+25

// DRY pump exchange
#define WARNING__DRY_PUMP_EXCHANGE__WAFER_COUNT__OVER				g_nALARM_BASE+26
#define ALARM__DRY_PUMP_EXCHANGE__WAFER_COUNT__OVER					g_nALARM_BASE+27
#define WARNING__DRY_PUMP_EXCHANGE__LIFE_TIME__OVER					g_nALARM_BASE+28
#define ALARM__DRY_PUMP_EXCHANGE__LIFE_TIME__OVER					g_nALARM_BASE+29

// TRAP exchange
#define WARNING__TRAP_EXCHANGE__WAFER_COUNT__OVER					g_nALARM_BASE+30
#define ALARM__TRAP_EXCHANGE__WAFER_COUNT__OVER						g_nALARM_BASE+31
#define WARNING__TRAP_EXCHANGE__LIFE_TIME__OVER						g_nALARM_BASE+32
#define ALARM__TRAP_EXCHANGE__LIFE_TIME__OVER						g_nALARM_BASE+33

// Maintenance period A
#define WARNING__MAINTENANCE_PERIOD_A__WAFER_COUNT__OVER			g_nALARM_BASE+34
#define ALARM__MAINTENANCE_PERIOD_A__WAFER_COUNT__OVER				g_nALARM_BASE+35
#define WARNING__MAINTENANCE_PERIOD_A__LIFE_TIME__OVER				g_nALARM_BASE+36
#define ALARM__MAINTENANCE_PERIOD_A__LIFE_TIME__OVER				g_nALARM_BASE+37

// Maintenance period B
#define WARNING__MAINTENANCE_PERIOD_B__WAFER_COUNT__OVER			g_nALARM_BASE+38
#define ALARM__MAINTENANCE_PERIOD_B__WAFER_COUNT__OVER				g_nALARM_BASE+39
#define WARNING__MAINTENANCE_PERIOD_B__LIFE_TIME__OVER				g_nALARM_BASE+40
#define ALARM__MAINTENANCE_PERIOD_B__LIFE_TIME__OVER				g_nALARM_BASE+41

// Maintenance period C
#define WARNING__MAINTENANCE_PERIOD_C__WAFER_COUNT__OVER			g_nALARM_BASE+42
#define ALARM__MAINTENANCE_PERIOD_C__WAFER_COUNT__OVER				g_nALARM_BASE+43
#define WARNING__MAINTENANCE_PERIOD_C__LIFE_TIME__OVER				g_nALARM_BASE+44
#define ALARM__MAINTENANCE_PERIOD_C__LIFE_TIME__OVER				g_nALARM_BASE+45

// Stage Heater 2 exchange
#define WARNING__STG_HEATER2_EXCHANGE__WAFER_COUNT__OVER			g_nALARM_BASE+46
#define ALARM__STG_HEATER2_EXCHANGE__WAFER_COUNT__OVER				g_nALARM_BASE+47
#define WARNING__STG_HEATER2_EXCHANGE__LIFE_TIME__OVER				g_nALARM_BASE+48
#define ALARM__STG_HEATER2_EXCHANGE__LIFE_TIME__OVER				g_nALARM_BASE+49

//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------

typedef enum {
	SCHEDULER				,
	PREVENTIVE_SAVE			,


	Process_Wafer_Count		,

	Clean_Enable_Type       ,
	PM1_ChamberStatus		,
	PM2_ChamberStatus       ,
	PM3_ChamberStatus       ,
	ATMAWaferSts           	,
	ATMAWaferResult        	,
	ATMBWaferSts           	,
	ATMBWaferResult         ,
	AlignerWaferSts        	,

	PROCESS_Completed		,
    // 2007.01.11 for online dry cleaning ///
	PROCESS_Completed1		,
	PROCESS_Completed2		,
	PROCESS_Completed3		,
	PROCESS_Completed4		,
	PROCESS_Completed5		,
	PROCESS_Completed6		,
	PROCESS_Completed7		,
	PROCESS_Completed8		,
	PROCESS_Completed9		,

	VLV_Total_Count			,
	SRC_Feeding_Amount		,
	// 2006.02.20
	Thickness_Amount		,

//	RFPwOnDO				,

	PRMD_WETEn_01			,
	PRMD_WETEn_02			,
	PRMD_WETEn_03			,
	//... 2018.06.17
	PRMD_WETEn_04		,
	PRMD_WETAlarm			,

	PRMD_WETCWfrCnt			,
	PRMD_WETWWfrCnt			,
	PRMD_WETAWfrCnt			,

	PRMD_WETCVlvCyc			,
	PRMD_WETWVlvCyc			,
	PRMD_WETAVlvCyc			,

	PRMD_WETCSrcFeed		,
	PRMD_WETWSrcFeed		,
	PRMD_WETASrcFeed		,
	PRMA_WETCThick		,
	PRMA_WETWThick		,
	PRMA_WETAThick		,

	PRMD_WET2CWfrCnt		,

	PRMD_WETRstCnt			,
	PRMS_WETRstTime			,
	PRMS_WETRstTime01		,
	PRMS_WETRstTime02		,
	PRMS_WETRstTime03		,
	PRMS_WETRstTime04		,
	PRMS_WETRstTime05		,
	PRMS_WETRstTime06		,
	PRMS_WETRstTime07		,
	PRMS_WETRstTime08		,
	PRMS_WETRstTime09		,
	PRMS_WETRstTime10		,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	PRMS_WETRstCount01		,
	PRMS_WETRstCount02		,
	PRMS_WETRstCount03		,
	PRMS_WETRstCount04		,
	PRMS_WETRstCount05		,
	PRMS_WETRstCount06		,
	PRMS_WETRstCount07		,
	PRMS_WETRstCount08		,
	PRMS_WETRstCount09		,
	PRMS_WETRstCount10		,

	PRMD_DRYEn_01			,
	PRMD_DRYEn_02			,
	PRMD_DRYEn_03			,
	PRMD_DRYAlarm			,

	PRMD_DRYCWfrCnt			,
	PRMD_DRYWWfrCnt			,
	PRMD_DRYAWfrCnt			,

	PRMD_DRYCVlvCyc			,
	PRMD_DRYWVlvCyc			,
	PRMD_DRYAVlvCyc			,

	PRMD_DRYCSrcFeed		,
	PRMD_DRYWSrcFeed		,
	PRMD_DRYASrcFeed		,

	PRMD_DRY2CWfrCnt		,

	PRMD_DRYRstCnt			,
	PRMS_DRYRstTime			,
	PRMS_DRYRstTime01		,
	PRMS_DRYRstTime02		,
	PRMS_DRYRstTime03		,
	PRMS_DRYRstTime04		,
	PRMS_DRYRstTime05		,
	PRMS_DRYRstTime06		,
	PRMS_DRYRstTime07		,
	PRMS_DRYRstTime08		,
	PRMS_DRYRstTime09		,
	PRMS_DRYRstTime10		,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	PRMS_DRYRstCount01		,
	PRMS_DRYRstCount02		,
	PRMS_DRYRstCount03		,
	PRMS_DRYRstCount04		,
	PRMS_DRYRstCount05		,
	PRMS_DRYRstCount06		,
	PRMS_DRYRstCount07		,
	PRMS_DRYRstCount08		,
	PRMS_DRYRstCount09		,
	PRMS_DRYRstCount10		,

	// 2006.02.20
	PRMD_DRYEn_04		,
	PRMA_DRYCThick		,
	PRMA_DRYWThick		,
	PRMA_DRYAThick		,

	PRMD_SRCEn_01			,
	PRMD_SRCEn_02			,
	PRMD_SRCEn_03			,
	PRMD_SRCEn_04			,
	PRMD_SRCAlarm			,

	PRMD_SRCCWfrCnt			,
	PRMD_SRCWWfrCnt			,
	PRMD_SRCAWfrCnt			,

	PRMD_SRCCVlvCyc			,
	PRMD_SRCWVlvCyc			,
	PRMD_SRCAVlvCyc			,

	PRMD_SRCCSrcFeed		,
	PRMD_SRCWSrcFeed		,
	PRMD_SRCASrcFeed		,

	PRMD_SRCCLTDay			,
	PRMD_SRCCLTHour			,
	PRMD_SRCCLTMin			,
	PRMD_SRCCLTSecond		,
	PRMD_SRCWLTDay			,
	PRMD_SRCWLTHour			,
	PRMD_SRCALTDay			,
	PRMD_SRCALTHour			,

	PRMD_SRC2CWfrCnt		,

	PRMD_SRCRstCnt			,
	PRMS_SRCRstTime			,
	PRMS_SRCRstTime01		,
	PRMS_SRCRstTime02		,
	PRMS_SRCRstTime03		,
	PRMS_SRCRstTime04		,
	PRMS_SRCRstTime05		,
	PRMS_SRCRstTime06		,
	PRMS_SRCRstTime07		,
	PRMS_SRCRstTime08		,
	PRMS_SRCRstTime09		,
	PRMS_SRCRstTime10		,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	PRMS_SRCRstCount01		,
	PRMS_SRCRstCount02		,
	PRMS_SRCRstCount03		,
	PRMS_SRCRstCount04		,
	PRMS_SRCRstCount05		,
	PRMS_SRCRstCount06		,
	PRMS_SRCRstCount07		,
	PRMS_SRCRstCount08		,
	PRMS_SRCRstCount09		,
	PRMS_SRCRstCount10		,

	PRMD_STGEn_01			,
	PRMD_STGEn_02			,
	PRMD_STGAlarm			,

	PRMD_STGCWfrCnt			,
	PRMD_STGWWfrCnt			,
	PRMD_STGAWfrCnt			,

	PRMD_STGCLTDay			,
	PRMD_STGCLTHour			,
	PRMD_STGCLTMin			,
	PRMD_STGCLTSecond		,
	PRMD_STGWLTDay			,
	PRMD_STGWLTHour			,
	PRMD_STGALTDay			,
	PRMD_STGALTHour			,

	PRMD_STGRstCnt			,
	PRMS_STGRstTime			,
	PRMS_STGRstTime01		,
	PRMS_STGRstTime02		,
	PRMS_STGRstTime03		,
	PRMS_STGRstTime04		,
	PRMS_STGRstTime05		,
	PRMS_STGRstTime06		,
	PRMS_STGRstTime07		,
	PRMS_STGRstTime08		,
	PRMS_STGRstTime09		,
	PRMS_STGRstTime10		,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	PRMS_STGRstCount01		,
	PRMS_STGRstCount02		,
	PRMS_STGRstCount03		,
	PRMS_STGRstCount04		,
	PRMS_STGRstCount05		,
	PRMS_STGRstCount06		,
	PRMS_STGRstCount07		,
	PRMS_STGRstCount08		,
	PRMS_STGRstCount09		,
	PRMS_STGRstCount10		,

	// Stage Heater 2
	PRMD_STG2En_01			,
	PRMD_STG2En_02			,
	PRMD_STG2Alarm			,

	PRMD_STG2CWfrCnt		,
	PRMD_STG2WWfrCnt		,
	PRMD_STG2AWfrCnt		,

	PRMD_STG2CLTDay			,
	PRMD_STG2CLTHour		,
	PRMD_STG2CLTMin			,
	PRMD_STG2CLTSecond		,
	PRMD_STG2WLTDay			,
	PRMD_STG2WLTHour		,
	PRMD_STG2ALTDay			,
	PRMD_STG2ALTHour		,

	PRMD_STG2RstCnt			,
	PRMS_STG2RstTime		,
	PRMS_STG2RstTime01		,
	PRMS_STG2RstTime02		,
	PRMS_STG2RstTime03		,
	PRMS_STG2RstTime04		,
	PRMS_STG2RstTime05		,
	PRMS_STG2RstTime06		,
	PRMS_STG2RstTime07		,
	PRMS_STG2RstTime08		,
	PRMS_STG2RstTime09		,
	PRMS_STG2RstTime10		,

	PRMS_STG2RstCount01		,
	PRMS_STG2RstCount02		,
	PRMS_STG2RstCount03		,
	PRMS_STG2RstCount04		,
	PRMS_STG2RstCount05		,
	PRMS_STG2RstCount06		,
	PRMS_STG2RstCount07		,
	PRMS_STG2RstCount08		,
	PRMS_STG2RstCount09		,
	PRMS_STG2RstCount10		,

	PRMD_RFGEn_01			,
	PRMD_RFGAlarm			,
		
	PRMD_RFGCLTDay			,
	PRMD_RFGCLTHour			,
	PRMD_RFGCLTMin			,
	PRMD_RFGCLTSecond		,
	PRMD_RFGWLTDay			,
	PRMD_RFGWLTHour			,
	PRMD_RFGALTDay			,
	PRMD_RFGALTHour			,
	
	PRMD_RFGRstCnt			,
	PRMS_RFGRstTime			,
	PRMS_RFGRstTime01		,
	PRMS_RFGRstTime02		,
	PRMS_RFGRstTime03		,
	PRMS_RFGRstTime04		,
	PRMS_RFGRstTime05		,
	PRMS_RFGRstTime06		,
	PRMS_RFGRstTime07		,
	PRMS_RFGRstTime08		,
	PRMS_RFGRstTime09		,
	PRMS_RFGRstTime10		,

	PRMD_DPEn_01			,
	PRMD_DPEn_02			,
	PRMD_DPAlarm			,

	PRMD_DPCWfrCnt			,
	PRMD_DPWWfrCnt			,
	PRMD_DPAWfrCnt			,

	PRMD_DPCLTDay			,
	PRMD_DPCLTHour			,
	PRMD_DPCLTMin			,
	PRMD_DPCLTSecond		,
	PRMD_DPWLTDay			,
	PRMD_DPWLTHour			,
	PRMD_DPALTDay			,
	PRMD_DPALTHour			,

	PRMD_DPRstCnt			,
	PRMS_DPRstTime			,
	PRMS_DPRstTime01		,
	PRMS_DPRstTime02		,
	PRMS_DPRstTime03		,
	PRMS_DPRstTime04		,
	PRMS_DPRstTime05		,
	PRMS_DPRstTime06		,
	PRMS_DPRstTime07		,
	PRMS_DPRstTime08		,
	PRMS_DPRstTime09		,
	PRMS_DPRstTime10		,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	PRMS_DPRstCount01		,
	PRMS_DPRstCount02		,
	PRMS_DPRstCount03		,
	PRMS_DPRstCount04		,
	PRMS_DPRstCount05		,
	PRMS_DPRstCount06		,
	PRMS_DPRstCount07		,
	PRMS_DPRstCount08		,
	PRMS_DPRstCount09		,
	PRMS_DPRstCount10		,

	PRMD_TRAPEn_01			,
	PRMD_TRAPEn_02			,
	PRMD_TRAPAlarm			,

	PRMD_TRAPCWfrCnt		,
	PRMD_TRAPWWfrCnt		,
	PRMD_TRAPAWfrCnt		,

	PRMD_TRAPCLTDay			,
	PRMD_TRAPCLTHour		,
	PRMD_TRAPCLTMin			,
	PRMD_TRAPCLTSecond		,
	PRMD_TRAPWLTDay			,
	PRMD_TRAPWLTHour		,
	PRMD_TRAPALTDay			,
	PRMD_TRAPALTHour		,

	PRMD_TRAPRstCnt			,
	PRMS_TRAPRstTime		,
	PRMS_TRAPRstTime01		,
	PRMS_TRAPRstTime02		,
	PRMS_TRAPRstTime03		,
	PRMS_TRAPRstTime04		,
	PRMS_TRAPRstTime05		,
	PRMS_TRAPRstTime06		,
	PRMS_TRAPRstTime07		,
	PRMS_TRAPRstTime08		,
	PRMS_TRAPRstTime09		,
	PRMS_TRAPRstTime10		,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	PRMS_TRAPRstCount01		,
	PRMS_TRAPRstCount02		,
	PRMS_TRAPRstCount03		,
	PRMS_TRAPRstCount04		,
	PRMS_TRAPRstCount05		,
	PRMS_TRAPRstCount06		,
	PRMS_TRAPRstCount07		,
	PRMS_TRAPRstCount08		,
	PRMS_TRAPRstCount09		,
	PRMS_TRAPRstCount10		,

	PRMD_MPAEn_01			,
	PRMD_MPAEn_02			,
	PRMD_MPAAlarm			,

	PRMD_MPACWfrCnt			,
	PRMD_MPAWWfrCnt			,
	PRMD_MPAAWfrCnt			,

	PRMD_MPACLTDay			,
	PRMD_MPACLTHour			,
	PRMD_MPACLTMin			,
	PRMD_MPACLTSecond		,
	PRMD_MPAWLTDay			,
	PRMD_MPAWLTHour			,
	PRMD_MPAALTDay			,
	PRMD_MPAALTHour			,

	PRMD_MPARstCnt			,
	PRMS_MPARstTime			,
	PRMS_MPARstTime01		,
	PRMS_MPARstTime02		,
	PRMS_MPARstTime03		,
	PRMS_MPARstTime04		,
	PRMS_MPARstTime05		,
	PRMS_MPARstTime06		,
	PRMS_MPARstTime07		,
	PRMS_MPARstTime08		,
	PRMS_MPARstTime09		,
	PRMS_MPARstTime10		,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	PRMS_MPARstCount01		,
	PRMS_MPARstCount02		,
	PRMS_MPARstCount03		,
	PRMS_MPARstCount04		,
	PRMS_MPARstCount05		,
	PRMS_MPARstCount06		,
	PRMS_MPARstCount07		,
	PRMS_MPARstCount08		,
	PRMS_MPARstCount09		,
	PRMS_MPARstCount10		,

	PRMD_MPBEn_01			,
	PRMD_MPBEn_02			,
	PRMD_MPBAlarm			,

	PRMD_MPBCWfrCnt			,
	PRMD_MPBWWfrCnt			,
	PRMD_MPBAWfrCnt			,

	PRMD_MPBCLTDay			,
	PRMD_MPBCLTHour			,
	PRMD_MPBCLTMin			,
	PRMD_MPBCLTSecond		,
	PRMD_MPBWLTDay			,
	PRMD_MPBWLTHour			,
	PRMD_MPBALTDay			,
	PRMD_MPBALTHour			,

	PRMD_MPBRstCnt			,
	PRMS_MPBRstTime			,
	PRMS_MPBRstTime01		,
	PRMS_MPBRstTime02		,
	PRMS_MPBRstTime03		,
	PRMS_MPBRstTime04		,
	PRMS_MPBRstTime05		,
	PRMS_MPBRstTime06		,
	PRMS_MPBRstTime07		,
	PRMS_MPBRstTime08		,
	PRMS_MPBRstTime09		,
	PRMS_MPBRstTime10		,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	PRMS_MPBRstCount01		,
	PRMS_MPBRstCount02		,
	PRMS_MPBRstCount03		,
	PRMS_MPBRstCount04		,
	PRMS_MPBRstCount05		,
	PRMS_MPBRstCount06		,
	PRMS_MPBRstCount07		,
	PRMS_MPBRstCount08		,
	PRMS_MPBRstCount09		,
	PRMS_MPBRstCount10		,

	PRMD_MPCEn_01			,
	PRMD_MPCEn_02			,
	PRMD_MPCAlarm			,

	PRMD_MPCCWfrCnt			,
	PRMD_MPCWWfrCnt			,
	PRMD_MPCAWfrCnt			,

	PRMD_MPCCLTDay			,
	PRMD_MPCCLTHour			,
	PRMD_MPCCLTMin			,
	PRMD_MPCCLTSecond		,
	PRMD_MPCWLTDay			,
	PRMD_MPCWLTHour			,
	PRMD_MPCALTDay			,
	PRMD_MPCALTHour			,

	PRMD_MPCRstCnt			,
	PRMS_MPCRstTime			,
	PRMS_MPCRstTime01		,
	PRMS_MPCRstTime02		,
	PRMS_MPCRstTime03		,
	PRMS_MPCRstTime04		,
	PRMS_MPCRstTime05		,
	PRMS_MPCRstTime06		,
	PRMS_MPCRstTime07		,
	PRMS_MPCRstTime08		,
	PRMS_MPCRstTime09		,
	PRMS_MPCRstTime10		,

	// added by ycchoi 2006.05.28 < V 1.0.0.2 >
	PRMS_MPCRstCount01		,
	PRMS_MPCRstCount02		,
	PRMS_MPCRstCount03		,
	PRMS_MPCRstCount04		,
	PRMS_MPCRstCount05		,
	PRMS_MPCRstCount06		,
	PRMS_MPCRstCount07		,
	PRMS_MPCRstCount08		,
	PRMS_MPCRstCount09		,
	PRMS_MPCRstCount10		,
	PRO_RcpTypeDM			,
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
