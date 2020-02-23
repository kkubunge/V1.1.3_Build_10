///////////////////////////////////////////////////////////////////////////////
// Heater Monitor
// Create : 2004.08.18

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

#include "DynamicIO.h"

#define FUNC_NAME	"HeaterMonitor"
#define MAX_HEATER	32


//------------------------------------------------------------------------------------------
// Alarm Definition(2200 ~ 2399)

#define ALARM_HTR01_TEMP_OUT_OF_DEV			4001	// 4001 ~ 4100 : Heater 01 ~ 64 Alarm
#define ALARM_HTR01_TEMP_HIGH_LIMIT			4101	// 4101 ~ 4200 : Heater 01 ~ 64 Alarm
#define ALARM_HTR01_TEMP_LOW_LIMIT			4201	// 4201 ~ 4300 : Heater 01 ~ 64 Alarm
#define ALARM_HTR01_CONTROL_ERROR			4301	// 4301 ~ 4400 : Heater 01 ~ 64 Alarm
#define ALARM_HTR01_TURNED_OFF				4401	// 4401 ~ 4500 : Heater 01 ~ 64 Alarm
#define ALARM_MLS300_HEATER_INIT_FAILED		1300
#define ALARM_WATLOW300_COMM_ERROR			9102
#define ALARM_HTR_AUTO_OFF					4800

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "SW_GbIntlckDM"	, _K_D_IO	,	SW_GbIntlckDM,	0	} ,
	{ "SW_ModuleFailDM"	, _K_D_IO	,	SW_ModuleFailDM,	0	} ,

	{ "HT_CmdDO",			_K_D_IO	,	HT_CmdDO, 0},
	{ "HT_CommStsDI",		_K_D_IO	,	HT_CommStsDI, 0},

	{ "HT01_OnOffDIO"	, _K_D_IO	,	HT01_OnOff,	0	} ,
	{ "HT02_OnOffDIO"	, _K_D_IO	,	HT02_OnOff,	0	} ,
	{ "HT03_OnOffDIO"	, _K_D_IO	,	HT03_OnOff,	0	} ,
	{ "HT04_OnOffDIO"	, _K_D_IO	,	HT04_OnOff,	0	} ,
	{ "HT05_OnOffDIO"	, _K_D_IO	,	HT05_OnOff,	0	} ,
	{ "HT06_OnOffDIO"	, _K_D_IO	,	HT06_OnOff,	0	} ,
	{ "HT07_OnOffDIO"	, _K_D_IO	,	HT07_OnOff,	0	} ,
	{ "HT08_OnOffDIO"	, _K_D_IO	,	HT08_OnOff,	0	} ,
	{ "HT09_OnOffDIO"	, _K_D_IO	,	HT09_OnOff,	0	} ,
	{ "HT10_OnOffDIO"	, _K_D_IO	,	HT10_OnOff,	0	} ,
	{ "HT11_OnOffDIO"	, _K_D_IO	,	HT11_OnOff,	0	} ,
	{ "HT12_OnOffDIO"	, _K_D_IO	,	HT12_OnOff,	0	} ,
	{ "HT13_OnOffDIO"	, _K_D_IO	,	HT13_OnOff,	0	} ,
	{ "HT14_OnOffDIO"	, _K_D_IO	,	HT14_OnOff,	0	} ,
	{ "HT15_OnOffDIO"	, _K_D_IO	,	HT15_OnOff,	0	} ,
	{ "HT16_OnOffDIO"	, _K_D_IO	,	HT16_OnOff,	0	} ,
	{ "HT17_OnOffDIO"	, _K_D_IO	,	HT17_OnOff,	0	} ,
	{ "HT18_OnOffDIO"	, _K_D_IO	,	HT18_OnOff,	0	} ,
	{ "HT19_OnOffDIO"	, _K_D_IO	,	HT19_OnOff,	0	} ,
	{ "HT20_OnOffDIO"	, _K_D_IO	,	HT20_OnOff,	0	} ,
	{ "HT21_OnOffDIO"	, _K_D_IO	,	HT21_OnOff,	0	} ,
	{ "HT22_OnOffDIO"	, _K_D_IO	,	HT22_OnOff,	0	} ,
	{ "HT23_OnOffDIO"	, _K_D_IO	,	HT23_OnOff,	0	} ,
	{ "HT24_OnOffDIO"	, _K_D_IO	,	HT24_OnOff,	0	} ,
	{ "HT25_OnOffDIO"	, _K_D_IO	,	HT25_OnOff,	0	} ,
	{ "HT26_OnOffDIO"	, _K_D_IO	,	HT26_OnOff,	0	} ,
	{ "HT27_OnOffDIO"	, _K_D_IO	,	HT27_OnOff,	0	} ,
	{ "HT28_OnOffDIO"	, _K_D_IO	,	HT28_OnOff,	0	} ,
	{ "HT29_OnOffDIO"	, _K_D_IO	,	HT29_OnOff,	0	} ,
	{ "HT30_OnOffDIO"	, _K_D_IO	,	HT30_OnOff,	0	} ,
	{ "HT31_OnOffDIO"	, _K_D_IO	,	HT31_OnOff,	0	} ,
	{ "HT32_OnOffDIO"	, _K_D_IO	,	HT32_OnOff,	0	} ,

	{ "HT01_AlrmDscDI"	, _K_D_IO	,	HT01_AlrmDscDI,	0	} ,
	{ "HT02_AlrmDscDI"	, _K_D_IO	,	HT02_AlrmDscDI,	0	} ,
	{ "HT03_AlrmDscDI"	, _K_D_IO	,	HT03_AlrmDscDI,	0	} ,
	{ "HT04_AlrmDscDI"	, _K_D_IO	,	HT04_AlrmDscDI,	0	} ,
	{ "HT05_AlrmDscDI"	, _K_D_IO	,	HT05_AlrmDscDI,	0	} ,
	{ "HT06_AlrmDscDI"	, _K_D_IO	,	HT06_AlrmDscDI,	0	} ,
	{ "HT07_AlrmDscDI"	, _K_D_IO	,	HT07_AlrmDscDI,	0	} ,
	{ "HT08_AlrmDscDI"	, _K_D_IO	,	HT08_AlrmDscDI,	0	} ,
	{ "HT09_AlrmDscDI"	, _K_D_IO	,	HT09_AlrmDscDI,	0	} ,
	{ "HT10_AlrmDscDI"	, _K_D_IO	,	HT10_AlrmDscDI,	0	} ,
	{ "HT11_AlrmDscDI"	, _K_D_IO	,	HT11_AlrmDscDI,	0	} ,
	{ "HT12_AlrmDscDI"	, _K_D_IO	,	HT12_AlrmDscDI,	0	} ,
	{ "HT13_AlrmDscDI"	, _K_D_IO	,	HT13_AlrmDscDI,	0	} ,
	{ "HT14_AlrmDscDI"	, _K_D_IO	,	HT14_AlrmDscDI,	0	} ,
	{ "HT15_AlrmDscDI"	, _K_D_IO	,	HT15_AlrmDscDI,	0	} ,
	{ "HT16_AlrmDscDI"	, _K_D_IO	,	HT16_AlrmDscDI,	0	} ,
	{ "HT17_AlrmDscDI"	, _K_D_IO	,	HT17_AlrmDscDI,	0	} ,
	{ "HT18_AlrmDscDI"	, _K_D_IO	,	HT18_AlrmDscDI,	0	} ,
	{ "HT19_AlrmDscDI"	, _K_D_IO	,	HT19_AlrmDscDI,	0	} ,
	{ "HT20_AlrmDscDI"	, _K_D_IO	,	HT20_AlrmDscDI,	0	} ,
	{ "HT21_AlrmDscDI"	, _K_D_IO	,	HT21_AlrmDscDI,	0	} ,
	{ "HT22_AlrmDscDI"	, _K_D_IO	,	HT22_AlrmDscDI,	0	} ,
	{ "HT23_AlrmDscDI"	, _K_D_IO	,	HT23_AlrmDscDI,	0	} ,
	{ "HT24_AlrmDscDI"	, _K_D_IO	,	HT24_AlrmDscDI,	0	} ,
	{ "HT25_AlrmDscDI"	, _K_D_IO	,	HT25_AlrmDscDI,	0	} ,
	{ "HT26_AlrmDscDI"	, _K_D_IO	,	HT26_AlrmDscDI,	0	} ,
	{ "HT27_AlrmDscDI"	, _K_D_IO	,	HT27_AlrmDscDI,	0	} ,
	{ "HT28_AlrmDscDI"	, _K_D_IO	,	HT28_AlrmDscDI,	0	} ,
	{ "HT29_AlrmDscDI"	, _K_D_IO	,	HT29_AlrmDscDI,	0	} ,
	{ "HT30_AlrmDscDI"	, _K_D_IO	,	HT30_AlrmDscDI,	0	} ,
	{ "HT31_AlrmDscDI"	, _K_D_IO	,	HT31_AlrmDscDI,	0	} ,
	{ "HT32_AlrmDscDI"	, _K_D_IO	,	HT32_AlrmDscDI,	0	} ,

	{ "HT01_MonOptDM"	, _K_D_IO	,	HT01_MonOptDM,	0	} ,
	{ "HT02_MonOptDM"	, _K_D_IO	,	HT02_MonOptDM,	0	} ,
	{ "HT03_MonOptDM"	, _K_D_IO	,	HT03_MonOptDM,	0	} ,
	{ "HT04_MonOptDM"	, _K_D_IO	,	HT04_MonOptDM,	0	} ,
	{ "HT05_MonOptDM"	, _K_D_IO	,	HT05_MonOptDM,	0	} ,
	{ "HT06_MonOptDM"	, _K_D_IO	,	HT06_MonOptDM,	0	} ,
	{ "HT07_MonOptDM"	, _K_D_IO	,	HT07_MonOptDM,	0	} ,
	{ "HT08_MonOptDM"	, _K_D_IO	,	HT08_MonOptDM,	0	} ,
	{ "HT09_MonOptDM"	, _K_D_IO	,	HT09_MonOptDM,	0	} ,
	{ "HT10_MonOptDM"	, _K_D_IO	,	HT10_MonOptDM,	0	} ,
	{ "HT11_MonOptDM"	, _K_D_IO	,	HT11_MonOptDM,	0	} ,
	{ "HT12_MonOptDM"	, _K_D_IO	,	HT12_MonOptDM,	0	} ,
	{ "HT13_MonOptDM"	, _K_D_IO	,	HT13_MonOptDM,	0	} ,
	{ "HT14_MonOptDM"	, _K_D_IO	,	HT14_MonOptDM,	0	} ,
	{ "HT15_MonOptDM"	, _K_D_IO	,	HT15_MonOptDM,	0	} ,
	{ "HT16_MonOptDM"	, _K_D_IO	,	HT16_MonOptDM,	0	} ,
	{ "HT17_MonOptDM"	, _K_D_IO	,	HT17_MonOptDM,	0	} ,
	{ "HT18_MonOptDM"	, _K_D_IO	,	HT18_MonOptDM,	0	} ,
	{ "HT19_MonOptDM"	, _K_D_IO	,	HT19_MonOptDM,	0	} ,
	{ "HT20_MonOptDM"	, _K_D_IO	,	HT20_MonOptDM,	0	} ,
	{ "HT21_MonOptDM"	, _K_D_IO	,	HT21_MonOptDM,	0	} ,
	{ "HT22_MonOptDM"	, _K_D_IO	,	HT22_MonOptDM,	0	} ,
	{ "HT23_MonOptDM"	, _K_D_IO	,	HT23_MonOptDM,	0	} ,
	{ "HT24_MonOptDM"	, _K_D_IO	,	HT24_MonOptDM,	0	} ,
	{ "HT25_MonOptDM"	, _K_D_IO	,	HT25_MonOptDM,	0	} ,
	{ "HT26_MonOptDM"	, _K_D_IO	,	HT26_MonOptDM,	0	} ,
	{ "HT27_MonOptDM"	, _K_D_IO	,	HT27_MonOptDM,	0	} ,
	{ "HT28_MonOptDM"	, _K_D_IO	,	HT28_MonOptDM,	0	} ,
	{ "HT29_MonOptDM"	, _K_D_IO	,	HT29_MonOptDM,	0	} ,
	{ "HT30_MonOptDM"	, _K_D_IO	,	HT30_MonOptDM,	0	} ,
	{ "HT31_MonOptDM"	, _K_D_IO	,	HT31_MonOptDM,	0	} ,
	{ "HT32_MonOptDM"	, _K_D_IO	,	HT32_MonOptDM,	0	} ,

	{ "HT01_UseDM"	, _K_D_IO	,	HT01_UseDM,	0	} ,
	{ "HT02_UseDM"	, _K_D_IO	,	HT02_UseDM,	0	} ,
	{ "HT03_UseDM"	, _K_D_IO	,	HT03_UseDM,	0	} ,
	{ "HT04_UseDM"	, _K_D_IO	,	HT04_UseDM,	0	} ,
	{ "HT05_UseDM"	, _K_D_IO	,	HT05_UseDM,	0	} ,
	{ "HT06_UseDM"	, _K_D_IO	,	HT06_UseDM,	0	} ,
	{ "HT07_UseDM"	, _K_D_IO	,	HT07_UseDM,	0	} ,
	{ "HT08_UseDM"	, _K_D_IO	,	HT08_UseDM,	0	} ,
	{ "HT09_UseDM"	, _K_D_IO	,	HT09_UseDM,	0	} ,
	{ "HT10_UseDM"	, _K_D_IO	,	HT10_UseDM,	0	} ,
	{ "HT11_UseDM"	, _K_D_IO	,	HT11_UseDM,	0	} ,
	{ "HT12_UseDM"	, _K_D_IO	,	HT12_UseDM,	0	} ,
	{ "HT13_UseDM"	, _K_D_IO	,	HT13_UseDM,	0	} ,
	{ "HT14_UseDM"	, _K_D_IO	,	HT14_UseDM,	0	} ,
	{ "HT15_UseDM"	, _K_D_IO	,	HT15_UseDM,	0	} ,
	{ "HT16_UseDM"	, _K_D_IO	,	HT16_UseDM,	0	} ,
	{ "HT17_UseDM"	, _K_D_IO	,	HT17_UseDM,	0	} ,
	{ "HT18_UseDM"	, _K_D_IO	,	HT18_UseDM,	0	} ,
	{ "HT19_UseDM"	, _K_D_IO	,	HT19_UseDM,	0	} ,
	{ "HT20_UseDM"	, _K_D_IO	,	HT20_UseDM,	0	} ,
	{ "HT21_UseDM"	, _K_D_IO	,	HT21_UseDM,	0	} ,
	{ "HT22_UseDM"	, _K_D_IO	,	HT22_UseDM,	0	} ,
	{ "HT23_UseDM"	, _K_D_IO	,	HT23_UseDM,	0	} ,
	{ "HT24_UseDM"	, _K_D_IO	,	HT24_UseDM,	0	} ,
	{ "HT25_UseDM"	, _K_D_IO	,	HT25_UseDM,	0	} ,
	{ "HT26_UseDM"	, _K_D_IO	,	HT26_UseDM,	0	} ,
	{ "HT27_UseDM"	, _K_D_IO	,	HT27_UseDM,	0	} ,
	{ "HT28_UseDM"	, _K_D_IO	,	HT28_UseDM,	0	} ,
	{ "HT29_UseDM"	, _K_D_IO	,	HT29_UseDM,	0	} ,
	{ "HT30_UseDM"	, _K_D_IO	,	HT30_UseDM,	0	} ,
	{ "HT31_UseDM"	, _K_D_IO	,	HT31_UseDM,	0	} ,
	{ "HT32_UseDM"	, _K_D_IO	,	HT32_UseDM,	0	} ,

	{ "HT01_CtrlMd2DM"	, _K_D_IO	,	HT01_CtrlMd2DM,	0	} ,
	{ "HT02_CtrlMd2DM"	, _K_D_IO	,	HT02_CtrlMd2DM,	0	} ,
	{ "HT03_CtrlMd2DM"	, _K_D_IO	,	HT03_CtrlMd2DM,	0	} ,
	{ "HT04_CtrlMd2DM"	, _K_D_IO	,	HT04_CtrlMd2DM,	0	} ,
	{ "HT05_CtrlMd2DM"	, _K_D_IO	,	HT05_CtrlMd2DM,	0	} ,
	{ "HT06_CtrlMd2DM"	, _K_D_IO	,	HT06_CtrlMd2DM,	0	} ,
	{ "HT07_CtrlMd2DM"	, _K_D_IO	,	HT07_CtrlMd2DM,	0	} ,
	{ "HT08_CtrlMd2DM"	, _K_D_IO	,	HT08_CtrlMd2DM,	0	} ,
	{ "HT09_CtrlMd2DM"	, _K_D_IO	,	HT09_CtrlMd2DM,	0	} ,
	{ "HT10_CtrlMd2DM"	, _K_D_IO	,	HT10_CtrlMd2DM,	0	} ,
	{ "HT11_CtrlMd2DM"	, _K_D_IO	,	HT11_CtrlMd2DM,	0	} ,
	{ "HT12_CtrlMd2DM"	, _K_D_IO	,	HT12_CtrlMd2DM,	0	} ,
	{ "HT13_CtrlMd2DM"	, _K_D_IO	,	HT13_CtrlMd2DM,	0	} ,
	{ "HT14_CtrlMd2DM"	, _K_D_IO	,	HT14_CtrlMd2DM,	0	} ,
	{ "HT15_CtrlMd2DM"	, _K_D_IO	,	HT15_CtrlMd2DM,	0	} ,
	{ "HT16_CtrlMd2DM"	, _K_D_IO	,	HT16_CtrlMd2DM,	0	} ,
	{ "HT17_CtrlMd2DM"	, _K_D_IO	,	HT17_CtrlMd2DM,	0	} ,
	{ "HT18_CtrlMd2DM"	, _K_D_IO	,	HT18_CtrlMd2DM,	0	} ,
	{ "HT19_CtrlMd2DM"	, _K_D_IO	,	HT19_CtrlMd2DM,	0	} ,
	{ "HT20_CtrlMd2DM"	, _K_D_IO	,	HT20_CtrlMd2DM,	0	} ,
	{ "HT21_CtrlMd2DM"	, _K_D_IO	,	HT21_CtrlMd2DM,	0	} ,
	{ "HT22_CtrlMd2DM"	, _K_D_IO	,	HT22_CtrlMd2DM,	0	} ,
	{ "HT23_CtrlMd2DM"	, _K_D_IO	,	HT23_CtrlMd2DM,	0	} ,
	{ "HT24_CtrlMd2DM"	, _K_D_IO	,	HT24_CtrlMd2DM,	0	} ,
	{ "HT25_CtrlMd2DM"	, _K_D_IO	,	HT25_CtrlMd2DM,	0	} ,
	{ "HT26_CtrlMd2DM"	, _K_D_IO	,	HT26_CtrlMd2DM,	0	} ,
	{ "HT27_CtrlMd2DM"	, _K_D_IO	,	HT27_CtrlMd2DM,	0	} ,
	{ "HT28_CtrlMd2DM"	, _K_D_IO	,	HT28_CtrlMd2DM,	0	} ,
	{ "HT29_CtrlMd2DM"	, _K_D_IO	,	HT29_CtrlMd2DM,	0	} ,
	{ "HT30_CtrlMd2DM"	, _K_D_IO	,	HT30_CtrlMd2DM,	0	} ,
	{ "HT31_CtrlMd2DM"	, _K_D_IO	,	HT31_CtrlMd2DM,	0	} ,
	{ "HT32_CtrlMd2DM"	, _K_D_IO	,	HT32_CtrlMd2DM,	0	} ,

	{ "HT01_OutPwAI"	, _K_A_IO	,	HT01_OutPwAI,	0	} ,
	{ "HT02_OutPwAI"	, _K_A_IO	,	HT02_OutPwAI,	0	} ,
	{ "HT03_OutPwAI"	, _K_A_IO	,	HT03_OutPwAI,	0	} ,
	{ "HT04_OutPwAI"	, _K_A_IO	,	HT04_OutPwAI,	0	} ,
	{ "HT05_OutPwAI"	, _K_A_IO	,	HT05_OutPwAI,	0	} ,
	{ "HT06_OutPwAI"	, _K_A_IO	,	HT06_OutPwAI,	0	} ,
	{ "HT07_OutPwAI"	, _K_A_IO	,	HT07_OutPwAI,	0	} ,
	{ "HT08_OutPwAI"	, _K_A_IO	,	HT08_OutPwAI,	0	} ,
	{ "HT09_OutPwAI"	, _K_A_IO	,	HT09_OutPwAI,	0	} ,
	{ "HT10_OutPwAI"	, _K_A_IO	,	HT10_OutPwAI,	0	} ,
	{ "HT11_OutPwAI"	, _K_A_IO	,	HT11_OutPwAI,	0	} ,
	{ "HT12_OutPwAI"	, _K_A_IO	,	HT12_OutPwAI,	0	} ,
	{ "HT13_OutPwAI"	, _K_A_IO	,	HT13_OutPwAI,	0	} ,
	{ "HT14_OutPwAI"	, _K_A_IO	,	HT14_OutPwAI,	0	} ,
	{ "HT15_OutPwAI"	, _K_A_IO	,	HT15_OutPwAI,	0	} ,
	{ "HT16_OutPwAI"	, _K_A_IO	,	HT16_OutPwAI,	0	} ,
	{ "HT17_OutPwAI"	, _K_A_IO	,	HT17_OutPwAI,	0	} ,
	{ "HT18_OutPwAI"	, _K_A_IO	,	HT18_OutPwAI,	0	} ,
	{ "HT19_OutPwAI"	, _K_A_IO	,	HT19_OutPwAI,	0	} ,
	{ "HT20_OutPwAI"	, _K_A_IO	,	HT20_OutPwAI,	0	} ,
	{ "HT21_OutPwAI"	, _K_A_IO	,	HT21_OutPwAI,	0	} ,
	{ "HT22_OutPwAI"	, _K_A_IO	,	HT22_OutPwAI,	0	} ,
	{ "HT23_OutPwAI"	, _K_A_IO	,	HT23_OutPwAI,	0	} ,
	{ "HT24_OutPwAI"	, _K_A_IO	,	HT24_OutPwAI,	0	} ,
	{ "HT25_OutPwAI"	, _K_A_IO	,	HT25_OutPwAI,	0	} ,
	{ "HT26_OutPwAI"	, _K_A_IO	,	HT26_OutPwAI,	0	} ,
	{ "HT27_OutPwAI"	, _K_A_IO	,	HT27_OutPwAI,	0	} ,
	{ "HT28_OutPwAI"	, _K_A_IO	,	HT28_OutPwAI,	0	} ,
	{ "HT29_OutPwAI"	, _K_A_IO	,	HT29_OutPwAI,	0	} ,
	{ "HT30_OutPwAI"	, _K_A_IO	,	HT30_OutPwAI,	0	} ,
	{ "HT31_OutPwAI"	, _K_A_IO	,	HT31_OutPwAI,	0	} ,
	{ "HT32_OutPwAI"	, _K_A_IO	,	HT32_OutPwAI,	0	} ,

	{ "PRMA_HTR_PowSet"	, _K_A_IO	,	PRMA_HTR_PowSet,	0	} ,
	//... 2018.06.17 if Heater channel is picking up Power, it turn off Heater Ch.
	{ "HT01_AutoOffDM"	, _K_D_IO	,	HT01_AutoOffDM,	0	} ,
	{ "HT02_AutoOffDM"	, _K_D_IO	,	HT02_AutoOffDM,	0	} ,
	{ "HT03_AutoOffDM"	, _K_D_IO	,	HT03_AutoOffDM,	0	} ,
	{ "HT04_AutoOffDM"	, _K_D_IO	,	HT04_AutoOffDM,	0	} ,
	{ "HT05_AutoOffDM"	, _K_D_IO	,	HT05_AutoOffDM,	0	} ,
	{ "HT06_AutoOffDM"	, _K_D_IO	,	HT06_AutoOffDM,	0	} ,
	{ "HT07_AutoOffDM"	, _K_D_IO	,	HT07_AutoOffDM,	0	} ,
	{ "HT08_AutoOffDM"	, _K_D_IO	,	HT08_AutoOffDM,	0	} ,
	{ "HT09_AutoOffDM"	, _K_D_IO	,	HT09_AutoOffDM,	0	} ,
	{ "HT10_AutoOffDM"	, _K_D_IO	,	HT10_AutoOffDM,	0	} ,
	{ "HT11_AutoOffDM"	, _K_D_IO	,	HT11_AutoOffDM,	0	} ,
	{ "HT12_AutoOffDM"	, _K_D_IO	,	HT12_AutoOffDM,	0	} ,
	{ "HT13_AutoOffDM"	, _K_D_IO	,	HT13_AutoOffDM,	0	} ,
	{ "HT14_AutoOffDM"	, _K_D_IO	,	HT14_AutoOffDM,	0	} ,
	{ "HT15_AutoOffDM"	, _K_D_IO	,	HT15_AutoOffDM,	0	} ,
	{ "HT16_AutoOffDM"	, _K_D_IO	,	HT16_AutoOffDM,	0	} ,
	{ "HT17_AutoOffDM"	, _K_D_IO	,	HT17_AutoOffDM,	0	} ,
	{ "HT18_AutoOffDM"	, _K_D_IO	,	HT18_AutoOffDM,	0	} ,
	{ "HT19_AutoOffDM"	, _K_D_IO	,	HT19_AutoOffDM,	0	} ,
	{ "HT20_AutoOffDM"	, _K_D_IO	,	HT20_AutoOffDM,	0	} ,
	{ "HT21_AutoOffDM"	, _K_D_IO	,	HT21_AutoOffDM,	0	} ,
	{ "HT22_AutoOffDM"	, _K_D_IO	,	HT22_AutoOffDM,	0	} ,
	{ "HT23_AutoOffDM"	, _K_D_IO	,	HT23_AutoOffDM,	0	} ,
	{ "HT24_AutoOffDM"	, _K_D_IO	,	HT24_AutoOffDM,	0	} ,
	{ "HT25_AutoOffDM"	, _K_D_IO	,	HT25_AutoOffDM,	0	} ,
	{ "HT26_AutoOffDM"	, _K_D_IO	,	HT26_AutoOffDM,	0	} ,
	{ "HT27_AutoOffDM"	, _K_D_IO	,	HT27_AutoOffDM,	0	} ,
	{ "HT28_AutoOffDM"	, _K_D_IO	,	HT28_AutoOffDM,	0	} ,
	{ "HT29_AutoOffDM"	, _K_D_IO	,	HT29_AutoOffDM,	0	} ,
	{ "HT30_AutoOffDM"	, _K_D_IO	,	HT30_AutoOffDM,	0	} ,
	{ "HT31_AutoOffDM"	, _K_D_IO	,	HT31_AutoOffDM,	0	} ,
	{ "HT32_AutoOffDM"	, _K_D_IO	,	HT32_AutoOffDM,	0	} ,
	""
};

