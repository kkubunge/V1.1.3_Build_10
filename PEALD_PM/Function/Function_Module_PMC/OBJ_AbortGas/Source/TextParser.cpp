// TextParser.cpp: implementation of the CTextParser class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <string.h>

#include "TextParser.h"

#define ASCII_TAB		0x09
#define ASCII_ENTER		0x0d
#define ASCII_CR		0x0d
#define ASCII_LF		0x0a

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextParser::CTextParser()
{
	m_pText = NULL;
	m_pPosition = NULL;
	m_nSize = 0;
	m_szLastPickedWord[0] = 0;
}

CTextParser::CTextParser(const char *pText)
{
	m_nSize = strlen(pText);
	if(m_nSize > DEF_BUF_SIZE-1) m_pText = new char[m_nSize+1];
	else m_pText = m_pTextBuffer;
	strcpy(m_pText, pText);
	m_pPosition = m_pText;
}

CTextParser::~CTextParser()
{
	if(m_pText != NULL && m_pText != m_pTextBuffer) delete m_pText;
}

//////////////////////////////////////////////////////////////////////
// Member Function

void CTextParser::SetText(const char* pText)
{
	if(m_pText != NULL && m_pText != m_pTextBuffer) delete m_pText;
	m_nSize = strlen(pText);
	if(m_nSize > DEF_BUF_SIZE-1) m_pText = new char[m_nSize+1];
	else m_pText = m_pTextBuffer;
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
	char szBuf[DEF_BUF_SIZE] = "";
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
	char szBuf[DEF_BUF_SIZE] = "";
	int i = 0;
	char ch;

	if(pBuf != NULL) *pBuf = 0;
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
						&& ch != 0 && i<DEF_BUF_SIZE-1)
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
	if(pBuf != NULL) strcpy(pBuf, szBuf);
	strcpy(m_szLastPickedWord, szBuf);
	if(i > 0) bRet = true;
	return bRet;
}

bool CTextParser::GetWord(char chToken, char *pBuf, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetWord(pBuf, chTok_B);
	return bRet;
}

