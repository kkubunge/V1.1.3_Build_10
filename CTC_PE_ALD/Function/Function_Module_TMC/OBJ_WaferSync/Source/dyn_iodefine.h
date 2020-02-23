/////////////////////////////////////////////////////////////////////////////////////////////
// dyn_iodefine.h (Dynamic I/O Define and Function)
// Create : 2005.04.12

//------------------------------------------------------------------------------------------
// IO Enum List Definition

enum {
	ATM_RB_A_WAFER,
	ATM_RB_B_WAFER,
	VAC_RB_A_WAFER,
	VAC_RB_B_WAFER,
	AL_WAFER,

	CM1_01_WAFER,
	CM1_02_WAFER,
	CM1_03_WAFER,
	CM1_04_WAFER,
	CM1_05_WAFER,
	CM1_06_WAFER,
	CM1_07_WAFER,
	CM1_08_WAFER,
	CM1_09_WAFER,
	CM1_10_WAFER,
	CM1_11_WAFER,
	CM1_12_WAFER,
	CM1_13_WAFER,
	CM1_14_WAFER,
	CM1_15_WAFER,
	CM1_16_WAFER,
	CM1_17_WAFER,
	CM1_18_WAFER,
	CM1_19_WAFER,
	CM1_20_WAFER,
	CM1_21_WAFER,
	CM1_22_WAFER,
	CM1_23_WAFER,
	CM1_24_WAFER,
	CM1_25_WAFER,
	CM1_26_WAFER,

	CM2_01_WAFER,
	CM2_02_WAFER,
	CM2_03_WAFER,
	CM2_04_WAFER,
	CM2_05_WAFER,
	CM2_06_WAFER,
	CM2_07_WAFER,
	CM2_08_WAFER,
	CM2_09_WAFER,
	CM2_10_WAFER,
	CM2_11_WAFER,
	CM2_12_WAFER,
	CM2_13_WAFER,
	CM2_14_WAFER,
	CM2_15_WAFER,
	CM2_16_WAFER,
	CM2_17_WAFER,
	CM2_18_WAFER,
	CM2_19_WAFER,
	CM2_20_WAFER,
	CM2_21_WAFER,
	CM2_22_WAFER,
	CM2_23_WAFER,
	CM2_24_WAFER,
	CM2_25_WAFER,
	CM2_26_WAFER,

	CM3_01_WAFER,
	CM3_02_WAFER,
	CM3_03_WAFER,
	CM3_04_WAFER,
	CM3_05_WAFER,
	CM3_06_WAFER,
	CM3_07_WAFER,
	CM3_08_WAFER,
	CM3_09_WAFER,
	CM3_10_WAFER,
	CM3_11_WAFER,
	CM3_12_WAFER,
	CM3_13_WAFER,
	CM3_14_WAFER,
	CM3_15_WAFER,
	CM3_16_WAFER,
	CM3_17_WAFER,
	CM3_18_WAFER,
	CM3_19_WAFER,
	CM3_20_WAFER,
	CM3_21_WAFER,
	CM3_22_WAFER,
	CM3_23_WAFER,
	CM3_24_WAFER,
	CM3_25_WAFER,
	CM3_26_WAFER,

	BM1_01_WAFER,
	BM1_02_WAFER,
	BM2_01_WAFER,
	BM2_02_WAFER,

	PM1_WAFER,
	PM2_WAFER,
	PM3_WAFER,
	PM4_WAFER,
	PM5_WAFER,
	PM6_WAFER,
	PM7_WAFER,
	PM8_WAFER,
	PM9_WAFER,

	ATM_RB_A_WF_SRC,
	ATM_RB_B_WF_SRC,
	VAC_RB_A_WF_SRC,
	VAC_RB_B_WF_SRC,
	AL_WF_SRC,

	BM1_01_WF_SRC,
	BM1_02_WF_SRC,
	BM2_01_WF_SRC,
	BM2_02_WF_SRC,

	PM1_WF_SRC,
	PM2_WF_SRC,
	PM3_WF_SRC,
	PM4_WF_SRC,
	PM5_WF_SRC,
	PM6_WF_SRC,
	PM7_WF_SRC,
	PM8_WF_SRC,
	PM9_WF_SRC,
} IO_INDEX_ENUM;

//------------------------------------------------------------------------------------------
// Function Definition

void dyn_InitIOList();

BOOL dyn_IOValid(int nIoIdxEnum);

void dyn_WRITE_DIGITAL(int nIoIdxEnum, int nValue, BOOL *pResult);
int dyn_READ_DIGITAL(int nIoIdxEnum, BOOL *pResult);

void dyn_WRITE_ANALOG(int nIoIdxEnum, double dbValue, BOOL *pResult);
double dyn_READ_ANALOG(int nIoIdxEnum, BOOL *pResult);

void dyn_WRITE_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult);
char *dyn_READ_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult);

Module_Status dyn_RUN_FUNCTION(int nIoIdxEnum, char *szParam);
