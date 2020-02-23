// Timer.h: interface for the CTimer class.
// Create : 2002.12.
// Last Update : 2006.12.01
// Programed by Changsu,Kim
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMER_H__CA6CF6A6_E098_11D6_B68C_009027E90DD2__INCLUDED_)
#define AFX_TIMER_H__CA6CF6A6_E098_11D6_B68C_009027E90DD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>

// Timer Status
enum { TIMER_STOP, TIMER_RUN, TIMER_PAUSE };
// Time Unit
// enum { TYPE_MILLISECOND, TYPE_SECOND, TYPE_MINUTE, TYPE_HOUR, TYPE_DAY } ;	// 2006.11.21
enum { UNIT_MILLISECOND, UNIT_SECOND, UNIT_MINUTE, UNIT_HOUR, UNIT_DAY } ;		// 2006.12.01

class CTimer  
{
public:
	CTimer();
	virtual ~CTimer();

// Member Function
public:
	void InitTimer();				// 2006.02.02
	void StartTimer();
	void StopTimer();
	void PauseTimer();
	void ResumeTimer();
	void CheckProgressTime();

	long GetProgressTime(int nType = UNIT_MILLISECOND);

	void SetPeriod(long lMillisecond);
	long GetRestTime();

// Member Variable
public:
	SYSTEMTIME m_StartTime;
	SYSTEMTIME m_StopTime;
	SYSTEMTIME m_NowTime;

	int m_nMillisecond;
	int m_nSecond;
	int m_nMinute;
	int m_nHour;
	int m_nDay;
	int m_nTimerStatus;

private:
	clock_t m_clkBegin;
	clock_t m_clkEnd;
	long m_lProgress;
	long m_lPeriod;
};

#endif // !defined(AFX_TIMER_H__CA6CF6A6_E098_11D6_B68C_009027E90DD2__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2002.12		Created
2005.10.12	Update History 기록 시작
			Timer Status Enum을 class 외부에서 참조 또는 사용할 수 있도록
			정의 위치 변경.
2006.02.01	GetProgressTime(...), CheckProgressTime(...) Function 에 내부
			연산을 절대값으로 구하도록 수정.
2006.02.02	InitTimer() Function 추가.
2006.11.21	GetProgressTime(...) 의 Parameter Type Enum Class 외부에서
			쓸 수 있도록 함.
2006.12.01	'TYPE_---' Enum을 'UNIT_---' Enum으로 수정
--------------------------------------------------------------------*/
