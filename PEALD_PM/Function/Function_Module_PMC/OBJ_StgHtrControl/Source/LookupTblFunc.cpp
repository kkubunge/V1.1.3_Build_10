// LookupTblFunc.cpp: implementation of the CLookupTblFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "LookupTblFunc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLookupTblFunc::CLookupTblFunc()
{

}

CLookupTblFunc::CLookupTblFunc(LOOKUP_TBL aLookupTbl[], int nTableCount)
{
	m_pLookupTbl = aLookupTbl;
	m_nTableCount = nTableCount;
}

CLookupTblFunc::~CLookupTblFunc()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

void CLookupTblFunc::SetTable(LOOKUP_TBL aLookupTbl[], int nTableCount)
{
	m_pLookupTbl = aLookupTbl;
	m_nTableCount = nTableCount;
}

double CLookupTblFunc::GetScaleValue(double dbValueA)
{
	double dbRet = 0;
	int i;
	int nStart, nEnd, nStep;
	double a, b, x, y;

	if(m_nTableCount == 0) return 0;
	if(m_pLookupTbl[0].dbValueA <= m_pLookupTbl[m_nTableCount-1].dbValueA)
	{
		nStart = 0;
		nEnd = m_nTableCount-1;
		nStep = 1;
	}
	else
	{
		nStart = m_nTableCount-1;
		nEnd = 0;
		nStep = -1;
	}

	do {
		i = nStart;
		if(dbValueA <= m_pLookupTbl[i].dbValueA || i == nEnd) { dbRet = m_pLookupTbl[i].dbValueB; break; }
		do {
			i += nStep;
			if(dbValueA == m_pLookupTbl[i].dbValueA) { dbRet = m_pLookupTbl[i].dbValueB; break; }
			else if(dbValueA < m_pLookupTbl[i].dbValueA)
			{
				// y = ax + b
				a = (m_pLookupTbl[i].dbValueB - m_pLookupTbl[i-1].dbValueB)
					/ (m_pLookupTbl[i].dbValueA - m_pLookupTbl[i-1].dbValueA);
				b = m_pLookupTbl[i].dbValueB - a * m_pLookupTbl[i].dbValueA;
				x = dbValueA;
				y = a * x + b;
				dbRet = y;
				break;
			}
			if(i == nEnd)  { dbRet = m_pLookupTbl[i].dbValueB; break; }
		} while(1);
	} while(0);

	return dbRet;
}

double CLookupTblFunc::GetRevScaleValue(double dbValueB)
{
	double dbRet = 0;
	int i;
	int nStart, nEnd, nStep;
	double a, b, x, y;

	if(m_nTableCount == 0) return 0;
	if(m_pLookupTbl[0].dbValueB <= m_pLookupTbl[m_nTableCount-1].dbValueB)
	{
		nStart = 0;
		nEnd = m_nTableCount-1;
		nStep = 1;
	}
	else
	{
		nStart = m_nTableCount-1;
		nEnd = 0;
		nStep = -1;
	}

	do {
		i = nStart;
		if(dbValueB <= m_pLookupTbl[i].dbValueB || i == nEnd) { dbRet = m_pLookupTbl[i].dbValueA; break; }
		do {
			i += nStep;
			if(dbValueB == m_pLookupTbl[i].dbValueB) { dbRet = m_pLookupTbl[i].dbValueA; break; }
			else if(dbValueB < m_pLookupTbl[i].dbValueB)
			{
				// y = ax + b
				a = (m_pLookupTbl[i].dbValueA - m_pLookupTbl[i-1].dbValueA)
					/ (m_pLookupTbl[i].dbValueB - m_pLookupTbl[i-1].dbValueB);
				b = m_pLookupTbl[i].dbValueA - a * m_pLookupTbl[i].dbValueB;
				x = dbValueB;
				y = a * x + b;
				dbRet = y;
				break;
			}
			if(i == nEnd)  { dbRet = m_pLookupTbl[i].dbValueA; break; }
		} while(1);
	} while(0);

	return dbRet;
}

