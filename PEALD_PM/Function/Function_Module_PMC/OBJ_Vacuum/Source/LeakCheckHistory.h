// LeakCheckHistory.h: interface for the CLeakCheckHistory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEAKCHECKHISTORY_H__7C079D72_C3B6_44E4_825B_AAC1F60DD266__INCLUDED_)
#define AFX_LEAKCHECKHISTORY_H__7C079D72_C3B6_44E4_825B_AAC1F60DD266__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "iodefine.h"

//2014.12.01 Leak Check History Save
#define FILE_PATH ".\\Parameter\\LeakCheckLog.cfg"
#define MAX_HISTORY_COUNT	5

typedef struct sLog{
	char    szTime[256];
	double  dbLastLeakRate;
	double  dbAvgLeakRate;
} S_LOG;

typedef struct {
	int leakCheckAvrIO;
	int leakCheckRateIO;
	int leakCheckTimeIO;
} HISTORY_IO;

static const HISTORY_IO g_HistoryIOTable[MAX_HISTORY_COUNT] = 
{
	{ Log_Avg1, Log_Rate1, Log_Time1 },
	{ Log_Avg2, Log_Rate2, Log_Time2 },
	{ Log_Avg3, Log_Rate3, Log_Time3 },
	{ Log_Avg4, Log_Rate4, Log_Time4 },
	{ Log_Avg5, Log_Rate5, Log_Time5 },
};

class CLeakCheckHistory  
{
private:
	CLeakCheckHistory();	// for singleton pattern( cannot be use 'new' keyword )

public:	
	virtual ~CLeakCheckHistory();	

	void Init();		// Load file and update io
	void SaveLeakCheckHistory(double dbLeakRate , double dbLeakAvg, int nLeakCkSts, int nLineSeq);
	void UpdateIO();	// manually update io from list

	static CLeakCheckHistory* GetInstance();

private:
	void LoadFile();		// Load history from file(FILE_PATH)
	void SaveFile();		// Save history to file

	void PushNewResult(S_LOG &log);	// Push new leak test result to list( maintain 5 records )
private:
	static CLeakCheckHistory* m_pInstance;
	std::list<S_LOG> m_history;
};

#endif // !defined(AFX_LEAKCHECKHISTORY_H__7C079D72_C3B6_44E4_825B_AAC1F60DD266__INCLUDED_)