enum HeaterUpDnSvcEnum{
	// added by ycchoi 2006.05.19 < 1. 0. 0. 7 >
	HT01_UpDnStsDM,
	HT02_UpDnStsDM,
	HT03_UpDnStsDM,
	HT04_UpDnStsDM,
	HT05_UpDnStsDM,
	HT06_UpDnStsDM,
	HT07_UpDnStsDM,
	HT08_UpDnStsDM,
	HT09_UpDnStsDM,
	HT10_UpDnStsDM,
	HT11_UpDnStsDM,
	HT12_UpDnStsDM,
	HT13_UpDnStsDM,
	HT14_UpDnStsDM,
	HT15_UpDnStsDM,
	HT16_UpDnStsDM,
	HT17_UpDnStsDM,
	HT18_UpDnStsDM,
	HT19_UpDnStsDM,
	HT20_UpDnStsDM,
	HT21_UpDnStsDM,
	HT22_UpDnStsDM,
	HT23_UpDnStsDM,
	HT24_UpDnStsDM,
	HT25_UpDnStsDM,
	HT26_UpDnStsDM,
	HT27_UpDnStsDM,
	HT28_UpDnStsDM,
	HT29_UpDnStsDM,
	HT30_UpDnStsDM,
	HT31_UpDnStsDM,
	HT32_UpDnStsDM,

