// DynamicIO.cpp: implementation of the CDynamicIO class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <string.h>
//====================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//====================================================================

#include "DynamicIO.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDynamicIO::CDynamicIO()
{
	m_bIOValidErrCheck = TRUE;
	m_nIOCount = 0;
	m_pIOList = NULL;
}

CDynamicIO::~CDynamicIO()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

void CDynamicIO::ReplaceIONameChar(IO_LIST pIOList[], char cOrg, char cNew)		// 2006.11.22
{
	int i=0, j;
	while(strlen(pIOList[i].szIOName) > 0)
	{
		for(j=0; pIOList[i].szIOName[j] != NULL; j++)
		{
			if(pIOList[i].szIOName[j] == cOrg) pIOList[i].szIOName[j] = cNew;
		}
		i++;
	}
}

BOOL CDynamicIO::InitIOList(IO_LIST pIOList[], bool bIOValidErrCheck, int nIOValidCheckEndIdx)
{
	BOOL bRet = TRUE;
	int i;
	m_pIOList = pIOList;
	m_bIOValidErrCheck = bIOValidErrCheck;
	for(i=0; m_pIOList[i].szIOName[0] != 0; i++)
	{
		if(strncmp("NotDefined", m_pIOList[i].szIOName, 10) == 0)
		{
			m_pIOList[i].nIOPoint = -1;
			continue;
		}
		m_pIOList[i].nIOPoint = _FIND_FROM_STRING(m_pIOList[i].nIOType, m_pIOList[i].szIOName);
		if(m_pIOList[i].nIOPoint == -1)
		{
			if(bIOValidErrCheck && i<=nIOValidCheckEndIdx)
			{
				printf("-----> Init IO failed ![Name:%s,Type:%d]\n",m_pIOList[i].szIOName, m_pIOList[i].nIOType);
				bRet = FALSE;
			}
		}
	}
	m_nIOCount = i;
	return bRet;
}


int CDynamicIO::RegistIOChannel(const char *szIOName, int nIOType)	// 2006.03.30
{
	int nIoIdx = -1;
	int i;

	for(i=0; m_pIOList[i].szIOName[0] != 0; i++)
	{
		if(strncmp("NotDefined", m_pIOList[i].szIOName, 10) == 0)
		{
			strcpy(m_pIOList[i].szIOName, szIOName);
			m_pIOList[i].nIOType = nIOType;
			nIoIdx = i;
			break;
		}
	}
	if(! InitIOChannel(nIoIdx))
	{
		strcpy(m_pIOList[i].szIOName, "NotDefined");
		nIoIdx = -1;
	}
	return nIoIdx;
}

int CDynamicIO::GetIoIndex(char *szIOName)		// 2006.03.30
{
	int nIoIdx = -1;
	int i;

	for(i=0; m_pIOList[i].szIOName[0] != 0; i++)
	{
		if(strcmp(m_pIOList[i].szIOName, szIOName) == 0)
		{
			nIoIdx = i;
			break;
		}
	}
	return nIoIdx;
}

BOOL CDynamicIO::InitIOChannel(int nIoIndex)		// 2006.03.30
{
	BOOL bRet;
	int i;
	i = nIoIndex;

	m_pIOList[i].nIOPoint = _FIND_FROM_STRING(m_pIOList[i].nIOType, m_pIOList[i].szIOName);
	if(m_pIOList[i].nIOPoint == -1)
	{
		printf("-----> Init IO failed ![Name:%s,Type:%d]\n",m_pIOList[i].szIOName, m_pIOList[i].nIOType);
		bRet = FALSE;
	}
	return bRet;
}

BOOL CDynamicIO::IOValid(int nIoIndex)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); return FALSE; }
	if(nIoIndex >= m_nIOCount) return FALSE;
	if(m_pIOList[nIoIndex].nIOPoint == -1) return FALSE;
	else return TRUE;
}

void CDynamicIO::dWRITE_DIGITAL(int nIoIndex, int nValue, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dWRITE_DIGITAL\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		*pResult = FALSE;
		return;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_D_IO)	// 2006.11.24
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dWRITE_DIGITAL\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		*pResult = FALSE;
		return;
	}
	_dWRITE_DIGITAL(m_pIOList[nIoIndex].nIOPoint, nValue, pResult);
}

int CDynamicIO::dREAD_DIGITAL(int nIoIndex, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return 0; }
	int nValue = 0;
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dREAD_DIGITAL\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		*pResult = FALSE;
		return 0;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_D_IO)	// 2006.11.24
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dREAD_DIGITAL\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		*pResult = FALSE;
		return 0;
	}
	nValue = _dREAD_DIGITAL(m_pIOList[nIoIndex].nIOPoint, pResult);
	return nValue;
}

