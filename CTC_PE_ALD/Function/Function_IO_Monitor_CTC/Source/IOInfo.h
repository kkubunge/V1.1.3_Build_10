// IOInfo.h: interface for the CIOInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOINFO_H__AF9FC313_0670_4C05_90CF_B61D3BA0523F__INCLUDED_)
#define AFX_IOINFO_H__AF9FC313_0670_4C05_90CF_B61D3BA0523F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Utility.h"

class CDataProcessor;
class CIOInfo  
{
public:
	CIOInfo(CDataProcessor* pDataProcessor = NULL);
	virtual ~CIOInfo();

	bool   CreateIO(long enIOType , char* szAddress);
	bool   CreateLookup(char* szLookup);
	bool   CalCurrentIOValue(unsigned char* ucIOMem);
	double ConversionPhysicalToLogical(double dbRawData);

	int    GetIOType  () {return m_enIOType;};
	int    GetIntValue() {return m_nCurrentValue;};
	double GetDBValue () {return m_dbCurrentValue;};
	char*  GetIOName  () {return m_szIOName;}

private:
	CDataProcessor* m_pDataProcessor;
	char            m_szIOName[32];
	long            m_enIOType;

private:	
	bool   m_bIsLookupNeed;	
	int    m_nStartByte;
	int    m_nStartBit;
	int	   m_nEndBit;	
	int    m_nCurrentValue;
	int    m_nID2; 
	double m_dbCurrentValue;

	LOOKUP_DATA m_sLookupTable;

};

#endif // !defined(AFX_IOINFO_H__AF9FC313_0670_4C05_90CF_B61D3BA0523F__INCLUDED_)
