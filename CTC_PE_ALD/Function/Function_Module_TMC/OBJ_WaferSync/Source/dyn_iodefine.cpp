/////////////////////////////////////////////////////////////////////////////////////////////
// dyn_iodefine.cpp

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

typedef struct _IO_LIST
{
	char szIOName[40];
	int nIOType;
	int nIOPoint;
} IO_LIST;

IO_LIST	g_IOList[] =
{
	{ "CTC.FA_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.FB_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.TA_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.TB_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.FM_AL_Wafer_Status",		_K_D_IO, 0},

	{ "CM1.C01_Wafer",				_K_D_IO, 0},
	{ "CM1.C02_Wafer",				_K_D_IO, 0},
	{ "CM1.C03_Wafer",				_K_D_IO, 0},
	{ "CM1.C04_Wafer",				_K_D_IO, 0},
	{ "CM1.C05_Wafer",				_K_D_IO, 0},
	{ "CM1.C06_Wafer",				_K_D_IO, 0},
	{ "CM1.C07_Wafer",				_K_D_IO, 0},
	{ "CM1.C08_Wafer",				_K_D_IO, 0},
	{ "CM1.C09_Wafer",				_K_D_IO, 0},
	{ "CM1.C10_Wafer",				_K_D_IO, 0},
	{ "CM1.C11_Wafer",				_K_D_IO, 0},
	{ "CM1.C12_Wafer",				_K_D_IO, 0},
	{ "CM1.C13_Wafer",				_K_D_IO, 0},
	{ "CM1.C14_Wafer",				_K_D_IO, 0},
	{ "CM1.C15_Wafer",				_K_D_IO, 0},
	{ "CM1.C16_Wafer",				_K_D_IO, 0},
	{ "CM1.C17_Wafer",				_K_D_IO, 0},
	{ "CM1.C18_Wafer",				_K_D_IO, 0},
	{ "CM1.C19_Wafer",				_K_D_IO, 0},
	{ "CM1.C20_Wafer",				_K_D_IO, 0},
	{ "CM1.C21_Wafer",				_K_D_IO, 0},
	{ "CM1.C22_Wafer",				_K_D_IO, 0},
	{ "CM1.C23_Wafer",				_K_D_IO, 0},
	{ "CM1.C24_Wafer",				_K_D_IO, 0},
	{ "CM1.C25_Wafer",				_K_D_IO, 0},
	{ "CM1.C26_Wafer",				_K_D_IO, 0},

	{ "CM2.C01_Wafer",				_K_D_IO, 0},
	{ "CM2.C02_Wafer",				_K_D_IO, 0},
	{ "CM2.C03_Wafer",				_K_D_IO, 0},
	{ "CM2.C04_Wafer",				_K_D_IO, 0},
	{ "CM2.C05_Wafer",				_K_D_IO, 0},
	{ "CM2.C06_Wafer",				_K_D_IO, 0},
	{ "CM2.C07_Wafer",				_K_D_IO, 0},
	{ "CM2.C08_Wafer",				_K_D_IO, 0},
	{ "CM2.C09_Wafer",				_K_D_IO, 0},
	{ "CM2.C10_Wafer",				_K_D_IO, 0},
	{ "CM2.C11_Wafer",				_K_D_IO, 0},
	{ "CM2.C12_Wafer",				_K_D_IO, 0},
	{ "CM2.C13_Wafer",				_K_D_IO, 0},
	{ "CM2.C14_Wafer",				_K_D_IO, 0},
	{ "CM2.C15_Wafer",				_K_D_IO, 0},
	{ "CM2.C16_Wafer",				_K_D_IO, 0},
	{ "CM2.C17_Wafer",				_K_D_IO, 0},
	{ "CM2.C18_Wafer",				_K_D_IO, 0},
	{ "CM2.C19_Wafer",				_K_D_IO, 0},
	{ "CM2.C20_Wafer",				_K_D_IO, 0},
	{ "CM2.C21_Wafer",				_K_D_IO, 0},
	{ "CM2.C22_Wafer",				_K_D_IO, 0},
	{ "CM2.C23_Wafer",				_K_D_IO, 0},
	{ "CM2.C24_Wafer",				_K_D_IO, 0},
	{ "CM2.C25_Wafer",				_K_D_IO, 0},
	{ "CM2.C26_Wafer",				_K_D_IO, 0},

	{ "CM3.C01_Wafer",				_K_D_IO, 0},
	{ "CM3.C02_Wafer",				_K_D_IO, 0},
	{ "CM3.C03_Wafer",				_K_D_IO, 0},
	{ "CM3.C04_Wafer",				_K_D_IO, 0},
	{ "CM3.C05_Wafer",				_K_D_IO, 0},
	{ "CM3.C06_Wafer",				_K_D_IO, 0},
	{ "CM3.C07_Wafer",				_K_D_IO, 0},
	{ "CM3.C08_Wafer",				_K_D_IO, 0},
	{ "CM3.C09_Wafer",				_K_D_IO, 0},
	{ "CM3.C10_Wafer",				_K_D_IO, 0},
	{ "CM3.C11_Wafer",				_K_D_IO, 0},
	{ "CM3.C12_Wafer",				_K_D_IO, 0},
	{ "CM3.C13_Wafer",				_K_D_IO, 0},
	{ "CM3.C14_Wafer",				_K_D_IO, 0},
	{ "CM3.C15_Wafer",				_K_D_IO, 0},
	{ "CM3.C16_Wafer",				_K_D_IO, 0},
	{ "CM3.C17_Wafer",				_K_D_IO, 0},
	{ "CM3.C18_Wafer",				_K_D_IO, 0},
	{ "CM3.C19_Wafer",				_K_D_IO, 0},
	{ "CM3.C20_Wafer",				_K_D_IO, 0},
	{ "CM3.C21_Wafer",				_K_D_IO, 0},
	{ "CM3.C22_Wafer",				_K_D_IO, 0},
	{ "CM3.C23_Wafer",				_K_D_IO, 0},
	{ "CM3.C24_Wafer",				_K_D_IO, 0},
	{ "CM3.C25_Wafer",				_K_D_IO, 0},
	{ "CM3.C26_Wafer",				_K_D_IO, 0},

	{ "CTC.BM1_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.BM1_Wafer_Status2",		_K_D_IO, 0},
	{ "CTC.BM2_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.BM2_Wafer_Status2",		_K_D_IO, 0},

	{ "CTC.PM1_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.PM2_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.PM3_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.PM4_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.PM5_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.PM6_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.PM7_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.PM8_Wafer_Status",		_K_D_IO, 0},
	{ "CTC.PM9_Wafer_Status",		_K_D_IO, 0},

	{ "CTC.FA_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.FB_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.TA_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.TB_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.FM_AL_Wafer_Source",		_K_D_IO, 0},

	{ "CTC.BM1_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.BM1_Wafer_Source2",		_K_D_IO, 0},
	{ "CTC.BM2_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.BM2_Wafer_Source2",		_K_D_IO, 0},

	{ "CTC.PM1_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.PM2_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.PM3_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.PM4_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.PM5_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.PM6_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.PM7_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.PM8_Wafer_Source",		_K_D_IO, 0},
	{ "CTC.PM9_Wafer_Source",		_K_D_IO, 0},
	""
};