	HT01_SV,
	HT02_SV,
	HT03_SV,
	HT04_SV,
	HT05_SV,
	HT06_SV,
	HT07_SV,
	HT08_SV,
	HT09_SV,
	HT10_SV,
	HT11_SV,
	HT12_SV,
	HT13_SV,
	HT14_SV,
	HT15_SV,
	HT16_SV,
	HT17_SV,
	HT18_SV,
	HT19_SV,
	HT20_SV,
	HT21_SV,
	HT22_SV,
	HT23_SV,
	HT24_SV,
	HT25_SV,
	HT26_SV,
	HT27_SV,
	HT28_SV,
	HT29_SV,
	HT30_SV,
	HT31_SV,
	HT32_SV,

	HT01_PV,
	HT02_PV,
	HT03_PV,
	HT04_PV,
	HT05_PV,
	HT06_PV,
	HT07_PV,
	HT08_PV,
	HT09_PV,
	HT10_PV,
	HT11_PV,
	HT12_PV,
	HT13_PV,
	HT14_PV,
	HT15_PV,
	HT16_PV,
	HT17_PV,
	HT18_PV,
	HT19_PV,
	HT20_PV,
	HT21_PV,
	HT22_PV,
	HT23_PV,
	HT24_PV,
	HT25_PV,
	HT26_PV,
	HT27_PV,
	HT28_PV,
	HT29_PV,
	HT30_PV,
	HT31_PV,
	HT32_PV,

