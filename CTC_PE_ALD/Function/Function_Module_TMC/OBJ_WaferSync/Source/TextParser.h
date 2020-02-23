// TextParser.h: interface for the CTextParser class.
// Version : 1.56
// Modi : 2004.12.14
// Date : 2002.12.07
// Programed by Changsu,Kim
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTPARSER_H__7C5DB991_08A8_11D7_B69A_009027E90DD2__INCLUDED_)
#define AFX_TEXTPARSER_H__7C5DB991_08A8_11D7_B69A_009027E90DD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTextParser  
{
public:
	CTextParser();
	CTextParser(const char* pText);
	virtual ~CTextParser();

// Member Function
	void SetText(const char* pText);
	bool MoveFirst();
	bool MoveLast();
	bool MoveNext();
	bool MoveBack();
	bool MoveTo(int nIndex);
	bool MoveNextWord();
	bool MoveBackWord();

	char GetChar();
	char PeekChar();

	// Default Token = SPACE, TAB, NULL, RETURN, CR, LF
	bool GetWord(char *pBuf, char chToken = ' ');
	bool GetWord(char chToken, char *pBuf, char chTok_B = ' ');
	bool GetString(char *pBuf, char chToken = '\n');
	bool GetString(char chToken, char *pBuf);
	bool GetLine(char *pBuf, char chToken = '\n');
	bool GetLine(char chToken, char *pBuf);
	bool GetInt(int *pValue, char chToken = '\n');
	bool GetInt(char chToken, int *pValue, char chTok_B = '\n');
	bool GetLong(long *pValue, char chToken = '\n');
	bool GetLong(char chToken, long *pValue, char chTok_B = '\n');
	bool GetFloat(float *pValue, char chToken = '\n');
	bool GetFloat(char chToken, float *pValue, char chTok_B = '\n');
	bool GetDbFloat(double *pValue, char chToken = '\n');
	bool GetDbFloat(char chToken, double *pValue, char chTok_B = '\n');
	bool GetHexa(unsigned int *pValue, char chToken = '\n');
	bool GetHexa(char chToken, unsigned int *pValue, char chTok_B = '\n');
	bool GetDate(int *pYear, int *pMonth, int *pDay, char chDiv = '.', char chToken = '\n');
	bool GetDate(char chToken, int *pYear, int *pMonth, int *pDay, char chDiv = '.', char chTok_B = '\n');

	// Yes/True/Enable => TRUE,   No/False/Disable => FALSE;
	bool GetBool(bool *pValue, char chToken = '\n');
	bool GetBool(char chToken, bool *pValue, char chTok_B = '\n');
	BOOL GetBOOL(BOOL *pValue, char chToken = '\n');
	BOOL GetBOOL(char chToken, BOOL *pValue, char chTok_B = '\n');

	// If Word is szTrue, return TRUE and If Word is szFalse, return FALSE;
	bool GetBool(bool *pValue, char *szTrue, char *szFalse, char chToken = '\n');
	bool GetBool(char chToken, bool *pValue, char *szTrue, char *szFalse, char chTok_B = '\n');
	BOOL GetBOOL(BOOL *pValue, char *szTrue, char *szFalse, char chToken = '\n');
	BOOL GetBOOL(char chToken, BOOL *pValue, char *szTrue, char *szFalse, char chTok_B = '\n');

	// Enum
	bool GetEnum(int *pValue, char* pszEnum[], char chToken = '\n');
	bool GetEnum(char chToken, int *pValue, char* pszEnum[], char chTok_B = '\n');

	// Sentence
	bool GetSentence(char *pBuf, char chToken = '\n');
	bool GetSentence(char chToken, char *pBuf, char chTok_B = '\n');

	bool Token(char chToken);
	bool SkipLine();
	bool IsEnd();

	unsigned int htoi(const char *szHexa);
	char* MakeUpper(char *szStr);
	char* MakeLower(char *szStr);
private:
	char* TrimLeft(char *pBuf);

// Member Variable
private:
	char* m_pText;
	char* m_pPosition;
public:
	int m_nSize;
};

#endif // !defined(AFX_TEXTPARSER_H__7C5DB991_08A8_11D7_B69A_009027E90DD2__INCLUDED_)