int g_nIOCount = 0;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void dyn_InitIOList()
{
	int i;
	for(i=0; g_IOList[i].szIOName[0] != 0; i++)
	{
		g_IOList[i].nIOPoint = _FIND_FROM_STRING(g_IOList[i].nIOType, g_IOList[i].szIOName);
		if(g_IOList[i].nIOPoint == -1)
		{
//			printf("-----> IO Init Failed(Name:%s, Type:%d)\n",
//						g_IOList[i].szIOName, g_IOList[i].nIOType);
		}
	}
	g_nIOCount = i;
}

BOOL dyn_IOValid(int nIoIdxEnum)
{
	if(nIoIdxEnum >= g_nIOCount) return FALSE;
	if(g_IOList[nIoIdxEnum].nIOPoint == -1) return FALSE;
	else return TRUE;
}

void dyn_WRITE_DIGITAL(int nIoIdxEnum, int nValue, BOOL *pResult)
{
	if(g_IOList[nIoIdxEnum].nIOPoint == -1 || g_IOList[nIoIdxEnum].nIOType != _K_D_IO)
	{
//		printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_WRITE_DIGITAL\n",
//			g_IOList[nIoIdxEnum].nIOPoint, g_IOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return;
	}
	_dWRITE_DIGITAL(g_IOList[nIoIdxEnum].nIOPoint, nValue, pResult);
}

