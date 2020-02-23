/////////////////////////////////////////////////////////////////////////////////////////////
// dyn_iodefine.h (Dynamic I/O Define and Function)
// Create : 2005.04.12
// Last Update : 2005.06.01

//------------------------------------------------------------------------------------------
// IO Enum List Definition

enum {
	MFM01FlwAI,
	PRMA_BsArHgLmt,
	PRMA_BsArLoLmt,
	PRMA_EC_BsAr,
} IO_INDEX_ENUM;

//------------------------------------------------------------------------------------------
// Function Definition

void dyn_InitIOList();
BOOL dyn_IOValid(int nIoIdxEnum);
int dyn_FindIO(char *szIOName);

void dyn_WRITE_DIGITAL(int nIoIdxEnum, int nValue, BOOL *pResult);
int dyn_READ_DIGITAL(int nIoIdxEnum, BOOL *pResult);

void dyn_WRITE_ANALOG(int nIoIdxEnum, double dbValue, BOOL *pResult);
double dyn_READ_ANALOG(int nIoIdxEnum, BOOL *pResult);

void dyn_WRITE_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult);
char *dyn_READ_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult);

Module_Status dyn_RUN_FUNCTION(int nIoIdxEnum, char *szParam);