	PRO_FlowControlDM,
};

IO_LIST HeaterUpDnSvc_IO_List[] = {
	// added by ycchoi 2006.05.19 < 1. 0. 0. 7 >
	{ "HT01_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT02_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT03_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT04_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT05_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT06_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT07_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT08_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT09_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT10_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT11_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT12_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT13_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT14_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT15_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT16_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT17_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT18_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT19_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT20_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT21_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT22_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT23_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT24_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT25_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT26_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT27_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT28_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT29_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT30_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT31_UpDnStsDM"	, _K_D_IO	,0	} ,
	{ "HT32_UpDnStsDM"	, _K_D_IO	,0	} ,

	{ "HT01_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT02_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT03_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT04_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT05_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT06_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT07_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT08_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT09_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT10_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT11_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT12_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT13_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT14_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT15_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT16_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT17_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT18_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT19_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT20_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT21_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT22_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT23_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT24_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT25_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT26_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT27_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT28_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT29_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT30_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT31_TmpSpAIO"	, _K_A_IO	,0	} ,
	{ "HT32_TmpSpAIO"	, _K_A_IO	,0	} ,

	{ "HT01_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT02_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT03_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT04_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT05_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT06_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT07_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT08_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT09_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT10_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT11_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT12_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT13_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT14_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT15_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT16_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT17_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT18_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT19_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT20_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT21_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT22_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT23_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT24_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT25_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT26_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT27_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT28_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT29_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT30_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT31_TmpRdAI"	, _K_A_IO	,0	} ,
	{ "HT32_TmpRdAI"	, _K_A_IO	,0	} ,

	// 2006.06.30
	{"PRO_FlowControlDM", _K_D_IO, 0},
	""
};

