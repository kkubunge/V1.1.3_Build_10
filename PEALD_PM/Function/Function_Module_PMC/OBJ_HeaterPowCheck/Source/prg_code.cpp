///////////////////////////////////////////////////////////////////////////////
// Line Heater Power Check
// Create : 2020.01.13

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "TextParser.h"
#define FUNC_NAME "HeaterPowerCheck"

//------------------------------------------------------------------------------------------
// Alarm Definition
#define	ALARM_HTR01_POW_TO_WARN 	4801
#define	ALARM_HTR01_POW_TO_ABOT		4841
//------------------------------------------------------------------------------------------
// Data Type

#define MAX_HEATER 32

typedef struct _HEATER_STRUCT
{
	int  nHTName;
	int  nAlarmCheck;
	int  nAlarmTime;
	int  nWarnPer;
	int  nAbortPer;
} HEATER_STRUCT;
HEATER_STRUCT g_HtrList[MAX_HEATER];

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {

	{"IO_Driver_Loding",	_K_D_IO,	IO_Driver_Loding,	0},
	{ "SW_GbIntlckDM", 		_K_D_IO,	SW_GbIntlckDM,		0},

	{"HT01_UseDM", _K_D_IO, HT01_UseDM, 0},
	{"HT02_UseDM", _K_D_IO, HT02_UseDM, 0},
	{"HT03_UseDM", _K_D_IO, HT03_UseDM, 0},
	{"HT04_UseDM", _K_D_IO, HT04_UseDM, 0},
	{"HT05_UseDM", _K_D_IO, HT05_UseDM, 0},
	{"HT06_UseDM", _K_D_IO, HT06_UseDM, 0},
	{"HT07_UseDM", _K_D_IO, HT07_UseDM, 0},
	{"HT08_UseDM", _K_D_IO, HT08_UseDM, 0},
	{"HT09_UseDM", _K_D_IO, HT09_UseDM, 0},
	{"HT10_UseDM", _K_D_IO, HT10_UseDM, 0},
	{"HT11_UseDM", _K_D_IO, HT11_UseDM, 0},
	{"HT12_UseDM", _K_D_IO, HT12_UseDM, 0},
	{"HT13_UseDM", _K_D_IO, HT13_UseDM, 0},
	{"HT14_UseDM", _K_D_IO, HT14_UseDM, 0},
	{"HT15_UseDM", _K_D_IO, HT15_UseDM, 0},
	{"HT16_UseDM", _K_D_IO, HT16_UseDM, 0},
	{"HT17_UseDM", _K_D_IO, HT17_UseDM, 0},
	{"HT18_UseDM", _K_D_IO, HT18_UseDM, 0},
	{"HT19_UseDM", _K_D_IO, HT19_UseDM, 0},
	{"HT20_UseDM", _K_D_IO, HT20_UseDM, 0},
	{"HT21_UseDM", _K_D_IO, HT21_UseDM, 0},
	{"HT22_UseDM", _K_D_IO, HT22_UseDM, 0},
	{"HT23_UseDM", _K_D_IO, HT23_UseDM, 0},
	{"HT24_UseDM", _K_D_IO, HT24_UseDM, 0},
	{"HT25_UseDM", _K_D_IO, HT25_UseDM, 0},
	{"HT26_UseDM", _K_D_IO, HT26_UseDM, 0},
	{"HT27_UseDM", _K_D_IO, HT27_UseDM, 0},
	{"HT28_UseDM", _K_D_IO, HT28_UseDM, 0},
	{"HT29_UseDM", _K_D_IO, HT29_UseDM, 0},
	{"HT30_UseDM", _K_D_IO, HT30_UseDM, 0},
	{"HT31_UseDM", _K_D_IO, HT31_UseDM, 0},
	{"HT32_UseDM", _K_D_IO, HT32_UseDM, 0},

	{ "HT01_OnOffDIO",	_K_D_IO,	HT01_OnOff,	0},
	{ "HT02_OnOffDIO",	_K_D_IO,	HT02_OnOff,	0},
	{ "HT03_OnOffDIO",	_K_D_IO,	HT03_OnOff,	0},
	{ "HT04_OnOffDIO",	_K_D_IO,	HT04_OnOff,	0},
	{ "HT05_OnOffDIO",	_K_D_IO,	HT05_OnOff,	0},
	{ "HT06_OnOffDIO",	_K_D_IO,	HT06_OnOff,	0},
	{ "HT07_OnOffDIO",	_K_D_IO,	HT07_OnOff,	0},
	{ "HT08_OnOffDIO",	_K_D_IO,	HT08_OnOff,	0},
	{ "HT09_OnOffDIO",	_K_D_IO,	HT09_OnOff,	0},
	{ "HT10_OnOffDIO",	_K_D_IO,	HT10_OnOff,	0},
	{ "HT11_OnOffDIO",	_K_D_IO,	HT11_OnOff,	0},
	{ "HT12_OnOffDIO",	_K_D_IO,	HT12_OnOff,	0},
	{ "HT13_OnOffDIO",	_K_D_IO,	HT13_OnOff,	0},
	{ "HT14_OnOffDIO",	_K_D_IO,	HT14_OnOff,	0},
	{ "HT15_OnOffDIO",	_K_D_IO,	HT15_OnOff,	0},
	{ "HT16_OnOffDIO",	_K_D_IO,	HT16_OnOff,	0},
	{ "HT17_OnOffDIO",	_K_D_IO,	HT17_OnOff,	0},
	{ "HT18_OnOffDIO",	_K_D_IO,	HT18_OnOff,	0},
	{ "HT19_OnOffDIO",	_K_D_IO,	HT19_OnOff,	0},
	{ "HT20_OnOffDIO",	_K_D_IO,	HT20_OnOff,	0},
	{ "HT21_OnOffDIO",	_K_D_IO,	HT21_OnOff,	0},
	{ "HT22_OnOffDIO",	_K_D_IO,	HT22_OnOff,	0},
	{ "HT23_OnOffDIO",	_K_D_IO,	HT23_OnOff,	0},
	{ "HT24_OnOffDIO",	_K_D_IO,	HT24_OnOff,	0},
	{ "HT25_OnOffDIO",	_K_D_IO,	HT25_OnOff,	0},
	{ "HT26_OnOffDIO",	_K_D_IO,	HT26_OnOff,	0},
	{ "HT27_OnOffDIO",	_K_D_IO,	HT27_OnOff,	0},
	{ "HT28_OnOffDIO",	_K_D_IO,	HT28_OnOff,	0},
	{ "HT29_OnOffDIO",	_K_D_IO,	HT29_OnOff,	0},
	{ "HT30_OnOffDIO",	_K_D_IO,	HT30_OnOff,	0},
	{ "HT31_OnOffDIO",	_K_D_IO,	HT31_OnOff,	0},
	{ "HT32_OnOffDIO",	_K_D_IO,	HT32_OnOff,	0},

	{"HT01_OutPwAI",	_K_A_IO,	HT01_OutPwAI,	0},
	{"HT02_OutPwAI",	_K_A_IO,	HT02_OutPwAI,	0},
	{"HT03_OutPwAI",	_K_A_IO,	HT03_OutPwAI,	0},
	{"HT04_OutPwAI",	_K_A_IO,	HT04_OutPwAI,	0},
	{"HT05_OutPwAI",	_K_A_IO,	HT05_OutPwAI,	0},
	{"HT06_OutPwAI",	_K_A_IO,	HT06_OutPwAI,	0},
	{"HT07_OutPwAI",	_K_A_IO,	HT07_OutPwAI,	0},
	{"HT08_OutPwAI",	_K_A_IO,	HT08_OutPwAI,	0},
	{"HT09_OutPwAI",	_K_A_IO,	HT09_OutPwAI,	0},
	{"HT10_OutPwAI",	_K_A_IO,	HT10_OutPwAI,	0},
	{"HT11_OutPwAI",	_K_A_IO,	HT11_OutPwAI,	0},
	{"HT12_OutPwAI",	_K_A_IO,	HT12_OutPwAI,	0},
	{"HT13_OutPwAI",	_K_A_IO,	HT13_OutPwAI,	0},
	{"HT14_OutPwAI",	_K_A_IO,	HT14_OutPwAI,	0},
	{"HT15_OutPwAI",	_K_A_IO,	HT15_OutPwAI,	0},
	{"HT16_OutPwAI",	_K_A_IO,	HT16_OutPwAI,	0},
	{"HT17_OutPwAI",	_K_A_IO,	HT17_OutPwAI,	0},
	{"HT18_OutPwAI",	_K_A_IO,	HT18_OutPwAI,	0},
	{"HT19_OutPwAI",	_K_A_IO,	HT19_OutPwAI,	0},
	{"HT20_OutPwAI",	_K_A_IO,	HT20_OutPwAI,	0},
	{"HT21_OutPwAI",	_K_A_IO,	HT21_OutPwAI,	0},
	{"HT22_OutPwAI",	_K_A_IO,	HT22_OutPwAI,	0},
	{"HT23_OutPwAI",	_K_A_IO,	HT23_OutPwAI,	0},
	{"HT24_OutPwAI",	_K_A_IO,	HT24_OutPwAI,	0},
	{"HT25_OutPwAI",	_K_A_IO,	HT25_OutPwAI,	0},
	{"HT26_OutPwAI",	_K_A_IO,	HT26_OutPwAI,	0},
	{"HT27_OutPwAI",	_K_A_IO,	HT27_OutPwAI,	0},
	{"HT28_OutPwAI",	_K_A_IO,	HT28_OutPwAI,	0},
	{"HT29_OutPwAI",	_K_A_IO,	HT29_OutPwAI,	0},
	{"HT30_OutPwAI",	_K_A_IO,	HT30_OutPwAI,	0},
	{"HT31_OutPwAI",	_K_A_IO,	HT31_OutPwAI,	0},
	{"HT32_OutPwAI",	_K_A_IO,	HT32_OutPwAI,	0},

	{"PRMD_HT01_PowChkUse",    _K_D_IO,    PRMD_HT01_PowChkUse,    0},
	{"PRMD_HT02_PowChkUse",    _K_D_IO,    PRMD_HT02_PowChkUse,    0},
	{"PRMD_HT03_PowChkUse",    _K_D_IO,    PRMD_HT03_PowChkUse,    0},
	{"PRMD_HT04_PowChkUse",    _K_D_IO,    PRMD_HT04_PowChkUse,    0},
	{"PRMD_HT05_PowChkUse",    _K_D_IO,    PRMD_HT05_PowChkUse,    0},
	{"PRMD_HT06_PowChkUse",    _K_D_IO,    PRMD_HT06_PowChkUse,    0},
	{"PRMD_HT07_PowChkUse",    _K_D_IO,    PRMD_HT07_PowChkUse,    0},
	{"PRMD_HT08_PowChkUse",    _K_D_IO,    PRMD_HT08_PowChkUse,    0},
	{"PRMD_HT09_PowChkUse",    _K_D_IO,    PRMD_HT09_PowChkUse,    0},
	{"PRMD_HT10_PowChkUse",    _K_D_IO,    PRMD_HT10_PowChkUse,    0},
	{"PRMD_HT11_PowChkUse",    _K_D_IO,    PRMD_HT11_PowChkUse,    0},
	{"PRMD_HT12_PowChkUse",    _K_D_IO,    PRMD_HT12_PowChkUse,    0},
	{"PRMD_HT13_PowChkUse",    _K_D_IO,    PRMD_HT13_PowChkUse,    0},
	{"PRMD_HT14_PowChkUse",    _K_D_IO,    PRMD_HT14_PowChkUse,    0},
	{"PRMD_HT15_PowChkUse",    _K_D_IO,    PRMD_HT15_PowChkUse,    0},
	{"PRMD_HT16_PowChkUse",    _K_D_IO,    PRMD_HT16_PowChkUse,    0},
	{"PRMD_HT17_PowChkUse",    _K_D_IO,    PRMD_HT17_PowChkUse,    0},
	{"PRMD_HT18_PowChkUse",    _K_D_IO,    PRMD_HT18_PowChkUse,    0},
	{"PRMD_HT19_PowChkUse",    _K_D_IO,    PRMD_HT19_PowChkUse,    0},
	{"PRMD_HT20_PowChkUse",    _K_D_IO,    PRMD_HT20_PowChkUse,    0},
	{"PRMD_HT21_PowChkUse",    _K_D_IO,    PRMD_HT21_PowChkUse,    0},
	{"PRMD_HT22_PowChkUse",    _K_D_IO,    PRMD_HT22_PowChkUse,    0},
	{"PRMD_HT23_PowChkUse",    _K_D_IO,    PRMD_HT23_PowChkUse,    0},
	{"PRMD_HT24_PowChkUse",    _K_D_IO,    PRMD_HT24_PowChkUse,    0},
	{"PRMD_HT25_PowChkUse",    _K_D_IO,    PRMD_HT25_PowChkUse,    0},
	{"PRMD_HT26_PowChkUse",    _K_D_IO,    PRMD_HT26_PowChkUse,    0},
	{"PRMD_HT27_PowChkUse",    _K_D_IO,    PRMD_HT27_PowChkUse,    0},
	{"PRMD_HT28_PowChkUse",    _K_D_IO,    PRMD_HT28_PowChkUse,    0},
	{"PRMD_HT29_PowChkUse",    _K_D_IO,    PRMD_HT29_PowChkUse,    0},
	{"PRMD_HT30_PowChkUse",    _K_D_IO,    PRMD_HT30_PowChkUse,    0},
	{"PRMD_HT31_PowChkUse",    _K_D_IO,    PRMD_HT31_PowChkUse,    0},
	{"PRMD_HT32_PowChkUse",    _K_D_IO,    PRMD_HT32_PowChkUse,    0},

	{"PRMA_HT01_PowChkTO",    _K_A_IO,    PRMA_HT01_PowChkTO,    0},
	{"PRMA_HT02_PowChkTO",    _K_A_IO,    PRMA_HT02_PowChkTO,    0},
	{"PRMA_HT03_PowChkTO",    _K_A_IO,    PRMA_HT03_PowChkTO,    0},
	{"PRMA_HT04_PowChkTO",    _K_A_IO,    PRMA_HT04_PowChkTO,    0},
	{"PRMA_HT05_PowChkTO",    _K_A_IO,    PRMA_HT05_PowChkTO,    0},
	{"PRMA_HT06_PowChkTO",    _K_A_IO,    PRMA_HT06_PowChkTO,    0},
	{"PRMA_HT07_PowChkTO",    _K_A_IO,    PRMA_HT07_PowChkTO,    0},
	{"PRMA_HT08_PowChkTO",    _K_A_IO,    PRMA_HT08_PowChkTO,    0},
	{"PRMA_HT09_PowChkTO",    _K_A_IO,    PRMA_HT09_PowChkTO,    0},
	{"PRMA_HT10_PowChkTO",    _K_A_IO,    PRMA_HT10_PowChkTO,    0},
	{"PRMA_HT11_PowChkTO",    _K_A_IO,    PRMA_HT11_PowChkTO,    0},
	{"PRMA_HT12_PowChkTO",    _K_A_IO,    PRMA_HT12_PowChkTO,    0},
	{"PRMA_HT13_PowChkTO",    _K_A_IO,    PRMA_HT13_PowChkTO,    0},
	{"PRMA_HT14_PowChkTO",    _K_A_IO,    PRMA_HT14_PowChkTO,    0},
	{"PRMA_HT15_PowChkTO",    _K_A_IO,    PRMA_HT15_PowChkTO,    0},
	{"PRMA_HT16_PowChkTO",    _K_A_IO,    PRMA_HT16_PowChkTO,    0},
	{"PRMA_HT17_PowChkTO",    _K_A_IO,    PRMA_HT17_PowChkTO,    0},
	{"PRMA_HT18_PowChkTO",    _K_A_IO,    PRMA_HT18_PowChkTO,    0},
	{"PRMA_HT19_PowChkTO",    _K_A_IO,    PRMA_HT19_PowChkTO,    0},
	{"PRMA_HT20_PowChkTO",    _K_A_IO,    PRMA_HT20_PowChkTO,    0},
	{"PRMA_HT21_PowChkTO",    _K_A_IO,    PRMA_HT21_PowChkTO,    0},
	{"PRMA_HT22_PowChkTO",    _K_A_IO,    PRMA_HT22_PowChkTO,    0},
	{"PRMA_HT23_PowChkTO",    _K_A_IO,    PRMA_HT23_PowChkTO,    0},
	{"PRMA_HT24_PowChkTO",    _K_A_IO,    PRMA_HT24_PowChkTO,    0},
	{"PRMA_HT25_PowChkTO",    _K_A_IO,    PRMA_HT25_PowChkTO,    0},
	{"PRMA_HT26_PowChkTO",    _K_A_IO,    PRMA_HT26_PowChkTO,    0},
	{"PRMA_HT27_PowChkTO",    _K_A_IO,    PRMA_HT27_PowChkTO,    0},
	{"PRMA_HT28_PowChkTO",    _K_A_IO,    PRMA_HT28_PowChkTO,    0},
	{"PRMA_HT29_PowChkTO",    _K_A_IO,    PRMA_HT29_PowChkTO,    0},
	{"PRMA_HT30_PowChkTO",    _K_A_IO,    PRMA_HT30_PowChkTO,    0},
	{"PRMA_HT31_PowChkTO",    _K_A_IO,    PRMA_HT31_PowChkTO,    0},
	{"PRMA_HT32_PowChkTO",    _K_A_IO,    PRMA_HT32_PowChkTO,    0},

	{"PRMA_HT01_PowChkWrn",    _K_A_IO,    PRMA_HT01_PowChkWrn,    0},
	{"PRMA_HT02_PowChkWrn",    _K_A_IO,    PRMA_HT02_PowChkWrn,    0},
	{"PRMA_HT03_PowChkWrn",    _K_A_IO,    PRMA_HT03_PowChkWrn,    0},
	{"PRMA_HT04_PowChkWrn",    _K_A_IO,    PRMA_HT04_PowChkWrn,    0},
	{"PRMA_HT05_PowChkWrn",    _K_A_IO,    PRMA_HT05_PowChkWrn,    0},
	{"PRMA_HT06_PowChkWrn",    _K_A_IO,    PRMA_HT06_PowChkWrn,    0},
	{"PRMA_HT07_PowChkWrn",    _K_A_IO,    PRMA_HT07_PowChkWrn,    0},
	{"PRMA_HT08_PowChkWrn",    _K_A_IO,    PRMA_HT08_PowChkWrn,    0},
	{"PRMA_HT09_PowChkWrn",    _K_A_IO,    PRMA_HT09_PowChkWrn,    0},
	{"PRMA_HT10_PowChkWrn",    _K_A_IO,    PRMA_HT10_PowChkWrn,    0},
	{"PRMA_HT11_PowChkWrn",    _K_A_IO,    PRMA_HT11_PowChkWrn,    0},
	{"PRMA_HT12_PowChkWrn",    _K_A_IO,    PRMA_HT12_PowChkWrn,    0},
	{"PRMA_HT13_PowChkWrn",    _K_A_IO,    PRMA_HT13_PowChkWrn,    0},
	{"PRMA_HT14_PowChkWrn",    _K_A_IO,    PRMA_HT14_PowChkWrn,    0},
	{"PRMA_HT15_PowChkWrn",    _K_A_IO,    PRMA_HT15_PowChkWrn,    0},
	{"PRMA_HT16_PowChkWrn",    _K_A_IO,    PRMA_HT16_PowChkWrn,    0},
	{"PRMA_HT17_PowChkWrn",    _K_A_IO,    PRMA_HT17_PowChkWrn,    0},
	{"PRMA_HT18_PowChkWrn",    _K_A_IO,    PRMA_HT18_PowChkWrn,    0},
	{"PRMA_HT19_PowChkWrn",    _K_A_IO,    PRMA_HT19_PowChkWrn,    0},
	{"PRMA_HT20_PowChkWrn",    _K_A_IO,    PRMA_HT20_PowChkWrn,    0},
	{"PRMA_HT21_PowChkWrn",    _K_A_IO,    PRMA_HT21_PowChkWrn,    0},
	{"PRMA_HT22_PowChkWrn",    _K_A_IO,    PRMA_HT22_PowChkWrn,    0},
	{"PRMA_HT23_PowChkWrn",    _K_A_IO,    PRMA_HT23_PowChkWrn,    0},
	{"PRMA_HT24_PowChkWrn",    _K_A_IO,    PRMA_HT24_PowChkWrn,    0},
	{"PRMA_HT25_PowChkWrn",    _K_A_IO,    PRMA_HT25_PowChkWrn,    0},
	{"PRMA_HT26_PowChkWrn",    _K_A_IO,    PRMA_HT26_PowChkWrn,    0},
	{"PRMA_HT27_PowChkWrn",    _K_A_IO,    PRMA_HT27_PowChkWrn,    0},
	{"PRMA_HT28_PowChkWrn",    _K_A_IO,    PRMA_HT28_PowChkWrn,    0},
	{"PRMA_HT29_PowChkWrn",    _K_A_IO,    PRMA_HT29_PowChkWrn,    0},
	{"PRMA_HT30_PowChkWrn",    _K_A_IO,    PRMA_HT30_PowChkWrn,    0},
	{"PRMA_HT31_PowChkWrn",    _K_A_IO,    PRMA_HT31_PowChkWrn,    0},
	{"PRMA_HT32_PowChkWrn",    _K_A_IO,    PRMA_HT32_PowChkWrn,    0},

	{"PRMA_HT01_PowChkAbt",    _K_A_IO,    PRMA_HT01_PowChkAbt,    0},
	{"PRMA_HT02_PowChkAbt",    _K_A_IO,    PRMA_HT02_PowChkAbt,    0},
	{"PRMA_HT03_PowChkAbt",    _K_A_IO,    PRMA_HT03_PowChkAbt,    0},
	{"PRMA_HT04_PowChkAbt",    _K_A_IO,    PRMA_HT04_PowChkAbt,    0},
	{"PRMA_HT05_PowChkAbt",    _K_A_IO,    PRMA_HT05_PowChkAbt,    0},
	{"PRMA_HT06_PowChkAbt",    _K_A_IO,    PRMA_HT06_PowChkAbt,    0},
	{"PRMA_HT07_PowChkAbt",    _K_A_IO,    PRMA_HT07_PowChkAbt,    0},
	{"PRMA_HT08_PowChkAbt",    _K_A_IO,    PRMA_HT08_PowChkAbt,    0},
	{"PRMA_HT09_PowChkAbt",    _K_A_IO,    PRMA_HT09_PowChkAbt,    0},
	{"PRMA_HT10_PowChkAbt",    _K_A_IO,    PRMA_HT10_PowChkAbt,    0},
	{"PRMA_HT11_PowChkAbt",    _K_A_IO,    PRMA_HT11_PowChkAbt,    0},
	{"PRMA_HT12_PowChkAbt",    _K_A_IO,    PRMA_HT12_PowChkAbt,    0},
	{"PRMA_HT13_PowChkAbt",    _K_A_IO,    PRMA_HT13_PowChkAbt,    0},
	{"PRMA_HT14_PowChkAbt",    _K_A_IO,    PRMA_HT14_PowChkAbt,    0},
	{"PRMA_HT15_PowChkAbt",    _K_A_IO,    PRMA_HT15_PowChkAbt,    0},
	{"PRMA_HT16_PowChkAbt",    _K_A_IO,    PRMA_HT16_PowChkAbt,    0},
	{"PRMA_HT17_PowChkAbt",    _K_A_IO,    PRMA_HT17_PowChkAbt,    0},
	{"PRMA_HT18_PowChkAbt",    _K_A_IO,    PRMA_HT18_PowChkAbt,    0},
	{"PRMA_HT19_PowChkAbt",    _K_A_IO,    PRMA_HT19_PowChkAbt,    0},
	{"PRMA_HT20_PowChkAbt",    _K_A_IO,    PRMA_HT20_PowChkAbt,    0},
	{"PRMA_HT21_PowChkAbt",    _K_A_IO,    PRMA_HT21_PowChkAbt,    0},
	{"PRMA_HT22_PowChkAbt",    _K_A_IO,    PRMA_HT22_PowChkAbt,    0},
	{"PRMA_HT23_PowChkAbt",    _K_A_IO,    PRMA_HT23_PowChkAbt,    0},
	{"PRMA_HT24_PowChkAbt",    _K_A_IO,    PRMA_HT24_PowChkAbt,    0},
	{"PRMA_HT25_PowChkAbt",    _K_A_IO,    PRMA_HT25_PowChkAbt,    0},
	{"PRMA_HT26_PowChkAbt",    _K_A_IO,    PRMA_HT26_PowChkAbt,    0},
	{"PRMA_HT27_PowChkAbt",    _K_A_IO,    PRMA_HT27_PowChkAbt,    0},
	{"PRMA_HT28_PowChkAbt",    _K_A_IO,    PRMA_HT28_PowChkAbt,    0},
	{"PRMA_HT29_PowChkAbt",    _K_A_IO,    PRMA_HT29_PowChkAbt,    0},
	{"PRMA_HT30_PowChkAbt",    _K_A_IO,    PRMA_HT30_PowChkAbt,    0},
	{"PRMA_HT31_PowChkAbt",    _K_A_IO,    PRMA_HT31_PowChkAbt,    0},
	{"PRMA_HT32_PowChkAbt",    _K_A_IO,    PRMA_HT32_PowChkAbt,    0},

	""};