int CTextParser::GetWords(char ppBuf[][WORD_MAX_SIZE], int nPieces, bool bMakeUpper, char chToken)
{
	int i;
	for(i=0; i<nPieces; i++)
	{
		if(! GetWord(ppBuf[i], chToken)) break;
		else if(bMakeUpper) MakeUpper(ppBuf[i]);
	}
	return i;
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
	char szBuf[DEF_BUF_SIZE] = "";
	*pBuf = 0;

	int i;
	for(i=0; i<DEF_BUF_SIZE && PeekChar() != chToken
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
	char szBuf[DEF_BUF_SIZE] = "";
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
	char szBuf[DEF_BUF_SIZE] = "";
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
	char szBuf[DEF_BUF_SIZE] = "";
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
	char szBuf[DEF_BUF_SIZE] = "";
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
	char szBuf[DEF_BUF_SIZE] = "";
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
	char szBuf[DEF_BUF_SIZE] = "";
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
	char szBuf[DEF_BUF_SIZE] = "";
	if(GetWord(szBuf, chToken) == true) *pValue = BoolFromStr(szBuf, &bRet);
	return bRet;
}

BOOL CTextParser::GetBOOL(BOOL *pValue, char chToken)
{
	bool bRet = false;
	bool bValue = false;
	char szBuf[DEF_BUF_SIZE] = "";
	if(GetWord(szBuf, chToken) == true) bValue = BoolFromStr(szBuf, &bRet);
	if(bValue) *pValue = TRUE;
	else *pValue = FALSE;
	if(bRet) return TRUE;
	return FALSE;
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

bool CTextParser::GetBool(bool *pValue, const char *szTrue, const char *szFalse, char chToken)
{
	bool bRet = false;
	char szBuf[DEF_BUF_SIZE] = "";
	char szTrue_A[DEF_BUF_SIZE] = "";
	char szFalse_A[DEF_BUF_SIZE] = "";
	if(GetWord(szBuf, chToken) == true)
	{
		strcpy(szTrue_A, szTrue);
		strcpy(szFalse_A, szFalse);
		MakeLower(szBuf);
		MakeLower(szTrue_A);
		MakeLower(szFalse_A);
		bRet = true;

		if(strcmp(szBuf, szTrue_A) == 0) *pValue = true;
		else if(strcmp(szBuf, szFalse_A) == 0) *pValue = false;
		else bRet = false;
	}
	return bRet;
}

BOOL CTextParser::GetBOOL(BOOL *pValue, const char *szTrue, const char *szFalse, char chToken)
{
	BOOL bRet = FALSE;
	bool bValue = false;
	bool bRslt = false;
	bRslt = GetBool(&bValue, szTrue, szFalse, chToken);
	if(bRslt)
	{
		if(bValue) *pValue = TRUE;
		else *pValue = FALSE;
		bRet = TRUE;
	}
	return bRet;
}

bool CTextParser::GetBool(char chToken, bool *pValue, const char *szTrue, const char *szFalse, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetBool(pValue, szTrue, szFalse, chTok_B);
	return bRet;
}

BOOL CTextParser::GetBOOL(char chToken, BOOL *pValue, const char *szTrue, const char *szFalse, char chTok_B)
{
	BOOL bRet = FALSE;
	if(Token(chToken) == true) bRet = GetBOOL(pValue, szTrue, szFalse, chTok_B);
	return bRet;
}

bool CTextParser::GetEnum(int *pValue, char* pszEnum[], char chToken)
{
	bool bRet = false;
	char szBuf[DEF_BUF_SIZE];
	char szEnum[DEF_BUF_SIZE];
	if(GetWord(szBuf, chToken) == true)
	{
		bRet = true;
		MakeLower(szBuf);
		int i;
		for(i=0; pszEnum[i] != NULL; i++)
		{
			strcpy(szEnum, pszEnum[i]);
			MakeLower(szEnum);
			if(strcmp(szBuf, szEnum) == 0)
			{
				*pValue = i;
				break;
			}
		}
	}
	return bRet;
}

bool CTextParser::GetEnum(char chToken, int *pValue, char* pszEnum[], char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetEnum(pValue, pszEnum, chTok_B);
	return bRet;
}

bool CTextParser::GetSentence(char *pBuf, char chToken)
{
	bool bRet = false;
	char szWord[DEF_BUF_SIZE];
	char szTemp[DEF_BUF_SIZE];
	*pBuf = NULL;

	while(GetWord(szWord, chToken) == true)
	{
		sprintf(szTemp, " %s", szWord);
		strcat(pBuf, szTemp);
		bRet = true;

		MoveBack();
		if(GetChar() == chToken) break;
	}
	return bRet;
}

bool CTextParser::GetSentence(char chToken, char *pBuf, char chTok_B)
{
	bool bRet = false;
	if(Token(chToken) == true) bRet = GetSentence(pBuf, chTok_B);
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

	if(strncmp(szHexa, "0x", 2) == 0) j = 2;
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

bool CTextParser::BoolFromStr(const char *szStr, bool *pRslt)
{
	bool bRet = false, bRslt = true;
	char szBuf[DEF_BUF_SIZE];
	strcpy(szBuf, szStr);
	MakeLower(szBuf);
	if('y' == szBuf[0] || 't' == szBuf[0] || 'e' == szBuf[0]) bRet = true;
	else if('n' == szBuf[0] || 'f' == szBuf[0] || 'd' == szBuf[0]) bRet = false;
	else bRslt = false;
	if(pRslt != NULL) *pRslt = bRslt;
	return bRet;
}

bool CTextParser::BeEqual(const char *szStr_A, const char *szStr_B)
{
	char szBuf_A[DEF_BUF_SIZE];
	char szBuf_B[DEF_BUF_SIZE];
	strcpy(szBuf_A, szStr_A);
	strcpy(szBuf_B, szStr_B);
	MakeLower(szBuf_A);
	MakeLower(szBuf_B);
	if(strcmp(szBuf_A, szBuf_B) == 0) return true;
	return false;
}

bool CTextParser::BeEqual(const char *szStr_A, const char *szStr_B, int nLength)
{
	char szBuf_A[DEF_BUF_SIZE];
	char szBuf_B[DEF_BUF_SIZE];
	strcpy(szBuf_A, szStr_A);
	strcpy(szBuf_B, szStr_B);
	MakeLower(szBuf_A);
	MakeLower(szBuf_B);
	if(strncmp(szBuf_A, szBuf_B, nLength) == 0) return true;
	return false;
}

bool CTextParser::BeEqual(const char *szStr)
{
	char szBuf_A[DEF_BUF_SIZE];
	char szBuf_B[DEF_BUF_SIZE];
	strcpy(szBuf_A, szStr);
	strcpy(szBuf_B, m_szLastPickedWord);
	MakeLower(szBuf_A);
	MakeLower(szBuf_B);
	if(strcmp(szBuf_A, szBuf_B) == 0) return true;
	return false;
}

bool CTextParser::BeEqual(const char *szStr, int nLength)
{
	char szBuf_A[DEF_BUF_SIZE];
	char szBuf_B[DEF_BUF_SIZE];
	strcpy(szBuf_A, szStr);
	strcpy(szBuf_B, m_szLastPickedWord);
	MakeLower(szBuf_A);
	MakeLower(szBuf_B);
	if(strncmp(szBuf_A, szBuf_B, nLength) == 0) return true;
	return false;
}

//--------------------------------------------------------------------
// 2006.09.14
bool CTextParser::Comp(const char *szStr_A, const char *szStr_B)
{
	if(strcmp(szStr_A, szStr_B) == 0) return true;
	return false;
}

bool CTextParser::Comp(const char *szStr_A, const char *szStr_B, int nLength)
{
	if(strncmp(szStr_A, szStr_B, nLength) == 0) return true;
	return false;
}

bool CTextParser::Comp(const char *szStr)
{
	if(strcmp(szStr, m_szLastPickedWord) == 0) return true;
	return false;
}

bool CTextParser::Comp(const char *szStr, int nLength)
{
	if(strncmp(szStr, m_szLastPickedWord, nLength) == 0) return true;
	return false;
}

//--------------------------------------------------------------------
// 2008.12.03
int CTextParser::_GetEnumValue(const char *szKeyStr, ENUM_STRING stEnumStringList[])
{
	int nValue = -1;
	int i;

	do {
		if(stEnumStringList == NULL) break;
		i = 0;
		while(stEnumStringList[i].szString[0] != NULL)
		{
			if(BeEqual(szKeyStr, stEnumStringList[i].szString))
			{
				nValue = stEnumStringList[i].nEnum;
				break;
			}
			i++;
		}
	} while(0);
	return nValue;
}

int CTextParser::_GetEnumStrIdx(int nEnum, ENUM_STRING stEnumStringList[])
{
	int nValue = -1;
	int i;

	do {
		if(stEnumStringList == NULL) break;
		i = 0;
		while(stEnumStringList[i].szString[0] != NULL)
		{
			if(stEnumStringList[i].nEnum == nEnum)
			{
				nValue = i;
				break;
			}
			i++;
		}
	} while(0);
	return nValue;
}
const char* CTextParser::_GetEnumString(int nEnum, ENUM_STRING stEnumStringList[])
{
	char* szValue = "Unknown";
	int i;

	do {
		if(stEnumStringList == NULL) break;
		i = 0;
		while(stEnumStringList[i].szString[0] != NULL)
		{
			if(stEnumStringList[i].nEnum == nEnum)
			{
				szValue = stEnumStringList[i].szString;
				break;
			}
			i++;
		}
	} while(0);
	return szValue;
}