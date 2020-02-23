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
	m_bErrPrint = TRUE;
	m_nIOCount = 0;
	m_pIOList = NULL;
}

CDynamicIO::~CDynamicIO()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function


BOOL CDynamicIO::InitIOList(IO_LIST pIOList[], BOOL bErrPrint)
{
	BOOL bRet = TRUE;
	int i;
	m_pIOList = pIOList;
	m_bErrPrint = bErrPrint;
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
			if(m_bErrPrint)
				printf("-----> Invalid IO Name : %s (ChannelType:%d)\n",m_pIOList[i].szIOName, m_pIOList[i].nIOType);
			bRet = FALSE;
		}
	}
	m_nIOCount = i;
	return bRet;
}

void CDynamicIO::ReplaceIONameCharSet(IO_LIST pIOList[], char* szOrg, char* SHTNum)
{
	int i = 0 ;

	while(strlen(pIOList[i].szIOName) > 0)
	{
		char* pszIONameTemp = NULL;
		char* strCTemp = NULL;
		char* strTail = NULL;
		char* strHead = NULL;
		char sumStr[40]="";
		
		pszIONameTemp = pIOList[i].szIOName;

		strCTemp = strstr( pszIONameTemp , szOrg );
				
		if(strCTemp != NULL)
		{
			strHead = strtok(pszIONameTemp, szOrg);   
			strcat(sumStr, strHead);
			strcat(sumStr, SHTNum);
			strTail = strCTemp+2;			
			strcat(sumStr, strTail);
			memset(pIOList[i].szIOName, 0, sizeof(pIOList[i].szIOName));
			strcpy(pIOList[i].szIOName, sumStr);
		}

		i++;		
	}
}


BOOL CDynamicIO::IOValid(int nIoIdxEnum)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); return FALSE; }
	if(nIoIdxEnum >= m_nIOCount) return FALSE;
	if(m_pIOList[nIoIdxEnum].nIOPoint == -1) return FALSE;
	else return TRUE;
}

void CDynamicIO::dWRITE_DIGITAL(int nIoIdxEnum, int nValue, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return; }
	if(m_pIOList[nIoIdxEnum].nIOPoint == -1 || m_pIOList[nIoIdxEnum].nIOType != _K_D_IO)
	{
		//	printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_WRITE_DIGITAL\n",
		//		m_pIOList[nIoIdxEnum].nIOPoint, m_pIOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return;
	}
	_dWRITE_DIGITAL(m_pIOList[nIoIdxEnum].nIOPoint, nValue, pResult);
}

int CDynamicIO::dREAD_DIGITAL(int nIoIdxEnum, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return 0; }
	int nValue = 0;
	if(m_pIOList[nIoIdxEnum].nIOPoint == -1 || m_pIOList[nIoIdxEnum].nIOType != _K_D_IO)
	{
		//	printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_READ_DIGITAL\n",
		//		m_pIOList[nIoIdxEnum].nIOPoint, m_pIOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return 0;
	}
	nValue = _dREAD_DIGITAL(m_pIOList[nIoIdxEnum].nIOPoint, pResult);
	return nValue;
}

void CDynamicIO::dWRITE_ANALOG(int nIoIdxEnum, double dbValue, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return; }
	if(m_pIOList[nIoIdxEnum].nIOPoint == -1 || m_pIOList[nIoIdxEnum].nIOType != _K_A_IO)
	{
		//	printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_WRITE_ANALOG\n",
		//		m_pIOList[nIoIdxEnum].nIOPoint, m_pIOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return;
	}
	_dWRITE_ANALOG(m_pIOList[nIoIdxEnum].nIOPoint, dbValue, pResult);
}

double CDynamicIO::dREAD_ANALOG(int nIoIdxEnum, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return 0; }
	double dbValue = 0;
	if(m_pIOList[nIoIdxEnum].nIOPoint == -1 || m_pIOList[nIoIdxEnum].nIOType != _K_A_IO)
	{
		//	printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_READ_ANALOG\n",
		//		m_pIOList[nIoIdxEnum].nIOPoint, m_pIOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return 0;
	}
	dbValue = _dREAD_ANALOG(m_pIOList[nIoIdxEnum].nIOPoint, pResult);
	return dbValue;
}

void CDynamicIO::dWRITE_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return; }
	if(m_pIOList[nIoIdxEnum].nIOPoint == -1 || m_pIOList[nIoIdxEnum].nIOType != _K_S_IO)
	{
		//	printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_WRITE_STRING\n",
		//		m_pIOList[nIoIdxEnum].nIOPoint, m_pIOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return;
	}
	_dWRITE_STRING(m_pIOList[nIoIdxEnum].nIOPoint, pValue, pResult);
}

char* CDynamicIO::dREAD_STRING(int nIoIdxEnum, char *pValue, BOOL *pResult)
{
	if(m_pIOList == NULL) { printf("-----> IO List is not initialized !\n"); *pResult = FALSE; return 0; }
	if(m_pIOList[nIoIdxEnum].nIOPoint == -1 || m_pIOList[nIoIdxEnum].nIOType != _K_S_IO)
	{
		// printf("-----> Invalid IOPoint(%d) or IOType(%d) in dyn_READ_STRING\n",
		//		m_pIOList[nIoIdxEnum].nIOPoint, m_pIOList[nIoIdxEnum].nIOType);
		*pResult = FALSE;
		return 0;
	}
	_dREAD_STRING(m_pIOList[nIoIdxEnum].nIOPoint, pValue, pResult);
	return pValue;
}
