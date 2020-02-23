// LeakCheckHistory.cpp: implementation of the CLeakCheckHistory class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <windows.h>

extern "C" 
{
	#include "cimseqnc.h"
}

#include "EnumDefine.h"
#include "LeakCheckHistory.h"
#include "TextParser.h"

using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLeakCheckHistory* CLeakCheckHistory::m_pInstance = NULL;

CLeakCheckHistory::CLeakCheckHistory()
{
}

CLeakCheckHistory::~CLeakCheckHistory()
{	
}

CLeakCheckHistory* CLeakCheckHistory::GetInstance()
{
	if( m_pInstance == NULL )
	{
		m_pInstance = new CLeakCheckHistory();
	}
	
	return m_pInstance;
}

void CLeakCheckHistory::Init()
{
	LoadFile();
	UpdateIO();
}

void CLeakCheckHistory::LoadFile()
{
	//Get Log from file
	FILE *fpLog;
	S_LOG log;
	int i=0;

	char szItem[256];
	char szRead[256];

	fpLog = fopen(FILE_PATH , "rt");
	if( fpLog == NULL )
	{
		return;
	}

	m_history.clear();		
	
	while( !feof(fpLog) ) 
	{
		if (NULL == fgets(szRead, 256, fpLog)) break;

		CTextParser tp(szRead);
		do {
			if (FALSE == tp.GetWord(szItem , 0))
			{
				break;
			}

			if (strlen(szItem) > 255) 
			{
				printf ("Read Time over limited count \n");
				break;
			}
			else 
			{
				strcpy(log.szTime , szItem);
			}

			if (FALSE == tp.GetDbFloat((double*)&log.dbLastLeakRate , 0)) break;			
			if (FALSE == tp.GetDbFloat((double*)&log.dbAvgLeakRate  , 0)) break;

			m_history.push_back(log);

			if (++i >= MAX_HISTORY_COUNT) break;
		} while (0);
	}
	fclose(fpLog);
}

void CLeakCheckHistory::SaveFile()
{
	//Save to file
	FILE *fpLog;
	fpLog = fopen(FILE_PATH , "w+");
	if (NULL == fpLog) 
	{
		return;
	}

	for( list<S_LOG>::iterator iter=m_history.begin(); iter != m_history.end(); ++iter)
	{
        if (NULL == iter->szTime)
			continue;

		fprintf(fpLog, "%s\t%f\t%f\n", iter->szTime , iter->dbLastLeakRate , iter->dbAvgLeakRate);
	}
	
	fclose(fpLog);	
}

void CLeakCheckHistory::PushNewResult( S_LOG &log )
{
	int nLength;

	m_history.push_front(log);

	nLength = m_history.size();
	while( nLength > MAX_HISTORY_COUNT )
	{
		m_history.pop_back();
		nLength = m_history.size();
	}
}

void CLeakCheckHistory::UpdateIO()
{
	int nIOResult = 0;
	int nLength = m_history.size();
	int nCount = (nLength < MAX_HISTORY_COUNT) ? nLength : MAX_HISTORY_COUNT;

	list<S_LOG>::iterator iter = m_history.begin();
	for( int i=0; i<nCount; i++ ) 
	{
		WRITE_STRING(g_HistoryIOTable[i].leakCheckTimeIO, iter->szTime, &nIOResult);
		WRITE_ANALOG(g_HistoryIOTable[i].leakCheckAvrIO, iter->dbAvgLeakRate, &nIOResult);
		WRITE_ANALOG(g_HistoryIOTable[i].leakCheckRateIO, iter->dbLastLeakRate, &nIOResult);
		
		if( ++iter == m_history.end() ) 
		{
			break;
		}
	}
}

void CLeakCheckHistory::SaveLeakCheckHistory( double dbLeakRate , double dbLeakAvg, int nLeakCkSts, int nLineSeq )
{
	SYSTEMTIME time;
	int        CommStatus;	
	char       buf[80];
	char	   szName[8] = {0,};	

	//2014.12.01 Save History
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(nLeakCkSts == eNormalLeakCheck)			{strcpy(szName,"");WRITE_ANALOG(SysLeakChkRateAM,dbLeakAvg,&CommStatus);}
	else if(nLeakCkSts == eALDValveLeakCheck)	strcpy(szName,"(Vlv)"); //... 2016.01.19 ALD Valve Leak Check 
	else if(nLeakCkSts == eGasLineLeckCheck)							//... 2016.01.19 Gas Line Leak Check
	{
		if(nLineSeq == ePURGELINE)       {strcpy(szName,"(Pur)");WRITE_ANALOG(SysPurLkChkRateAM,dbLeakAvg,&CommStatus);}
		else if(nLineSeq == eCLEANLINE)  {strcpy(szName,"(Cln)");WRITE_ANALOG(SysClnLkChkRateAM,dbLeakAvg,&CommStatus);}
	}
	else if(nLeakCkSts == eOtherLeckCheck)		strcpy(szName,"(Etc)"); //... 2016.01.19 User Mode Leak Check

	GetLocalTime(&time);		
	sprintf(buf, "%d/%02d/%02d:%d:%02d%s", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, szName);

	S_LOG leakCheckLog;
	leakCheckLog.dbLastLeakRate = dbLeakRate;
	leakCheckLog.dbAvgLeakRate = dbLeakAvg;
	strcpy(leakCheckLog.szTime, buf);

	PushNewResult(leakCheckLog);
	SaveFile();
	UpdateIO();
}

