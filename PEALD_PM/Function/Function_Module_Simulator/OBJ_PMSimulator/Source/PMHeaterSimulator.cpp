// PMHeaterSimulator.cpp: implementation of the CPMHeaterSimulator class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

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
#include "Definition.h"
#include "PMHeaterSimulator.h"

// #include "TextLogDll.h"
extern void _LOG(const char *pFormat, ...);

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	HT01_TmpSpAIO,
	HT02_TmpSpAIO,
	HT03_TmpSpAIO,
	HT04_TmpSpAIO,
	HT05_TmpSpAIO,
	HT06_TmpSpAIO,
	HT07_TmpSpAIO,
	HT08_TmpSpAIO,
	HT09_TmpSpAIO,
	HT10_TmpSpAIO,
	HT11_TmpSpAIO,
	HT12_TmpSpAIO,
	HT13_TmpSpAIO,
	HT14_TmpSpAIO,
	HT15_TmpSpAIO,
	HT16_TmpSpAIO,
	HT17_TmpSpAIO,
	HT18_TmpSpAIO,
	HT19_TmpSpAIO,
	HT20_TmpSpAIO,
	HT21_TmpSpAIO,
	HT22_TmpSpAIO,
	HT23_TmpSpAIO,
	HT24_TmpSpAIO,
	HT25_TmpSpAIO,
	HT26_TmpSpAIO,
	HT27_TmpSpAIO,
	HT28_TmpSpAIO,
	HT29_TmpSpAIO,
	HT30_TmpSpAIO,
	HT31_TmpSpAIO,
	HT32_TmpSpAIO,
	
	// Heater Temp Read
	HT01_TmpRdAI,
	HT02_TmpRdAI,
	HT03_TmpRdAI,
	HT04_TmpRdAI,
	HT05_TmpRdAI,
	HT06_TmpRdAI,
	HT07_TmpRdAI,
	HT08_TmpRdAI,
	HT09_TmpRdAI,
	HT10_TmpRdAI,
	HT11_TmpRdAI,
	HT12_TmpRdAI,
	HT13_TmpRdAI,
	HT14_TmpRdAI,
	HT15_TmpRdAI,
	HT16_TmpRdAI,
	HT17_TmpRdAI,
	HT18_TmpRdAI,
	HT19_TmpRdAI,
	HT20_TmpRdAI,
	HT21_TmpRdAI,
	HT22_TmpRdAI,
	HT23_TmpRdAI,
	HT24_TmpRdAI,
	HT25_TmpRdAI,
	HT26_TmpRdAI,
	HT27_TmpRdAI,
	HT28_TmpRdAI,
	HT29_TmpRdAI,
	HT30_TmpRdAI,
	HT31_TmpRdAI,
	HT32_TmpRdAI,
	
	// Heater Temp High Limit
	HT01_TmpHgLmtAIO,
	HT02_TmpHgLmtAIO,
	HT03_TmpHgLmtAIO,
	HT04_TmpHgLmtAIO,
	HT05_TmpHgLmtAIO,
	HT06_TmpHgLmtAIO,
	HT07_TmpHgLmtAIO,
	HT08_TmpHgLmtAIO,
	HT09_TmpHgLmtAIO,
	HT10_TmpHgLmtAIO,
	HT11_TmpHgLmtAIO,
	HT12_TmpHgLmtAIO,
	HT13_TmpHgLmtAIO,
	HT14_TmpHgLmtAIO,
	HT15_TmpHgLmtAIO,
	HT16_TmpHgLmtAIO,
	HT17_TmpHgLmtAIO,
	HT18_TmpHgLmtAIO,
	HT19_TmpHgLmtAIO,
	HT20_TmpHgLmtAIO,
	HT21_TmpHgLmtAIO,
	HT22_TmpHgLmtAIO,
	HT23_TmpHgLmtAIO,
	HT24_TmpHgLmtAIO,
	HT25_TmpHgLmtAIO,
	HT26_TmpHgLmtAIO,
	HT27_TmpHgLmtAIO,
	HT28_TmpHgLmtAIO,
	HT29_TmpHgLmtAIO,
	HT30_TmpHgLmtAIO,
	HT31_TmpHgLmtAIO,
	HT32_TmpHgLmtAIO,

	// Heater Use NotUse Channels
	HT01_UseDM,
	HT02_UseDM,
	HT03_UseDM,
	HT04_UseDM,
	HT05_UseDM,
	HT06_UseDM,
	HT07_UseDM,
	HT08_UseDM,
	HT09_UseDM,
	HT10_UseDM,
	HT11_UseDM,
	HT12_UseDM,
	HT13_UseDM,
	HT14_UseDM,
	HT15_UseDM,
	HT16_UseDM,
	HT17_UseDM,
	HT18_UseDM,
	HT19_UseDM,
	HT20_UseDM,
	HT21_UseDM,
	HT22_UseDM,
	HT23_UseDM,
	HT24_UseDM,
	HT25_UseDM,
	HT26_UseDM,
	HT27_UseDM,
	HT28_UseDM,
	HT29_UseDM,
	HT30_UseDM,
	HT31_UseDM,
	HT32_UseDM,

	// Heater Deviation
	HT01_DeviAIO,
	HT02_DeviAIO,
	HT03_DeviAIO,
	HT04_DeviAIO,
	HT05_DeviAIO,
	HT06_DeviAIO,
	HT07_DeviAIO,
	HT08_DeviAIO,
	HT09_DeviAIO,
	HT10_DeviAIO,
	HT11_DeviAIO,
	HT12_DeviAIO,
	HT13_DeviAIO,
	HT14_DeviAIO,
	HT15_DeviAIO,
	HT16_DeviAIO,
	HT17_DeviAIO,
	HT18_DeviAIO,
	HT19_DeviAIO,
	HT20_DeviAIO,
	HT21_DeviAIO,
	HT22_DeviAIO,
	HT23_DeviAIO,
	HT24_DeviAIO,
	HT25_DeviAIO,
	HT26_DeviAIO,
	HT27_DeviAIO,
	HT28_DeviAIO,
	HT29_DeviAIO,
	HT30_DeviAIO,
	HT31_DeviAIO,
	HT32_DeviAIO,

	//--------------------------------------------------
	// 2006.03.03
	HT01_OnOffDIO,
	HT02_OnOffDIO,
	HT03_OnOffDIO,
	HT04_OnOffDIO,
	HT05_OnOffDIO,
	HT06_OnOffDIO,
	HT07_OnOffDIO,
	HT08_OnOffDIO,
	HT09_OnOffDIO,
	HT10_OnOffDIO,
	HT11_OnOffDIO,
	HT12_OnOffDIO,
	HT13_OnOffDIO,
	HT14_OnOffDIO,
	HT15_OnOffDIO,
	HT16_OnOffDIO,
	HT17_OnOffDIO,
	HT18_OnOffDIO,
	HT19_OnOffDIO,
	HT20_OnOffDIO,
	HT21_OnOffDIO,
	HT22_OnOffDIO,
	HT23_OnOffDIO,
	HT24_OnOffDIO,
	HT25_OnOffDIO,
	HT26_OnOffDIO,
	HT27_OnOffDIO,
	HT28_OnOffDIO,
	HT29_OnOffDIO,
	HT30_OnOffDIO,
	HT31_OnOffDIO,
	HT32_OnOffDIO,

	HT01_CtrlMdDIO,
	HT02_CtrlMdDIO,
	HT03_CtrlMdDIO,
	HT04_CtrlMdDIO,
	HT05_CtrlMdDIO,
	HT06_CtrlMdDIO,
	HT07_CtrlMdDIO,
	HT08_CtrlMdDIO,
	HT09_CtrlMdDIO,
	HT10_CtrlMdDIO,
	HT11_CtrlMdDIO,
	HT12_CtrlMdDIO,
	HT13_CtrlMdDIO,
	HT14_CtrlMdDIO,
	HT15_CtrlMdDIO,
	HT16_CtrlMdDIO,
	HT17_CtrlMdDIO,
	HT18_CtrlMdDIO,
	HT19_CtrlMdDIO,
	HT20_CtrlMdDIO,
	HT21_CtrlMdDIO,
	HT22_CtrlMdDIO,
	HT23_CtrlMdDIO,
	HT24_CtrlMdDIO,
	HT25_CtrlMdDIO,
	HT26_CtrlMdDIO,
	HT27_CtrlMdDIO,
	HT28_CtrlMdDIO,
	HT29_CtrlMdDIO,
	HT30_CtrlMdDIO,
	HT31_CtrlMdDIO,
	HT32_CtrlMdDIO,

	HT01_OutPwAI,
	HT02_OutPwAI,
	HT03_OutPwAI,
	HT04_OutPwAI,
	HT05_OutPwAI,
	HT06_OutPwAI,
	HT07_OutPwAI,
	HT08_OutPwAI,
	HT09_OutPwAI,
	HT10_OutPwAI,
	HT11_OutPwAI,
	HT12_OutPwAI,
	HT13_OutPwAI,
	HT14_OutPwAI,
	HT15_OutPwAI,
	HT16_OutPwAI,
	HT17_OutPwAI,
	HT18_OutPwAI,
	HT19_OutPwAI,
	HT20_OutPwAI,
	HT21_OutPwAI,
	HT22_OutPwAI,
	HT23_OutPwAI,
	HT24_OutPwAI,
	HT25_OutPwAI,
	HT26_OutPwAI,
	HT27_OutPwAI,
	HT28_OutPwAI,
	HT29_OutPwAI,
	HT30_OutPwAI,
	HT31_OutPwAI,
	HT32_OutPwAI,

	//--------------------------------------------------
	// 2006.03.03
	HT01_AlrmStsDI,
	HT02_AlrmStsDI,
	HT03_AlrmStsDI,
	HT04_AlrmStsDI,
	HT05_AlrmStsDI,
	HT06_AlrmStsDI,
	HT07_AlrmStsDI,
	HT08_AlrmStsDI,
	HT09_AlrmStsDI,
	HT10_AlrmStsDI,
	HT11_AlrmStsDI,
	HT12_AlrmStsDI,
	HT13_AlrmStsDI,
	HT14_AlrmStsDI,
	HT15_AlrmStsDI,
	HT16_AlrmStsDI,
	HT17_AlrmStsDI,
	HT18_AlrmStsDI,
	HT19_AlrmStsDI,
	HT20_AlrmStsDI,
	HT21_AlrmStsDI,
	HT22_AlrmStsDI,
	HT23_AlrmStsDI,
	HT24_AlrmStsDI,
	HT25_AlrmStsDI,
	HT26_AlrmStsDI,
	HT27_AlrmStsDI,
	HT28_AlrmStsDI,
	HT29_AlrmStsDI,
	HT30_AlrmStsDI,
	HT31_AlrmStsDI,
	HT32_AlrmStsDI,

} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{

	{ "HT01_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT02_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT03_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT04_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT05_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT06_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT07_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT08_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT09_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT10_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT11_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT12_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT13_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT14_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT15_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT16_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT17_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT18_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT19_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT20_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT21_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT22_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT23_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT24_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT25_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT26_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT27_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT28_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT29_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT30_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT31_TmpSpAIO",	_K_A_IO,	0	},
	{ "HT32_TmpSpAIO",	_K_A_IO,	0	},

	// Heater Temp Read
	{ "HT01_TmpRdAI",	_K_A_IO,	0	},
	{ "HT02_TmpRdAI",	_K_A_IO,	0	},
	{ "HT03_TmpRdAI",	_K_A_IO,	0	},
	{ "HT04_TmpRdAI",	_K_A_IO,	0	},
	{ "HT05_TmpRdAI",	_K_A_IO,	0	},
	{ "HT06_TmpRdAI",	_K_A_IO,	0	},
	{ "HT07_TmpRdAI",	_K_A_IO,	0	},
	{ "HT08_TmpRdAI",	_K_A_IO,	0	},
	{ "HT09_TmpRdAI",	_K_A_IO,	0	},
	{ "HT10_TmpRdAI",	_K_A_IO,	0	},
	{ "HT11_TmpRdAI",	_K_A_IO,	0	},
	{ "HT12_TmpRdAI",	_K_A_IO,	0	},
	{ "HT13_TmpRdAI",	_K_A_IO,	0	},
	{ "HT14_TmpRdAI",	_K_A_IO,	0	},
	{ "HT15_TmpRdAI",	_K_A_IO,	0	},
	{ "HT16_TmpRdAI",	_K_A_IO,	0	},
	{ "HT17_TmpRdAI",	_K_A_IO,	0	},
	{ "HT18_TmpRdAI",	_K_A_IO,	0	},
	{ "HT19_TmpRdAI",	_K_A_IO,	0	},
	{ "HT20_TmpRdAI",	_K_A_IO,	0	},
	{ "HT21_TmpRdAI",	_K_A_IO,	0	},
	{ "HT22_TmpRdAI",	_K_A_IO,	0	},
	{ "HT23_TmpRdAI",	_K_A_IO,	0	},
	{ "HT24_TmpRdAI",	_K_A_IO,	0	},
	{ "HT25_TmpRdAI",	_K_A_IO,	0	},
	{ "HT26_TmpRdAI",	_K_A_IO,	0	},
	{ "HT27_TmpRdAI",	_K_A_IO,	0	},
	{ "HT28_TmpRdAI",	_K_A_IO,	0	},
	{ "HT29_TmpRdAI",	_K_A_IO,	0	},
	{ "HT30_TmpRdAI",	_K_A_IO,	0	},
	{ "HT31_TmpRdAI",	_K_A_IO,	0	},
	{ "HT32_TmpRdAI",	_K_A_IO,	0	},

	// Heater Temp High Limit
	{ "HT01_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT02_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT03_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT04_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT05_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT06_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT07_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT08_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT09_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT10_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT11_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT12_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT13_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT14_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT15_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT16_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT17_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT18_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT19_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT20_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT21_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT22_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT23_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT24_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT25_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT26_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT27_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT28_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT29_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT30_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT31_TmpHgLmtAIO",	_K_A_IO,	0	},
	{ "HT32_TmpHgLmtAIO",	_K_A_IO,	0	},

	// Heater Use Not Use Enum
	{ "HT01_UseDM",	_K_D_IO,	0	},
	{ "HT02_UseDM",	_K_D_IO,	0	},
	{ "HT03_UseDM",	_K_D_IO,	0	},
	{ "HT04_UseDM",	_K_D_IO,	0	},
	{ "HT05_UseDM",	_K_D_IO,	0	},
	{ "HT06_UseDM",	_K_D_IO,	0	},
	{ "HT07_UseDM",	_K_D_IO,	0	},
	{ "HT08_UseDM",	_K_D_IO,	0	},
	{ "HT09_UseDM",	_K_D_IO,	0	},
	{ "HT10_UseDM",	_K_D_IO,	0	},
	{ "HT11_UseDM",	_K_D_IO,	0	},
	{ "HT12_UseDM",	_K_D_IO,	0	},
	{ "HT13_UseDM",	_K_D_IO,	0	},
	{ "HT14_UseDM",	_K_D_IO,	0	},
	{ "HT15_UseDM",	_K_D_IO,	0	},
	{ "HT16_UseDM",	_K_D_IO,	0	},
	{ "HT17_UseDM",	_K_D_IO,	0	},
	{ "HT18_UseDM",	_K_D_IO,	0	},
	{ "HT19_UseDM",	_K_D_IO,	0	},
	{ "HT20_UseDM",	_K_D_IO,	0	},
	{ "HT21_UseDM",	_K_D_IO,	0	},
	{ "HT22_UseDM",	_K_D_IO,	0	},
	{ "HT23_UseDM",	_K_D_IO,	0	},
	{ "HT24_UseDM",	_K_D_IO,	0	},
	{ "HT25_UseDM",	_K_D_IO,	0	},
	{ "HT26_UseDM",	_K_D_IO,	0	},
	{ "HT27_UseDM",	_K_D_IO,	0	},
	{ "HT28_UseDM",	_K_D_IO,	0	},
	{ "HT29_UseDM",	_K_D_IO,	0	},
	{ "HT30_UseDM",	_K_D_IO,	0	},
	{ "HT31_UseDM",	_K_D_IO,	0	},
	{ "HT32_UseDM",	_K_D_IO,	0	},

	// Heater Deviation
	{ "HT01_DeviAIO",	_K_A_IO,	0	},
	{ "HT02_DeviAIO",	_K_A_IO,	0	},
	{ "HT03_DeviAIO",	_K_A_IO,	0	},
	{ "HT04_DeviAIO",	_K_A_IO,	0	},
	{ "HT05_DeviAIO",	_K_A_IO,	0	},
	{ "HT06_DeviAIO",	_K_A_IO,	0	},
	{ "HT07_DeviAIO",	_K_A_IO,	0	},
	{ "HT08_DeviAIO",	_K_A_IO,	0	},
	{ "HT09_DeviAIO",	_K_A_IO,	0	},
	{ "HT10_DeviAIO",	_K_A_IO,	0	},
	{ "HT11_DeviAIO",	_K_A_IO,	0	},
	{ "HT12_DeviAIO",	_K_A_IO,	0	},
	{ "HT13_DeviAIO",	_K_A_IO,	0	},
	{ "HT14_DeviAIO",	_K_A_IO,	0	},
	{ "HT15_DeviAIO",	_K_A_IO,	0	},
	{ "HT16_DeviAIO",	_K_A_IO,	0	},
	{ "HT17_DeviAIO",	_K_A_IO,	0	},
	{ "HT18_DeviAIO",	_K_A_IO,	0	},
	{ "HT19_DeviAIO",	_K_A_IO,	0	},
	{ "HT20_DeviAIO",	_K_A_IO,	0	},
	{ "HT21_DeviAIO",	_K_A_IO,	0	},
	{ "HT22_DeviAIO",	_K_A_IO,	0	},
	{ "HT23_DeviAIO",	_K_A_IO,	0	},
	{ "HT24_DeviAIO",	_K_A_IO,	0	},
	{ "HT25_DeviAIO",	_K_A_IO,	0	},
	{ "HT26_DeviAIO",	_K_A_IO,	0	},
	{ "HT27_DeviAIO",	_K_A_IO,	0	},
	{ "HT28_DeviAIO",	_K_A_IO,	0	},
	{ "HT29_DeviAIO",	_K_A_IO,	0	},
	{ "HT30_DeviAIO",	_K_A_IO,	0	},
	{ "HT31_DeviAIO",	_K_A_IO,	0	},
	{ "HT32_DeviAIO",	_K_A_IO,	0	},

	//--------------------------------------------------
	// 2006.03.03
	{ "HT01_OnOffDIO",	_K_D_IO,	0	},
	{ "HT02_OnOffDIO",	_K_D_IO,	0	},
	{ "HT03_OnOffDIO",	_K_D_IO,	0	},
	{ "HT04_OnOffDIO",	_K_D_IO,	0	},
	{ "HT05_OnOffDIO",	_K_D_IO,	0	},
	{ "HT06_OnOffDIO",	_K_D_IO,	0	},
	{ "HT07_OnOffDIO",	_K_D_IO,	0	},
	{ "HT08_OnOffDIO",	_K_D_IO,	0	},
	{ "HT09_OnOffDIO",	_K_D_IO,	0	},
	{ "HT10_OnOffDIO",	_K_D_IO,	0	},
	{ "HT11_OnOffDIO",	_K_D_IO,	0	},
	{ "HT12_OnOffDIO",	_K_D_IO,	0	},
	{ "HT13_OnOffDIO",	_K_D_IO,	0	},
	{ "HT14_OnOffDIO",	_K_D_IO,	0	},
	{ "HT15_OnOffDIO",	_K_D_IO,	0	},
	{ "HT16_OnOffDIO",	_K_D_IO,	0	},
	{ "HT17_OnOffDIO",	_K_D_IO,	0	},
	{ "HT18_OnOffDIO",	_K_D_IO,	0	},
	{ "HT19_OnOffDIO",	_K_D_IO,	0	},
	{ "HT20_OnOffDIO",	_K_D_IO,	0	},
	{ "HT21_OnOffDIO",	_K_D_IO,	0	},
	{ "HT22_OnOffDIO",	_K_D_IO,	0	},
	{ "HT23_OnOffDIO",	_K_D_IO,	0	},
	{ "HT24_OnOffDIO",	_K_D_IO,	0	},
	{ "HT25_OnOffDIO",	_K_D_IO,	0	},
	{ "HT26_OnOffDIO",	_K_D_IO,	0	},
	{ "HT27_OnOffDIO",	_K_D_IO,	0	},
	{ "HT28_OnOffDIO",	_K_D_IO,	0	},
	{ "HT29_OnOffDIO",	_K_D_IO,	0	},
	{ "HT30_OnOffDIO",	_K_D_IO,	0	},
	{ "HT31_OnOffDIO",	_K_D_IO,	0	},
	{ "HT32_OnOffDIO",	_K_D_IO,	0	},

	{ "HT01_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT02_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT03_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT04_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT05_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT06_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT07_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT08_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT09_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT10_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT11_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT12_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT13_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT14_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT15_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT16_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT17_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT18_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT19_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT20_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT21_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT22_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT23_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT24_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT25_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT26_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT27_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT28_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT29_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT30_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT31_CtrlMdDIO",	_K_D_IO,	0	},
	{ "HT32_CtrlMdDIO",	_K_D_IO,	0	},

	{ "HT01_OutPwAI",	_K_A_IO,	0	},
	{ "HT02_OutPwAI",	_K_A_IO,	0	},
	{ "HT03_OutPwAI",	_K_A_IO,	0	},
	{ "HT04_OutPwAI",	_K_A_IO,	0	},
	{ "HT05_OutPwAI",	_K_A_IO,	0	},
	{ "HT06_OutPwAI",	_K_A_IO,	0	},
	{ "HT07_OutPwAI",	_K_A_IO,	0	},
	{ "HT08_OutPwAI",	_K_A_IO,	0	},
	{ "HT09_OutPwAI",	_K_A_IO,	0	},
	{ "HT10_OutPwAI",	_K_A_IO,	0	},
	{ "HT11_OutPwAI",	_K_A_IO,	0	},
	{ "HT12_OutPwAI",	_K_A_IO,	0	},
	{ "HT13_OutPwAI",	_K_A_IO,	0	},
	{ "HT14_OutPwAI",	_K_A_IO,	0	},
	{ "HT15_OutPwAI",	_K_A_IO,	0	},
	{ "HT16_OutPwAI",	_K_A_IO,	0	},
	{ "HT17_OutPwAI",	_K_A_IO,	0	},
	{ "HT18_OutPwAI",	_K_A_IO,	0	},
	{ "HT19_OutPwAI",	_K_A_IO,	0	},
	{ "HT20_OutPwAI",	_K_A_IO,	0	},
	{ "HT21_OutPwAI",	_K_A_IO,	0	},
	{ "HT22_OutPwAI",	_K_A_IO,	0	},
	{ "HT23_OutPwAI",	_K_A_IO,	0	},
	{ "HT24_OutPwAI",	_K_A_IO,	0	},
	{ "HT25_OutPwAI",	_K_A_IO,	0	},
	{ "HT26_OutPwAI",	_K_A_IO,	0	},
	{ "HT27_OutPwAI",	_K_A_IO,	0	},
	{ "HT28_OutPwAI",	_K_A_IO,	0	},
	{ "HT29_OutPwAI",	_K_A_IO,	0	},
	{ "HT30_OutPwAI",	_K_A_IO,	0	},
	{ "HT31_OutPwAI",	_K_A_IO,	0	},
	{ "HT32_OutPwAI",	_K_A_IO,	0	},

	//--------------------------------------------------
	// 2006.03.14
	{ "HT01_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT02_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT03_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT04_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT05_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT06_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT07_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT08_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT09_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT10_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT11_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT12_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT13_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT14_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT15_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT16_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT17_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT18_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT19_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT20_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT21_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT22_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT23_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT24_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT25_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT26_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT27_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT28_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT29_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT30_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT31_AlrmStsDI",	_K_D_IO,	0	},
	{ "HT32_AlrmStsDI",	_K_D_IO,	0	},

	""
};
//--------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
CPMHeaterSimulator::CPMHeaterSimulator()
{
	m_bHtr_SimEnable = true;
}

