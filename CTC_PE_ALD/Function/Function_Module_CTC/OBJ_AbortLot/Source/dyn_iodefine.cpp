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
	{"PM1.PRO_RunStsDM", _K_D_IO, 0},
	{"PM2.PRO_RunStsDM", _K_D_IO, 0},
	{"PM3.PRO_RunStsDM", _K_D_IO, 0},
	{"PM4.PRO_RunStsDM", _K_D_IO, 0},
	{"PM5.PRO_RunStsDM", _K_D_IO, 0},
	""
};

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