int dyn_READ_DIGITAL(int nIoIdxEnum, BOOL *pResult)
{
	int nValue = 0;
	if(g_IOList[nIoIdxEnum].nIOPoint == -1 || g_IOList[nIoIdxEnum].nIOType != _K_D_IO)
	{
//		printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_READ_DIGITAL\n",
//			g_IOList[nIoIdxEnum].nIOPoint, g_IOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return 0;
	}
	nValue = _dREAD_DIGITAL(g_IOList[nIoIdxEnum].nIOPoint, pResult);
	return nValue;
}

void dyn_WRITE_ANALOG(int nIoIdxEnum, double dbValue, BOOL *pResult)
{
	if(g_IOList[nIoIdxEnum].nIOPoint == -1 || g_IOList[nIoIdxEnum].nIOType != _K_A_IO)
	{
//		printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_WRITE_ANALOG\n",
//			g_IOList[nIoIdxEnum].nIOPoint, g_IOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return;
	}
	_dWRITE_ANALOG(g_IOList[nIoIdxEnum].nIOPoint, dbValue, pResult);
}

double dyn_READ_ANALOG(int nIoIdxEnum, BOOL *pResult)
{
	double dbValue = 0;
	if(g_IOList[nIoIdxEnum].nIOPoint == -1 || g_IOList[nIoIdxEnum].nIOType != _K_A_IO)
	{
//		printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_READ_ANALOG\n",
//			g_IOList[nIoIdxEnum].nIOPoint, g_IOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return 0;
	}
	dbValue = _dREAD_ANALOG(g_IOList[nIoIdxEnum].nIOPoint, pResult);
	return dbValue;
}

void dyn_WRITE_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult)
{
	if(g_IOList[nIoIdxEnum].nIOPoint == -1 || g_IOList[nIoIdxEnum].nIOType != _K_S_IO)
	{
//		printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_WRITE_STRING\n",
//			g_IOList[nIoIdxEnum].nIOPoint, g_IOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return;
	}
	_dWRITE_STRING(g_IOList[nIoIdxEnum].nIOPoint, pValue, pResult);
}

char* dyn_READ_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult)
{
	if(g_IOList[nIoIdxEnum].nIOPoint == -1 || g_IOList[nIoIdxEnum].nIOType != _K_S_IO)
	{
//		printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_READ_STRING\n",
//			g_IOList[nIoIdxEnum].nIOPoint, g_IOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return 0;
	}
	_dREAD_STRING(g_IOList[nIoIdxEnum].nIOPoint, pValue, pResult);
	return pValue;
}

Module_Status dyn_RUN_FUNCTION(int nIoIdxEnum, char *szParam)
{
	if(g_IOList[nIoIdxEnum].nIOPoint == -1 || g_IOList[nIoIdxEnum].nIOType != _K_F_IO)
	{
		return SYS_ABORTED;
	}
	return _dRUN_FUNCTION(nIoIdxEnum, szParam);
}