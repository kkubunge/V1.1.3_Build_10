/////////////////////////////////////////////////////////////////////////////////////////////
// dyn_iodefine.h (Dynamic I/O Define and Function)
// Create : 2005.04.12

//------------------------------------------------------------------------------------------
// IO Enum List Definition

enum {
	PM1_RunStsDM,
	PM2_RunStsDM,
	PM3_RunStsDM,
	PM4_RunStsDM,
	PM5_RunStsDM,
} IO_INDEX_ENUM;

//------------------------------------------------------------------------------------------
// Function Definition

void dyn_InitIOList();

void dyn_WRITE_DIGITAL(int nIoIdxEnum, int nValue, BOOL *pResult);
int dyn_READ_DIGITAL(int nIoIdxEnum, BOOL *pResult);

void dyn_WRITE_ANALOG(int nIoIdxEnum, double dbValue, BOOL *pResult);
double dyn_READ_ANALOG(int nIoIdxEnum, BOOL *pResult);

void dyn_WRITE_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult);
char *dyn_READ_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult);