//--------------------------------------------------------------------
// 2006.06.30
CDynamicIO g_DynIO;

// added by ycchoi 2006.05.22 < 1.0.0.7 >
enum HEATER_UP_DN_STAT { HUDS_STABLE, HUDS_UP, HUDS_DN };

//------------------------------------------------------------------------------------------
int g_nHtrCommErrRetry		= 5;
int g_nWatlow300CommErrCnt	= 0;
BOOL g_bWatlow300DriverMon		= TRUE;

//------------------------------------------------------------------------------------------
// Thread Type Definition for Using thread
typedef void(__cdecl * _THREAD_FUNC)(void* lpParam);
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void ApplyAlarmEffect(int nMonOpt)
{
	// 2006.06.30
	enum { ALM_IGNORE, ALM_WARING, ALM_CHMDIS, ALM_ABORT, ALM_RETRY };
	enum { PRO_ABORT, PRO_RETRY };
	enum { INTLK_OFF, INTLK_ON };

	int nIOStatus;
	switch(nMonOpt)
	{
	case ALM_IGNORE:
	case ALM_WARING:
		break;
	case ALM_CHMDIS:
		WRITE_DIGITAL(SW_ModuleFailDM, INTLK_ON, &nIOStatus);
		break;
	case ALM_ABORT:
		WRITE_DIGITAL(SW_GbIntlckDM, INTLK_ON, &nIOStatus);
	case ALM_RETRY:
		g_DynIO.dWRITE_DIGITAL(PRO_FlowControlDM, PRO_RETRY, &nIOStatus);
		WRITE_DIGITAL(SW_GbIntlckDM, INTLK_ON, &nIOStatus);
		break;
	}
}

