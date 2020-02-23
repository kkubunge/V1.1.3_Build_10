// StopWatch.cpp: implementation of the CStopWatch class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>

#include "StopWatch.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStopWatch::CStopWatch()
{

}

CStopWatch::~CStopWatch()
{

}

void CStopWatch::Start()
{
	Reset();

	GetLocalTime(&m_StartTime);	
}

void CStopWatch::CheckStart( const char *desc )
{
	TIME_RECORD record;	
		
	record.end = record.begin = clock();		
	strncpy(record.desc, desc, sizeof(record.desc));

	m_RecordingList.push_front(record);	
}

void CStopWatch::CheckEnd( const char *desc )
{
	for( list<TIME_RECORD>::iterator iter = m_RecordingList.begin(); iter != m_RecordingList.end(); ++iter )
	{
		if( !strcmp(iter->desc, desc) )
		{
			iter->end = clock();
			m_ResultList.push_back(*iter);

			m_RecordingList.erase(iter);
			break;
		}
	}
}

void CStopWatch::Report( FILE *fp )
{	
	if( m_ResultList.empty() ) {
		return;
	}

	m_ResultList.sort();

	fprintf(fp, "[StopWatch] %04d/%02d/%02d %02d:%02d:%02d.%03d Start\n"
		, m_StartTime.wYear, m_StartTime.wMonth, m_StartTime.wDay
		, m_StartTime.wHour, m_StartTime.wMinute, m_StartTime.wSecond
		, m_StartTime.wMilliseconds );

	for( list<TIME_RECORD>::iterator iter = m_ResultList.begin(); iter != m_ResultList.end(); ++iter )
	{
		fprintf(fp, "[StopWatch] (%ld) %s\t%.2fs elapsed\n"
			, iter->begin
			, iter->desc
			, static_cast<double>(iter->end - iter->begin) / CLOCKS_PER_SEC	);
	}

	fprintf(fp, "[StopWatch] %04d/%02d/%02d %02d:%02d:%02d.%03d Stop\n"
		, m_StopTime.wYear, m_StopTime.wMonth, m_StopTime.wDay
		, m_StopTime.wHour, m_StopTime.wMinute, m_StopTime.wSecond
		, m_StopTime.wMilliseconds );
}

void CStopWatch::Reset()
{
	m_RecordingList.clear();
	m_ResultList.clear();
}

void CStopWatch::Stop()
{
	GetLocalTime(&m_StopTime);
}
