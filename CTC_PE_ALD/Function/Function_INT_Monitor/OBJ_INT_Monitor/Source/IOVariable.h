// IOVariable.h: interface for the CIOVariable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOVARIABLE_H__C60D34AB_0DBE_40A1_89FE_6521C2A39C53__INCLUDED_)
#define AFX_IOVARIABLE_H__C60D34AB_0DBE_40A1_89FE_6521C2A39C53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicIO.h"
//--------------------------------------------------------------------
enum { _VTYPE_DIGITAL_IO, _VTYPE_ANALOG_IO, _VTYPE_DIGITAL_VAR, _VTYPE_ANALOG_VAR };
typedef struct _VAR_LIST
{
	int nVarType;
	int nValue;
	double dbValue;
	int nIoIndex;
} VAR_LIST;

//--------------------------------------------------------------------
class CIOVariable  : public CDynamicIO
{
public:
	CIOVariable();
	virtual ~CIOVariable();

	BOOL Initialize();
	void Deinitialize();

	int ReadInteger(int nVarIndex, BOOL *pbIOStatus = NULL);
	double ReadDouble(int nVarIndex, BOOL *pbIOStatus = NULL);

	void WriteInteger(int nVarIndex, int nValue, BOOL *pbIOStatus = NULL);
	void WriteDouble(int nVarIndex, double dbValue, BOOL *pbIOStatus = NULL);

	int GetVarType(int nVarIndex);
	int GetIoIndex(int nVarIndex);
	int GetIoType(int nVarIndex);
	BOOL GetIoName(int nVarIndex, char *szIoName);

	int AddVariable(const char *szVarWord, int nIOType);
public:
	#define MAX_VARIABLE	300
	int m_nVarCount;
	VAR_LIST	m_VarList[MAX_VARIABLE];

	#define MAX_IO_LIST		200
	IO_LIST		m_IOList[MAX_IO_LIST];
};

#endif // !defined(AFX_IOVARIABLE_H__C60D34AB_0DBE_40A1_89FE_6521C2A39C53__INCLUDED_)