///------------------------------------------------------------------------------------------
// Initialize Heater

BOOL InitMLS300HeaterChannel()
{
	BOOL bRet = TRUE;
	int nIOStatus;
	int nAlarmRecovery;

	do {
		WRITE_DIGITAL(HT_CmdDO, 0, &nIOStatus);		// 0:Refresh All Channel
		if(nIOStatus == FALSE) bRet = FALSE;
		else bRet = TRUE;
		if(bRet == FALSE)
		{
			printf("MLS300 32Channel Init Failed.\n");
			nAlarmRecovery = ALARM_MANAGE(ALARM_MLS300_HEATER_INIT_FAILED);
			if(nAlarmRecovery == ALM_IGNORE) {bRet = TRUE; break; }
			if(nAlarmRecovery != ALM_RETRY) break;
		}
		else break;
	} while(1);
	if(bRet == FALSE)
	{
		printf("-----> MLS300 Heater Initialize failed in %s\n", FUNC_NAME);
	}
	return bRet;
}

///------------------------------------------------------------------------------------------
void Watlow300CommStsMonThread()
{
	int nValue = 0;
	int nIOStatus;

	WAIT_SECONDS(5);
	InitMLS300HeaterChannel();
	WAIT_SECONDS(5);
	while(1)
	{
		do{
			nValue = READ_DIGITAL(HT_CommStsDI, &nIOStatus);		// 0:OffLine 1:OnLine
			if(nValue == 1)
			{
				g_nWatlow300CommErrCnt = 0;
				break;
			}
			else
			{
				g_nWatlow300CommErrCnt++;
				if(g_nWatlow300CommErrCnt > g_nHtrCommErrRetry)
				{
					// Set Process Disable Interlock
					WRITE_DIGITAL(SW_GbIntlckDM, 1, &nIOStatus);
					if(ALM_PAUSED != ALARM_STATUS(ALARM_WATLOW300_COMM_ERROR))
						ALARM_POST(ALARM_WATLOW300_COMM_ERROR);
					g_nWatlow300CommErrCnt = 0;
				}
			}
			//WAIT_SECONDS(1);
			//... 2017.03.07
			Sleep(1000);

		}while(1);

		//WAIT_SECONDS(2);
		//... 2017.03.07
		Sleep(2000);
	}
	_endthread();
}

