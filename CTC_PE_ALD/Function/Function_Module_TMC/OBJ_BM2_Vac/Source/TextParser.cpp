// TextParser.cpp: implementation of the CTextParser class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include "TextParser.h"

#define MAX_BUF		256
#define ASCII_TAB	0x09
#define ASCII_ENTER	0x0d
#define ASCII_CR	0x0d
#define ASCII_LF	0x0a

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextParser::CTextParser()
{
	m_pText = NULL;
	m_pPosition = NULL;
	m_nSize = 0;
}

CTextParser::CTextParser(const char *pText)
{
	m_nSize = strlen(pText);
	m_pText = new char[m_nSize+1];
	strcpy(m_pText, pText);
	m_pPosition = m_pText;
}

CTextParser::~CTextParser()
{
	if(m_pText != NULL) delete m_pText;
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CTextParser::SetText(const char* pText)
{
	if(m_pText != NULL) delete m_pText;
	m_nSize = strlen(pText);
	m_pText = new char[m_nSize+1];
	m_pPosition = m_pText;
	strcpy(m_pText, pText);
}

bool CTextParser::MoveFirst()
{
	bool bRet = false;
	if(m_pText != NULL) { m_pPosition = m_pText; bRet = true; }
	return bRet;
}

bool CTextParser::MoveLast()
{
	bool bRet = false;
	if(m_pText != NULL) { m_pPosition = m_pText + m_nSize; bRet = true; }
	return bRet;
}

bool CTextParser::MoveNext()
{
	bool bRet = false;
	if(m_pPosition-m_pText < m_nSize) { m_pPosition++; bRet = true; }
	return bRet;
}

bool CTextParser::MoveBack()
{
	bool bRet = false;
	if(m_pPosition > m_pText) { m_pPosition--; bRet = true; }
	return bRet;
}

bool CTextParser::MoveTo(int nIndex)
{
	bool bRet = false;
	if(nIndex <= m_nSize) { m_pPosition = m_pText + nIndex; bRet = true; }
	return bRet;
}

bool CTextParser::MoveNextWord()
{
	bool bRet = false;
	char szBuf[MAX_BUF] = "";
	if(! IsEnd()) {	GetWord(szBuf);	bRet = true; }
	return bRet;
}

bool CTextParser::MoveBackWord()
{
	bool bRet = false;
	return bRet;
}

char CTextParser::PeekChar()
{
	if(m_nSize > 0) return *m_pPosition;
	return 0;
}

char CTextParser::GetChar()
{
	char chValue = 0;
	if(! IsEnd())
	{
		chValue = *m_pPosition;
		MoveNext();
		return chValue;
	}
	return 0;
}

bool CTextParser::GetWord(char *pBuf, char chToken)
{
	bool bRet = false;
	char szBuf[MAX_BUF] = "";
	int i = 0;
	char ch;
	*pBuf = 0;

	do {
		if(IsEnd()) break;
		do {
			ch = GetChar();
		} while(' ' == ch || '\r' == ch || '\n' == ch	|| ASCII_TAB == ch);
		if(ch != chToken && ch != 0)
		{
			szBuf[i] = ch;
			i++;
			ch = GetChar();
			while(ch != ' ' && ch != '\r' && ch != '\n'&& ch != ASCII_TAB && ch != chToken
						&& ch != 0 && i<MAX_BUF-1)
			{
				szBuf[i] = ch;
				i++;
				ch = GetChar();
			}
		}

		while(' ' == szBuf[0] || '\r' == szBuf[0]
							|| '\n' == szBuf[0]	|| ASCII_TAB == szBuf[0])
		{
			ch = GetChar();
		}
		if(ch != chToken && ch != 0) MoveBack();
	} while(0);

	szBuf[i] = 0;
	strcpy(pBuf, szBuf);
	if(i > 0) bRet = true;
	return bRet;
}

bool CTextParser::GetWord(char chToken, char *pBuf, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetWord(pBuf, chTok_B);
	return bRet;
}

bool CTextParser::GetString(char chToken, char *pBuf)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetString(pBuf);
	return bRet;
}

bool CTextParser::GetString(char *pBuf, char chToken)
{
	bool bRet = true;
	char szBuf[MAX_BUF] = "";
	*pBuf = 0;

	int i; for(i=0; i<MAX_BUF && PeekChar() != chToken
		&& PeekChar() != '\n' && (! IsEnd()); i++)
	{
		szBuf[i] = GetChar();
		if('\r' == szBuf[i] && '\n' == PeekChar())
		{
			MoveBack();
			szBuf[i] = NULL;
			break;
		}
	}
	strcpy(pBuf, TrimLeft(szBuf));
	return bRet;
}

bool CTextParser::GetLine(char chToken, char *pBuf)
{
	bool bRet = false;
	if(Token(chToken) == true)
	{
		bRet = GetString(pBuf);
		SkipLine();
	}
	return bRet;
}

bool CTextParser::GetLine(char *pBuf, char chToken)
{
	bool bRet = false;
	bRet = GetString(pBuf, chToken);
	Token(chToken);
	return bRet;
}

bool CTextParser::GetInt(int *pValue, char chToken)
{
	bool bRet = false;
	char szBuf[MAX_BUF] = "";
	if(GetWord(szBuf, chToken) == true)
	{
		*pValue = (int) atof(szBuf);
		bRet = true;
	}
	return bRet;
}

bool CTextParser::GetInt(char chToken, int *pValue, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetInt(pValue, chTok_B);
	return bRet;
}

bool CTextParser::GetLong(long *pValue, char chToken)
{
	bool bRet = false;
	char szBuf[MAX_BUF] = "";
	if(GetWord(szBuf, chToken) == true)
	{
		*pValue = (long) atof(szBuf);
		bRet = true;
	}
	return bRet;
}

