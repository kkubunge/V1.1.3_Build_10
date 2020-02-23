// IOVariable.cpp: implementation of the CIOVariable class.
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

#include "TextParser.h"
#include "IOVariable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOVariable::CIOVariable()
{
	int i;
	m_nVarCount = 0;
	for(i=0; i<MAX_VARIABLE; i++)
	{
		m_VarList[i].nVarType = _VTYPE_DIGITAL_VAR;
		m_VarList[i].nIoIndex = -1;
		m_VarList[i].dbValue = 0;
		m_VarList[i].nValue = 0;
	}
}

CIOVariable::~CIOVariable()
{
}

BOOL CIOVariable::Initialize()
{
	ClearAndInitIOList(m_IOList, MAX_IO_LIST);
	return TRUE;
}

void CIOVariable::Deinitialize()
{
}

int CIOVariable::ReadInteger(int nVarIndex, BOOL *pbIOStatus)
{
	int nRet = 0;
	BOOL bIOStatus = TRUE;

	if(nVarIndex >= m_nVarCount) bIOStatus = FALSE;
	else switch(m_VarList[nVarIndex].nVarType)
	{
	case _VTYPE_DIGITAL_IO:		nRet = dREAD_DIGITAL(m_VarList[nVarIndex].nIoIndex, &bIOStatus);
								break;
	case _VTYPE_ANALOG_IO:		nRet = (int) dREAD_ANALOG(m_VarList[nVarIndex].nIoIndex, &bIOStatus);
								break;
	case _VTYPE_DIGITAL_VAR:	nRet = m_VarList[nVarIndex].nValue;
								break;
	case _VTYPE_ANALOG_VAR:		nRet = (int) m_VarList[nVarIndex].dbValue;
								break;
	}
	if(pbIOStatus != NULL) *pbIOStatus = bIOStatus;
	return nRet;
}

double CIOVariable::ReadDouble(int nVarIndex, BOOL *pbIOStatus)
{
	double dbRet = 0;
	BOOL bIOStatus = TRUE;

	if(nVarIndex >= m_nVarCount) bIOStatus = FALSE;
	else switch(m_VarList[nVarIndex].nVarType)
	{
	case _VTYPE_DIGITAL_IO:		dbRet = dREAD_DIGITAL(m_VarList[nVarIndex].nIoIndex, &bIOStatus);
								break;
	case _VTYPE_ANALOG_IO:		dbRet = dREAD_ANALOG(m_VarList[nVarIndex].nIoIndex, &bIOStatus);
								break;
	case _VTYPE_DIGITAL_VAR:	dbRet = m_VarList[nVarIndex].nValue;
								break;
	case _VTYPE_ANALOG_VAR:		dbRet = m_VarList[nVarIndex].dbValue;
								break;
	}
	if(pbIOStatus != NULL) *pbIOStatus = bIOStatus;
	return dbRet;
}

void CIOVariable::WriteInteger(int nVarIndex, int nValue, BOOL *pbIOStatus)
{
	BOOL bIOStatus = TRUE;

	if(nVarIndex >= m_nVarCount) bIOStatus = FALSE;
	else switch(m_VarList[nVarIndex].nVarType)
	{
	case _VTYPE_DIGITAL_IO:		dWRITE_DIGITAL(m_VarList[nVarIndex].nIoIndex, nValue, &bIOStatus);
								break;
	case _VTYPE_ANALOG_IO:		dWRITE_ANALOG(m_VarList[nVarIndex].nIoIndex, nValue, &bIOStatus);
								break;
	case _VTYPE_DIGITAL_VAR:	m_VarList[nVarIndex].nValue = nValue;
								break;
	case _VTYPE_ANALOG_VAR:		m_VarList[nVarIndex].dbValue = nValue;
								break;
	}
	if(pbIOStatus != NULL) *pbIOStatus = bIOStatus;
}

