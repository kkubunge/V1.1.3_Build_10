// TextParser.cpp: implementation of the CTextParser class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "TextParser_C.h"

#define MAX_BUF		256
#define ASCII_TAB	0x09
#define ASCII_ENTER	0x0d
#define ASCII_CR	0x0d
#define ASCII_LF	0x0a

//////////////////////////////////////////////////////////////////////
// Global Variable

const char* g_pText = 0;
const char* g_pPosition = 0;
int g_nTextSize = 0;

//////////////////////////////////////////////////////////////////////
// Local Function

char* tpTrimLeft(char *pBuf)
{
	char* pTrim = pBuf;
	while(' ' == *pTrim || ASCII_TAB == *pTrim) pTrim++;
	return pTrim;
}

//////////////////////////////////////////////////////////////////////
// External Function

void tpSetText(const char* pText)
{
	g_nTextSize = strlen(pText);
	g_pText = pText;
	g_pPosition = g_pText;
}

BOOL tpMoveFirst()
{
	BOOL bRet = FALSE;
	if(g_pText != 0) { g_pPosition = g_pText; bRet = TRUE; }
	return bRet;
}

BOOL tpMoveLast()
{
	BOOL bRet = FALSE;
	if(g_pText != 0) { g_pPosition = g_pText + g_nTextSize; bRet = TRUE; }
	return bRet;
}

BOOL tpMoveNext()
{
	BOOL bRet = FALSE;
	if(g_pPosition - g_pText < g_nTextSize) { g_pPosition++; bRet = TRUE; }
	return bRet;
}

BOOL tpMoveBack()
{
	BOOL bRet = FALSE;
	if(g_pPosition > g_pText) { g_pPosition--; bRet = TRUE; }
	return bRet;
}

BOOL tpMoveTo(int nIndex)
{
	BOOL bRet = FALSE;
	if(nIndex <= g_nTextSize) { g_pPosition = g_pText + nIndex; bRet = TRUE; }
	return bRet;
}

BOOL tpMoveNextWord()
{
	BOOL bRet = FALSE;
	char szBuf[MAX_BUF] = "";
	if(! tpIsEnd()) {	tpGetWord(szBuf, 0); bRet = TRUE; }
	return bRet;
}

BOOL tpMoveBackWord()
{
	BOOL bRet = FALSE;
	return bRet;
}

char tpPeekChar()
{
	if(g_nTextSize > 0) return *g_pPosition;
	return 0;
}

char tpGetChar()
{
	char chValue = 0;
	if(! tpIsEnd())
	{
		chValue = *g_pPosition;
		tpMoveNext();
		return chValue;
	}
	return 0;
}

BOOL tpGetWord(char *pBuf, char chToken)
{
	BOOL bRet = FALSE;
	char szBuf[MAX_BUF] = "";
	int i = 0;
	char ch;
	*pBuf = 0;

	do {
		if(tpIsEnd()) break;
		do {
			ch = tpGetChar();
		} while(' ' == ch || '\r' == ch || '\n' == ch	|| ASCII_TAB == ch);
		if(ch != chToken && ch != 0)
		{
			szBuf[i] = ch;
			i++;
			ch = tpGetChar();
			while(ch != ' ' && ch != '\r' && ch != '\n'&& ch != ASCII_TAB && ch != chToken
						&& ch != 0 && i<MAX_BUF-1)
			{
				szBuf[i] = ch;
				i++;
				ch = tpGetChar();
			}
		}

		while(' ' == szBuf[0] || '\r' == szBuf[0]
							|| '\n' == szBuf[0]	|| ASCII_TAB == szBuf[0])
		{
			ch = tpGetChar();
		}
		if(ch != chToken && ch != 0) tpMoveBack();
	} while(0);

	szBuf[i] = 0;
	strcpy(pBuf, szBuf);
	if(i > 0) bRet = TRUE;
	return bRet;
}

BOOL tpGetWordFromTk(char chToken, char *pBuf, char chTok_B)
{
	BOOL bRet = FALSE;
	if(tpToken(chToken) == TRUE) bRet = tpGetWord(pBuf, chTok_B);
	return bRet;
}

BOOL tpGetString(char *pBuf, char chToken)
{
	BOOL bRet = TRUE;
	char szBuf[MAX_BUF] = "";
	int i; 
	*pBuf = 0;

	for(i=0; i<MAX_BUF && tpPeekChar() != chToken
		&& tpPeekChar() != '\n' && (! tpIsEnd()); i++)
	{
		szBuf[i] = tpGetChar();
		if('\r' == szBuf[i] && '\n' == tpPeekChar())
		{
			tpMoveBack();
			szBuf[i] = 0;
			break;
		}
	}
	strcpy(pBuf, tpTrimLeft(szBuf));
	return bRet;
}

BOOL tpGetStringFromTk(char chToken, char *pBuf)
{
	BOOL bRet = FALSE;
	if(tpToken(chToken) == TRUE) bRet = tpGetString(pBuf, 0);
	return bRet;
}

BOOL tpGetLine(char *pBuf, char chToken)
{
	BOOL bRet = FALSE;
	bRet = tpGetString(pBuf, chToken);
	tpToken(chToken);
	return bRet;
}

BOOL tpGetLineFromTk(char chToken, char *pBuf)
{
	BOOL bRet = FALSE;
	if(tpToken(chToken) == TRUE)
	{
		bRet = tpGetString(pBuf, 0);
		tpSkipLine();
	}
	return bRet;
}

