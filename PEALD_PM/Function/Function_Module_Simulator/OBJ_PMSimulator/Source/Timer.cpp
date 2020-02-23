// Timer.cpp: implementation of the CTimer class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "Timer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimer::CTimer()
{
	m_lProgress = 0;
	m_lPeriod = 0;
	m_nMillisecond = 0;
	m_nSecond = 0;
	m_nMinute = 0;
	m_nHour = 0;
	m_nDay = 0;
	m_nTimerStatus = TIMER_STOP;
}

CTimer::~CTimer()
{

}

void CTimer::InitTimer()		// 2006.02.02
{
	m_lProgress = 0;
	m_lPeriod = 0;
	m_nMillisecond = 0;
	m_nSecond = 0;
	m_nMinute = 0;
	m_nHour = 0;
	m_nDay = 0;
	m_nTimerStatus = TIMER_STOP;
}

//////////////////////////////////////////////////////////////////////
// Member Variable

void CTimer::StartTimer()
{
	m_nTimerStatus = TIMER_RUN;
	GetLocalTime(&m_StartTime);
	m_clkBegin = clock();
	m_clkEnd = 0;
}

void CTimer::StopTimer()
{
	if(m_nTimerStatus == TIMER_RUN)
	{
		m_clkEnd = clock();
		GetLocalTime(&m_StopTime);
		m_lProgress = m_clkEnd - m_clkBegin;
	}
	else if(m_nTimerStatus == TIMER_PAUSE)
	{
		m_clkEnd = m_clkBegin + m_lProgress;
		memcpy(&m_StopTime, &m_NowTime, sizeof(SYSTEMTIME));
	}
	m_nMillisecond = m_lProgress % 1000;
	m_nSecond = (m_lProgress % (1000 * 60)) / 1000;
	m_nMinute = (m_lProgress % (1000 * 60 * 60)) / (1000 * 60);
	m_nHour = (m_lProgress % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60);
	m_nDay = m_lProgress / (1000 * 60 * 60 * 24);
	m_nTimerStatus = TIMER_STOP;
}

void CTimer::PauseTimer()
{
	if(TIMER_RUN == m_nTimerStatus)
	{
		m_lProgress = clock() - m_clkBegin;
		GetLocalTime(&m_NowTime);
		m_nTimerStatus = TIMER_PAUSE;
	}
}

void CTimer::ResumeTimer()
{
	if(TIMER_PAUSE == m_nTimerStatus)
	{
		m_clkBegin = clock() - m_lProgress;
		GetLocalTime(&m_NowTime);
		m_nTimerStatus = TIMER_RUN;
	}
}

void CTimer::CheckProgressTime()
{
	if(TIMER_RUN == m_nTimerStatus) m_lProgress = labs(clock() - m_clkBegin);	// 2006.02.01
	GetLocalTime(&m_NowTime);

	m_nMillisecond = m_lProgress % 1000;
	m_nSecond = (m_lProgress % (1000 * 60)) / 1000;
	m_nMinute = (m_lProgress % (1000 * 60 * 60)) / (1000 * 60);
	m_nHour = (m_lProgress % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60);
	m_nDay = m_lProgress / (1000 * 60 * 60 * 24);
}

long CTimer::GetProgressTime(int nType)
{
	if(TIMER_RUN == m_nTimerStatus) m_lProgress = labs(clock() - m_clkBegin);	// 2006.02.01
	GetLocalTime(&m_NowTime);
	switch(nType)
	{
	case TYPE_MILLISECOND: return m_lProgress;
	case TYPE_SECOND: return (m_lProgress / 1000);
	case TYPE_MINUTE: return (m_lProgress / (1000 * 60));
	case TYPE_HOUR: return (m_lProgress / (1000 * 60 * 60));
	case TYPE_DAY: return (m_lProgress / (1000 * 60 * 60 * 24));
	}
	return 0;
}

void CTimer::SetPeriod(long lMillisecond)
{
	m_lPeriod = lMillisecond;
}

long CTimer::GetRestTime()
{
	if(TIMER_RUN == m_nTimerStatus) m_lProgress = clock() - m_clkBegin;
	GetLocalTime(&m_NowTime);
	return (m_lPeriod - m_lProgress);
}