// 2008.10.23
int CDynamicIO::dREAD_DIGITAL_TRUE(int nIoIndex, BOOL *pResult)
{
	int nValue;
	do {
		nValue = dREAD_DIGITAL(nIoIndex, pResult);
		if( *pResult ) break;
		Sleep(100);
		nValue = dREAD_DIGITAL(nIoIndex, pResult);
		if( *pResult ) break;
		Sleep(300);
		nValue = dREAD_DIGITAL(nIoIndex, pResult);
		if( *pResult ) break;
		Sleep(500);
		nValue = dREAD_DIGITAL(nIoIndex, pResult);
		if( *pResult ) break;
		Sleep(1000);
		nValue = dREAD_DIGITAL(nIoIndex, pResult);
		if( *pResult ) break;
		Sleep(1000);
		nValue = dREAD_DIGITAL(nIoIndex, pResult);
	} while(0);
	return nValue;
}

void CDynamicIO::dWRITE_ANALOG(int nIoIndex, double dbValue, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dWRITE_ANALOG\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		*pResult = FALSE;
		return;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_A_IO)	// 2006.11.24
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dWRITE_ANALOG\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		*pResult = FALSE;
		return;
	}
	_dWRITE_ANALOG(m_pIOList[nIoIndex].nIOPoint, dbValue, pResult);
}

double CDynamicIO::dREAD_ANALOG(int nIoIndex, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return 0; }
	double dbValue = 0;
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dREAD_ANALOG\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		*pResult = FALSE;
		return 0;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_A_IO)	// 2006.11.24
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dREAD_ANALOG\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		*pResult = FALSE;
		return 0;
	}
	dbValue = _dREAD_ANALOG(m_pIOList[nIoIndex].nIOPoint, pResult);
	return dbValue;
}

void CDynamicIO::dWRITE_STRING(int nIoIndex, char *pValue, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dWRITE_STRING\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		*pResult = FALSE;
		return;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_S_IO)	// 2006.11.24
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dWRITE_STRING\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		*pResult = FALSE;
		return;
	}
	_dWRITE_STRING(m_pIOList[nIoIndex].nIOPoint, pValue, pResult);
}

char* CDynamicIO::dREAD_STRING(int nIoIndex, char *pValue, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return 0; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dREAD_STRING\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		*pResult = FALSE;
		return 0;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_S_IO)	// 2006.11.24
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dREAD_STRING\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		return 0;
	}
	_dREAD_STRING(m_pIOList[nIoIndex].nIOPoint, pValue, pResult);
	return pValue;
}

//-----------------------------------------------------------------
// 2008.12.03
void CDynamicIO::dWRITE_DIGITAL_FROM_NAME(char *szIOName, int nValue, BOOL *pResult)
{
	int nIOPoint;
	nIOPoint = _FIND_FROM_STRING(_K_D_IO, szIOName);
	if(nIOPoint != -1)
	{
		_dWRITE_DIGITAL(nIOPoint, nValue, pResult);
	}
}

void CDynamicIO::dWRITE_ANALOG_FROM_NAME(char *szIOName, double dbValue, BOOL *pResult)
{
	int nIOPoint;
	nIOPoint = _FIND_FROM_STRING(_K_A_IO, szIOName);
	if(nIOPoint != -1)
	{
		_dWRITE_ANALOG(nIOPoint, dbValue, pResult);
	}
}

void CDynamicIO::dWRITE_STRING_FROM_NAME(char *szIOName, char *szValue, BOOL *pResult)		// 2009.12.21
{
	int nIOPoint;
	nIOPoint = _FIND_FROM_STRING(_K_A_IO, szIOName);
	if(nIOPoint != -1)
	{
		_dWRITE_STRING(nIOPoint, szValue, pResult);
	}
}

void CDynamicIO::dWRITE_IO_FROM_NAME(char *szIOName, int nIOType, int nValue, double dbValue, BOOL *pResult)
{
	switch(nIOType)
	{
	case _K_D_IO: dWRITE_DIGITAL_FROM_NAME(szIOName, nValue, pResult); break;
	case _K_A_IO: dWRITE_ANALOG_FROM_NAME(szIOName, dbValue, pResult); break;
	default : *pResult = FALSE; break;
	}
}

void CDynamicIO::dWRITE_IO_FROM_NAME(char *szIOName, int nIOType, char *szValue, BOOL *pResult)	// 2009.12.21
{
	switch(nIOType)
	{
	case _K_D_IO: dWRITE_DIGITAL_FROM_NAME(szIOName, atoi(szValue), pResult); break;
	case _K_A_IO: dWRITE_ANALOG_FROM_NAME(szIOName, atof(szValue), pResult); break;
	case _K_S_IO: dWRITE_STRING_FROM_NAME(szIOName, szValue, pResult); break;
	default : *pResult = FALSE; break;
	}
}

