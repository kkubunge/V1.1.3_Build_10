///////////////////////////////////////////////////////////////////////////////
// StgHtr Monitor
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

#include "TextParser.h"
#define FUNC_NAME "HeaterControl"

//------------------------------------------------------------------------------------------
// Alarm Definition( StgHtr_Control.Alarm 	2700 ~ 2799)
// Alarm Definition( HTR_Control.Alarm 		4100 - 4600)
#define HTRAutoSetMaint 4500
#define HTRAutoSetVacuum 4501
#define HTRAutoSetPramSetFailed 4502
#define HTRAutoSetPramReadFailed 4503
//------------------------------------------------------------------------------------------
// Data Type

#define MAX_HEATER 32
#define SETTING_ITEM 8
typedef struct _HEATER_STRUCT
{
	double dbSp;
	int nOpt; // 0:NotSet 1:Set
	BOOL bCheck;
} HEATER_STRUCT;
HEATER_STRUCT g_HtrList[MAX_HEATER];

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {
	{"HT01_TmpRdAI", _K_A_IO, HT01_TmpRdAI, 0},
	{"HT02_TmpRdAI", _K_A_IO, HT02_TmpRdAI, 0},
	{"HT03_TmpRdAI", _K_A_IO, HT03_TmpRdAI, 0},
	{"HT04_TmpRdAI", _K_A_IO, HT04_TmpRdAI, 0},
	{"HT05_TmpRdAI", _K_A_IO, HT05_TmpRdAI, 0},
	{"HT06_TmpRdAI", _K_A_IO, HT06_TmpRdAI, 0},
	{"HT07_TmpRdAI", _K_A_IO, HT07_TmpRdAI, 0},
	{"HT08_TmpRdAI", _K_A_IO, HT08_TmpRdAI, 0},
	{"HT09_TmpRdAI", _K_A_IO, HT09_TmpRdAI, 0},
	{"HT10_TmpRdAI", _K_A_IO, HT10_TmpRdAI, 0},
	{"HT11_TmpRdAI", _K_A_IO, HT11_TmpRdAI, 0},
	{"HT12_TmpRdAI", _K_A_IO, HT12_TmpRdAI, 0},
	{"HT13_TmpRdAI", _K_A_IO, HT13_TmpRdAI, 0},
	{"HT14_TmpRdAI", _K_A_IO, HT14_TmpRdAI, 0},
	{"HT15_TmpRdAI", _K_A_IO, HT15_TmpRdAI, 0},
	{"HT16_TmpRdAI", _K_A_IO, HT16_TmpRdAI, 0},
	{"HT17_TmpRdAI", _K_A_IO, HT17_TmpRdAI, 0},
	{"HT18_TmpRdAI", _K_A_IO, HT18_TmpRdAI, 0},
	{"HT19_TmpRdAI", _K_A_IO, HT19_TmpRdAI, 0},
	{"HT20_TmpRdAI", _K_A_IO, HT20_TmpRdAI, 0},
	{"HT21_TmpRdAI", _K_A_IO, HT21_TmpRdAI, 0},
	{"HT22_TmpRdAI", _K_A_IO, HT22_TmpRdAI, 0},
	{"HT23_TmpRdAI", _K_A_IO, HT23_TmpRdAI, 0},
	{"HT24_TmpRdAI", _K_A_IO, HT24_TmpRdAI, 0},
	{"HT25_TmpRdAI", _K_A_IO, HT25_TmpRdAI, 0},
	{"HT26_TmpRdAI", _K_A_IO, HT26_TmpRdAI, 0},
	{"HT27_TmpRdAI", _K_A_IO, HT27_TmpRdAI, 0},
	{"HT28_TmpRdAI", _K_A_IO, HT28_TmpRdAI, 0},
	{"HT29_TmpRdAI", _K_A_IO, HT29_TmpRdAI, 0},
	{"HT30_TmpRdAI", _K_A_IO, HT30_TmpRdAI, 0},
	{"HT31_TmpRdAI", _K_A_IO, HT31_TmpRdAI, 0},
	{"HT32_TmpRdAI", _K_A_IO, HT32_TmpRdAI, 0},

	{"HT01_TmpSpAIO", _K_A_IO, HT01_TmpSpAIO, 0},
	{"HT02_TmpSpAIO", _K_A_IO, HT02_TmpSpAIO, 0},
	{"HT03_TmpSpAIO", _K_A_IO, HT03_TmpSpAIO, 0},
	{"HT04_TmpSpAIO", _K_A_IO, HT04_TmpSpAIO, 0},
	{"HT05_TmpSpAIO", _K_A_IO, HT05_TmpSpAIO, 0},
	{"HT06_TmpSpAIO", _K_A_IO, HT06_TmpSpAIO, 0},
	{"HT07_TmpSpAIO", _K_A_IO, HT07_TmpSpAIO, 0},
	{"HT08_TmpSpAIO", _K_A_IO, HT08_TmpSpAIO, 0},
	{"HT09_TmpSpAIO", _K_A_IO, HT09_TmpSpAIO, 0},
	{"HT10_TmpSpAIO", _K_A_IO, HT10_TmpSpAIO, 0},
	{"HT11_TmpSpAIO", _K_A_IO, HT11_TmpSpAIO, 0},
	{"HT12_TmpSpAIO", _K_A_IO, HT12_TmpSpAIO, 0},
	{"HT13_TmpSpAIO", _K_A_IO, HT13_TmpSpAIO, 0},
	{"HT14_TmpSpAIO", _K_A_IO, HT14_TmpSpAIO, 0},
	{"HT15_TmpSpAIO", _K_A_IO, HT15_TmpSpAIO, 0},
	{"HT16_TmpSpAIO", _K_A_IO, HT16_TmpSpAIO, 0},
	{"HT17_TmpSpAIO", _K_A_IO, HT17_TmpSpAIO, 0},
	{"HT18_TmpSpAIO", _K_A_IO, HT18_TmpSpAIO, 0},
	{"HT19_TmpSpAIO", _K_A_IO, HT19_TmpSpAIO, 0},
	{"HT20_TmpSpAIO", _K_A_IO, HT20_TmpSpAIO, 0},
	{"HT21_TmpSpAIO", _K_A_IO, HT21_TmpSpAIO, 0},
	{"HT22_TmpSpAIO", _K_A_IO, HT22_TmpSpAIO, 0},
	{"HT23_TmpSpAIO", _K_A_IO, HT23_TmpSpAIO, 0},
	{"HT24_TmpSpAIO", _K_A_IO, HT24_TmpSpAIO, 0},
	{"HT25_TmpSpAIO", _K_A_IO, HT25_TmpSpAIO, 0},
	{"HT26_TmpSpAIO", _K_A_IO, HT26_TmpSpAIO, 0},
	{"HT27_TmpSpAIO", _K_A_IO, HT27_TmpSpAIO, 0},
	{"HT28_TmpSpAIO", _K_A_IO, HT28_TmpSpAIO, 0},
	{"HT29_TmpSpAIO", _K_A_IO, HT29_TmpSpAIO, 0},
	{"HT30_TmpSpAIO", _K_A_IO, HT30_TmpSpAIO, 0},
	{"HT31_TmpSpAIO", _K_A_IO, HT31_TmpSpAIO, 0},
	{"HT32_TmpSpAIO", _K_A_IO, HT32_TmpSpAIO, 0},

	{"HT01_CtrlMdDIO", _K_D_IO, HT01_CtrlMdDIO, 0},
	{"HT02_CtrlMdDIO", _K_D_IO, HT02_CtrlMdDIO, 0},
	{"HT03_CtrlMdDIO", _K_D_IO, HT03_CtrlMdDIO, 0},
	{"HT04_CtrlMdDIO", _K_D_IO, HT04_CtrlMdDIO, 0},
	{"HT05_CtrlMdDIO", _K_D_IO, HT05_CtrlMdDIO, 0},
	{"HT06_CtrlMdDIO", _K_D_IO, HT06_CtrlMdDIO, 0},
	{"HT07_CtrlMdDIO", _K_D_IO, HT07_CtrlMdDIO, 0},
	{"HT08_CtrlMdDIO", _K_D_IO, HT08_CtrlMdDIO, 0},
	{"HT09_CtrlMdDIO", _K_D_IO, HT09_CtrlMdDIO, 0},
	{"HT10_CtrlMdDIO", _K_D_IO, HT10_CtrlMdDIO, 0},
	{"HT11_CtrlMdDIO", _K_D_IO, HT11_CtrlMdDIO, 0},
	{"HT12_CtrlMdDIO", _K_D_IO, HT12_CtrlMdDIO, 0},
	{"HT13_CtrlMdDIO", _K_D_IO, HT13_CtrlMdDIO, 0},
	{"HT14_CtrlMdDIO", _K_D_IO, HT14_CtrlMdDIO, 0},
	{"HT15_CtrlMdDIO", _K_D_IO, HT15_CtrlMdDIO, 0},
	{"HT16_CtrlMdDIO", _K_D_IO, HT16_CtrlMdDIO, 0},
	{"HT17_CtrlMdDIO", _K_D_IO, HT17_CtrlMdDIO, 0},
	{"HT18_CtrlMdDIO", _K_D_IO, HT18_CtrlMdDIO, 0},
	{"HT19_CtrlMdDIO", _K_D_IO, HT19_CtrlMdDIO, 0},
	{"HT20_CtrlMdDIO", _K_D_IO, HT20_CtrlMdDIO, 0},
	{"HT21_CtrlMdDIO", _K_D_IO, HT21_CtrlMdDIO, 0},
	{"HT22_CtrlMdDIO", _K_D_IO, HT22_CtrlMdDIO, 0},
	{"HT23_CtrlMdDIO", _K_D_IO, HT23_CtrlMdDIO, 0},
	{"HT24_CtrlMdDIO", _K_D_IO, HT24_CtrlMdDIO, 0},
	{"HT25_CtrlMdDIO", _K_D_IO, HT25_CtrlMdDIO, 0},
	{"HT26_CtrlMdDIO", _K_D_IO, HT26_CtrlMdDIO, 0},
	{"HT27_CtrlMdDIO", _K_D_IO, HT27_CtrlMdDIO, 0},
	{"HT28_CtrlMdDIO", _K_D_IO, HT28_CtrlMdDIO, 0},
	{"HT29_CtrlMdDIO", _K_D_IO, HT29_CtrlMdDIO, 0},
	{"HT30_CtrlMdDIO", _K_D_IO, HT30_CtrlMdDIO, 0},
	{"HT31_CtrlMdDIO", _K_D_IO, HT31_CtrlMdDIO, 0},
	{"HT32_CtrlMdDIO", _K_D_IO, HT32_CtrlMdDIO, 0},

	{"HT01_DeviAIO", _K_A_IO, HT01_DeviAIO, 0},
	{"HT02_DeviAIO", _K_A_IO, HT02_DeviAIO, 0},
	{"HT03_DeviAIO", _K_A_IO, HT03_DeviAIO, 0},
	{"HT04_DeviAIO", _K_A_IO, HT04_DeviAIO, 0},
	{"HT05_DeviAIO", _K_A_IO, HT05_DeviAIO, 0},
	{"HT06_DeviAIO", _K_A_IO, HT06_DeviAIO, 0},
	{"HT07_DeviAIO", _K_A_IO, HT07_DeviAIO, 0},
	{"HT08_DeviAIO", _K_A_IO, HT08_DeviAIO, 0},
	{"HT09_DeviAIO", _K_A_IO, HT09_DeviAIO, 0},
	{"HT10_DeviAIO", _K_A_IO, HT10_DeviAIO, 0},
	{"HT11_DeviAIO", _K_A_IO, HT11_DeviAIO, 0},
	{"HT12_DeviAIO", _K_A_IO, HT12_DeviAIO, 0},
	{"HT13_DeviAIO", _K_A_IO, HT13_DeviAIO, 0},
	{"HT14_DeviAIO", _K_A_IO, HT14_DeviAIO, 0},
	{"HT15_DeviAIO", _K_A_IO, HT15_DeviAIO, 0},
	{"HT16_DeviAIO", _K_A_IO, HT16_DeviAIO, 0},
	{"HT17_DeviAIO", _K_A_IO, HT17_DeviAIO, 0},
	{"HT18_DeviAIO", _K_A_IO, HT18_DeviAIO, 0},
	{"HT19_DeviAIO", _K_A_IO, HT19_DeviAIO, 0},
	{"HT20_DeviAIO", _K_A_IO, HT20_DeviAIO, 0},
	{"HT21_DeviAIO", _K_A_IO, HT21_DeviAIO, 0},
	{"HT22_DeviAIO", _K_A_IO, HT22_DeviAIO, 0},
	{"HT23_DeviAIO", _K_A_IO, HT23_DeviAIO, 0},
	{"HT24_DeviAIO", _K_A_IO, HT24_DeviAIO, 0},
	{"HT25_DeviAIO", _K_A_IO, HT25_DeviAIO, 0},
	{"HT26_DeviAIO", _K_A_IO, HT26_DeviAIO, 0},
	{"HT27_DeviAIO", _K_A_IO, HT27_DeviAIO, 0},
	{"HT28_DeviAIO", _K_A_IO, HT28_DeviAIO, 0},
	{"HT29_DeviAIO", _K_A_IO, HT29_DeviAIO, 0},
	{"HT30_DeviAIO", _K_A_IO, HT30_DeviAIO, 0},
	{"HT31_DeviAIO", _K_A_IO, HT31_DeviAIO, 0},
	{"HT32_DeviAIO", _K_A_IO, HT32_DeviAIO, 0},

	{"HT01_CtrlMd2DM", _K_D_IO, HT01_CtrlMd2DM, 0},
	{"HT02_CtrlMd2DM", _K_D_IO, HT02_CtrlMd2DM, 0},
	{"HT03_CtrlMd2DM", _K_D_IO, HT03_CtrlMd2DM, 0},
	{"HT04_CtrlMd2DM", _K_D_IO, HT04_CtrlMd2DM, 0},
	{"HT05_CtrlMd2DM", _K_D_IO, HT05_CtrlMd2DM, 0},
	{"HT06_CtrlMd2DM", _K_D_IO, HT06_CtrlMd2DM, 0},
	{"HT07_CtrlMd2DM", _K_D_IO, HT07_CtrlMd2DM, 0},
	{"HT08_CtrlMd2DM", _K_D_IO, HT08_CtrlMd2DM, 0},
	{"HT09_CtrlMd2DM", _K_D_IO, HT09_CtrlMd2DM, 0},
	{"HT10_CtrlMd2DM", _K_D_IO, HT10_CtrlMd2DM, 0},
	{"HT11_CtrlMd2DM", _K_D_IO, HT11_CtrlMd2DM, 0},
	{"HT12_CtrlMd2DM", _K_D_IO, HT12_CtrlMd2DM, 0},
	{"HT13_CtrlMd2DM", _K_D_IO, HT13_CtrlMd2DM, 0},
	{"HT14_CtrlMd2DM", _K_D_IO, HT14_CtrlMd2DM, 0},
	{"HT15_CtrlMd2DM", _K_D_IO, HT15_CtrlMd2DM, 0},
	{"HT16_CtrlMd2DM", _K_D_IO, HT16_CtrlMd2DM, 0},
	{"HT17_CtrlMd2DM", _K_D_IO, HT17_CtrlMd2DM, 0},
	{"HT18_CtrlMd2DM", _K_D_IO, HT18_CtrlMd2DM, 0},
	{"HT19_CtrlMd2DM", _K_D_IO, HT19_CtrlMd2DM, 0},
	{"HT20_CtrlMd2DM", _K_D_IO, HT20_CtrlMd2DM, 0},
	{"HT21_CtrlMd2DM", _K_D_IO, HT21_CtrlMd2DM, 0},
	{"HT22_CtrlMd2DM", _K_D_IO, HT22_CtrlMd2DM, 0},
	{"HT23_CtrlMd2DM", _K_D_IO, HT23_CtrlMd2DM, 0},
	{"HT24_CtrlMd2DM", _K_D_IO, HT24_CtrlMd2DM, 0},
	{"HT25_CtrlMd2DM", _K_D_IO, HT25_CtrlMd2DM, 0},
	{"HT26_CtrlMd2DM", _K_D_IO, HT26_CtrlMd2DM, 0},
	{"HT27_CtrlMd2DM", _K_D_IO, HT27_CtrlMd2DM, 0},
	{"HT28_CtrlMd2DM", _K_D_IO, HT28_CtrlMd2DM, 0},
	{"HT29_CtrlMd2DM", _K_D_IO, HT29_CtrlMd2DM, 0},
	{"HT30_CtrlMd2DM", _K_D_IO, HT30_CtrlMd2DM, 0},
	{"HT31_CtrlMd2DM", _K_D_IO, HT31_CtrlMd2DM, 0},
	{"HT32_CtrlMd2DM", _K_D_IO, HT32_CtrlMd2DM, 0},

	{"HT01_TmpHgLmtAIO", _K_A_IO, HT01_TmpHgLmtAIO, 0},
	{"HT02_TmpHgLmtAIO", _K_A_IO, HT02_TmpHgLmtAIO, 0},
	{"HT03_TmpHgLmtAIO", _K_A_IO, HT03_TmpHgLmtAIO, 0},
	{"HT04_TmpHgLmtAIO", _K_A_IO, HT04_TmpHgLmtAIO, 0},
	{"HT05_TmpHgLmtAIO", _K_A_IO, HT05_TmpHgLmtAIO, 0},
	{"HT06_TmpHgLmtAIO", _K_A_IO, HT06_TmpHgLmtAIO, 0},
	{"HT07_TmpHgLmtAIO", _K_A_IO, HT07_TmpHgLmtAIO, 0},
	{"HT08_TmpHgLmtAIO", _K_A_IO, HT08_TmpHgLmtAIO, 0},
	{"HT09_TmpHgLmtAIO", _K_A_IO, HT09_TmpHgLmtAIO, 0},
	{"HT10_TmpHgLmtAIO", _K_A_IO, HT10_TmpHgLmtAIO, 0},
	{"HT11_TmpHgLmtAIO", _K_A_IO, HT11_TmpHgLmtAIO, 0},
	{"HT12_TmpHgLmtAIO", _K_A_IO, HT12_TmpHgLmtAIO, 0},
	{"HT13_TmpHgLmtAIO", _K_A_IO, HT13_TmpHgLmtAIO, 0},
	{"HT14_TmpHgLmtAIO", _K_A_IO, HT14_TmpHgLmtAIO, 0},
	{"HT15_TmpHgLmtAIO", _K_A_IO, HT15_TmpHgLmtAIO, 0},
	{"HT16_TmpHgLmtAIO", _K_A_IO, HT16_TmpHgLmtAIO, 0},
	{"HT17_TmpHgLmtAIO", _K_A_IO, HT17_TmpHgLmtAIO, 0},
	{"HT18_TmpHgLmtAIO", _K_A_IO, HT18_TmpHgLmtAIO, 0},
	{"HT19_TmpHgLmtAIO", _K_A_IO, HT19_TmpHgLmtAIO, 0},
	{"HT20_TmpHgLmtAIO", _K_A_IO, HT20_TmpHgLmtAIO, 0},
	{"HT21_TmpHgLmtAIO", _K_A_IO, HT21_TmpHgLmtAIO, 0},
	{"HT22_TmpHgLmtAIO", _K_A_IO, HT22_TmpHgLmtAIO, 0},
	{"HT23_TmpHgLmtAIO", _K_A_IO, HT23_TmpHgLmtAIO, 0},
	{"HT24_TmpHgLmtAIO", _K_A_IO, HT24_TmpHgLmtAIO, 0},
	{"HT25_TmpHgLmtAIO", _K_A_IO, HT25_TmpHgLmtAIO, 0},
	{"HT26_TmpHgLmtAIO", _K_A_IO, HT26_TmpHgLmtAIO, 0},
	{"HT27_TmpHgLmtAIO", _K_A_IO, HT27_TmpHgLmtAIO, 0},
	{"HT28_TmpHgLmtAIO", _K_A_IO, HT28_TmpHgLmtAIO, 0},
	{"HT29_TmpHgLmtAIO", _K_A_IO, HT29_TmpHgLmtAIO, 0},
	{"HT30_TmpHgLmtAIO", _K_A_IO, HT30_TmpHgLmtAIO, 0},
	{"HT31_TmpHgLmtAIO", _K_A_IO, HT31_TmpHgLmtAIO, 0},
	{"HT32_TmpHgLmtAIO", _K_A_IO, HT32_TmpHgLmtAIO, 0},

	{"HT01_TmpLwLmtAIO", _K_A_IO, HT01_TmpLwLmtAIO, 0},
	{"HT02_TmpLwLmtAIO", _K_A_IO, HT02_TmpLwLmtAIO, 0},
	{"HT03_TmpLwLmtAIO", _K_A_IO, HT03_TmpLwLmtAIO, 0},
	{"HT04_TmpLwLmtAIO", _K_A_IO, HT04_TmpLwLmtAIO, 0},
	{"HT05_TmpLwLmtAIO", _K_A_IO, HT05_TmpLwLmtAIO, 0},
	{"HT06_TmpLwLmtAIO", _K_A_IO, HT06_TmpLwLmtAIO, 0},
	{"HT07_TmpLwLmtAIO", _K_A_IO, HT07_TmpLwLmtAIO, 0},
	{"HT08_TmpLwLmtAIO", _K_A_IO, HT08_TmpLwLmtAIO, 0},
	{"HT09_TmpLwLmtAIO", _K_A_IO, HT09_TmpLwLmtAIO, 0},
	{"HT10_TmpLwLmtAIO", _K_A_IO, HT10_TmpLwLmtAIO, 0},
	{"HT11_TmpLwLmtAIO", _K_A_IO, HT11_TmpLwLmtAIO, 0},
	{"HT12_TmpLwLmtAIO", _K_A_IO, HT12_TmpLwLmtAIO, 0},
	{"HT13_TmpLwLmtAIO", _K_A_IO, HT13_TmpLwLmtAIO, 0},
	{"HT14_TmpLwLmtAIO", _K_A_IO, HT14_TmpLwLmtAIO, 0},
	{"HT15_TmpLwLmtAIO", _K_A_IO, HT15_TmpLwLmtAIO, 0},
	{"HT16_TmpLwLmtAIO", _K_A_IO, HT16_TmpLwLmtAIO, 0},
	{"HT17_TmpLwLmtAIO", _K_A_IO, HT17_TmpLwLmtAIO, 0},
	{"HT18_TmpLwLmtAIO", _K_A_IO, HT18_TmpLwLmtAIO, 0},
	{"HT19_TmpLwLmtAIO", _K_A_IO, HT19_TmpLwLmtAIO, 0},
	{"HT20_TmpLwLmtAIO", _K_A_IO, HT20_TmpLwLmtAIO, 0},
	{"HT21_TmpLwLmtAIO", _K_A_IO, HT21_TmpLwLmtAIO, 0},
	{"HT22_TmpLwLmtAIO", _K_A_IO, HT22_TmpLwLmtAIO, 0},
	{"HT23_TmpLwLmtAIO", _K_A_IO, HT23_TmpLwLmtAIO, 0},
	{"HT24_TmpLwLmtAIO", _K_A_IO, HT24_TmpLwLmtAIO, 0},
	{"HT25_TmpLwLmtAIO", _K_A_IO, HT25_TmpLwLmtAIO, 0},
	{"HT26_TmpLwLmtAIO", _K_A_IO, HT26_TmpLwLmtAIO, 0},
	{"HT27_TmpLwLmtAIO", _K_A_IO, HT27_TmpLwLmtAIO, 0},
	{"HT28_TmpLwLmtAIO", _K_A_IO, HT28_TmpLwLmtAIO, 0},
	{"HT29_TmpLwLmtAIO", _K_A_IO, HT29_TmpLwLmtAIO, 0},
	{"HT30_TmpLwLmtAIO", _K_A_IO, HT30_TmpLwLmtAIO, 0},
	{"HT31_TmpLwLmtAIO", _K_A_IO, HT31_TmpLwLmtAIO, 0},
	{"HT32_TmpLwLmtAIO", _K_A_IO, HT32_TmpLwLmtAIO, 0},

	{"HT01_OutPwAO", _K_A_IO, HT01_OutPwAO, 0},
	{"HT02_OutPwAO", _K_A_IO, HT02_OutPwAO, 0},
	{"HT03_OutPwAO", _K_A_IO, HT03_OutPwAO, 0},
	{"HT04_OutPwAO", _K_A_IO, HT04_OutPwAO, 0},
	{"HT05_OutPwAO", _K_A_IO, HT05_OutPwAO, 0},
	{"HT06_OutPwAO", _K_A_IO, HT06_OutPwAO, 0},
	{"HT07_OutPwAO", _K_A_IO, HT07_OutPwAO, 0},
	{"HT08_OutPwAO", _K_A_IO, HT08_OutPwAO, 0},
	{"HT09_OutPwAO", _K_A_IO, HT09_OutPwAO, 0},
	{"HT10_OutPwAO", _K_A_IO, HT10_OutPwAO, 0},
	{"HT11_OutPwAO", _K_A_IO, HT11_OutPwAO, 0},
	{"HT12_OutPwAO", _K_A_IO, HT12_OutPwAO, 0},
	{"HT13_OutPwAO", _K_A_IO, HT13_OutPwAO, 0},
	{"HT14_OutPwAO", _K_A_IO, HT14_OutPwAO, 0},
	{"HT15_OutPwAO", _K_A_IO, HT15_OutPwAO, 0},
	{"HT16_OutPwAO", _K_A_IO, HT16_OutPwAO, 0},
	{"HT17_OutPwAO", _K_A_IO, HT17_OutPwAO, 0},
	{"HT18_OutPwAO", _K_A_IO, HT18_OutPwAO, 0},
	{"HT19_OutPwAO", _K_A_IO, HT19_OutPwAO, 0},
	{"HT20_OutPwAO", _K_A_IO, HT20_OutPwAO, 0},
	{"HT21_OutPwAO", _K_A_IO, HT21_OutPwAO, 0},
	{"HT22_OutPwAO", _K_A_IO, HT22_OutPwAO, 0},
	{"HT23_OutPwAO", _K_A_IO, HT23_OutPwAO, 0},
	{"HT24_OutPwAO", _K_A_IO, HT24_OutPwAO, 0},
	{"HT25_OutPwAO", _K_A_IO, HT25_OutPwAO, 0},
	{"HT26_OutPwAO", _K_A_IO, HT26_OutPwAO, 0},
	{"HT27_OutPwAO", _K_A_IO, HT27_OutPwAO, 0},
	{"HT28_OutPwAO", _K_A_IO, HT28_OutPwAO, 0},
	{"HT29_OutPwAO", _K_A_IO, HT29_OutPwAO, 0},
	{"HT30_OutPwAO", _K_A_IO, HT30_OutPwAO, 0},
	{"HT31_OutPwAO", _K_A_IO, HT31_OutPwAO, 0},
	{"HT32_OutPwAO", _K_A_IO, HT32_OutPwAO, 0},

	{"HT01_OutPwLmtAIO", _K_A_IO, HT01_OutPwLmtAIO, 0},
	{"HT02_OutPwLmtAIO", _K_A_IO, HT02_OutPwLmtAIO, 0},
	{"HT03_OutPwLmtAIO", _K_A_IO, HT03_OutPwLmtAIO, 0},
	{"HT04_OutPwLmtAIO", _K_A_IO, HT04_OutPwLmtAIO, 0},
	{"HT05_OutPwLmtAIO", _K_A_IO, HT05_OutPwLmtAIO, 0},
	{"HT06_OutPwLmtAIO", _K_A_IO, HT06_OutPwLmtAIO, 0},
	{"HT07_OutPwLmtAIO", _K_A_IO, HT07_OutPwLmtAIO, 0},
	{"HT08_OutPwLmtAIO", _K_A_IO, HT08_OutPwLmtAIO, 0},
	{"HT09_OutPwLmtAIO", _K_A_IO, HT09_OutPwLmtAIO, 0},
	{"HT10_OutPwLmtAIO", _K_A_IO, HT10_OutPwLmtAIO, 0},
	{"HT11_OutPwLmtAIO", _K_A_IO, HT11_OutPwLmtAIO, 0},
	{"HT12_OutPwLmtAIO", _K_A_IO, HT12_OutPwLmtAIO, 0},
	{"HT13_OutPwLmtAIO", _K_A_IO, HT13_OutPwLmtAIO, 0},
	{"HT14_OutPwLmtAIO", _K_A_IO, HT14_OutPwLmtAIO, 0},
	{"HT15_OutPwLmtAIO", _K_A_IO, HT15_OutPwLmtAIO, 0},
	{"HT16_OutPwLmtAIO", _K_A_IO, HT16_OutPwLmtAIO, 0},
	{"HT17_OutPwLmtAIO", _K_A_IO, HT17_OutPwLmtAIO, 0},
	{"HT18_OutPwLmtAIO", _K_A_IO, HT18_OutPwLmtAIO, 0},
	{"HT19_OutPwLmtAIO", _K_A_IO, HT19_OutPwLmtAIO, 0},
	{"HT20_OutPwLmtAIO", _K_A_IO, HT20_OutPwLmtAIO, 0},
	{"HT21_OutPwLmtAIO", _K_A_IO, HT21_OutPwLmtAIO, 0},
	{"HT22_OutPwLmtAIO", _K_A_IO, HT22_OutPwLmtAIO, 0},
	{"HT23_OutPwLmtAIO", _K_A_IO, HT23_OutPwLmtAIO, 0},
	{"HT24_OutPwLmtAIO", _K_A_IO, HT24_OutPwLmtAIO, 0},
	{"HT25_OutPwLmtAIO", _K_A_IO, HT25_OutPwLmtAIO, 0},
	{"HT26_OutPwLmtAIO", _K_A_IO, HT26_OutPwLmtAIO, 0},
	{"HT27_OutPwLmtAIO", _K_A_IO, HT27_OutPwLmtAIO, 0},
	{"HT28_OutPwLmtAIO", _K_A_IO, HT28_OutPwLmtAIO, 0},
	{"HT29_OutPwLmtAIO", _K_A_IO, HT29_OutPwLmtAIO, 0},
	{"HT30_OutPwLmtAIO", _K_A_IO, HT30_OutPwLmtAIO, 0},
	{"HT31_OutPwLmtAIO", _K_A_IO, HT31_OutPwLmtAIO, 0},
	{"HT32_OutPwLmtAIO", _K_A_IO, HT32_OutPwLmtAIO, 0},

	{"HT01_CtrlGnAIO", _K_A_IO, HT01_CtrlGnAIO, 0},
	{"HT02_CtrlGnAIO", _K_A_IO, HT02_CtrlGnAIO, 0},
	{"HT03_CtrlGnAIO", _K_A_IO, HT03_CtrlGnAIO, 0},
	{"HT04_CtrlGnAIO", _K_A_IO, HT04_CtrlGnAIO, 0},
	{"HT05_CtrlGnAIO", _K_A_IO, HT05_CtrlGnAIO, 0},
	{"HT06_CtrlGnAIO", _K_A_IO, HT06_CtrlGnAIO, 0},
	{"HT07_CtrlGnAIO", _K_A_IO, HT07_CtrlGnAIO, 0},
	{"HT08_CtrlGnAIO", _K_A_IO, HT08_CtrlGnAIO, 0},
	{"HT09_CtrlGnAIO", _K_A_IO, HT09_CtrlGnAIO, 0},
	{"HT10_CtrlGnAIO", _K_A_IO, HT10_CtrlGnAIO, 0},
	{"HT11_CtrlGnAIO", _K_A_IO, HT11_CtrlGnAIO, 0},
	{"HT12_CtrlGnAIO", _K_A_IO, HT12_CtrlGnAIO, 0},
	{"HT13_CtrlGnAIO", _K_A_IO, HT13_CtrlGnAIO, 0},
	{"HT14_CtrlGnAIO", _K_A_IO, HT14_CtrlGnAIO, 0},
	{"HT15_CtrlGnAIO", _K_A_IO, HT15_CtrlGnAIO, 0},
	{"HT16_CtrlGnAIO", _K_A_IO, HT16_CtrlGnAIO, 0},
	{"HT17_CtrlGnAIO", _K_A_IO, HT17_CtrlGnAIO, 0},
	{"HT18_CtrlGnAIO", _K_A_IO, HT18_CtrlGnAIO, 0},
	{"HT19_CtrlGnAIO", _K_A_IO, HT19_CtrlGnAIO, 0},
	{"HT20_CtrlGnAIO", _K_A_IO, HT20_CtrlGnAIO, 0},
	{"HT21_CtrlGnAIO", _K_A_IO, HT21_CtrlGnAIO, 0},
	{"HT22_CtrlGnAIO", _K_A_IO, HT22_CtrlGnAIO, 0},
	{"HT23_CtrlGnAIO", _K_A_IO, HT23_CtrlGnAIO, 0},
	{"HT24_CtrlGnAIO", _K_A_IO, HT24_CtrlGnAIO, 0},
	{"HT25_CtrlGnAIO", _K_A_IO, HT25_CtrlGnAIO, 0},
	{"HT26_CtrlGnAIO", _K_A_IO, HT26_CtrlGnAIO, 0},
	{"HT27_CtrlGnAIO", _K_A_IO, HT27_CtrlGnAIO, 0},
	{"HT28_CtrlGnAIO", _K_A_IO, HT28_CtrlGnAIO, 0},
	{"HT29_CtrlGnAIO", _K_A_IO, HT29_CtrlGnAIO, 0},
	{"HT30_CtrlGnAIO", _K_A_IO, HT30_CtrlGnAIO, 0},
	{"HT31_CtrlGnAIO", _K_A_IO, HT31_CtrlGnAIO, 0},
	{"HT32_CtrlGnAIO", _K_A_IO, HT32_CtrlGnAIO, 0},

	{"HT01_CtrlInTmAIO", _K_A_IO, HT01_CtrlInTmAIO, 0},
	{"HT02_CtrlInTmAIO", _K_A_IO, HT02_CtrlInTmAIO, 0},
	{"HT03_CtrlInTmAIO", _K_A_IO, HT03_CtrlInTmAIO, 0},
	{"HT04_CtrlInTmAIO", _K_A_IO, HT04_CtrlInTmAIO, 0},
	{"HT05_CtrlInTmAIO", _K_A_IO, HT05_CtrlInTmAIO, 0},
	{"HT06_CtrlInTmAIO", _K_A_IO, HT06_CtrlInTmAIO, 0},
	{"HT07_CtrlInTmAIO", _K_A_IO, HT07_CtrlInTmAIO, 0},
	{"HT08_CtrlInTmAIO", _K_A_IO, HT08_CtrlInTmAIO, 0},
	{"HT09_CtrlInTmAIO", _K_A_IO, HT09_CtrlInTmAIO, 0},
	{"HT10_CtrlInTmAIO", _K_A_IO, HT10_CtrlInTmAIO, 0},
	{"HT11_CtrlInTmAIO", _K_A_IO, HT11_CtrlInTmAIO, 0},
	{"HT12_CtrlInTmAIO", _K_A_IO, HT12_CtrlInTmAIO, 0},
	{"HT13_CtrlInTmAIO", _K_A_IO, HT13_CtrlInTmAIO, 0},
	{"HT14_CtrlInTmAIO", _K_A_IO, HT14_CtrlInTmAIO, 0},
	{"HT15_CtrlInTmAIO", _K_A_IO, HT15_CtrlInTmAIO, 0},
	{"HT16_CtrlInTmAIO", _K_A_IO, HT16_CtrlInTmAIO, 0},
	{"HT17_CtrlInTmAIO", _K_A_IO, HT17_CtrlInTmAIO, 0},
	{"HT18_CtrlInTmAIO", _K_A_IO, HT18_CtrlInTmAIO, 0},
	{"HT19_CtrlInTmAIO", _K_A_IO, HT19_CtrlInTmAIO, 0},
	{"HT20_CtrlInTmAIO", _K_A_IO, HT20_CtrlInTmAIO, 0},
	{"HT21_CtrlInTmAIO", _K_A_IO, HT21_CtrlInTmAIO, 0},
	{"HT22_CtrlInTmAIO", _K_A_IO, HT22_CtrlInTmAIO, 0},
	{"HT23_CtrlInTmAIO", _K_A_IO, HT23_CtrlInTmAIO, 0},
	{"HT24_CtrlInTmAIO", _K_A_IO, HT24_CtrlInTmAIO, 0},
	{"HT25_CtrlInTmAIO", _K_A_IO, HT25_CtrlInTmAIO, 0},
	{"HT26_CtrlInTmAIO", _K_A_IO, HT26_CtrlInTmAIO, 0},
	{"HT27_CtrlInTmAIO", _K_A_IO, HT27_CtrlInTmAIO, 0},
	{"HT28_CtrlInTmAIO", _K_A_IO, HT28_CtrlInTmAIO, 0},
	{"HT29_CtrlInTmAIO", _K_A_IO, HT29_CtrlInTmAIO, 0},
	{"HT30_CtrlInTmAIO", _K_A_IO, HT30_CtrlInTmAIO, 0},
	{"HT31_CtrlInTmAIO", _K_A_IO, HT31_CtrlInTmAIO, 0},
	{"HT32_CtrlInTmAIO", _K_A_IO, HT32_CtrlInTmAIO, 0},

	{"HT01_CtrlDfTmAIO", _K_A_IO, HT01_CtrlDfTmAIO, 0},
	{"HT02_CtrlDfTmAIO", _K_A_IO, HT02_CtrlDfTmAIO, 0},
	{"HT03_CtrlDfTmAIO", _K_A_IO, HT03_CtrlDfTmAIO, 0},
	{"HT04_CtrlDfTmAIO", _K_A_IO, HT04_CtrlDfTmAIO, 0},
	{"HT05_CtrlDfTmAIO", _K_A_IO, HT05_CtrlDfTmAIO, 0},
	{"HT06_CtrlDfTmAIO", _K_A_IO, HT06_CtrlDfTmAIO, 0},
	{"HT07_CtrlDfTmAIO", _K_A_IO, HT07_CtrlDfTmAIO, 0},
	{"HT08_CtrlDfTmAIO", _K_A_IO, HT08_CtrlDfTmAIO, 0},
	{"HT09_CtrlDfTmAIO", _K_A_IO, HT09_CtrlDfTmAIO, 0},
	{"HT10_CtrlDfTmAIO", _K_A_IO, HT10_CtrlDfTmAIO, 0},
	{"HT11_CtrlDfTmAIO", _K_A_IO, HT11_CtrlDfTmAIO, 0},
	{"HT12_CtrlDfTmAIO", _K_A_IO, HT12_CtrlDfTmAIO, 0},
	{"HT13_CtrlDfTmAIO", _K_A_IO, HT13_CtrlDfTmAIO, 0},
	{"HT14_CtrlDfTmAIO", _K_A_IO, HT14_CtrlDfTmAIO, 0},
	{"HT15_CtrlDfTmAIO", _K_A_IO, HT15_CtrlDfTmAIO, 0},
	{"HT16_CtrlDfTmAIO", _K_A_IO, HT16_CtrlDfTmAIO, 0},
	{"HT17_CtrlDfTmAIO", _K_A_IO, HT17_CtrlDfTmAIO, 0},
	{"HT18_CtrlDfTmAIO", _K_A_IO, HT18_CtrlDfTmAIO, 0},
	{"HT19_CtrlDfTmAIO", _K_A_IO, HT19_CtrlDfTmAIO, 0},
	{"HT20_CtrlDfTmAIO", _K_A_IO, HT20_CtrlDfTmAIO, 0},
	{"HT21_CtrlDfTmAIO", _K_A_IO, HT21_CtrlDfTmAIO, 0},
	{"HT22_CtrlDfTmAIO", _K_A_IO, HT22_CtrlDfTmAIO, 0},
	{"HT23_CtrlDfTmAIO", _K_A_IO, HT23_CtrlDfTmAIO, 0},
	{"HT24_CtrlDfTmAIO", _K_A_IO, HT24_CtrlDfTmAIO, 0},
	{"HT25_CtrlDfTmAIO", _K_A_IO, HT25_CtrlDfTmAIO, 0},
	{"HT26_CtrlDfTmAIO", _K_A_IO, HT26_CtrlDfTmAIO, 0},
	{"HT27_CtrlDfTmAIO", _K_A_IO, HT27_CtrlDfTmAIO, 0},
	{"HT28_CtrlDfTmAIO", _K_A_IO, HT28_CtrlDfTmAIO, 0},
	{"HT29_CtrlDfTmAIO", _K_A_IO, HT29_CtrlDfTmAIO, 0},
	{"HT30_CtrlDfTmAIO", _K_A_IO, HT30_CtrlDfTmAIO, 0},
	{"HT31_CtrlDfTmAIO", _K_A_IO, HT31_CtrlDfTmAIO, 0},
	{"HT32_CtrlDfTmAIO", _K_A_IO, HT32_CtrlDfTmAIO, 0},

	{"HT01_OnOffDIO", _K_D_IO, HT01_OnOff, 0},
	{"HT02_OnOffDIO", _K_D_IO, HT02_OnOff, 0},
	{"HT03_OnOffDIO", _K_D_IO, HT03_OnOff, 0},
	{"HT04_OnOffDIO", _K_D_IO, HT04_OnOff, 0},
	{"HT05_OnOffDIO", _K_D_IO, HT05_OnOff, 0},
	{"HT06_OnOffDIO", _K_D_IO, HT06_OnOff, 0},
	{"HT07_OnOffDIO", _K_D_IO, HT07_OnOff, 0},
	{"HT08_OnOffDIO", _K_D_IO, HT08_OnOff, 0},
	{"HT09_OnOffDIO", _K_D_IO, HT09_OnOff, 0},
	{"HT10_OnOffDIO", _K_D_IO, HT10_OnOff, 0},
	{"HT11_OnOffDIO", _K_D_IO, HT11_OnOff, 0},
	{"HT12_OnOffDIO", _K_D_IO, HT12_OnOff, 0},
	{"HT13_OnOffDIO", _K_D_IO, HT13_OnOff, 0},
	{"HT14_OnOffDIO", _K_D_IO, HT14_OnOff, 0},
	{"HT15_OnOffDIO", _K_D_IO, HT15_OnOff, 0},
	{"HT16_OnOffDIO", _K_D_IO, HT16_OnOff, 0},
	{"HT17_OnOffDIO", _K_D_IO, HT17_OnOff, 0},
	{"HT18_OnOffDIO", _K_D_IO, HT18_OnOff, 0},
	{"HT19_OnOffDIO", _K_D_IO, HT19_OnOff, 0},
	{"HT20_OnOffDIO", _K_D_IO, HT20_OnOff, 0},
	{"HT21_OnOffDIO", _K_D_IO, HT21_OnOff, 0},
	{"HT22_OnOffDIO", _K_D_IO, HT22_OnOff, 0},
	{"HT23_OnOffDIO", _K_D_IO, HT23_OnOff, 0},
	{"HT24_OnOffDIO", _K_D_IO, HT24_OnOff, 0},
	{"HT25_OnOffDIO", _K_D_IO, HT25_OnOff, 0},
	{"HT26_OnOffDIO", _K_D_IO, HT26_OnOff, 0},
	{"HT27_OnOffDIO", _K_D_IO, HT27_OnOff, 0},
	{"HT28_OnOffDIO", _K_D_IO, HT28_OnOff, 0},
	{"HT29_OnOffDIO", _K_D_IO, HT29_OnOff, 0},
	{"HT30_OnOffDIO", _K_D_IO, HT30_OnOff, 0},
	{"HT31_OnOffDIO", _K_D_IO, HT31_OnOff, 0},
	{"HT32_OnOffDIO", _K_D_IO, HT32_OnOff, 0},

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

	{"HT01_AlrmDscDI", _K_D_IO, HT01_AlrmDscDI, 0},
	{"HT02_AlrmDscDI", _K_D_IO, HT02_AlrmDscDI, 0},
	{"HT03_AlrmDscDI", _K_D_IO, HT03_AlrmDscDI, 0},
	{"HT04_AlrmDscDI", _K_D_IO, HT04_AlrmDscDI, 0},
	{"HT05_AlrmDscDI", _K_D_IO, HT05_AlrmDscDI, 0},
	{"HT06_AlrmDscDI", _K_D_IO, HT06_AlrmDscDI, 0},
	{"HT07_AlrmDscDI", _K_D_IO, HT07_AlrmDscDI, 0},
	{"HT08_AlrmDscDI", _K_D_IO, HT08_AlrmDscDI, 0},
	{"HT09_AlrmDscDI", _K_D_IO, HT09_AlrmDscDI, 0},
	{"HT10_AlrmDscDI", _K_D_IO, HT10_AlrmDscDI, 0},
	{"HT11_AlrmDscDI", _K_D_IO, HT11_AlrmDscDI, 0},
	{"HT12_AlrmDscDI", _K_D_IO, HT12_AlrmDscDI, 0},
	{"HT13_AlrmDscDI", _K_D_IO, HT13_AlrmDscDI, 0},
	{"HT14_AlrmDscDI", _K_D_IO, HT14_AlrmDscDI, 0},
	{"HT15_AlrmDscDI", _K_D_IO, HT15_AlrmDscDI, 0},
	{"HT16_AlrmDscDI", _K_D_IO, HT16_AlrmDscDI, 0},
	{"HT17_AlrmDscDI", _K_D_IO, HT17_AlrmDscDI, 0},
	{"HT18_AlrmDscDI", _K_D_IO, HT18_AlrmDscDI, 0},
	{"HT19_AlrmDscDI", _K_D_IO, HT19_AlrmDscDI, 0},
	{"HT20_AlrmDscDI", _K_D_IO, HT20_AlrmDscDI, 0},
	{"HT21_AlrmDscDI", _K_D_IO, HT21_AlrmDscDI, 0},
	{"HT22_AlrmDscDI", _K_D_IO, HT22_AlrmDscDI, 0},
	{"HT23_AlrmDscDI", _K_D_IO, HT23_AlrmDscDI, 0},
	{"HT24_AlrmDscDI", _K_D_IO, HT24_AlrmDscDI, 0},
	{"HT25_AlrmDscDI", _K_D_IO, HT25_AlrmDscDI, 0},
	{"HT26_AlrmDscDI", _K_D_IO, HT26_AlrmDscDI, 0},
	{"HT27_AlrmDscDI", _K_D_IO, HT27_AlrmDscDI, 0},
	{"HT28_AlrmDscDI", _K_D_IO, HT28_AlrmDscDI, 0},
	{"HT29_AlrmDscDI", _K_D_IO, HT29_AlrmDscDI, 0},
	{"HT30_AlrmDscDI", _K_D_IO, HT30_AlrmDscDI, 0},
	{"HT31_AlrmDscDI", _K_D_IO, HT31_AlrmDscDI, 0},
	{"HT32_AlrmDscDI", _K_D_IO, HT32_AlrmDscDI, 0},

	{"PRMD_HT01_OnOff_LT", _K_D_IO, PRMD_HT01_OnOff_LT, 0},
	{"PRMD_HT02_OnOff_LT", _K_D_IO, PRMD_HT02_OnOff_LT, 0},
	{"PRMD_HT03_OnOff_LT", _K_D_IO, PRMD_HT03_OnOff_LT, 0},
	{"PRMD_HT04_OnOff_LT", _K_D_IO, PRMD_HT04_OnOff_LT, 0},
	{"PRMD_HT05_OnOff_LT", _K_D_IO, PRMD_HT05_OnOff_LT, 0},
	{"PRMD_HT06_OnOff_LT", _K_D_IO, PRMD_HT06_OnOff_LT, 0},
	{"PRMD_HT07_OnOff_LT", _K_D_IO, PRMD_HT07_OnOff_LT, 0},
	{"PRMD_HT08_OnOff_LT", _K_D_IO, PRMD_HT08_OnOff_LT, 0},
	{"PRMD_HT09_OnOff_LT", _K_D_IO, PRMD_HT09_OnOff_LT, 0},
	{"PRMD_HT10_OnOff_LT", _K_D_IO, PRMD_HT10_OnOff_LT, 0},
	{"PRMD_HT11_OnOff_LT", _K_D_IO, PRMD_HT11_OnOff_LT, 0},
	{"PRMD_HT12_OnOff_LT", _K_D_IO, PRMD_HT12_OnOff_LT, 0},
	{"PRMD_HT13_OnOff_LT", _K_D_IO, PRMD_HT13_OnOff_LT, 0},
	{"PRMD_HT14_OnOff_LT", _K_D_IO, PRMD_HT14_OnOff_LT, 0},
	{"PRMD_HT15_OnOff_LT", _K_D_IO, PRMD_HT15_OnOff_LT, 0},
	{"PRMD_HT16_OnOff_LT", _K_D_IO, PRMD_HT16_OnOff_LT, 0},
	{"PRMD_HT17_OnOff_LT", _K_D_IO, PRMD_HT17_OnOff_LT, 0},
	{"PRMD_HT18_OnOff_LT", _K_D_IO, PRMD_HT18_OnOff_LT, 0},
	{"PRMD_HT19_OnOff_LT", _K_D_IO, PRMD_HT19_OnOff_LT, 0},
	{"PRMD_HT20_OnOff_LT", _K_D_IO, PRMD_HT20_OnOff_LT, 0},
	{"PRMD_HT21_OnOff_LT", _K_D_IO, PRMD_HT21_OnOff_LT, 0},
	{"PRMD_HT22_OnOff_LT", _K_D_IO, PRMD_HT22_OnOff_LT, 0},
	{"PRMD_HT23_OnOff_LT", _K_D_IO, PRMD_HT23_OnOff_LT, 0},
	{"PRMD_HT24_OnOff_LT", _K_D_IO, PRMD_HT24_OnOff_LT, 0},
	{"PRMD_HT25_OnOff_LT", _K_D_IO, PRMD_HT25_OnOff_LT, 0},
	{"PRMD_HT26_OnOff_LT", _K_D_IO, PRMD_HT26_OnOff_LT, 0},
	{"PRMD_HT27_OnOff_LT", _K_D_IO, PRMD_HT27_OnOff_LT, 0},
	{"PRMD_HT28_OnOff_LT", _K_D_IO, PRMD_HT28_OnOff_LT, 0},
	{"PRMD_HT29_OnOff_LT", _K_D_IO, PRMD_HT29_OnOff_LT, 0},
	{"PRMD_HT30_OnOff_LT", _K_D_IO, PRMD_HT30_OnOff_LT, 0},
	{"PRMD_HT31_OnOff_LT", _K_D_IO, PRMD_HT31_OnOff_LT, 0},
	{"PRMD_HT32_OnOff_LT", _K_D_IO, PRMD_HT32_OnOff_LT, 0},

	{"PRMA_HT01_TmpSp_LT", _K_A_IO, PRMA_HT01_TmpSp_LT, 0},
	{"PRMA_HT02_TmpSp_LT", _K_A_IO, PRMA_HT02_TmpSp_LT, 0},
	{"PRMA_HT03_TmpSp_LT", _K_A_IO, PRMA_HT03_TmpSp_LT, 0},
	{"PRMA_HT04_TmpSp_LT", _K_A_IO, PRMA_HT04_TmpSp_LT, 0},
	{"PRMA_HT05_TmpSp_LT", _K_A_IO, PRMA_HT05_TmpSp_LT, 0},
	{"PRMA_HT06_TmpSp_LT", _K_A_IO, PRMA_HT06_TmpSp_LT, 0},
	{"PRMA_HT07_TmpSp_LT", _K_A_IO, PRMA_HT07_TmpSp_LT, 0},
	{"PRMA_HT08_TmpSp_LT", _K_A_IO, PRMA_HT08_TmpSp_LT, 0},
	{"PRMA_HT09_TmpSp_LT", _K_A_IO, PRMA_HT09_TmpSp_LT, 0},
	{"PRMA_HT10_TmpSp_LT", _K_A_IO, PRMA_HT10_TmpSp_LT, 0},
	{"PRMA_HT11_TmpSp_LT", _K_A_IO, PRMA_HT11_TmpSp_LT, 0},
	{"PRMA_HT12_TmpSp_LT", _K_A_IO, PRMA_HT12_TmpSp_LT, 0},
	{"PRMA_HT13_TmpSp_LT", _K_A_IO, PRMA_HT13_TmpSp_LT, 0},
	{"PRMA_HT14_TmpSp_LT", _K_A_IO, PRMA_HT14_TmpSp_LT, 0},
	{"PRMA_HT15_TmpSp_LT", _K_A_IO, PRMA_HT15_TmpSp_LT, 0},
	{"PRMA_HT16_TmpSp_LT", _K_A_IO, PRMA_HT16_TmpSp_LT, 0},
	{"PRMA_HT17_TmpSp_LT", _K_A_IO, PRMA_HT17_TmpSp_LT, 0},
	{"PRMA_HT18_TmpSp_LT", _K_A_IO, PRMA_HT18_TmpSp_LT, 0},
	{"PRMA_HT19_TmpSp_LT", _K_A_IO, PRMA_HT19_TmpSp_LT, 0},
	{"PRMA_HT20_TmpSp_LT", _K_A_IO, PRMA_HT20_TmpSp_LT, 0},
	{"PRMA_HT21_TmpSp_LT", _K_A_IO, PRMA_HT21_TmpSp_LT, 0},
	{"PRMA_HT22_TmpSp_LT", _K_A_IO, PRMA_HT22_TmpSp_LT, 0},
	{"PRMA_HT23_TmpSp_LT", _K_A_IO, PRMA_HT23_TmpSp_LT, 0},
	{"PRMA_HT24_TmpSp_LT", _K_A_IO, PRMA_HT24_TmpSp_LT, 0},
	{"PRMA_HT25_TmpSp_LT", _K_A_IO, PRMA_HT25_TmpSp_LT, 0},
	{"PRMA_HT26_TmpSp_LT", _K_A_IO, PRMA_HT26_TmpSp_LT, 0},
	{"PRMA_HT27_TmpSp_LT", _K_A_IO, PRMA_HT27_TmpSp_LT, 0},
	{"PRMA_HT28_TmpSp_LT", _K_A_IO, PRMA_HT28_TmpSp_LT, 0},
	{"PRMA_HT29_TmpSp_LT", _K_A_IO, PRMA_HT29_TmpSp_LT, 0},
	{"PRMA_HT30_TmpSp_LT", _K_A_IO, PRMA_HT30_TmpSp_LT, 0},
	{"PRMA_HT31_TmpSp_LT", _K_A_IO, PRMA_HT31_TmpSp_LT, 0},
	{"PRMA_HT32_TmpSp_LT", _K_A_IO, PRMA_HT32_TmpSp_LT, 0},

	{"PRMD_HT01_CtrlMd_LT", _K_D_IO, PRMD_HT01_CtrlMd_LT, 0},
	{"PRMD_HT02_CtrlMd_LT", _K_D_IO, PRMD_HT02_CtrlMd_LT, 0},
	{"PRMD_HT03_CtrlMd_LT", _K_D_IO, PRMD_HT03_CtrlMd_LT, 0},
	{"PRMD_HT04_CtrlMd_LT", _K_D_IO, PRMD_HT04_CtrlMd_LT, 0},
	{"PRMD_HT05_CtrlMd_LT", _K_D_IO, PRMD_HT05_CtrlMd_LT, 0},
	{"PRMD_HT06_CtrlMd_LT", _K_D_IO, PRMD_HT06_CtrlMd_LT, 0},
	{"PRMD_HT07_CtrlMd_LT", _K_D_IO, PRMD_HT07_CtrlMd_LT, 0},
	{"PRMD_HT08_CtrlMd_LT", _K_D_IO, PRMD_HT08_CtrlMd_LT, 0},
	{"PRMD_HT09_CtrlMd_LT", _K_D_IO, PRMD_HT09_CtrlMd_LT, 0},
	{"PRMD_HT10_CtrlMd_LT", _K_D_IO, PRMD_HT10_CtrlMd_LT, 0},
	{"PRMD_HT11_CtrlMd_LT", _K_D_IO, PRMD_HT11_CtrlMd_LT, 0},
	{"PRMD_HT12_CtrlMd_LT", _K_D_IO, PRMD_HT12_CtrlMd_LT, 0},
	{"PRMD_HT13_CtrlMd_LT", _K_D_IO, PRMD_HT13_CtrlMd_LT, 0},
	{"PRMD_HT14_CtrlMd_LT", _K_D_IO, PRMD_HT14_CtrlMd_LT, 0},
	{"PRMD_HT15_CtrlMd_LT", _K_D_IO, PRMD_HT15_CtrlMd_LT, 0},
	{"PRMD_HT16_CtrlMd_LT", _K_D_IO, PRMD_HT16_CtrlMd_LT, 0},
	{"PRMD_HT17_CtrlMd_LT", _K_D_IO, PRMD_HT17_CtrlMd_LT, 0},
	{"PRMD_HT18_CtrlMd_LT", _K_D_IO, PRMD_HT18_CtrlMd_LT, 0},
	{"PRMD_HT19_CtrlMd_LT", _K_D_IO, PRMD_HT19_CtrlMd_LT, 0},
	{"PRMD_HT20_CtrlMd_LT", _K_D_IO, PRMD_HT20_CtrlMd_LT, 0},
	{"PRMD_HT21_CtrlMd_LT", _K_D_IO, PRMD_HT21_CtrlMd_LT, 0},
	{"PRMD_HT22_CtrlMd_LT", _K_D_IO, PRMD_HT22_CtrlMd_LT, 0},
	{"PRMD_HT23_CtrlMd_LT", _K_D_IO, PRMD_HT23_CtrlMd_LT, 0},
	{"PRMD_HT24_CtrlMd_LT", _K_D_IO, PRMD_HT24_CtrlMd_LT, 0},
	{"PRMD_HT25_CtrlMd_LT", _K_D_IO, PRMD_HT25_CtrlMd_LT, 0},
	{"PRMD_HT26_CtrlMd_LT", _K_D_IO, PRMD_HT26_CtrlMd_LT, 0},
	{"PRMD_HT27_CtrlMd_LT", _K_D_IO, PRMD_HT27_CtrlMd_LT, 0},
	{"PRMD_HT28_CtrlMd_LT", _K_D_IO, PRMD_HT28_CtrlMd_LT, 0},
	{"PRMD_HT29_CtrlMd_LT", _K_D_IO, PRMD_HT29_CtrlMd_LT, 0},
	{"PRMD_HT30_CtrlMd_LT", _K_D_IO, PRMD_HT30_CtrlMd_LT, 0},
	{"PRMD_HT31_CtrlMd_LT", _K_D_IO, PRMD_HT31_CtrlMd_LT, 0},
	{"PRMD_HT32_CtrlMd_LT", _K_D_IO, PRMD_HT32_CtrlMd_LT, 0},

	{"PRMA_HT01_TmpHgLmt_LT", _K_A_IO, PRMA_HT01_TmpHgLmt_LT, 0},
	{"PRMA_HT02_TmpHgLmt_LT", _K_A_IO, PRMA_HT02_TmpHgLmt_LT, 0},
	{"PRMA_HT03_TmpHgLmt_LT", _K_A_IO, PRMA_HT03_TmpHgLmt_LT, 0},
	{"PRMA_HT04_TmpHgLmt_LT", _K_A_IO, PRMA_HT04_TmpHgLmt_LT, 0},
	{"PRMA_HT05_TmpHgLmt_LT", _K_A_IO, PRMA_HT05_TmpHgLmt_LT, 0},
	{"PRMA_HT06_TmpHgLmt_LT", _K_A_IO, PRMA_HT06_TmpHgLmt_LT, 0},
	{"PRMA_HT07_TmpHgLmt_LT", _K_A_IO, PRMA_HT07_TmpHgLmt_LT, 0},
	{"PRMA_HT08_TmpHgLmt_LT", _K_A_IO, PRMA_HT08_TmpHgLmt_LT, 0},
	{"PRMA_HT09_TmpHgLmt_LT", _K_A_IO, PRMA_HT09_TmpHgLmt_LT, 0},
	{"PRMA_HT10_TmpHgLmt_LT", _K_A_IO, PRMA_HT10_TmpHgLmt_LT, 0},
	{"PRMA_HT11_TmpHgLmt_LT", _K_A_IO, PRMA_HT11_TmpHgLmt_LT, 0},
	{"PRMA_HT12_TmpHgLmt_LT", _K_A_IO, PRMA_HT12_TmpHgLmt_LT, 0},
	{"PRMA_HT13_TmpHgLmt_LT", _K_A_IO, PRMA_HT13_TmpHgLmt_LT, 0},
	{"PRMA_HT14_TmpHgLmt_LT", _K_A_IO, PRMA_HT14_TmpHgLmt_LT, 0},
	{"PRMA_HT15_TmpHgLmt_LT", _K_A_IO, PRMA_HT15_TmpHgLmt_LT, 0},
	{"PRMA_HT16_TmpHgLmt_LT", _K_A_IO, PRMA_HT16_TmpHgLmt_LT, 0},
	{"PRMA_HT17_TmpHgLmt_LT", _K_A_IO, PRMA_HT17_TmpHgLmt_LT, 0},
	{"PRMA_HT18_TmpHgLmt_LT", _K_A_IO, PRMA_HT18_TmpHgLmt_LT, 0},
	{"PRMA_HT19_TmpHgLmt_LT", _K_A_IO, PRMA_HT19_TmpHgLmt_LT, 0},
	{"PRMA_HT20_TmpHgLmt_LT", _K_A_IO, PRMA_HT20_TmpHgLmt_LT, 0},
	{"PRMA_HT21_TmpHgLmt_LT", _K_A_IO, PRMA_HT21_TmpHgLmt_LT, 0},
	{"PRMA_HT22_TmpHgLmt_LT", _K_A_IO, PRMA_HT22_TmpHgLmt_LT, 0},
	{"PRMA_HT23_TmpHgLmt_LT", _K_A_IO, PRMA_HT23_TmpHgLmt_LT, 0},
	{"PRMA_HT24_TmpHgLmt_LT", _K_A_IO, PRMA_HT24_TmpHgLmt_LT, 0},
	{"PRMA_HT25_TmpHgLmt_LT", _K_A_IO, PRMA_HT25_TmpHgLmt_LT, 0},
	{"PRMA_HT26_TmpHgLmt_LT", _K_A_IO, PRMA_HT26_TmpHgLmt_LT, 0},
	{"PRMA_HT27_TmpHgLmt_LT", _K_A_IO, PRMA_HT27_TmpHgLmt_LT, 0},
	{"PRMA_HT28_TmpHgLmt_LT", _K_A_IO, PRMA_HT28_TmpHgLmt_LT, 0},
	{"PRMA_HT29_TmpHgLmt_LT", _K_A_IO, PRMA_HT29_TmpHgLmt_LT, 0},
	{"PRMA_HT30_TmpHgLmt_LT", _K_A_IO, PRMA_HT30_TmpHgLmt_LT, 0},
	{"PRMA_HT31_TmpHgLmt_LT", _K_A_IO, PRMA_HT31_TmpHgLmt_LT, 0},
	{"PRMA_HT32_TmpHgLmt_LT", _K_A_IO, PRMA_HT32_TmpHgLmt_LT, 0},

	{"PRMA_HT01_TmpLwLmt_LT", _K_A_IO, PRMA_HT01_TmpLwLmt_LT, 0},
	{"PRMA_HT02_TmpLwLmt_LT", _K_A_IO, PRMA_HT02_TmpLwLmt_LT, 0},
	{"PRMA_HT03_TmpLwLmt_LT", _K_A_IO, PRMA_HT03_TmpLwLmt_LT, 0},
	{"PRMA_HT04_TmpLwLmt_LT", _K_A_IO, PRMA_HT04_TmpLwLmt_LT, 0},
	{"PRMA_HT05_TmpLwLmt_LT", _K_A_IO, PRMA_HT05_TmpLwLmt_LT, 0},
	{"PRMA_HT06_TmpLwLmt_LT", _K_A_IO, PRMA_HT06_TmpLwLmt_LT, 0},
	{"PRMA_HT07_TmpLwLmt_LT", _K_A_IO, PRMA_HT07_TmpLwLmt_LT, 0},
	{"PRMA_HT08_TmpLwLmt_LT", _K_A_IO, PRMA_HT08_TmpLwLmt_LT, 0},
	{"PRMA_HT09_TmpLwLmt_LT", _K_A_IO, PRMA_HT09_TmpLwLmt_LT, 0},
	{"PRMA_HT10_TmpLwLmt_LT", _K_A_IO, PRMA_HT10_TmpLwLmt_LT, 0},
	{"PRMA_HT11_TmpLwLmt_LT", _K_A_IO, PRMA_HT11_TmpLwLmt_LT, 0},
	{"PRMA_HT12_TmpLwLmt_LT", _K_A_IO, PRMA_HT12_TmpLwLmt_LT, 0},
	{"PRMA_HT13_TmpLwLmt_LT", _K_A_IO, PRMA_HT13_TmpLwLmt_LT, 0},
	{"PRMA_HT14_TmpLwLmt_LT", _K_A_IO, PRMA_HT14_TmpLwLmt_LT, 0},
	{"PRMA_HT15_TmpLwLmt_LT", _K_A_IO, PRMA_HT15_TmpLwLmt_LT, 0},
	{"PRMA_HT16_TmpLwLmt_LT", _K_A_IO, PRMA_HT16_TmpLwLmt_LT, 0},
	{"PRMA_HT17_TmpLwLmt_LT", _K_A_IO, PRMA_HT17_TmpLwLmt_LT, 0},
	{"PRMA_HT18_TmpLwLmt_LT", _K_A_IO, PRMA_HT18_TmpLwLmt_LT, 0},
	{"PRMA_HT19_TmpLwLmt_LT", _K_A_IO, PRMA_HT19_TmpLwLmt_LT, 0},
	{"PRMA_HT20_TmpLwLmt_LT", _K_A_IO, PRMA_HT20_TmpLwLmt_LT, 0},
	{"PRMA_HT21_TmpLwLmt_LT", _K_A_IO, PRMA_HT21_TmpLwLmt_LT, 0},
	{"PRMA_HT22_TmpLwLmt_LT", _K_A_IO, PRMA_HT22_TmpLwLmt_LT, 0},
	{"PRMA_HT23_TmpLwLmt_LT", _K_A_IO, PRMA_HT23_TmpLwLmt_LT, 0},
	{"PRMA_HT24_TmpLwLmt_LT", _K_A_IO, PRMA_HT24_TmpLwLmt_LT, 0},
	{"PRMA_HT25_TmpLwLmt_LT", _K_A_IO, PRMA_HT25_TmpLwLmt_LT, 0},
	{"PRMA_HT26_TmpLwLmt_LT", _K_A_IO, PRMA_HT26_TmpLwLmt_LT, 0},
	{"PRMA_HT27_TmpLwLmt_LT", _K_A_IO, PRMA_HT27_TmpLwLmt_LT, 0},
	{"PRMA_HT28_TmpLwLmt_LT", _K_A_IO, PRMA_HT28_TmpLwLmt_LT, 0},
	{"PRMA_HT29_TmpLwLmt_LT", _K_A_IO, PRMA_HT29_TmpLwLmt_LT, 0},
	{"PRMA_HT30_TmpLwLmt_LT", _K_A_IO, PRMA_HT30_TmpLwLmt_LT, 0},
	{"PRMA_HT31_TmpLwLmt_LT", _K_A_IO, PRMA_HT31_TmpLwLmt_LT, 0},
	{"PRMA_HT32_TmpLwLmt_LT", _K_A_IO, PRMA_HT32_TmpLwLmt_LT, 0},

	{"PRMA_HT01_Devi_LT", _K_A_IO, PRMA_HT01_Devi_LT, 0},
	{"PRMA_HT02_Devi_LT", _K_A_IO, PRMA_HT02_Devi_LT, 0},
	{"PRMA_HT03_Devi_LT", _K_A_IO, PRMA_HT03_Devi_LT, 0},
	{"PRMA_HT04_Devi_LT", _K_A_IO, PRMA_HT04_Devi_LT, 0},
	{"PRMA_HT05_Devi_LT", _K_A_IO, PRMA_HT05_Devi_LT, 0},
	{"PRMA_HT06_Devi_LT", _K_A_IO, PRMA_HT06_Devi_LT, 0},
	{"PRMA_HT07_Devi_LT", _K_A_IO, PRMA_HT07_Devi_LT, 0},
	{"PRMA_HT08_Devi_LT", _K_A_IO, PRMA_HT08_Devi_LT, 0},
	{"PRMA_HT09_Devi_LT", _K_A_IO, PRMA_HT09_Devi_LT, 0},
	{"PRMA_HT10_Devi_LT", _K_A_IO, PRMA_HT10_Devi_LT, 0},
	{"PRMA_HT11_Devi_LT", _K_A_IO, PRMA_HT11_Devi_LT, 0},
	{"PRMA_HT12_Devi_LT", _K_A_IO, PRMA_HT12_Devi_LT, 0},
	{"PRMA_HT13_Devi_LT", _K_A_IO, PRMA_HT13_Devi_LT, 0},
	{"PRMA_HT14_Devi_LT", _K_A_IO, PRMA_HT14_Devi_LT, 0},
	{"PRMA_HT15_Devi_LT", _K_A_IO, PRMA_HT15_Devi_LT, 0},
	{"PRMA_HT16_Devi_LT", _K_A_IO, PRMA_HT16_Devi_LT, 0},
	{"PRMA_HT17_Devi_LT", _K_A_IO, PRMA_HT17_Devi_LT, 0},
	{"PRMA_HT18_Devi_LT", _K_A_IO, PRMA_HT18_Devi_LT, 0},
	{"PRMA_HT19_Devi_LT", _K_A_IO, PRMA_HT19_Devi_LT, 0},
	{"PRMA_HT20_Devi_LT", _K_A_IO, PRMA_HT20_Devi_LT, 0},
	{"PRMA_HT21_Devi_LT", _K_A_IO, PRMA_HT21_Devi_LT, 0},
	{"PRMA_HT22_Devi_LT", _K_A_IO, PRMA_HT22_Devi_LT, 0},
	{"PRMA_HT23_Devi_LT", _K_A_IO, PRMA_HT23_Devi_LT, 0},
	{"PRMA_HT24_Devi_LT", _K_A_IO, PRMA_HT24_Devi_LT, 0},
	{"PRMA_HT25_Devi_LT", _K_A_IO, PRMA_HT25_Devi_LT, 0},
	{"PRMA_HT26_Devi_LT", _K_A_IO, PRMA_HT26_Devi_LT, 0},
	{"PRMA_HT27_Devi_LT", _K_A_IO, PRMA_HT27_Devi_LT, 0},
	{"PRMA_HT28_Devi_LT", _K_A_IO, PRMA_HT28_Devi_LT, 0},
	{"PRMA_HT29_Devi_LT", _K_A_IO, PRMA_HT29_Devi_LT, 0},
	{"PRMA_HT30_Devi_LT", _K_A_IO, PRMA_HT30_Devi_LT, 0},
	{"PRMA_HT31_Devi_LT", _K_A_IO, PRMA_HT31_Devi_LT, 0},
	{"PRMA_HT32_Devi_LT", _K_A_IO, PRMA_HT32_Devi_LT, 0},

	{"PRMA_HT01_OutPw_LT", _K_A_IO, PRMA_HT01_OutPw_LT, 0},
	{"PRMA_HT02_OutPw_LT", _K_A_IO, PRMA_HT02_OutPw_LT, 0},
	{"PRMA_HT03_OutPw_LT", _K_A_IO, PRMA_HT03_OutPw_LT, 0},
	{"PRMA_HT04_OutPw_LT", _K_A_IO, PRMA_HT04_OutPw_LT, 0},
	{"PRMA_HT05_OutPw_LT", _K_A_IO, PRMA_HT05_OutPw_LT, 0},
	{"PRMA_HT06_OutPw_LT", _K_A_IO, PRMA_HT06_OutPw_LT, 0},
	{"PRMA_HT07_OutPw_LT", _K_A_IO, PRMA_HT07_OutPw_LT, 0},
	{"PRMA_HT08_OutPw_LT", _K_A_IO, PRMA_HT08_OutPw_LT, 0},
	{"PRMA_HT09_OutPw_LT", _K_A_IO, PRMA_HT09_OutPw_LT, 0},
	{"PRMA_HT10_OutPw_LT", _K_A_IO, PRMA_HT10_OutPw_LT, 0},
	{"PRMA_HT11_OutPw_LT", _K_A_IO, PRMA_HT11_OutPw_LT, 0},
	{"PRMA_HT12_OutPw_LT", _K_A_IO, PRMA_HT12_OutPw_LT, 0},
	{"PRMA_HT13_OutPw_LT", _K_A_IO, PRMA_HT13_OutPw_LT, 0},
	{"PRMA_HT14_OutPw_LT", _K_A_IO, PRMA_HT14_OutPw_LT, 0},
	{"PRMA_HT15_OutPw_LT", _K_A_IO, PRMA_HT15_OutPw_LT, 0},
	{"PRMA_HT16_OutPw_LT", _K_A_IO, PRMA_HT16_OutPw_LT, 0},
	{"PRMA_HT17_OutPw_LT", _K_A_IO, PRMA_HT17_OutPw_LT, 0},
	{"PRMA_HT18_OutPw_LT", _K_A_IO, PRMA_HT18_OutPw_LT, 0},
	{"PRMA_HT19_OutPw_LT", _K_A_IO, PRMA_HT19_OutPw_LT, 0},
	{"PRMA_HT20_OutPw_LT", _K_A_IO, PRMA_HT20_OutPw_LT, 0},
	{"PRMA_HT21_OutPw_LT", _K_A_IO, PRMA_HT21_OutPw_LT, 0},
	{"PRMA_HT22_OutPw_LT", _K_A_IO, PRMA_HT22_OutPw_LT, 0},
	{"PRMA_HT23_OutPw_LT", _K_A_IO, PRMA_HT23_OutPw_LT, 0},
	{"PRMA_HT24_OutPw_LT", _K_A_IO, PRMA_HT24_OutPw_LT, 0},
	{"PRMA_HT25_OutPw_LT", _K_A_IO, PRMA_HT25_OutPw_LT, 0},
	{"PRMA_HT26_OutPw_LT", _K_A_IO, PRMA_HT26_OutPw_LT, 0},
	{"PRMA_HT27_OutPw_LT", _K_A_IO, PRMA_HT27_OutPw_LT, 0},
	{"PRMA_HT28_OutPw_LT", _K_A_IO, PRMA_HT28_OutPw_LT, 0},
	{"PRMA_HT29_OutPw_LT", _K_A_IO, PRMA_HT29_OutPw_LT, 0},
	{"PRMA_HT30_OutPw_LT", _K_A_IO, PRMA_HT30_OutPw_LT, 0},
	{"PRMA_HT31_OutPw_LT", _K_A_IO, PRMA_HT31_OutPw_LT, 0},
	{"PRMA_HT32_OutPw_LT", _K_A_IO, PRMA_HT32_OutPw_LT, 0},

	{"PRMA_HT01_OutPwLmt_LT", _K_A_IO, PRMA_HT01_OutPwLmt_LT, 0},
	{"PRMA_HT02_OutPwLmt_LT", _K_A_IO, PRMA_HT02_OutPwLmt_LT, 0},
	{"PRMA_HT03_OutPwLmt_LT", _K_A_IO, PRMA_HT03_OutPwLmt_LT, 0},
	{"PRMA_HT04_OutPwLmt_LT", _K_A_IO, PRMA_HT04_OutPwLmt_LT, 0},
	{"PRMA_HT05_OutPwLmt_LT", _K_A_IO, PRMA_HT05_OutPwLmt_LT, 0},
	{"PRMA_HT06_OutPwLmt_LT", _K_A_IO, PRMA_HT06_OutPwLmt_LT, 0},
	{"PRMA_HT07_OutPwLmt_LT", _K_A_IO, PRMA_HT07_OutPwLmt_LT, 0},
	{"PRMA_HT08_OutPwLmt_LT", _K_A_IO, PRMA_HT08_OutPwLmt_LT, 0},
	{"PRMA_HT09_OutPwLmt_LT", _K_A_IO, PRMA_HT09_OutPwLmt_LT, 0},
	{"PRMA_HT10_OutPwLmt_LT", _K_A_IO, PRMA_HT10_OutPwLmt_LT, 0},
	{"PRMA_HT11_OutPwLmt_LT", _K_A_IO, PRMA_HT11_OutPwLmt_LT, 0},
	{"PRMA_HT12_OutPwLmt_LT", _K_A_IO, PRMA_HT12_OutPwLmt_LT, 0},
	{"PRMA_HT13_OutPwLmt_LT", _K_A_IO, PRMA_HT13_OutPwLmt_LT, 0},
	{"PRMA_HT14_OutPwLmt_LT", _K_A_IO, PRMA_HT14_OutPwLmt_LT, 0},
	{"PRMA_HT15_OutPwLmt_LT", _K_A_IO, PRMA_HT15_OutPwLmt_LT, 0},
	{"PRMA_HT16_OutPwLmt_LT", _K_A_IO, PRMA_HT16_OutPwLmt_LT, 0},
	{"PRMA_HT17_OutPwLmt_LT", _K_A_IO, PRMA_HT17_OutPwLmt_LT, 0},
	{"PRMA_HT18_OutPwLmt_LT", _K_A_IO, PRMA_HT18_OutPwLmt_LT, 0},
	{"PRMA_HT19_OutPwLmt_LT", _K_A_IO, PRMA_HT19_OutPwLmt_LT, 0},
	{"PRMA_HT20_OutPwLmt_LT", _K_A_IO, PRMA_HT20_OutPwLmt_LT, 0},
	{"PRMA_HT21_OutPwLmt_LT", _K_A_IO, PRMA_HT21_OutPwLmt_LT, 0},
	{"PRMA_HT22_OutPwLmt_LT", _K_A_IO, PRMA_HT22_OutPwLmt_LT, 0},
	{"PRMA_HT23_OutPwLmt_LT", _K_A_IO, PRMA_HT23_OutPwLmt_LT, 0},
	{"PRMA_HT24_OutPwLmt_LT", _K_A_IO, PRMA_HT24_OutPwLmt_LT, 0},
	{"PRMA_HT25_OutPwLmt_LT", _K_A_IO, PRMA_HT25_OutPwLmt_LT, 0},
	{"PRMA_HT26_OutPwLmt_LT", _K_A_IO, PRMA_HT26_OutPwLmt_LT, 0},
	{"PRMA_HT27_OutPwLmt_LT", _K_A_IO, PRMA_HT27_OutPwLmt_LT, 0},
	{"PRMA_HT28_OutPwLmt_LT", _K_A_IO, PRMA_HT28_OutPwLmt_LT, 0},
	{"PRMA_HT29_OutPwLmt_LT", _K_A_IO, PRMA_HT29_OutPwLmt_LT, 0},
	{"PRMA_HT30_OutPwLmt_LT", _K_A_IO, PRMA_HT30_OutPwLmt_LT, 0},
	{"PRMA_HT31_OutPwLmt_LT", _K_A_IO, PRMA_HT31_OutPwLmt_LT, 0},
	{"PRMA_HT32_OutPwLmt_LT", _K_A_IO, PRMA_HT32_OutPwLmt_LT, 0},

	{"PRMD_HT01_OnOff_RT", _K_D_IO, PRMD_HT01_OnOff_RT, 0},
	{"PRMD_HT02_OnOff_RT", _K_D_IO, PRMD_HT02_OnOff_RT, 0},
	{"PRMD_HT03_OnOff_RT", _K_D_IO, PRMD_HT03_OnOff_RT, 0},
	{"PRMD_HT04_OnOff_RT", _K_D_IO, PRMD_HT04_OnOff_RT, 0},
	{"PRMD_HT05_OnOff_RT", _K_D_IO, PRMD_HT05_OnOff_RT, 0},
	{"PRMD_HT06_OnOff_RT", _K_D_IO, PRMD_HT06_OnOff_RT, 0},
	{"PRMD_HT07_OnOff_RT", _K_D_IO, PRMD_HT07_OnOff_RT, 0},
	{"PRMD_HT08_OnOff_RT", _K_D_IO, PRMD_HT08_OnOff_RT, 0},
	{"PRMD_HT09_OnOff_RT", _K_D_IO, PRMD_HT09_OnOff_RT, 0},
	{"PRMD_HT10_OnOff_RT", _K_D_IO, PRMD_HT10_OnOff_RT, 0},
	{"PRMD_HT11_OnOff_RT", _K_D_IO, PRMD_HT11_OnOff_RT, 0},
	{"PRMD_HT12_OnOff_RT", _K_D_IO, PRMD_HT12_OnOff_RT, 0},
	{"PRMD_HT13_OnOff_RT", _K_D_IO, PRMD_HT13_OnOff_RT, 0},
	{"PRMD_HT14_OnOff_RT", _K_D_IO, PRMD_HT14_OnOff_RT, 0},
	{"PRMD_HT15_OnOff_RT", _K_D_IO, PRMD_HT15_OnOff_RT, 0},
	{"PRMD_HT16_OnOff_RT", _K_D_IO, PRMD_HT16_OnOff_RT, 0},
	{"PRMD_HT17_OnOff_RT", _K_D_IO, PRMD_HT17_OnOff_RT, 0},
	{"PRMD_HT18_OnOff_RT", _K_D_IO, PRMD_HT18_OnOff_RT, 0},
	{"PRMD_HT19_OnOff_RT", _K_D_IO, PRMD_HT19_OnOff_RT, 0},
	{"PRMD_HT20_OnOff_RT", _K_D_IO, PRMD_HT20_OnOff_RT, 0},
	{"PRMD_HT21_OnOff_RT", _K_D_IO, PRMD_HT21_OnOff_RT, 0},
	{"PRMD_HT22_OnOff_RT", _K_D_IO, PRMD_HT22_OnOff_RT, 0},
	{"PRMD_HT23_OnOff_RT", _K_D_IO, PRMD_HT23_OnOff_RT, 0},
	{"PRMD_HT24_OnOff_RT", _K_D_IO, PRMD_HT24_OnOff_RT, 0},
	{"PRMD_HT25_OnOff_RT", _K_D_IO, PRMD_HT25_OnOff_RT, 0},
	{"PRMD_HT26_OnOff_RT", _K_D_IO, PRMD_HT26_OnOff_RT, 0},
	{"PRMD_HT27_OnOff_RT", _K_D_IO, PRMD_HT27_OnOff_RT, 0},
	{"PRMD_HT28_OnOff_RT", _K_D_IO, PRMD_HT28_OnOff_RT, 0},
	{"PRMD_HT29_OnOff_RT", _K_D_IO, PRMD_HT29_OnOff_RT, 0},
	{"PRMD_HT30_OnOff_RT", _K_D_IO, PRMD_HT30_OnOff_RT, 0},
	{"PRMD_HT31_OnOff_RT", _K_D_IO, PRMD_HT31_OnOff_RT, 0},
	{"PRMD_HT32_OnOff_RT", _K_D_IO, PRMD_HT32_OnOff_RT, 0},

	{"PRMA_HT01_TmpSp_RT", _K_A_IO, PRMA_HT01_TmpSp_RT, 0},
	{"PRMA_HT02_TmpSp_RT", _K_A_IO, PRMA_HT02_TmpSp_RT, 0},
	{"PRMA_HT03_TmpSp_RT", _K_A_IO, PRMA_HT03_TmpSp_RT, 0},
	{"PRMA_HT04_TmpSp_RT", _K_A_IO, PRMA_HT04_TmpSp_RT, 0},
	{"PRMA_HT05_TmpSp_RT", _K_A_IO, PRMA_HT05_TmpSp_RT, 0},
	{"PRMA_HT06_TmpSp_RT", _K_A_IO, PRMA_HT06_TmpSp_RT, 0},
	{"PRMA_HT07_TmpSp_RT", _K_A_IO, PRMA_HT07_TmpSp_RT, 0},
	{"PRMA_HT08_TmpSp_RT", _K_A_IO, PRMA_HT08_TmpSp_RT, 0},
	{"PRMA_HT09_TmpSp_RT", _K_A_IO, PRMA_HT09_TmpSp_RT, 0},
	{"PRMA_HT10_TmpSp_RT", _K_A_IO, PRMA_HT10_TmpSp_RT, 0},
	{"PRMA_HT11_TmpSp_RT", _K_A_IO, PRMA_HT11_TmpSp_RT, 0},
	{"PRMA_HT12_TmpSp_RT", _K_A_IO, PRMA_HT12_TmpSp_RT, 0},
	{"PRMA_HT13_TmpSp_RT", _K_A_IO, PRMA_HT13_TmpSp_RT, 0},
	{"PRMA_HT14_TmpSp_RT", _K_A_IO, PRMA_HT14_TmpSp_RT, 0},
	{"PRMA_HT15_TmpSp_RT", _K_A_IO, PRMA_HT15_TmpSp_RT, 0},
	{"PRMA_HT16_TmpSp_RT", _K_A_IO, PRMA_HT16_TmpSp_RT, 0},
	{"PRMA_HT17_TmpSp_RT", _K_A_IO, PRMA_HT17_TmpSp_RT, 0},
	{"PRMA_HT18_TmpSp_RT", _K_A_IO, PRMA_HT18_TmpSp_RT, 0},
	{"PRMA_HT19_TmpSp_RT", _K_A_IO, PRMA_HT19_TmpSp_RT, 0},
	{"PRMA_HT20_TmpSp_RT", _K_A_IO, PRMA_HT20_TmpSp_RT, 0},
	{"PRMA_HT21_TmpSp_RT", _K_A_IO, PRMA_HT21_TmpSp_RT, 0},
	{"PRMA_HT22_TmpSp_RT", _K_A_IO, PRMA_HT22_TmpSp_RT, 0},
	{"PRMA_HT23_TmpSp_RT", _K_A_IO, PRMA_HT23_TmpSp_RT, 0},
	{"PRMA_HT24_TmpSp_RT", _K_A_IO, PRMA_HT24_TmpSp_RT, 0},
	{"PRMA_HT25_TmpSp_RT", _K_A_IO, PRMA_HT25_TmpSp_RT, 0},
	{"PRMA_HT26_TmpSp_RT", _K_A_IO, PRMA_HT26_TmpSp_RT, 0},
	{"PRMA_HT27_TmpSp_RT", _K_A_IO, PRMA_HT27_TmpSp_RT, 0},
	{"PRMA_HT28_TmpSp_RT", _K_A_IO, PRMA_HT28_TmpSp_RT, 0},
	{"PRMA_HT29_TmpSp_RT", _K_A_IO, PRMA_HT29_TmpSp_RT, 0},
	{"PRMA_HT30_TmpSp_RT", _K_A_IO, PRMA_HT30_TmpSp_RT, 0},
	{"PRMA_HT31_TmpSp_RT", _K_A_IO, PRMA_HT31_TmpSp_RT, 0},
	{"PRMA_HT32_TmpSp_RT", _K_A_IO, PRMA_HT32_TmpSp_RT, 0},

	{"PRMD_HT01_CtrlMd_RT", _K_D_IO, PRMD_HT01_CtrlMd_RT, 0},
	{"PRMD_HT02_CtrlMd_RT", _K_D_IO, PRMD_HT02_CtrlMd_RT, 0},
	{"PRMD_HT03_CtrlMd_RT", _K_D_IO, PRMD_HT03_CtrlMd_RT, 0},
	{"PRMD_HT04_CtrlMd_RT", _K_D_IO, PRMD_HT04_CtrlMd_RT, 0},
	{"PRMD_HT05_CtrlMd_RT", _K_D_IO, PRMD_HT05_CtrlMd_RT, 0},
	{"PRMD_HT06_CtrlMd_RT", _K_D_IO, PRMD_HT06_CtrlMd_RT, 0},
	{"PRMD_HT07_CtrlMd_RT", _K_D_IO, PRMD_HT07_CtrlMd_RT, 0},
	{"PRMD_HT08_CtrlMd_RT", _K_D_IO, PRMD_HT08_CtrlMd_RT, 0},
	{"PRMD_HT09_CtrlMd_RT", _K_D_IO, PRMD_HT09_CtrlMd_RT, 0},
	{"PRMD_HT10_CtrlMd_RT", _K_D_IO, PRMD_HT10_CtrlMd_RT, 0},
	{"PRMD_HT11_CtrlMd_RT", _K_D_IO, PRMD_HT11_CtrlMd_RT, 0},
	{"PRMD_HT12_CtrlMd_RT", _K_D_IO, PRMD_HT12_CtrlMd_RT, 0},
	{"PRMD_HT13_CtrlMd_RT", _K_D_IO, PRMD_HT13_CtrlMd_RT, 0},
	{"PRMD_HT14_CtrlMd_RT", _K_D_IO, PRMD_HT14_CtrlMd_RT, 0},
	{"PRMD_HT15_CtrlMd_RT", _K_D_IO, PRMD_HT15_CtrlMd_RT, 0},
	{"PRMD_HT16_CtrlMd_RT", _K_D_IO, PRMD_HT16_CtrlMd_RT, 0},
	{"PRMD_HT17_CtrlMd_RT", _K_D_IO, PRMD_HT17_CtrlMd_RT, 0},
	{"PRMD_HT18_CtrlMd_RT", _K_D_IO, PRMD_HT18_CtrlMd_RT, 0},
	{"PRMD_HT19_CtrlMd_RT", _K_D_IO, PRMD_HT19_CtrlMd_RT, 0},
	{"PRMD_HT20_CtrlMd_RT", _K_D_IO, PRMD_HT20_CtrlMd_RT, 0},
	{"PRMD_HT21_CtrlMd_RT", _K_D_IO, PRMD_HT21_CtrlMd_RT, 0},
	{"PRMD_HT22_CtrlMd_RT", _K_D_IO, PRMD_HT22_CtrlMd_RT, 0},
	{"PRMD_HT23_CtrlMd_RT", _K_D_IO, PRMD_HT23_CtrlMd_RT, 0},
	{"PRMD_HT24_CtrlMd_RT", _K_D_IO, PRMD_HT24_CtrlMd_RT, 0},
	{"PRMD_HT25_CtrlMd_RT", _K_D_IO, PRMD_HT25_CtrlMd_RT, 0},
	{"PRMD_HT26_CtrlMd_RT", _K_D_IO, PRMD_HT26_CtrlMd_RT, 0},
	{"PRMD_HT27_CtrlMd_RT", _K_D_IO, PRMD_HT27_CtrlMd_RT, 0},
	{"PRMD_HT28_CtrlMd_RT", _K_D_IO, PRMD_HT28_CtrlMd_RT, 0},
	{"PRMD_HT29_CtrlMd_RT", _K_D_IO, PRMD_HT29_CtrlMd_RT, 0},
	{"PRMD_HT30_CtrlMd_RT", _K_D_IO, PRMD_HT30_CtrlMd_RT, 0},
	{"PRMD_HT31_CtrlMd_RT", _K_D_IO, PRMD_HT31_CtrlMd_RT, 0},
	{"PRMD_HT32_CtrlMd_RT", _K_D_IO, PRMD_HT32_CtrlMd_RT, 0},

	{"PRMA_HT01_TmpHgLmt_RT", _K_A_IO, PRMA_HT01_TmpHgLmt_RT, 0},
	{"PRMA_HT02_TmpHgLmt_RT", _K_A_IO, PRMA_HT02_TmpHgLmt_RT, 0},
	{"PRMA_HT03_TmpHgLmt_RT", _K_A_IO, PRMA_HT03_TmpHgLmt_RT, 0},
	{"PRMA_HT04_TmpHgLmt_RT", _K_A_IO, PRMA_HT04_TmpHgLmt_RT, 0},
	{"PRMA_HT05_TmpHgLmt_RT", _K_A_IO, PRMA_HT05_TmpHgLmt_RT, 0},
	{"PRMA_HT06_TmpHgLmt_RT", _K_A_IO, PRMA_HT06_TmpHgLmt_RT, 0},
	{"PRMA_HT07_TmpHgLmt_RT", _K_A_IO, PRMA_HT07_TmpHgLmt_RT, 0},
	{"PRMA_HT08_TmpHgLmt_RT", _K_A_IO, PRMA_HT08_TmpHgLmt_RT, 0},
	{"PRMA_HT09_TmpHgLmt_RT", _K_A_IO, PRMA_HT09_TmpHgLmt_RT, 0},
	{"PRMA_HT10_TmpHgLmt_RT", _K_A_IO, PRMA_HT10_TmpHgLmt_RT, 0},
	{"PRMA_HT11_TmpHgLmt_RT", _K_A_IO, PRMA_HT11_TmpHgLmt_RT, 0},
	{"PRMA_HT12_TmpHgLmt_RT", _K_A_IO, PRMA_HT12_TmpHgLmt_RT, 0},
	{"PRMA_HT13_TmpHgLmt_RT", _K_A_IO, PRMA_HT13_TmpHgLmt_RT, 0},
	{"PRMA_HT14_TmpHgLmt_RT", _K_A_IO, PRMA_HT14_TmpHgLmt_RT, 0},
	{"PRMA_HT15_TmpHgLmt_RT", _K_A_IO, PRMA_HT15_TmpHgLmt_RT, 0},
	{"PRMA_HT16_TmpHgLmt_RT", _K_A_IO, PRMA_HT16_TmpHgLmt_RT, 0},
	{"PRMA_HT17_TmpHgLmt_RT", _K_A_IO, PRMA_HT17_TmpHgLmt_RT, 0},
	{"PRMA_HT18_TmpHgLmt_RT", _K_A_IO, PRMA_HT18_TmpHgLmt_RT, 0},
	{"PRMA_HT19_TmpHgLmt_RT", _K_A_IO, PRMA_HT19_TmpHgLmt_RT, 0},
	{"PRMA_HT20_TmpHgLmt_RT", _K_A_IO, PRMA_HT20_TmpHgLmt_RT, 0},
	{"PRMA_HT21_TmpHgLmt_RT", _K_A_IO, PRMA_HT21_TmpHgLmt_RT, 0},
	{"PRMA_HT22_TmpHgLmt_RT", _K_A_IO, PRMA_HT22_TmpHgLmt_RT, 0},
	{"PRMA_HT23_TmpHgLmt_RT", _K_A_IO, PRMA_HT23_TmpHgLmt_RT, 0},
	{"PRMA_HT24_TmpHgLmt_RT", _K_A_IO, PRMA_HT24_TmpHgLmt_RT, 0},
	{"PRMA_HT25_TmpHgLmt_RT", _K_A_IO, PRMA_HT25_TmpHgLmt_RT, 0},
	{"PRMA_HT26_TmpHgLmt_RT", _K_A_IO, PRMA_HT26_TmpHgLmt_RT, 0},
	{"PRMA_HT27_TmpHgLmt_RT", _K_A_IO, PRMA_HT27_TmpHgLmt_RT, 0},
	{"PRMA_HT28_TmpHgLmt_RT", _K_A_IO, PRMA_HT28_TmpHgLmt_RT, 0},
	{"PRMA_HT29_TmpHgLmt_RT", _K_A_IO, PRMA_HT29_TmpHgLmt_RT, 0},
	{"PRMA_HT30_TmpHgLmt_RT", _K_A_IO, PRMA_HT30_TmpHgLmt_RT, 0},
	{"PRMA_HT31_TmpHgLmt_RT", _K_A_IO, PRMA_HT31_TmpHgLmt_RT, 0},
	{"PRMA_HT32_TmpHgLmt_RT", _K_A_IO, PRMA_HT32_TmpHgLmt_RT, 0},

	{"PRMA_HT01_TmpLwLmt_RT", _K_A_IO, PRMA_HT01_TmpLwLmt_RT, 0},
	{"PRMA_HT02_TmpLwLmt_RT", _K_A_IO, PRMA_HT02_TmpLwLmt_RT, 0},
	{"PRMA_HT03_TmpLwLmt_RT", _K_A_IO, PRMA_HT03_TmpLwLmt_RT, 0},
	{"PRMA_HT04_TmpLwLmt_RT", _K_A_IO, PRMA_HT04_TmpLwLmt_RT, 0},
	{"PRMA_HT05_TmpLwLmt_RT", _K_A_IO, PRMA_HT05_TmpLwLmt_RT, 0},
	{"PRMA_HT06_TmpLwLmt_RT", _K_A_IO, PRMA_HT06_TmpLwLmt_RT, 0},
	{"PRMA_HT07_TmpLwLmt_RT", _K_A_IO, PRMA_HT07_TmpLwLmt_RT, 0},
	{"PRMA_HT08_TmpLwLmt_RT", _K_A_IO, PRMA_HT08_TmpLwLmt_RT, 0},
	{"PRMA_HT09_TmpLwLmt_RT", _K_A_IO, PRMA_HT09_TmpLwLmt_RT, 0},
	{"PRMA_HT10_TmpLwLmt_RT", _K_A_IO, PRMA_HT10_TmpLwLmt_RT, 0},
	{"PRMA_HT11_TmpLwLmt_RT", _K_A_IO, PRMA_HT11_TmpLwLmt_RT, 0},
	{"PRMA_HT12_TmpLwLmt_RT", _K_A_IO, PRMA_HT12_TmpLwLmt_RT, 0},
	{"PRMA_HT13_TmpLwLmt_RT", _K_A_IO, PRMA_HT13_TmpLwLmt_RT, 0},
	{"PRMA_HT14_TmpLwLmt_RT", _K_A_IO, PRMA_HT14_TmpLwLmt_RT, 0},
	{"PRMA_HT15_TmpLwLmt_RT", _K_A_IO, PRMA_HT15_TmpLwLmt_RT, 0},
	{"PRMA_HT16_TmpLwLmt_RT", _K_A_IO, PRMA_HT16_TmpLwLmt_RT, 0},
	{"PRMA_HT17_TmpLwLmt_RT", _K_A_IO, PRMA_HT17_TmpLwLmt_RT, 0},
	{"PRMA_HT18_TmpLwLmt_RT", _K_A_IO, PRMA_HT18_TmpLwLmt_RT, 0},
	{"PRMA_HT19_TmpLwLmt_RT", _K_A_IO, PRMA_HT19_TmpLwLmt_RT, 0},
	{"PRMA_HT20_TmpLwLmt_RT", _K_A_IO, PRMA_HT20_TmpLwLmt_RT, 0},
	{"PRMA_HT21_TmpLwLmt_RT", _K_A_IO, PRMA_HT21_TmpLwLmt_RT, 0},
	{"PRMA_HT22_TmpLwLmt_RT", _K_A_IO, PRMA_HT22_TmpLwLmt_RT, 0},
	{"PRMA_HT23_TmpLwLmt_RT", _K_A_IO, PRMA_HT23_TmpLwLmt_RT, 0},
	{"PRMA_HT24_TmpLwLmt_RT", _K_A_IO, PRMA_HT24_TmpLwLmt_RT, 0},
	{"PRMA_HT25_TmpLwLmt_RT", _K_A_IO, PRMA_HT25_TmpLwLmt_RT, 0},
	{"PRMA_HT26_TmpLwLmt_RT", _K_A_IO, PRMA_HT26_TmpLwLmt_RT, 0},
	{"PRMA_HT27_TmpLwLmt_RT", _K_A_IO, PRMA_HT27_TmpLwLmt_RT, 0},
	{"PRMA_HT28_TmpLwLmt_RT", _K_A_IO, PRMA_HT28_TmpLwLmt_RT, 0},
	{"PRMA_HT29_TmpLwLmt_RT", _K_A_IO, PRMA_HT29_TmpLwLmt_RT, 0},
	{"PRMA_HT30_TmpLwLmt_RT", _K_A_IO, PRMA_HT30_TmpLwLmt_RT, 0},
	{"PRMA_HT31_TmpLwLmt_RT", _K_A_IO, PRMA_HT31_TmpLwLmt_RT, 0},
	{"PRMA_HT32_TmpLwLmt_RT", _K_A_IO, PRMA_HT32_TmpLwLmt_RT, 0},

	{"PRMA_HT01_Devi_RT", _K_A_IO, PRMA_HT01_Devi_RT, 0},
	{"PRMA_HT02_Devi_RT", _K_A_IO, PRMA_HT02_Devi_RT, 0},
	{"PRMA_HT03_Devi_RT", _K_A_IO, PRMA_HT03_Devi_RT, 0},
	{"PRMA_HT04_Devi_RT", _K_A_IO, PRMA_HT04_Devi_RT, 0},
	{"PRMA_HT05_Devi_RT", _K_A_IO, PRMA_HT05_Devi_RT, 0},
	{"PRMA_HT06_Devi_RT", _K_A_IO, PRMA_HT06_Devi_RT, 0},
	{"PRMA_HT07_Devi_RT", _K_A_IO, PRMA_HT07_Devi_RT, 0},
	{"PRMA_HT08_Devi_RT", _K_A_IO, PRMA_HT08_Devi_RT, 0},
	{"PRMA_HT09_Devi_RT", _K_A_IO, PRMA_HT09_Devi_RT, 0},
	{"PRMA_HT10_Devi_RT", _K_A_IO, PRMA_HT10_Devi_RT, 0},
	{"PRMA_HT11_Devi_RT", _K_A_IO, PRMA_HT11_Devi_RT, 0},
	{"PRMA_HT12_Devi_RT", _K_A_IO, PRMA_HT12_Devi_RT, 0},
	{"PRMA_HT13_Devi_RT", _K_A_IO, PRMA_HT13_Devi_RT, 0},
	{"PRMA_HT14_Devi_RT", _K_A_IO, PRMA_HT14_Devi_RT, 0},
	{"PRMA_HT15_Devi_RT", _K_A_IO, PRMA_HT15_Devi_RT, 0},
	{"PRMA_HT16_Devi_RT", _K_A_IO, PRMA_HT16_Devi_RT, 0},
	{"PRMA_HT17_Devi_RT", _K_A_IO, PRMA_HT17_Devi_RT, 0},
	{"PRMA_HT18_Devi_RT", _K_A_IO, PRMA_HT18_Devi_RT, 0},
	{"PRMA_HT19_Devi_RT", _K_A_IO, PRMA_HT19_Devi_RT, 0},
	{"PRMA_HT20_Devi_RT", _K_A_IO, PRMA_HT20_Devi_RT, 0},
	{"PRMA_HT21_Devi_RT", _K_A_IO, PRMA_HT21_Devi_RT, 0},
	{"PRMA_HT22_Devi_RT", _K_A_IO, PRMA_HT22_Devi_RT, 0},
	{"PRMA_HT23_Devi_RT", _K_A_IO, PRMA_HT23_Devi_RT, 0},
	{"PRMA_HT24_Devi_RT", _K_A_IO, PRMA_HT24_Devi_RT, 0},
	{"PRMA_HT25_Devi_RT", _K_A_IO, PRMA_HT25_Devi_RT, 0},
	{"PRMA_HT26_Devi_RT", _K_A_IO, PRMA_HT26_Devi_RT, 0},
	{"PRMA_HT27_Devi_RT", _K_A_IO, PRMA_HT27_Devi_RT, 0},
	{"PRMA_HT28_Devi_RT", _K_A_IO, PRMA_HT28_Devi_RT, 0},
	{"PRMA_HT29_Devi_RT", _K_A_IO, PRMA_HT29_Devi_RT, 0},
	{"PRMA_HT30_Devi_RT", _K_A_IO, PRMA_HT30_Devi_RT, 0},
	{"PRMA_HT31_Devi_RT", _K_A_IO, PRMA_HT31_Devi_RT, 0},
	{"PRMA_HT32_Devi_RT", _K_A_IO, PRMA_HT32_Devi_RT, 0},

	{"PRMA_HT01_OutPw_RT", _K_A_IO, PRMA_HT01_OutPw_RT, 0},
	{"PRMA_HT02_OutPw_RT", _K_A_IO, PRMA_HT02_OutPw_RT, 0},
	{"PRMA_HT03_OutPw_RT", _K_A_IO, PRMA_HT03_OutPw_RT, 0},
	{"PRMA_HT04_OutPw_RT", _K_A_IO, PRMA_HT04_OutPw_RT, 0},
	{"PRMA_HT05_OutPw_RT", _K_A_IO, PRMA_HT05_OutPw_RT, 0},
	{"PRMA_HT06_OutPw_RT", _K_A_IO, PRMA_HT06_OutPw_RT, 0},
	{"PRMA_HT07_OutPw_RT", _K_A_IO, PRMA_HT07_OutPw_RT, 0},
	{"PRMA_HT08_OutPw_RT", _K_A_IO, PRMA_HT08_OutPw_RT, 0},
	{"PRMA_HT09_OutPw_RT", _K_A_IO, PRMA_HT09_OutPw_RT, 0},
	{"PRMA_HT10_OutPw_RT", _K_A_IO, PRMA_HT10_OutPw_RT, 0},
	{"PRMA_HT11_OutPw_RT", _K_A_IO, PRMA_HT11_OutPw_RT, 0},
	{"PRMA_HT12_OutPw_RT", _K_A_IO, PRMA_HT12_OutPw_RT, 0},
	{"PRMA_HT13_OutPw_RT", _K_A_IO, PRMA_HT13_OutPw_RT, 0},
	{"PRMA_HT14_OutPw_RT", _K_A_IO, PRMA_HT14_OutPw_RT, 0},
	{"PRMA_HT15_OutPw_RT", _K_A_IO, PRMA_HT15_OutPw_RT, 0},
	{"PRMA_HT16_OutPw_RT", _K_A_IO, PRMA_HT16_OutPw_RT, 0},
	{"PRMA_HT17_OutPw_RT", _K_A_IO, PRMA_HT17_OutPw_RT, 0},
	{"PRMA_HT18_OutPw_RT", _K_A_IO, PRMA_HT18_OutPw_RT, 0},
	{"PRMA_HT19_OutPw_RT", _K_A_IO, PRMA_HT19_OutPw_RT, 0},
	{"PRMA_HT20_OutPw_RT", _K_A_IO, PRMA_HT20_OutPw_RT, 0},
	{"PRMA_HT21_OutPw_RT", _K_A_IO, PRMA_HT21_OutPw_RT, 0},
	{"PRMA_HT22_OutPw_RT", _K_A_IO, PRMA_HT22_OutPw_RT, 0},
	{"PRMA_HT23_OutPw_RT", _K_A_IO, PRMA_HT23_OutPw_RT, 0},
	{"PRMA_HT24_OutPw_RT", _K_A_IO, PRMA_HT24_OutPw_RT, 0},
	{"PRMA_HT25_OutPw_RT", _K_A_IO, PRMA_HT25_OutPw_RT, 0},
	{"PRMA_HT26_OutPw_RT", _K_A_IO, PRMA_HT26_OutPw_RT, 0},
	{"PRMA_HT27_OutPw_RT", _K_A_IO, PRMA_HT27_OutPw_RT, 0},
	{"PRMA_HT28_OutPw_RT", _K_A_IO, PRMA_HT28_OutPw_RT, 0},
	{"PRMA_HT29_OutPw_RT", _K_A_IO, PRMA_HT29_OutPw_RT, 0},
	{"PRMA_HT30_OutPw_RT", _K_A_IO, PRMA_HT30_OutPw_RT, 0},
	{"PRMA_HT31_OutPw_RT", _K_A_IO, PRMA_HT31_OutPw_RT, 0},
	{"PRMA_HT32_OutPw_RT", _K_A_IO, PRMA_HT32_OutPw_RT, 0},

	{"PRMA_HT01_OutPwLmt_RT", _K_A_IO, PRMA_HT01_OutPwLmt_RT, 0},
	{"PRMA_HT02_OutPwLmt_RT", _K_A_IO, PRMA_HT02_OutPwLmt_RT, 0},
	{"PRMA_HT03_OutPwLmt_RT", _K_A_IO, PRMA_HT03_OutPwLmt_RT, 0},
	{"PRMA_HT04_OutPwLmt_RT", _K_A_IO, PRMA_HT04_OutPwLmt_RT, 0},
	{"PRMA_HT05_OutPwLmt_RT", _K_A_IO, PRMA_HT05_OutPwLmt_RT, 0},
	{"PRMA_HT06_OutPwLmt_RT", _K_A_IO, PRMA_HT06_OutPwLmt_RT, 0},
	{"PRMA_HT07_OutPwLmt_RT", _K_A_IO, PRMA_HT07_OutPwLmt_RT, 0},
	{"PRMA_HT08_OutPwLmt_RT", _K_A_IO, PRMA_HT08_OutPwLmt_RT, 0},
	{"PRMA_HT09_OutPwLmt_RT", _K_A_IO, PRMA_HT09_OutPwLmt_RT, 0},
	{"PRMA_HT10_OutPwLmt_RT", _K_A_IO, PRMA_HT10_OutPwLmt_RT, 0},
	{"PRMA_HT11_OutPwLmt_RT", _K_A_IO, PRMA_HT11_OutPwLmt_RT, 0},
	{"PRMA_HT12_OutPwLmt_RT", _K_A_IO, PRMA_HT12_OutPwLmt_RT, 0},
	{"PRMA_HT13_OutPwLmt_RT", _K_A_IO, PRMA_HT13_OutPwLmt_RT, 0},
	{"PRMA_HT14_OutPwLmt_RT", _K_A_IO, PRMA_HT14_OutPwLmt_RT, 0},
	{"PRMA_HT15_OutPwLmt_RT", _K_A_IO, PRMA_HT15_OutPwLmt_RT, 0},
	{"PRMA_HT16_OutPwLmt_RT", _K_A_IO, PRMA_HT16_OutPwLmt_RT, 0},
	{"PRMA_HT17_OutPwLmt_RT", _K_A_IO, PRMA_HT17_OutPwLmt_RT, 0},
	{"PRMA_HT18_OutPwLmt_RT", _K_A_IO, PRMA_HT18_OutPwLmt_RT, 0},
	{"PRMA_HT19_OutPwLmt_RT", _K_A_IO, PRMA_HT19_OutPwLmt_RT, 0},
	{"PRMA_HT20_OutPwLmt_RT", _K_A_IO, PRMA_HT20_OutPwLmt_RT, 0},
	{"PRMA_HT21_OutPwLmt_RT", _K_A_IO, PRMA_HT21_OutPwLmt_RT, 0},
	{"PRMA_HT22_OutPwLmt_RT", _K_A_IO, PRMA_HT22_OutPwLmt_RT, 0},
	{"PRMA_HT23_OutPwLmt_RT", _K_A_IO, PRMA_HT23_OutPwLmt_RT, 0},
	{"PRMA_HT24_OutPwLmt_RT", _K_A_IO, PRMA_HT24_OutPwLmt_RT, 0},
	{"PRMA_HT25_OutPwLmt_RT", _K_A_IO, PRMA_HT25_OutPwLmt_RT, 0},
	{"PRMA_HT26_OutPwLmt_RT", _K_A_IO, PRMA_HT26_OutPwLmt_RT, 0},
	{"PRMA_HT27_OutPwLmt_RT", _K_A_IO, PRMA_HT27_OutPwLmt_RT, 0},
	{"PRMA_HT28_OutPwLmt_RT", _K_A_IO, PRMA_HT28_OutPwLmt_RT, 0},
	{"PRMA_HT29_OutPwLmt_RT", _K_A_IO, PRMA_HT29_OutPwLmt_RT, 0},
	{"PRMA_HT30_OutPwLmt_RT", _K_A_IO, PRMA_HT30_OutPwLmt_RT, 0},
	{"PRMA_HT31_OutPwLmt_RT", _K_A_IO, PRMA_HT31_OutPwLmt_RT, 0},
	{"PRMA_HT32_OutPwLmt_RT", _K_A_IO, PRMA_HT32_OutPwLmt_RT, 0},

	{"CTC_CommStatus", _K_D_IO, PM_COMMSTATUS, 0},
	{"VacuumStateDM", _K_D_IO, VacuumStateDM, 0},

	""};

