// PersistIO.cpp: implementation of the CPersistIO class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>

//====================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//====================================================================

#include "TextParser.h"
#include "PersistIO.h"

//--------------------------------------------------------------------
#ifndef _DEBUG
// #define _DEBUG
#endif

//--------------------------------------------------------------------
#define DEFAULT_PERSIST_IO_CONFIG	".\\Parameter\\PersistIOList.cfg"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPersistIO::CPersistIO()
{
	int i;
	for(i=0; i<MAX_IO_LIST; i++)
	{
		m_pIOList[i].szIOName[0] = 0;
		m_pIOList[i].nIOPoint = -1;
		m_abValueInit[i] = FALSE;
	}
}

CPersistIO::~CPersistIO()
{

}

//////////////////////////////////////////////////////////////////////
// Virtual Function for Thread

void CPersistIO::DoMonitor()
{
	bool bUpdated = true;
	BOOL bIOStatus;
	int i, nErrCount;
	char szValue[100];   //2013.08.23

	while(WaitSec(1))
	{
		for(i=0; i<MAX_IO_LIST && m_pIOList[i].szIOName[0]!=0; i++)
		{
			// 2006.12.06
			nErrCount = 0;
			bIOStatus = FALSE;
			do {
				switch(m_pIOList[i].nIOType)
				{
				case _K_D_IO: sprintf(szValue, "%d", dREAD_DIGITAL(i, &bIOStatus)); break;
				case _K_A_IO: sprintf(szValue, "%.6f", dREAD_ANALOG(i, &bIOStatus)); break;
				case _K_S_IO: dREAD_STRING(i, szValue, &bIOStatus); break;
				}
				if(bIOStatus) break;
				if(! WaitSec(1)) break;
			} while(++nErrCount < 5);

			// 2006.12.06
			// 2009.11.03
			if(bIOStatus && strcmp(szValue, m_pIOValue[i]) != 0 && FALSE == m_abValueInit[i])
			{
				strcpy(m_pIOValue[i], szValue);
				bUpdated = false;
			}

			// 2006.12.20
			if(m_nIOCount < 10) { if(! WaitSec(3)) break; }
			else  { if(! WaitSec(0.3)) break; }
		}
		if(IsThreadStopping()) break;

		if(! bUpdated)
		{
			WriteIOValueToFile();
			bUpdated = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Member Function

bool CPersistIO::Initialize(char *szConfigPath)
{
	bool bRet = false;
	do {
		if(szConfigPath != NULL) strcpy(m_szConfigPath, szConfigPath);
		else strcpy(m_szConfigPath, DEFAULT_PERSIST_IO_CONFIG);
		if(! ReadIOList()) break;
		InitIOList(m_pIOList);		// 2006.03.21
		UpdateIOList();				// 2006.03.21
		CreateThread();
		bRet = true;
	} while(0);
	if(! bRet) printf("-----> Initialization failed[%s] in PersistIO\n", szConfigPath);
	return bRet;
}

void CPersistIO::Deinitialize()
{
	DestroyThread();
	SaveIOList();
}

bool CPersistIO::ReadIOList()
{
	bool bRet = false;
	FILE *fp;
	char szRead[256];
	CTextParser tp;
	char szChName[40];
	char szType[8];
	char szValue[100];     //2013.08.23
	int nIOCount = 0;
	char szComment[80];

	do {
		ClearComment();
		fp = fopen(m_szConfigPath, "rt");
		if(fp == NULL) break;
		do {
			if(NULL == fgets(szRead, 255, fp)) break;
			if(strlen(szRead) >= MAX_CMT_CHAR) szRead[MAX_CMT_CHAR-1] = 0;
			tp.SetText(szRead);
			
			if(szRead[0] == '#' || szRead[0] == ';')
			{
				tp.GetString(szComment);
				SetComment(nIOCount, szComment);
				continue;
			}
			if(! tp.GetWord(szChName)) continue;
			if(! tp.GetWord(szType)) continue;

			// 2009.11.03
			if(tp.BeEqual(szType, "INIT"))
			{
				m_abValueInit[nIOCount] = TRUE;
				if(! tp.GetWord(szType)) continue;
			}
			// 2013.08.23
			if(tp.BeEqual(szType, "S")){
				if(! tp.GetString(szValue)) szValue[0] = 0;
			}else{
				if(! tp.GetWord(szValue)) szValue[0] = 0;
			}
            //
			strcpy(m_pIOList[nIOCount].szIOName, szChName);
			m_pIOList[nIOCount].nIOType = GetIOType(szType);
			strcpy(m_pIOValue[nIOCount], szValue);

			nIOCount++;
			if(nIOCount >= MAX_IO_LIST) break;
		} while(feof(fp) == 0);
		bRet = TRUE;
	} while(0);
	if(fp != NULL) fclose(fp);
	return bRet;
}

bool CPersistIO::UpdateIOList()
{
	BOOL bIOStatus;
	int i;
	bool bRet = true;

	for(i=0; i<MAX_IO_LIST && m_pIOList[i].szIOName[0]!=0; i++)
	{
		if(m_pIOValue[i][0] != 0) switch(m_pIOList[i].nIOType)
		{
		case _K_D_IO: dWRITE_DIGITAL(i, atoi(m_pIOValue[i]), &bIOStatus); break;
		case _K_A_IO: dWRITE_ANALOG(i, atof(m_pIOValue[i]), &bIOStatus); break;
		case _K_S_IO: dWRITE_STRING(i, m_pIOValue[i], &bIOStatus); break;
		}
		if(! bIOStatus) bRet = false;
	}
	return bRet;
}

bool CPersistIO::SaveIOList()
{
	// ReadValueFromIO();			// 2006.12.06
	return WriteIOValueToFile();
}

bool CPersistIO::WriteIOValueToFile()
{
	FILE *fp;
	int i;
	bool bRet = false;
	char szTemp[256];
	char *pComment;
	char szInit[40];

	do {
		InitComment();
		fp = fopen(m_szConfigPath, "wt");
		if(fp == NULL) break;

		while(NULL != (pComment = GetComment(0)))
		{
			sprintf(szTemp, "%s\n", pComment);
			fputs(szTemp, fp);
		}
		for(i=0; i<MAX_IO_LIST && m_pIOList[i].szIOName[0]!=0; i++)
		{
			while(NULL != (pComment = GetComment(i)))
			{
				sprintf(szTemp, "%s\n", pComment);
				fputs(szTemp, fp);
			}

			// 2009.11.03
			if(m_abValueInit[i]) strcpy(szInit, "INIT");
			else strcpy(szInit, " ");

			switch(m_pIOList[i].nIOType)
			{
			case _K_D_IO: sprintf(szTemp, "%s	%s	D	%s\n", m_pIOList[i].szIOName, szInit, m_pIOValue[i]); break;
			case _K_A_IO: sprintf(szTemp, "%s	%s	A	%s\n", m_pIOList[i].szIOName, szInit, m_pIOValue[i]); break;
			case _K_S_IO: sprintf(szTemp, "%s	%s	S	%s\n", m_pIOList[i].szIOName, szInit, m_pIOValue[i]); break;
			}
			fputs(szTemp, fp);
		}
		bRet = true;
	} while(0);
	if(fp != NULL) fclose(fp);
	if(! bRet) printf("-----> SaveIOList Failed in PersistIO\n");
	else
	{
		#ifdef _DEBUG
		printf("-----> PersistIO Value Is Saved\n");
		#endif
	}
	return bRet;
}

bool CPersistIO::ReadValueFromIO()
{
	bool bRet = false;
	BOOL bIOStatus;
	int i;
	do {	
		for(i=0; i<MAX_IO_LIST && m_pIOList[i].szIOName[0]!=0; i++)
		{
			switch(m_pIOList[i].nIOType)
			{
			case _K_D_IO: sprintf(m_pIOValue[i], "%d", dREAD_DIGITAL(i, &bIOStatus)); break;
			case _K_A_IO: sprintf(m_pIOValue[i], "%.6f", dREAD_ANALOG(i, &bIOStatus)); break;
			case _K_S_IO: dREAD_STRING(i, m_pIOValue[i], &bIOStatus); break;
			}
		}
		bRet = true;
	} while(0);
	return bRet;
}

int CPersistIO::GetIOType(char *szType)
{
	switch(szType[0])
	{
	case 'D':
	case 'd': return _K_D_IO;
	case 'A':
	case 'a': return _K_A_IO;
	case 'S':
	case 's': return _K_S_IO;
	}
	return 0;
}

char CPersistIO::GetIOTypeChar(int nType)
{
	switch(nType)
	{
	case _K_D_IO: return 'D';
	case _K_A_IO: return 'A';
	case _K_S_IO: return 'S';
	}
	return 'E';
}

void CPersistIO::ClearComment()
{
	int i;
	for(i=0; i<MAX_COMMENT; i++)
	{
		m_aCmtList[i].nIndex = -1;
		m_aCmtList[i].szComment[0] = 0;
	}
	m_nCommentCount = 0;
}


void CPersistIO::InitComment()
{
	m_nCommentCount = 0;
}

void CPersistIO::SetComment(int nIndex, char *szComment)
{
	if(m_nCommentCount >= MAX_COMMENT) return;
	m_aCmtList[m_nCommentCount].nIndex = nIndex;
	strcpy(m_aCmtList[m_nCommentCount].szComment, szComment);
	m_nCommentCount++;
}

char* CPersistIO::GetComment(int nIndex)
{
	char *pComment = NULL;
	if(m_nCommentCount >= MAX_COMMENT
		|| m_aCmtList[m_nCommentCount].nIndex == -1
		|| m_aCmtList[m_nCommentCount].nIndex > nIndex) return NULL;
	pComment = m_aCmtList[m_nCommentCount].szComment;
	m_nCommentCount++;
	return pComment;
}
