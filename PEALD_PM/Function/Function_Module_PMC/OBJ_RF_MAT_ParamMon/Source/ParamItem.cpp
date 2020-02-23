// ParamItem.cpp: implementation of the CParamItem class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>

extern "C" {
#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
}

#include <string>
#include <vector>

using namespace std;

#include "ParamItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParamItem::CParamItem()
{
	
}

CParamItem::~CParamItem()
{

}

void CParamItem::SetIOType( string strType )
{
	if( strType == "D" ) 
	{
		m_nIoType = _K_D_IO;
	} 
	else if( strType == "A" ) 
	{
		m_nIoType = _K_A_IO;
	}
	else if( strType == "S" ) 
	{
		m_nIoType = _K_S_IO;
	} 
	else 
	{
		m_nIoType = _K_D_IO;
	}
}

void CParamItem::SetVirtualIOName( const char *szVirtualIOName )
{	
	strcpy(m_szVirtualIOName, szVirtualIOName);
	m_nVirtualIOPoint = _FIND_FROM_STRING(m_nIoType, m_szVirtualIOName);
}

void CParamItem::SetRealIOName( const char *szRealIoName )
{
	strcpy(m_szRealIOName, szRealIoName);
	m_nRealIOPoint = _FIND_FROM_STRING(m_nIoType, m_szRealIOName);
}

std::string CParamItem::ToString()
{
	char szBuffer[1024];
	sprintf(szBuffer, "Item(Type %d) : %s(%d), %s(%d)"
		, m_nIoType
		, m_szVirtualIOName, m_nVirtualIOPoint
		, m_szRealIOName, m_nRealIOPoint);

	return string(szBuffer);
}

bool CParamItem::Compare()
{
	int nIOResult;
	int nValue = 0;

	if( m_nVirtualIOPoint < 0 || m_nRealIOPoint < 0 ) 
	{
		return false;
	}
	
	if( m_nIoType == _K_D_IO ) 
	{
		int lval = _dREAD_DIGITAL(m_nVirtualIOPoint, &nIOResult);
		int rval = _dREAD_DIGITAL(m_nRealIOPoint, &nIOResult);

		return lval == rval;
	}
	else if( m_nIoType == _K_A_IO ) 
	{
		double lval = _dREAD_ANALOG(m_nVirtualIOPoint, &nIOResult);
		double rval = _dREAD_ANALOG(m_nRealIOPoint, &nIOResult);

		int lval_int = (int)(lval * 100);
		int rval_int = (int)(rval * 100);

		printf("[RF Matcher Compare] == >lval_int: %d ,rval_int: %d \n",lval_int,rval_int);
		nValue = abs(lval_int - rval_int);
		printf("[RF Matcher Compare] == >ABS_Value: %d \n",nValue);

		if(nValue >= 0 && nValue <= 1) return true;
		else						   return false;

		//return lval_int == rval_int;
	} 
	else if( m_nIoType == _K_S_IO ) 
	{
		char lval[40];
		char rval[40];

		_dREAD_STRING(m_nVirtualIOPoint, lval, &nIOResult);
		_dREAD_STRING(m_nRealIOPoint, rval, &nIOResult);

		return (strcmp(lval, rval) == 0) ? true : false;
	}
	else 
	{
		return false;
	}
}