//------------------------------------------------------------------------------------------

typedef void(__cdecl *_THREAD_FUNC)(void *lpParam);
enum
{
	THREAD_STOP,
	THREAD_RUN,
	THREAD_FAILED
};
enum
{
	HTR_SET_STOP,
	HTR_SET_START,
	HTR_SETTING,
	HTR_WAITING,
	HTR_SET_END,
	HTR_SET_FAILED
};
enum
{
	Unuse,
	Local,
	DisConn,
	Standby,
	Maint,
	Uninit,
	EnablePM,
	CTCinUSE,
	CTCDis,
	CTCDis2,
	CTCEnaPM,
	CTCDisHW,
	CTCDIsHW2
};
enum
{
	ATM,
	Vacuum,
	GoATM,
	GoVacuum,
	Abort
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int g_nHtrSet_Thread = THREAD_STOP;
int g_nHtrSet_Status = HTR_SET_END;
int g_dbHtrTempLimit_Offset = 0;

void HtrSet_Thread()
{
	// 2006.07.11
	enum
	{
		HA_SPARE0,
		HA_SPARE1,
		HA_LOWDEV,
		HA_HIGHDEV,
		HA_LOWLMT,
		HA_HIGHLMT,
		HA_TCREV,
		HA_TCSHORT,
		HA_TCBREAK,
		HA_RTDOPEN,
		HA_RTDSHORT,
		HA_SPARE11,
		HA_SPARE12,
		HA_FCALERR,
		HA_OCALERR,
		HA_NORMAL
	};
	enum
	{
		HTR_OFF,
		HTR_ON
	};
	enum
	{
		HTR_NOTUSE,
		HTR_USE
	};
	enum
	{
		HCTRL_STABLE,
		HCTRL_SETTEMP,
		HCTRL_UNSTABLE
	};

	int i;
	int nIOStatus;
	int nHtrCnt;
	double dbLowLmt;
	double dbHighLmt;
	double dbTemp;
	double dbTol;
	double dbNowSp;
	int nAlarm; //2006.06.12

	do
	{
		// Set
		g_nHtrSet_Status = HTR_SETTING;
		for (i = 0; i < MAX_HEATER; i++)
		{
			// 2006.06.12
			if (READ_DIGITAL(HT01_UseDM + i, &nIOStatus) == HTR_USE && READ_DIGITAL(HT01_OnOff + i, &nIOStatus) == HTR_ON)
				WRITE_DIGITAL(HT01_CtrlMd2DM + i, HCTRL_SETTEMP, &nIOStatus);

			if (g_HtrList[i].nOpt == 1) // 0:NotSet 1:Set
			{
				dbNowSp = READ_ANALOG(HT01_TmpSpAIO + i, &nIOStatus);
				dbTol = READ_ANALOG(HT01_DeviAIO + i, &nIOStatus);

				// 2006.03.02
				if (g_dbHtrTempLimit_Offset > 0)
				{
					if (g_HtrList[i].dbSp > dbNowSp)
					{
						dbHighLmt = g_HtrList[i].dbSp + dbTol + g_dbHtrTempLimit_Offset;
						WRITE_ANALOG(HT01_TmpHgLmtAIO + i, dbHighLmt, &nIOStatus);
					}
					else if (g_HtrList[i].dbSp < dbNowSp)
					{
						dbLowLmt = g_HtrList[i].dbSp - dbTol - g_dbHtrTempLimit_Offset;
						WRITE_ANALOG(HT01_TmpLwLmtAIO + i, dbLowLmt, &nIOStatus);
					}
				}

				if (g_HtrList[i].dbSp != dbNowSp)
				{
					// WRITE_DIGITAL(HT01_CtrlMd2DM + i, 1, &nIOStatus);
					WRITE_ANALOG(HT01_TmpSpAIO + i, g_HtrList[i].dbSp, &nIOStatus);
					if (g_nHtrSet_Status != HTR_SETTING)
					{
						g_nHtrSet_Status = HTR_SET_FAILED;
						break;
					};
				}
				if (!WAIT_SECONDS(1))
				{
					g_nHtrSet_Status = HTR_SET_FAILED;
					break;
				};
			}
		}
		if (g_nHtrSet_Status != HTR_SETTING)
			break;

		// Monitoring

		do
		{
			nHtrCnt = 0;
			for (i = 0; i < MAX_HEATER; i++)
			{
				if (g_HtrList[i].nOpt == 1 /* && g_HtrList[i].bCheck == FALSE*/)
				{
					dbTol = READ_ANALOG(HT01_DeviAIO + i, &nIOStatus);
					dbLowLmt = g_HtrList[i].dbSp - dbTol;
					dbHighLmt = g_HtrList[i].dbSp + dbTol;

					if (g_nHtrSet_Status != HTR_SETTING)
					{
						g_nHtrSet_Status = HTR_SET_FAILED;
						break;
					};
					if (!WAIT_SECONDS(1))
					{
						g_nHtrSet_Status = HTR_SET_FAILED;
						break;
					};

					dbTemp = READ_ANALOG(HT01_TmpRdAI + i, &nIOStatus);
					nAlarm = READ_DIGITAL(HT01_AlrmDscDI + i, &nIOStatus);

					if (dbTemp >= dbLowLmt && dbTemp <= dbHighLmt && nAlarm != HA_LOWDEV && nAlarm != HA_HIGHDEV)
					{
						g_HtrList[i].bCheck = TRUE;

						// 2006.03.02
						if (g_dbHtrTempLimit_Offset > 0)
						{
							dbHighLmt += g_dbHtrTempLimit_Offset;
							dbLowLmt -= g_dbHtrTempLimit_Offset;
							WRITE_ANALOG(HT01_TmpHgLmtAIO + i, dbHighLmt, &nIOStatus);
							WRITE_ANALOG(HT01_TmpLwLmtAIO + i, dbLowLmt, &nIOStatus);
						}
					}
					else
						nHtrCnt++;

					if (g_nHtrSet_Status != HTR_SETTING)
					{
						g_nHtrSet_Status = HTR_SET_FAILED;
						break;
					};
					if (!WAIT_SECONDS(1))
					{
						g_nHtrSet_Status = HTR_SET_FAILED;
						break;
					};
				}
				//-----------------------------------------------------------------------
				// 2006.06.12
				else if (READ_DIGITAL(HT01_UseDM + i, &nIOStatus) == 1 && READ_DIGITAL(HT01_OnOff + i, &nIOStatus) == 1)
				{
					nAlarm = READ_DIGITAL(HT01_AlrmDscDI + i, &nIOStatus);
					if (nAlarm == HA_LOWDEV || nAlarm == HA_HIGHDEV // 2006.07.11
						|| nAlarm == HA_LOWLMT || nAlarm == HA_HIGHLMT)
						nHtrCnt++;
				}
				//-----------------------------------------------------------------------
			}

			if (g_nHtrSet_Status == HTR_SETTING && nHtrCnt == 0)
				g_nHtrSet_Status = HTR_WAITING;
		} while (g_nHtrSet_Status == HTR_SETTING);

		if (g_nHtrSet_Status == HTR_SET_FAILED)
			break;
		for (i = 0; i < 15 && g_nHtrSet_Status == HTR_WAITING; i++)
		{
			if (!WAIT_SECONDS(1))
				break;
		}
	} while (0);

	for (i = 0; i < MAX_HEATER; i++)
	{
		// if(g_HtrList[i].nOpt == 1)		// 0:NotSet 1:Set

		// 2006.06.12
		if (READ_DIGITAL(HT01_UseDM + i, &nIOStatus) == HTR_USE && READ_DIGITAL(HT01_OnOff + i, &nIOStatus) == HTR_ON)
			WRITE_DIGITAL(HT01_CtrlMd2DM + i, HCTRL_STABLE, &nIOStatus);
	}
	if (g_nHtrSet_Status == HTR_WAITING)
		g_nHtrSet_Status = HTR_SET_END;
	g_nHtrSet_Thread = THREAD_STOP;
	_endthread();
}

BOOL StartHeaterSetting()
{
	g_nHtrSet_Status = HTR_SET_STOP;
	while (g_nHtrSet_Thread == THREAD_RUN)
	{
		g_nHtrSet_Status = HTR_SET_STOP;
		if (!WAIT_SECONDS(1))
			return FALSE;
	}

	g_nHtrSet_Thread = THREAD_RUN;
	g_nHtrSet_Status = HTR_SET_START;
	_beginthread((_THREAD_FUNC)HtrSet_Thread, 0, NULL);
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status SetTemp(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	int nIOStatus = 0;
	int nHtrNum, nHtrIdx;
	double dbSetPoint;

	int i;
	for (i = 0; i < MAX_HEATER; i++)
	{
		g_HtrList[i].nOpt = 0;
		g_HtrList[i].bCheck = FALSE;
	}

	CTextParser tp(szParam);
	do
	{
		if (!tp.GetInt(&nHtrNum))
			break;
		tp.GetDbFloat(&dbSetPoint);

		nHtrIdx = nHtrNum - 1;
		g_HtrList[nHtrIdx].dbSp = dbSetPoint;
		g_HtrList[nHtrIdx].nOpt = 1;
	} while (!tp.IsEnd());

	if (!StartHeaterSetting())
		msRet = SYS_ABORTED;
	return msRet;
}

Module_Status CheckTemp(char *szParam)
{
	Module_Status msRet = SYS_ABORTED;

	do
	{
		if (g_nHtrSet_Status != HTR_SET_END)
			break;
		msRet = SYS_SUCCESS;
	} while (0);
	return msRet;
}

Module_Status StopControl(char *szParam)
{
	g_nHtrSet_Status = HTR_SET_STOP;
	while (g_nHtrSet_Thread == THREAD_RUN)
	{
		g_nHtrSet_Status = HTR_SET_STOP;
		if (!WAIT_SECONDS(1))
		{
			return SYS_ABORTED;
		}
	}
	return SYS_SUCCESS;
}

Module_Status HeaterAutoSet(char *szParam)
{
	Module_Status msRet = SYS_SUCCESS;
	enum
	{
		FAILED,
		SETTING
	};

	int i, chkSumIdx, reCnt = 0;
	int rdIOidx = 0, rdChkSum = 0;
	int wrIOidx = 0, wrChkSum = 0;
	int nCommStatus;
	int nReadIOStatus[SETTING_ITEM];
	int nWriteIOStatus[SETTING_ITEM];
	int nHtrSts[MAX_HEATER];
	int nHtrMode[MAX_HEATER];
	int nAutoSetStatus = SETTING;
	double dbHtrSetTmp[MAX_HEATER];
	double dbHtrHiLmt[MAX_HEATER];
	double dbHtrLwLmt[MAX_HEATER];
	double dbHtrDev[MAX_HEATER];
	double dbHtrSetPw[MAX_HEATER];
	double dbHtrPwLmt[MAX_HEATER];

	if (READ_DIGITAL(PM_COMMSTATUS, &nCommStatus) != Maint)
	{
		msRet = SYS_ABORTED;
		if (ALARM_STATUS(HTRAutoSetMaint) == 0)
		{
			ALARM_POST(HTRAutoSetMaint);
			printf("[Debug] PM_COMMSTATUS not Maint!!! \n");
		}
		return msRet;
	}
	if (READ_DIGITAL(VacuumStateDM, &nCommStatus) != Vacuum)
	{
		msRet = SYS_ABORTED;
		if (ALARM_STATUS(HTRAutoSetVacuum) == 0)
		{
			ALARM_POST(HTRAutoSetVacuum);
			printf("[Debug] VacuumStateDM not Vacuum!!! \n");
		}
		return msRet;
	}

	for (i = 0; i < MAX_HEATER; i++)
	{
		if (!WAIT_SECONDS(0.1))
			break;
		if (READ_DIGITAL(HT01_UseDM + i, &nCommStatus) == 1)
		{
			do
			{
				if (strcmp(szParam, "LT_AUTOSET_TEMP") == 0)
				{
					nHtrSts[i] = READ_DIGITAL(PRMD_HT01_OnOff_LT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrSetTmp[i] = READ_ANALOG(PRMA_HT01_TmpSp_LT + i, &nReadIOStatus[rdIOidx++]);
					nHtrMode[i] = READ_DIGITAL(PRMD_HT01_CtrlMd_LT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrHiLmt[i] = READ_ANALOG(PRMA_HT01_TmpHgLmt_LT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrLwLmt[i] = READ_ANALOG(PRMA_HT01_TmpLwLmt_LT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrDev[i] = READ_ANALOG(PRMA_HT01_Devi_LT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrSetPw[i] = READ_ANALOG(PRMA_HT01_OutPw_LT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrPwLmt[i] = READ_ANALOG(PRMA_HT01_OutPwLmt_LT + i, &nReadIOStatus[rdIOidx++]);
				}
				else if (strcmp(szParam, "RT_AUTOSET_TEMP") == 0)
				{
					nHtrSts[i] = READ_DIGITAL(PRMD_HT01_OnOff_RT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrSetTmp[i] = READ_ANALOG(PRMA_HT01_TmpSp_RT + i, &nReadIOStatus[rdIOidx++]);
					nHtrMode[i] = READ_DIGITAL(PRMD_HT01_CtrlMd_RT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrHiLmt[i] = READ_ANALOG(PRMA_HT01_TmpHgLmt_RT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrLwLmt[i] = READ_ANALOG(PRMA_HT01_TmpLwLmt_RT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrDev[i] = READ_ANALOG(PRMA_HT01_Devi_RT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrSetPw[i] = READ_ANALOG(PRMA_HT01_OutPw_RT + i, &nReadIOStatus[rdIOidx++]);
					dbHtrPwLmt[i] = READ_ANALOG(PRMA_HT01_OutPwLmt_RT + i, &nReadIOStatus[rdIOidx++]);
				}
				// printf("[Debug] i:[%d] READ_PARAMETER:[%d] [%f] [%d] [%f] [%f] [%f] [%f] [%f] [%f] [%f] [%f] \n",
				// i, nHtrSts[i], dbHtrSetTmp[i], nHtrMode[i], dbHtrHiLmt[i], dbHtrLwLmt[i], dbHtrDev[i], dbHtrSetPw[i], dbHtrPwLmt[i]);

				WRITE_DIGITAL(HT01_OnOff + i, nHtrSts[i], &nWriteIOStatus[wrIOidx++]);
				WRITE_ANALOG(HT01_TmpSpAIO + i, dbHtrSetTmp[i], &nWriteIOStatus[wrIOidx++]);
				WRITE_DIGITAL(HT01_CtrlMdDIO + i, nHtrMode[i], &nWriteIOStatus[wrIOidx++]);
				WRITE_ANALOG(HT01_TmpHgLmtAIO + i, dbHtrHiLmt[i], &nWriteIOStatus[wrIOidx++]);
				WRITE_ANALOG(HT01_TmpLwLmtAIO + i, dbHtrLwLmt[i], &nWriteIOStatus[wrIOidx++]);
				WRITE_ANALOG(HT01_DeviAIO + i, dbHtrDev[i], &nWriteIOStatus[wrIOidx++]);
				WRITE_ANALOG(HT01_OutPwAO + i, dbHtrSetPw[i], &nWriteIOStatus[wrIOidx++]);
				WRITE_ANALOG(HT01_OutPwLmtAIO + i, dbHtrPwLmt[i], &nWriteIOStatus[wrIOidx++]);

				for (chkSumIdx = 0; chkSumIdx < SETTING_ITEM; chkSumIdx++)
				{
					rdChkSum = rdChkSum + nWriteIOStatus[chkSumIdx];
					wrChkSum = wrChkSum + nWriteIOStatus[chkSumIdx];
				}
				if (rdChkSum != SETTING_ITEM || wrChkSum != SETTING_ITEM)
				{
					nAutoSetStatus = FAILED;
					reCnt++;
					if (reCnt > 3)
					{
						if (rdChkSum != SETTING_ITEM)
						{
							if (ALARM_STATUS(HTRAutoSetPramReadFailed) == 0)
							{
								ALARM_POST(HTRAutoSetPramReadFailed);
								printf("[Debug] Temp Set Error !!! \n");
							}
						}
						else
						{
							if (ALARM_STATUS(HTRAutoSetPramSetFailed) == 0)
							{
								ALARM_POST(HTRAutoSetPramSetFailed);
								printf("[Debug] Temp Set Error !!! \n");
							}
						}
						break;
					}
				}
				else
				{
					nAutoSetStatus = SETTING;
				}

				rdChkSum = 0;
				wrChkSum = 0;
				rdIOidx = 0;
				wrIOidx = 0;

			} while (nAutoSetStatus != SETTING);
		}
		if (reCnt > 3 && nAutoSetStatus == FAILED)
		{
			// Error Alarm
			msRet = SYS_ABORTED;
			break;
		}
	}

	return msRet;
}

//==========================================================================================
extern "C"
{

	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------

	Module_Status Program_Main()
	{
		Module_Status msRet = SYS_SUCCESS;
		int nParLen = 0;
		char *pParam;
		char szCmd[41], szSubParam[1000];

		//	printf("Entered %s\n", FUNC_NAME);
		do
		{
			pParam = PROGRAM_PARAMETER_READ();
			nParLen = strlen(pParam);

			if (nParLen <= 0)
			{
				printf("Invalid Parameter in %s\n", FUNC_NAME);
				msRet = SYS_ABORTED;
				break;
			}
			else
			{
				STR_SEPERATE(pParam, szCmd, szSubParam, 999);
				if (strcmp(szCmd, "SET_TEMP") == 0)
					msRet = SetTemp(szSubParam);
				else if (strcmp(szCmd, "CHECK_TEMP") == 0)
					msRet = CheckTemp(szSubParam);
				else if (strcmp(szCmd, "STOP") == 0)
					msRet = StopControl(szSubParam);
				else if (strcmp(szCmd, "LT_AUTOSET_TEMP") == 0)
				{
					printf("[Debug] LT_AUTOSET_TEMP Parameter in %s\n", FUNC_NAME);
					msRet = HeaterAutoSet(szCmd);
				}
				else if (strcmp(szCmd, "RT_AUTOSET_TEMP") == 0)
				{
					printf("[Debug] RT_AUTOSET_TEMP Parameter in %s\n", FUNC_NAME);
					msRet = HeaterAutoSet(szCmd);
				}
				else
				{
					printf("Invalid Parameter in %s\n", FUNC_NAME);
					msRet = SYS_ABORTED;
					break;
				}
			}
		} while (0);

		//	printf("Leave %s\n", FUNC_NAME);
		return msRet;
	}

	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	void Event_Message_Received()
	{
	}

	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------
	void Program_Enter_Code()
	{

		//-------------------------------------------------------------
		// 2006.03.02
		char szParam[256];
		strcpy(szParam, PROGRAM_ARGUMENT_READ());
#ifdef _DEBUG
		printf("Argument = %s in Program_Enter_Code [Heater Control]\n", szParam);
#endif
		if (strlen(szParam) > 0)
			g_dbHtrTempLimit_Offset = atoi(szParam);
		else
			g_dbHtrTempLimit_Offset = 0;
		//-------------------------------------------------------------
	}

} // End of [extern "C"]
//==========================================================================================