void CIOVariable::WriteDouble(int nVarIndex, double dbValue, BOOL *pbIOStatus)
{
	BOOL bIOStatus = TRUE;

	if(nVarIndex >= m_nVarCount) bIOStatus = FALSE;
	else switch(m_VarList[nVarIndex].nVarType)
	{
	case _VTYPE_DIGITAL_IO:		dWRITE_DIGITAL(m_VarList[nVarIndex].nIoIndex, (int) dbValue, &bIOStatus);
								break;
	case _VTYPE_ANALOG_IO:		dWRITE_ANALOG(m_VarList[nVarIndex].nIoIndex, dbValue, &bIOStatus);
								break;
	case _VTYPE_DIGITAL_VAR:	m_VarList[nVarIndex].nValue = (int) dbValue;
								break;
	case _VTYPE_ANALOG_VAR:		m_VarList[nVarIndex].dbValue = dbValue;
								break;
	}
	if(pbIOStatus != NULL) *pbIOStatus = bIOStatus;
}

int CIOVariable::GetVarType(int nVarIndex)
{
	int nRet = -1;
	if(m_nVarCount >= MAX_VARIABLE) return -1;
	nRet = m_VarList[nVarIndex].nVarType;
	return nRet;
}

int CIOVariable::GetIoIndex(int nVarIndex)
{
	int nRet = -1;
	if(m_nVarCount >= MAX_VARIABLE) return -1;
	nRet = m_VarList[nVarIndex].nIoIndex;
	return nRet;
}

int CIOVariable::GetIoType(int nVarIndex)
{
	int nRet = -1;
	int nIoIndex = -1;

	if(m_nVarCount >= MAX_VARIABLE) return -1;
	nIoIndex = GetIoIndex(nVarIndex);
	if(nIoIndex < 0) return -1;
	nRet = m_IOList[nIoIndex].nIOType;
	return nRet;
}

BOOL CIOVariable::GetIoName(int nVarIndex, char *szIoName)
{
	int nIoIndex = -1;

	if(m_nVarCount >= MAX_VARIABLE) return -1;
	nIoIndex = GetIoIndex(nVarIndex);
	if(nIoIndex < 0)
	{
		strcpy(szIoName, "");
		return FALSE;
	}
	strcpy(szIoName, m_IOList[nIoIndex].szIOName);
	return TRUE;
}


int CIOVariable::AddVariable(const char *szVarWord, int nIOType)
{
	int nRet = -1;
	CTextParser txtP;

	// printf("--->AddVar %s, %d\n", szVarWord, nIOType);
	if(m_nVarCount >= MAX_VARIABLE) return -1;
	nRet = m_nVarCount;

	if(txtP.IsNumeric(szVarWord))
	{
		switch(nIOType)
		{
		case _K_D_IO:
			m_VarList[m_nVarCount].nVarType = _VTYPE_DIGITAL_VAR;
			m_VarList[m_nVarCount].nValue = atoi(szVarWord);
			break;
		case _K_A_IO:
			m_VarList[m_nVarCount].nVarType = _VTYPE_ANALOG_VAR;
			m_VarList[m_nVarCount].dbValue = atof(szVarWord);
			break;
		default : nRet = -1; break;
		}
	}
	else
	{
		switch(nIOType)
		{
		case _K_D_IO:
			m_VarList[m_nVarCount].nVarType = _VTYPE_DIGITAL_IO;
			m_VarList[m_nVarCount].nIoIndex = RegistIOChannel(szVarWord, _K_D_IO);
			if(m_VarList[m_nVarCount].nIoIndex < 0) nRet = -1;
			break;
		case _K_A_IO:
			m_VarList[m_nVarCount].nVarType = _VTYPE_ANALOG_IO;
			m_VarList[m_nVarCount].nIoIndex = RegistIOChannel(szVarWord, _K_A_IO);
			if(m_VarList[m_nVarCount].nIoIndex < 0) nRet = -1;
			break;
		default : nRet = -1; break;
		}
	}

	if(nRet == m_nVarCount) m_nVarCount++;
	else
	{
		// printf("-----> AddVar Error [%s]\n", szVarWord);
	}
	return nRet;
}
