// StopWatch.h: interface for the CStopWatch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STOPWATCH_H__B47C46AC_4B60_4E7D_88AB_23E4A0C112D6__INCLUDED_)
#define AFX_STOPWATCH_H__B47C46AC_4B60_4E7D_88AB_23E4A0C112D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <time.h>

using namespace std;

typedef struct _TIME_RECORD {
	clock_t begin, end;	
	char desc[80];

	bool operator<( const _TIME_RECORD& rhs ) const
	{
		return begin < rhs.begin;
	}

} TIME_RECORD;

class CStopWatch  
{
public:
	CStopWatch();
	virtual ~CStopWatch();

	void Start();
	void Stop();
	void Reset();

	void CheckStart(const char *desc);
	void CheckEnd(const char *desc);

	void Report(FILE *fp);

private:
	list<TIME_RECORD> m_RecordingList;
	list<TIME_RECORD> m_ResultList;

	SYSTEMTIME m_StartTime, m_StopTime;
};

#endif // !defined(AFX_STOPWATCH_H__B47C46AC_4B60_4E7D_88AB_23E4A0C112D6__INCLUDED_)