//-----------------------------------------------------------------
int CDynamicIO::dRUN_FUNCTION(int nIoIndex, char *szParam)
{
	int nRet = SYS_ABORTED;
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); return nRet; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dRUN_FUNCTION\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		return nRet;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_F_IO)	// 2006.11.24
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dRUN_FUNCTION\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		return nRet;
	}
	nRet = _dRUN_FUNCTION(m_pIOList[nIoIndex].nIOPoint, szParam);
	return nRet;
}

int CDynamicIO::dRUN_WAIT_FUNCTION(int nIoIndex)
{
	int nRet = SYS_ABORTED;
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); return nRet; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dRUN_WAIT_FUNCTION\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		return nRet;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_F_IO)
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dRUN_WAIT_FUNCTION\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		return nRet;
	}
	nRet = _dRUN_WAIT_FUNCTION(m_pIOList[nIoIndex].nIOPoint);
	return nRet;
}

BOOL CDynamicIO::dRUN_FUNCTION_FREE(int nIoIndex, char *szParam)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); return FALSE; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dRUN_FUNCTION_FREE\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		return FALSE;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_F_IO)
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dRUN_FUNCTION_FREE\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		return FALSE;
	}
	_dRUN_FUNCTION_FREE(m_pIOList[nIoIndex].nIOPoint, szParam);
	return TRUE;
}

BOOL CDynamicIO::dRUN_FUNCTION_ABORT(int nIoIndex)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); return FALSE; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dRUN_FUNCTION_ABORT\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		return FALSE;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_F_IO)
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dRUN_FUNCTION_ABORT\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		return FALSE;
	}
	_dRUN_FUNCTION_ABORT(m_pIOList[nIoIndex].nIOPoint);
	return TRUE;
}

BOOL CDynamicIO::dRUN_SET_FUNCTION(int nIoIndex, char *szParam)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); return FALSE; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dRUN_SET_FUNCTION\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		return FALSE;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_F_IO)
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dRUN_SET_FUNCTION\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		return FALSE;
	}
	_dRUN_SET_FUNCTION(m_pIOList[nIoIndex].nIOPoint, szParam);
	return TRUE;
}

int CDynamicIO::dREAD_FUNCTION(int nIoIndex)
{
	int nRet = SYS_ABORTED;
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); return nRet; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dREAD_FUNCTION\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		return nRet;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_F_IO)
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dREAD_FUNCTION\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		return nRet;
	}
	nRet = _dREAD_FUNCTION(m_pIOList[nIoIndex].nIOPoint);
	return nRet;
}

BOOL CDynamicIO::dWRITE_FUNCTION_EVENT(int nIoIndex, char *szParam)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); return FALSE; }
	if(m_pIOList[nIoIndex].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dWRITE_FUNCTION_EVENT\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		#endif
		return FALSE;
	}
	if(m_pIOList[nIoIndex].nIOType != _K_F_IO)
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dWRITE_FUNCTION_EVENT\n",
				m_pIOList[nIoIndex].szIOName, m_pIOList[nIoIndex].nIOType, m_pIOList[nIoIndex].nIOPoint);
		return FALSE;
	}
	_dWRITE_FUNCTION_EVENT(m_pIOList[nIoIndex].nIOPoint, szParam);
	return TRUE;
}

// __BEGIN__ 2006.09.28 by ycchoi
int CDynamicIO::dWRITE_FUNCTION(int nIoIdxEnum, char *szParam)
{
	int nRet = SYS_ABORTED;
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); return nRet; }
	if(m_pIOList[nIoIdxEnum].nIOPoint == -1)
	{
		#ifdef _DEBUG
		printf("-----> Invalid IO [Name:%s,Type:%d,Point:%d] in dWRITE_FUNCTION\n",
				m_pIOList[nIoIdxEnum].szIOName, m_pIOList[nIoIdxEnum].nIOType, m_pIOList[nIoIdxEnum].nIOPoint);
		#endif
		return nRet;
	}
	if(m_pIOList[nIoIdxEnum].nIOType != _K_F_IO)
	{
		printf("-----> Invalid IO Type[Name:%s,Type:%d,Point:%d] in dWRITE_FUNCTION\n",
				m_pIOList[nIoIdxEnum].szIOName, m_pIOList[nIoIdxEnum].nIOType, m_pIOList[nIoIdxEnum].nIOPoint);
		return nRet;
	}
	nRet = _dWRITE_FUNCTION(m_pIOList[nIoIdxEnum].nIOPoint, szParam);
	return nRet;
}
// __END__ 2006.09.28 by ycchoi