void ExcuteHeaterChCtrl(int HTRno)				//2017. 09. 14 by kimhc
{
	int nIOStatus;
	enum { HTR_OFF, HTR_ON };

	//printf("Heater number is %d, PRMA_HTR_PowSet is %d, HT12_OutPwAI is %d. \n", HTRno, (int)READ_ANALOG(PRMA_HTR_PowSet, &nIOStatus), (int)READ_ANALOG(HT12_OutPwAI, &nIOStatus));

     if(READ_ANALOG(PRMA_HTR_PowSet, &nIOStatus) <= 0) return;

	if(READ_ANALOG(PRMA_HTR_PowSet, &nIOStatus) < READ_ANALOG(HT01_OutPwAI+HTRno, &nIOStatus))
	{
		if(ALM_PAUSED != ALARM_STATUS(ALARM_HTR_AUTO_OFF)) //.... 2018.06.17 ghlee
		{
			ALARM_POST(ALARM_HTR_AUTO_OFF);
		}
		WRITE_DIGITAL(HT01_OnOff+HTRno, HTR_OFF, &nIOStatus);
	}
}

///------------------------------------------------------------------------------------------
Module_Status StartMonitor(char *pParam)
{
	// 2006.07.11
	enum { HA_SPARE0, HA_SPARE1, HA_LOWDEV, HA_HIGHDEV, HA_LOWLMT, HA_HIGHLMT,
		HA_TCREV, HA_TCSHORT, HA_TCBREAK,
		HA_RTDOPEN, HA_RTDSHORT, HA_SPARE11, HA_SPARE12,
		HA_FCALERR, HA_OCALERR, HA_NORMAL
	};
	enum { HTR_OFF, HTR_ON };
	enum { HTR_NOTUSE, HTR_USE };
	enum { HCTRL_STABLE, HCTRL_SETTEMP, HCTRL_UNSTABLE };

	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus;
	int i;
	int nAlarm;
	int nMonOpt;
	int nCtrlMod;
	int nOnOff;
	int pHTR_nOnOff[MAX_HEATER];
	int pHTR_nErrCnt[MAX_HEATER];
	char szCmd[256], szSubParam[256];
	int nAutoOffCtrl = HTR_NOTUSE;

	// added by ycchoi 2006.05.22 < 1.0.0.7 >
	BOOL bHeaterUpDnSvc = FALSE;
	int nHeaterUpDnSvcTolerance = 30;
	double dbSV, dbPV;
	HEATER_UP_DN_STAT enHtSts = HUDS_STABLE;

	for(i=0; i<MAX_HEATER; i++)
	{
		pHTR_nOnOff[i] = 0;
		pHTR_nErrCnt[i] = 0;
	}

	WAIT_SECONDS(3);
	if(g_bWatlow300DriverMon) _beginthread((_THREAD_FUNC) Watlow300CommStsMonThread, 0, NULL);
	WAIT_SECONDS(3);

	// added by ycchoi 2006.05.22 < 1.0.0.7 >
	if(strlen(pParam))
	{
		STR_SEPERATE_CHAR(pParam , ':', szCmd, szSubParam, 255 );
		if(STRCMP(szCmd, "HeaterUpDnSvc")){
			nHeaterUpDnSvcTolerance = atoi(szSubParam);
			if(nHeaterUpDnSvcTolerance <= 0) nHeaterUpDnSvcTolerance = 30;
			bHeaterUpDnSvc = TRUE;
		}
	}

	while(1)
	{
		for(i=0; i<MAX_HEATER; i++)
		{
			nOnOff = READ_DIGITAL(HT01_OnOff + i, &nIOStatus);	// 0:Off 1:On
			nAutoOffCtrl = READ_DIGITAL(HT01_AutoOffDM + i, &nIOStatus);	// 0:Off 1:On

			// this condition is requirement. don't touch.
			//  1. When SW initialize and 'nOnOff' of HT01_OnOff is '0'(Off), HT01 is not monitored.
			if(HTR_USE == READ_DIGITAL(HT01_UseDM + i, &nIOStatus)	// 0:NotUse 1:Use
				&& (pHTR_nOnOff[i] == HTR_ON || nOnOff == HTR_ON))
			{
				// Monitoring Option => 0:Ignore 1:Warning 2:ChmDis 3:Abort
				nMonOpt = READ_DIGITAL(HT01_MonOptDM + i, &nIOStatus);

				// on --> off
				if(pHTR_nOnOff[i] == HTR_ON && nOnOff == HTR_OFF)
				{
					if(nMonOpt > 0)
					{
						if(ALM_PAUSED != ALARM_STATUS(ALARM_HTR01_TURNED_OFF + i))
							ALARM_POST(ALARM_HTR01_TURNED_OFF + i);
					}
					ApplyAlarmEffect(nMonOpt);
					if(++pHTR_nErrCnt[i] > 2)
					{
						pHTR_nOnOff[i] = nOnOff;
						pHTR_nErrCnt[i] = 0;

						// added by ycchoi 2006.05.22 < 1.0.0.7 >
						// On --> Off 전환시 Heater Up/Dn상태를 Stable로 표시한다.
						if(bHeaterUpDnSvc)
						{
							g_DynIO.dWRITE_DIGITAL(HT01_UpDnStsDM+i, HUDS_STABLE, &nIOStatus);
						}
					}
				}
				else pHTR_nOnOff[i] = nOnOff;

				nAlarm = READ_DIGITAL(HT01_AlrmDscDI + i, &nIOStatus);
				nCtrlMod = READ_DIGITAL(HT01_CtrlMd2DM + i, &nIOStatus);

				if(nAlarm == HA_LOWDEV || nAlarm == HA_HIGHDEV)
				{
					if(nMonOpt > 0 && nCtrlMod == HCTRL_STABLE)
					{
						if(ALM_PAUSED != ALARM_STATUS(ALARM_HTR01_TEMP_OUT_OF_DEV + i))
							ALARM_POST(ALARM_HTR01_TEMP_OUT_OF_DEV + i);
					}
				}
				else if(nAlarm == HA_LOWLMT)
				{
					if(nMonOpt > 0 && nCtrlMod == HCTRL_STABLE)
					{
						if(ALM_PAUSED != ALARM_STATUS(ALARM_HTR01_TEMP_LOW_LIMIT + i))
							ALARM_POST(ALARM_HTR01_TEMP_LOW_LIMIT + i);
					}
					//... 2018.06.17
					if( nAutoOffCtrl == HTR_USE )	ExcuteHeaterChCtrl(i);
					//...
					ApplyAlarmEffect(nMonOpt);
				}
				else if(nAlarm == HA_HIGHLMT)
				{
					if(nMonOpt > 0 && nCtrlMod == HCTRL_STABLE)
					{
						if(ALM_PAUSED != ALARM_STATUS(ALARM_HTR01_TEMP_HIGH_LIMIT + i))
							ALARM_POST(ALARM_HTR01_TEMP_HIGH_LIMIT + i);
					}
					ApplyAlarmEffect(nMonOpt);
				}
				else if(nAlarm == HA_TCREV || nAlarm == HA_TCSHORT || nAlarm == HA_TCBREAK
					|| nAlarm == HA_RTDOPEN || nAlarm == HA_RTDSHORT
					|| nAlarm == HA_FCALERR || nAlarm == HA_OCALERR)
				{
					if(nMonOpt > 0)
					{
						if(ALM_PAUSED != ALARM_STATUS(ALARM_HTR01_CONTROL_ERROR + i))
							ALARM_POST(ALARM_HTR01_CONTROL_ERROR + i);
					}
					ApplyAlarmEffect(nMonOpt);
				}

				// added by ycchoi 2006.05.22 < 1.0.0.7 >
				if(bHeaterUpDnSvc && pHTR_nOnOff[i])	// 0:Off 1:On
				{
					dbPV = g_DynIO.dREAD_ANALOG(HT01_PV+i, &nIOStatus);
					dbSV = g_DynIO.dREAD_ANALOG(HT01_SV+i, &nIOStatus);
					if(dbPV > dbSV + nHeaterUpDnSvcTolerance)		g_DynIO.dWRITE_DIGITAL(HT01_UpDnStsDM+i, HUDS_DN, &nIOStatus);
					else if(dbPV < dbSV - nHeaterUpDnSvcTolerance)	g_DynIO.dWRITE_DIGITAL(HT01_UpDnStsDM+i, HUDS_UP, &nIOStatus);
					else											g_DynIO.dWRITE_DIGITAL(HT01_UpDnStsDM+i, HUDS_STABLE, &nIOStatus);
				}
			}
			/*
			if(FALSE == WAIT_SECONDS(0.2))
			{
				msRet = SYS_ABORTED;
				break;
			}
			*/
			Sleep(200);
		}
		if(msRet != SYS_SUCCESS) break;
	}
	return msRet;
}

//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;
	int nParLen = 0;
	char *pParam;
//	char szCmd[41], szSubParam[256];

	printf("Entered %s\n", FUNC_NAME);
	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

//		STR_SEPERATE(pParam , szCmd, szSubParam, 255 );

		msRet = StartMonitor(pParam);
	} while(0);

	printf("Leave %s\n", FUNC_NAME);
	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	g_DynIO.InitIOList(HeaterUpDnSvc_IO_List, false);
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}

} // End of [extern "C"]
//==========================================================================================
