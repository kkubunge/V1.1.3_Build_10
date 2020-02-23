/////////////////////////////////////////////////////////////////////////////////////////////
// dyn_iodefine.h (Dynamic I/O Define and Function)
// Create : 2005.04.12

//------------------------------------------------------------------------------------------
// IO Enum List Definition

enum {
	PM1_PROCESS_RUN_STATUS	,
	PM2_PROCESS_RUN_STATUS	,
	PM3_PROCESS_RUN_STATUS	,
	PM4_PROCESS_RUN_STATUS	,
	PM5_PROCESS_RUN_STATUS	,

	PM1_PRO_IdleRcpStsDM	,
	PM2_PRO_IdleRcpStsDM	,
	PM3_PRO_IdleRcpStsDM	,
	PM4_PRO_IdleRcpStsDM	,
	PM5_PRO_IdleRcpStsDM	,

	CTC_PM1_CommStatus,
	CTC_PM2_CommStatus,
	CTC_PM3_CommStatus,
	CTC_PM4_CommStatus,
	CTC_PM5_CommStatus,
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