bool CTextParser::GetLong(char chToken, long *pValue, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetLong(pValue, chTok_B);
	return bRet;
}

bool CTextParser::GetFloat(float *pValue, char chToken)
{
	bool bRet = false;
	char szBuf[MAX_BUF] = "";
	if(GetWord(szBuf, chToken) == true)
	{
		*pValue = (float) atof(szBuf);
		bRet = true;
	}
	return bRet;
}

bool CTextParser::GetFloat(char chToken, float *pValue, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetFloat(pValue, chTok_B);
	return bRet;
}

bool CTextParser::GetDbFloat(double *pValue, char chToken)
{
	bool bRet = false;
	char szBuf[MAX_BUF] = "";
	if(GetWord(szBuf, chToken) == true)
	{
		*pValue = atof(szBuf);
		bRet = true;
	}
	return bRet;
}

bool CTextParser::GetDbFloat(char chToken, double *pValue, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetDbFloat(pValue, chTok_B);
	return bRet;
}

bool CTextParser::GetHexa(unsigned int *pValue, char chToken)
{
	bool bRet = false;
	char szBuf[MAX_BUF] = "";
	if(GetWord(szBuf, chToken) == true)
	{
		*pValue = htoi(szBuf);
		bRet = true;
	}
	return bRet;
}

bool CTextParser::GetHexa(char chToken, unsigned int *pValue, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetHexa(pValue, chTok_B);
	return bRet;
}

bool CTextParser::GetDate(int *pYear, int *pMonth, int *pDay, char chDiv, char chToken)
{
	bool bRet = false;
	char szBuf[MAX_BUF] = "";
	do
	{
		if(GetWord(szBuf, chDiv) == false) break;
		*pYear = (int) atof(szBuf);
		if(GetWord(szBuf, chDiv) == false) break;
		*pMonth = (int) atof(szBuf);
		if(GetWord(szBuf, chToken) == false) break;
		*pDay = (int) atof(szBuf);
		bRet = true;
	} while(0);
	return bRet;
}

bool CTextParser::GetDate(char chToken, int *pYear, int *pMonth, int *pDay, char chDiv, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetDate(pYear, pMonth, pDay, chDiv, chTok_B);
	return bRet;
}

bool CTextParser::GetBool(bool *pValue, char chToken)
{
	bool bRet = false;
	char szBuf[MAX_BUF] = "";
	if(GetWord(szBuf, chToken) == true)
	{
		MakeLower(szBuf);
		bRet = true;
		if('y' == szBuf[0] || 't' == szBuf[0] || 'e' == szBuf[0] ) *pValue = true;
		else if('n' == szBuf[0] || 'f' == szBuf[0] || 'd' == szBuf[0]) *pValue = false;
		else bRet = false;
	}
	return bRet;
}

BOOL CTextParser::GetBOOL(BOOL *pValue, char chToken)
{
	BOOL bRet = FALSE;
	char szBuf[MAX_BUF] = "";
	if(GetWord(szBuf, chToken) == true)
	{
		MakeLower(szBuf);
		bRet = TRUE;
		if('y' == szBuf[0] || 't' == szBuf[0] || 'e' == szBuf[0]) *pValue = TRUE;
		else if('n' == szBuf[0] || 'f' == szBuf[0] || 'd' == szBuf[0]) *pValue = FALSE;
		else bRet = FALSE;
	}
	return bRet;
}


bool CTextParser::GetBool(char chToken, bool *pValue, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetBool(pValue, chTok_B);
	return bRet;
}

BOOL CTextParser::GetBOOL(char chToken, BOOL *pValue, char chTok_B)
{
	BOOL bRet = FALSE;
	if(Token(chToken) == true) bRet = GetBOOL(pValue, chTok_B);
	return bRet;
}

bool CTextParser::Token(char chToken)
{
	bool bRet = false;
	while(PeekChar() != chToken && (! IsEnd())) MoveNext();
	if(! IsEnd()) { MoveNext(); bRet = true; }
	return bRet;
}

bool CTextParser::SkipLine()
{
	bool bRet = true;
	do {
		Token('\r');
	} while(PeekChar() != '\n' && (! IsEnd()));
	return bRet;
}

bool CTextParser::IsEnd()
{
	if(m_pPosition-m_pText < m_nSize) return false;
	return true;
}

char* CTextParser::TrimLeft(char *pBuf)
{
	char* pTrim = pBuf;
	while(' ' == *pTrim || ASCII_TAB == *pTrim) pTrim++;
	return pTrim;
}

unsigned int CTextParser::htoi(const char *szHexa)
{
	int i;
	int j=0;
	bool bEnd = false;
	char cha;
	unsigned int h = 0;

	while(bEnd == false)
	{
		cha = szHexa[j];
		if( cha>='0' && cha<='9' ) i = cha - '0';
		else if( cha>='a' && cha<='f' ) i = 10+(cha -'a');
		else if( cha>='A' && cha<='F' ) i = 10+(cha -'A');
		else bEnd = true;

		if(bEnd == false) h = (h<<4) | i;
		j++;
	}
	return(h);
}

char* CTextParser::MakeUpper(char *szStr)
{
	char ch;
	for(int i=0; *(szStr+i) != NULL; i++)
	{
		ch = *(szStr+i);
		if(ch >= 'a' && ch <= 'z') *(szStr+i) = ch - 'a' + 'A';
	}
	return szStr;
}

char* CTextParser::MakeLower(char *szStr)
{
	char ch;
	for(int i=0; *(szStr+i) != NULL; i++)
	{
		ch = *(szStr+i);
		if(ch >= 'A' && ch <= 'Z') *(szStr+i) = ch - 'A' + 'a';
	}
	return szStr;
}