CPMHeaterSimulator::~CPMHeaterSimulator()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

BOOL CPMHeaterSimulator::Initialize()
{
	InitIOList(g_pIOList);
	CreateThread();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//...
void CPMHeaterSimulator::DoMonitor()
{
	char pszLocFtnName[] = "CPMHeaterSimulator";
	char pszFtnName[256];
	BOOL bRet = FALSE;
	int nIOStatus = 0;


	int		pnUsedHeaterHandle[_MAX_LINE_HEATER];
	int		nUsedHeaterNo	= 0;
	int		nRetryCount		= 5;
	int		i				= 0;
	int		nHtrOnOff		= 0;
	int		nCtrlMode		= 1;

	double	fTempSetPoint	= 0;
	double	fTempRead		= 0;

	//.....
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s\n", pszFtnName);

	//.....
	do
	{
		// Get All Used Heater Handles
		GetAllUsedHeaterHandle( pnUsedHeaterHandle, &nUsedHeaterNo);

		//-----------------------------------------------------------------------
		// 2006.03.03
		if(m_bHtr_SimEnable)
		{
			WaitSec(1);

			for(i = 0; i < nUsedHeaterNo; i++)
			{
				dWRITE_DIGITAL(HT01_OnOffDIO	+ pnUsedHeaterHandle[i], HTR_ON,	&nIOStatus);
				if(pnUsedHeaterHandle[i] == 0 || pnUsedHeaterHandle[i] == 16)
				{
					dWRITE_DIGITAL(HT01_CtrlMdDIO	+ pnUsedHeaterHandle[i], 7,			&nIOStatus);
				}
				else
				{
					dWRITE_DIGITAL(HT01_CtrlMdDIO	+ pnUsedHeaterHandle[i], 4,			&nIOStatus);
				}
				dWRITE_ANALOG(HT01_TmpHgLmtAIO	+ pnUsedHeaterHandle[i], 800,		&nIOStatus);
				dWRITE_ANALOG(HT01_DeviAIO		+ pnUsedHeaterHandle[i], 5,			&nIOStatus);
				dWRITE_ANALOG(HT01_TmpSpAIO		+ pnUsedHeaterHandle[i], 50,		&nIOStatus);
			}
		}
		//-----------------------------------------------------------------------

		//	LOG(pszFtnName,_ELT_Console, "Total Used Heater No. = [%d]", nUsedHeaterNo);
		// Simulation Loop		
		while(WaitSec(1))
		{
			if(! m_bHtr_SimEnable)		continue;

			//.....
			for( i=0; i < nUsedHeaterNo; i++)
			{
				fTempSetPoint	= dREAD_ANALOG(HT01_TmpSpAIO	+ pnUsedHeaterHandle[i], &nIOStatus);
				fTempRead		= dREAD_ANALOG(HT01_TmpRdAI		+ pnUsedHeaterHandle[i], &nIOStatus);
				nHtrOnOff		= dREAD_DIGITAL(HT01_OnOffDIO	+ pnUsedHeaterHandle[i], &nIOStatus);
				nCtrlMode		= dREAD_DIGITAL(HT01_CtrlMdDIO	+ pnUsedHeaterHandle[i], &nIOStatus);
				
				if(nHtrOnOff == HTR_ON)
				{
					if( (fTempSetPoint - fTempRead) != 0 && nCtrlMode == 1)
					{
						if(fTempRead > fTempSetPoint)
						{
							fTempRead -= (1.3+(i%4));
							dWRITE_ANALOG(HT01_OutPwAI + pnUsedHeaterHandle[i], 10-(1.3+(i%4)), &nIOStatus);
						}
						else
						{
							fTempRead += (1.3+(i%2));
							dWRITE_ANALOG(HT01_OutPwAI + pnUsedHeaterHandle[i], 30+(1.3+(i%4)), &nIOStatus);
						}

						if(fTempRead < 23) fTempRead = 23;
						dWRITE_ANALOG(HT01_TmpRdAI + pnUsedHeaterHandle[i], fTempRead, &nIOStatus);
					}
				}
				else
				{
					fTempRead -= 1.3;
					if(fTempRead < 23) fTempRead = 23;
					dWRITE_ANALOG(HT01_TmpRdAI + pnUsedHeaterHandle[i], fTempRead, &nIOStatus);
					dWRITE_ANALOG(HT01_OutPwAI + pnUsedHeaterHandle[i], 0, &nIOStatus);
				}

				WaitSec(0.1);
				//-----------------------------------------------------------------------
			}
		}
		bRet = TRUE;
	}while(0);
	printf("Leaved %s\n", pszFtnName);	
}

//////////////////////////////////////////////////////////////////////////
BOOL CPMHeaterSimulator::GetAllUsedHeaterHandle(int * pnUsedHeaterHandle, int * pnSize)
{
	char pszLocFtnName[] = "GetAllUsedHeaterHandle";
	char pszFtnName[256];
	BOOL bRet = FALSE;	
	int nIOStatus = 0;

	int i = 0;
	*pnSize = 0;
	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	
	do
	{			
		for(i = 0; i < _MAX_LINE_HEATER; i++)
		{
			if( Use == dREAD_DIGITAL(HT01_UseDM+i, &nIOStatus))
			{
				pnUsedHeaterHandle[*pnSize] = i;
				*pnSize = *pnSize + 1;
			}
		}

		if( 0 == *pnSize )	bRet = FALSE;
		else				bRet = TRUE;
	}while(0);

	return	bRet;
}