//------------------------------------------------------------------------------------------
typedef void(__cdecl *_THREAD_FUNC)(void *lpParam);
enum
{
	OFF_0,
	ON_1
};
enum
{
	HTR_NOTUSE,
	HTR_USE
};
enum
{
	HTR_OFF,
	HTR_ON
};
enum
{
	INTLK_OFF,
	INTLK_ON
};

BOOL	SIMULATION_MODE = FALSE;

void LineHeater_PowCheck()
{
	int i, limitCnt[MAX_HEATER];
	int nIOStatus;
	double m_dbCurrPow, m_dbTimeCount, m_dbWarnHiLim, m_dbAbortHiLim;

	while (TRUE)
	{
		for (i = 0; i < MAX_HEATER; i++)
		{
			if (READ_DIGITAL(HT01_UseDM + i, &nIOStatus) == HTR_USE
				&& READ_DIGITAL(PRMD_HT01_PowChkUse + i, &nIOStatus) == ON_1)
			{
				m_dbCurrPow = READ_ANALOG(HT01_OutPwAI + i, &nIOStatus);
				m_dbTimeCount = READ_ANALOG(PRMA_HT01_PowChkTO + i, &nIOStatus);
				m_dbWarnHiLim = READ_ANALOG(PRMA_HT01_PowChkWrn + i, &nIOStatus);
				m_dbAbortHiLim = READ_ANALOG(PRMA_HT01_PowChkAbt + i, &nIOStatus);

				if (m_dbCurrPow >= m_dbWarnHiLim)
				{
					if (limitCnt[i] >= m_dbTimeCount * 60) // min
					{
						if (m_dbCurrPow >= m_dbAbortHiLim)
						{
							// Abort Alarm
							// ALARM_HTR01_POW_TO_ABOT
							if(ALM_PAUSED != ALARM_STATUS(ALARM_HTR01_POW_TO_ABOT + i)) //.... 2018.06.17 ghlee
							{
								ALARM_POST(ALARM_HTR01_POW_TO_ABOT + i);
							}
							printf("[Abort] Heater[%d] Power Abort CurrPower [%f] ParmPower [%f] \n", i+1, m_dbCurrPow, m_dbAbortHiLim);
							WRITE_DIGITAL(HT01_OnOff + i, HTR_OFF, &nIOStatus);
							WRITE_DIGITAL(SW_GbIntlckDM, INTLK_ON, &nIOStatus);
						}
						// Warning Alarm
						// ALARM_HTR01_POW_TO_WARN
						if(ALM_PAUSED != ALARM_STATUS(ALARM_HTR01_POW_TO_WARN + i)
							&& ALM_PAUSED != ALARM_STATUS(ALARM_HTR01_POW_TO_ABOT + i)) //.... 2018.06.17 ghlee
						{
							ALARM_POST(ALARM_HTR01_POW_TO_WARN + i);
							printf("[Warning] Heater[%d] Power Warning CurrPower [%f] ParmPower [%f] \n", i+1, m_dbCurrPow, m_dbWarnHiLim);
						}
					}

					limitCnt[i]++;
					if(ALM_PAUSED != ALARM_STATUS(ALARM_HTR01_POW_TO_WARN + i)
						&& ALM_PAUSED != ALARM_STATUS(ALARM_HTR01_POW_TO_ABOT + i)) //.... 2018.06.17 ghlee
					{
						printf("[%s] Heater[%d] Power Check!!! CurrPower [%f] ParmPower [%f] \n",FUNC_NAME, i+1, m_dbCurrPow, m_dbWarnHiLim);
					}
				}
				else
				{
					limitCnt[i] = 0;
				}
			}
		}
		Sleep(1000);
	}
	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main()
{
	Module_Status msRet = SYS_SUCCESS;

	Sleep(10000);

	printf("%s Start..\n", FUNC_NAME);

	_beginthread((_THREAD_FUNC)LineHeater_PowCheck, 0, NULL);

	return msRet;
}

//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code()
{
	int CommStatus;

	if (READ_DIGITAL(IO_Driver_Loding, &CommStatus) == 0 )	SIMULATION_MODE = TRUE;
	else													SIMULATION_MODE = FALSE;

	return TRUE;

}
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no )
{

}
} // End of [extern "C"]
//==========================================================================================