BOOL tpGetInt(int *pValue, char chToken)
{
	BOOL bRet = FALSE;
	char szBuf[MAX_BUF] = "";
	if(tpGetWord(szBuf, chToken) == TRUE)
	{
		*pValue = (int) atof(szBuf);
		bRet = TRUE;
	}
	return bRet;
}

BOOL tpGetIntFromTk(char chToken, int *pValue, char chTok_B)
{
	BOOL bRet = FALSE;
	if(tpToken(chToken) == TRUE) bRet = tpGetInt(pValue, chTok_B);
	return bRet;
}

BOOL tpGetLong(long *pValue, char chToken)
{
	BOOL bRet = FALSE;
	char szBuf[MAX_BUF] = "";
	if(tpGetWord(szBuf, chToken) == TRUE)
	{
		*pValue = (long) atof(szBuf);
		bRet = TRUE;
	}
	return bRet;
}

BOOL tpGetLongFromTk(char chToken, long *pValue, char chTok_B)
{
	BOOL bRet = FALSE;
	if(tpToken(chToken) == TRUE) bRet = tpGetLong(pValue, chTok_B);
	return bRet;
}

BOOL tpGetFloat(float *pValue, char chToken)
{
	BOOL bRet = FALSE;
	char szBuf[MAX_BUF] = "";
	if(tpGetWord(szBuf, chToken) == TRUE)
	{
		*pValue = (float) atof(szBuf);
		bRet = TRUE;
	}
	return bRet;
}

BOOL tpGetFloatFromTk(char chToken, float *pValue, char chTok_B)
{
	BOOL bRet = FALSE;
	if(tpToken(chToken) == TRUE) bRet = tpGetFloat(pValue, chTok_B);
	return bRet;
}

BOOL tpGetHexa(unsigned int *pValue, char chToken)
{
	BOOL bRet = FALSE;
	char szBuf[MAX_BUF] = "";
	if(tpGetWord(szBuf, chToken) == TRUE)
	{
		*pValue = tpHtoi(szBuf);
		bRet = TRUE;
	}
	return bRet;
}

BOOL tpGetHexaFromTk(char chToken, unsigned int *pValue, char chTok_B)
{
	BOOL bRet = FALSE;
	if(tpToken(chToken) == TRUE) bRet = tpGetHexa(pValue, chTok_B);
	return bRet;
}

BOOL tpGetDate(int *pYear, int *pMonth, int *pDay, char chDiv, char chToken)
{
	BOOL bRet = FALSE;
	char szBuf[MAX_BUF] = "";
	do
	{
		if(tpGetWord(szBuf, chDiv) == FALSE) break;
		*pYear = (int) atof(szBuf);
		if(tpGetWord(szBuf, chDiv) == FALSE) break;
		*pMonth = (int) atof(szBuf);
		if(tpGetWord(szBuf, chToken) == FALSE) break;
		*pDay = (int) atof(szBuf);
		bRet = TRUE;
	} while(0);
	return bRet;
}

BOOL tpGetDateFromTk(char chToken, int *pYear, int *pMonth, int *pDay, char chDiv, char chTok_B)
{
	BOOL bRet = FALSE;
	if(tpToken(chToken) == TRUE) bRet = tpGetDate(pYear, pMonth, pDay, chDiv, chTok_B);
	return bRet;
}

BOOL tpGetBool(BOOL *pValue, char chToken)
{
	BOOL bRet = FALSE;
	char szBuf[MAX_BUF] = "";
	if(tpGetWord(szBuf, chToken) == TRUE)
	{
		bRet = TRUE;
		if('Y' == szBuf[0] || 'y' == szBuf[0] || 'T' == szBuf[0] || 't' == szBuf[0])
			*pValue = TRUE;
		else if('N' == szBuf[0] || 'n' == szBuf[0] || 'F' == szBuf[0] || 'f' == szBuf[0])
			*pValue = FALSE;
		else bRet = FALSE;
	}
	return bRet;
}

BOOL tpGetBoolFromTk(char chToken, BOOL *pValue, char chTok_B)
{
	BOOL bRet = FALSE;
	if(tpToken(chToken) == TRUE) bRet = tpGetBool(pValue, chTok_B);
	return bRet;
}

BOOL tpToken(char chToken)
{
	BOOL bRet = FALSE;
	while(tpPeekChar() != chToken && (! tpIsEnd())) tpMoveNext();
	if(! tpIsEnd()) { tpMoveNext(); bRet = TRUE; }
	return bRet;
}

BOOL tpSkipLine()
{
	BOOL bRet = TRUE;
	do {
		tpToken('\r');
	} while(tpPeekChar() != '\n' && (! tpIsEnd()));
	return bRet;
}

BOOL tpIsEnd()
{
	if(g_pPosition - g_pText < g_nTextSize) return FALSE;
	return TRUE;
}

unsigned int tpHtoi(const char *szHexa)
{
	int i;
	int j=0;
	BOOL bEnd = FALSE;
	char cha;
	unsigned int h = 0;

	while(bEnd == FALSE)
	{
		cha = szHexa[j];
		if( cha>='0' && cha<='9' ) i = cha - '0';
		else if( cha>='a' && cha<='f' ) i = 10+(cha -'a');
		else if( cha>='A' && cha<='F' ) i = 10+(cha -'A');
		else bEnd = TRUE;

		if(bEnd == FALSE) h = (h<<4) | i;
		j++;
	}
	return(h);
}

char* tpUpperStr(char *szStr)
{
	char ch;
	int i;
	for(i=0; *(szStr+i) != 0; i++)
	{
		ch = *(szStr+i);
		if(ch >= 'a' && ch <= 'z') *(szStr+i) = ch - 'a' + 'A';
	}
	return szStr;
}