// ParamMonitor.cpp: implementation of the CParamMonitor class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning( disable: 4786 )

#include <string>
#include <vector>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//==========================================================================================

using namespace std;

#include "ParamItem.h"
#include "ParamMonitor.h"

#include "StopWatch.h"

extern CStopWatch g_Watch;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParamMonitor::CParamMonitor()
{
	m_nAlarmId = -1;
	m_nUpdateIoPoint = -1;
	m_nUpdateStatusIoPoint = -1;
}

CParamMonitor::~CParamMonitor()
{
	m_ParamItems.clear();
}

CParamMonitor* CParamMonitor::CreateInstance( const char* szFileName )
{	
	CParamMonitor *pParamMonitor = new CParamMonitor();
	char szBuffer[256];
	vector<string> words;
	FILE *fp = fopen(szFileName, "rt");	

	if( fp == NULL ) {
		printf("File Open Failed : %s\n", szFileName);
		delete pParamMonitor;
		return NULL;
	}	

	g_Watch.CheckStart("CParamMonitor::CreateInstance()");

	while( fgets(szBuffer, sizeof(szBuffer), fp) ) 
	{
		SplitString(szBuffer, words);
		ParsingConfigure(pParamMonitor, words);		
	}	

	fclose(fp);

	// check
	if( pParamMonitor->m_nAlarmId < 0 ||
		pParamMonitor->m_nUpdateIoPoint < 0 ||
		pParamMonitor->m_nUpdateStatusIoPoint < 0 ||
		pParamMonitor->m_ParamItems.size() < 1 ) 
	{
		printf("Config file Load Error, maybe File or IO Invalid.\n");
		delete pParamMonitor;
		pParamMonitor = NULL;
	}

	g_Watch.CheckEnd("CParamMonitor::CreateInstance()");


	return pParamMonitor;
}

void CParamMonitor::ParsingConfigure( CParamMonitor *pParamMonitor, vector<string> &words )
{
	char szBuffer[80];

	if( words.size() < 2 ) 
	{
		return;
	}
	else if( words[0] == "AlarmID" )
	{
		pParamMonitor->m_nAlarmId = atoi(words[1].data());
	}
	else if( words[0] == "UpdateIO")
	{
		strcpy(szBuffer, words[1].data());
		pParamMonitor->m_nUpdateIoPoint = _FIND_FROM_STRING(_K_D_IO, szBuffer);
	}
	else if( words[0] == "UpdateStatusIO" )
	{
		strcpy(szBuffer, words[1].data());
		pParamMonitor->m_nUpdateStatusIoPoint = _FIND_FROM_STRING(_K_D_IO, szBuffer);
	} 
	else if( words[0] == "PARAM" )
	{		
		if( words.size() < 4 ) {
			return;
		}

		CParamItem item;
		item.SetIOType(words[1]);
		item.SetVirtualIOName(words[2].data());
		item.SetRealIOName(words[3].data());
		pParamMonitor->m_ParamItems.push_back(item);

		// if debug mode. print config data.
		//printf("%s\n", item.ToString().data());
	}
}

void CParamMonitor::SplitString( string text, vector<string> &words )
{
	int i=0;
	char ch;
	string word;

	words.clear();
	while(ch=text[i++])
	{
		if (isspace(ch))
		{
			if (!word.empty())
			{
				words.push_back(word);
			}
			word = "";
		}
		else
		{
			word += ch;
		}
	}
	if (!word.empty())
	{
		words.push_back(word);
	}
}

void CParamMonitor::CompareParameters()
{
	g_Watch.CheckStart("CParamMonitor::CompareParameters()");

	for( int i=0; i<m_ParamItems.size(); i++ ) 
	{
		if( !m_ParamItems[i].Compare() ) 
		{
			string s = m_ParamItems[i].ToString();
			s += " Mismatch!";
			printf("%s\n", s.data());

			if( ALARM_STATUS(m_nAlarmId) != ALM_PAUSED )
			{
				ALARM_POST(m_nAlarmId);
			}
		}
	}

	g_Watch.CheckEnd("CParamMonitor::CompareParameters()");
}

bool CParamMonitor::UpdateParameters()
{
	int nIOStatus;
	int nRetryCount = 10;

	bool bReturn = false;

	enum { OFF, ON };
	enum { READY, UPDATE };

	g_Watch.CheckStart("CParamMonitor::UpdateParameters()");

	_dWRITE_DIGITAL(m_nUpdateIoPoint, ON, &nIOStatus);

	for( int i=0; i<nRetryCount; i++ ) 
	{
		if( _dREAD_DIGITAL(m_nUpdateStatusIoPoint, &nIOStatus) == READY ) 
		{
			bReturn = true;
		}

		if( !WAIT_SECONDS(1) ) 
		{
			bReturn = false;
		}
	}

	g_Watch.CheckEnd("CParamMonitor::UpdateParameters()");

	return bReturn;
}
