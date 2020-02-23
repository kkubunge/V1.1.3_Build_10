// TextParser.h: interface for the CTextParser class.
// Create : 2002.12.07
// Last Update : 2008.12.17
// Programed by Changsu,Kim
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTPARSER_H__7C5DB991_08A8_11D7_B69A_009027E90DD2__INCLUDED_)
#define AFX_TEXTPARSER_H__7C5DB991_08A8_11D7_B69A_009027E90DD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _ENUM_STRING
{
	char szString[40];
	int nEnum;
} ENUM_STRING;

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
	bool GetWord(char *pBuf = NULL, char chToken = ' ');
	bool GetWord(char chToken, char *pBuf = NULL, char chTok_B = ' ');
	int GetWords(char ppBuf[][80], int nPieces, bool bMakeUpper = false, char chToken = ' ');
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
	bool GetBool(bool *pValue, const char *szTrue, const char *szFalse, char chToken = '\n');
	bool GetBool(char chToken, bool *pValue, const char *szTrue, const char *szFalse, char chTok_B = '\n');
	BOOL GetBOOL(BOOL *pValue, const char *szTrue, const char *szFalse, char chToken = '\n');
	BOOL GetBOOL(char chToken, BOOL *pValue, const char *szTrue, const char *szFalse, char chTok_B = '\n');

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
	bool BoolFromStr(const char *szStr, bool *pRslt = NULL);
	bool BeEqual(const char *szStr_A, const char *szStr_B);
	bool BeEqual(const char *szStr_A, const char *szStr_B, int nLength);
	bool BeEqual(const char *szStr);
	bool BeEqual(const char *szStr, int nLength);

	// 2006.09.14
	bool Comp(const char *szStr_A, const char *szStr_B);
	bool Comp(const char *szStr_A, const char *szStr_B, int nLength);
	bool Comp(const char *szStr);
	bool Comp(const char *szStr, int nLength);

	// 2008.12.03
	int _GetEnumValue(const char *szKeyStr, ENUM_STRING stEnumStringList[]);
	int _GetEnumStrIdx(int nEnum, ENUM_STRING stEnumStringList[]);
	const char *_GetEnumString(int nEnum, ENUM_STRING stEnumStringList[]);

	// 2008.12.17
	bool IsNumeric(const char *szstr);

private:
	char* TrimLeft(char *pBuf);

// Member Variable
public:
	int m_nSize;
	char m_szLastPickedWord[80];
private:
	#define DEFAULT_BUFFER_SIZE		256
	char* m_pText;
	char* m_pPosition;
	char m_pTextBuffer[DEFAULT_BUFFER_SIZE];		// 2006.06.08
};

#endif // !defined(AFX_TEXTPARSER_H__7C5DB991_08A8_11D7_B69A_009027E90DD2__INCLUDED_)

//////////////////////////////////////////////////////////////////////
/* Update History
2002.12.07	Created
2004.12.14	기존 bool Type 외에 BOOL(대문자) Type Reading Function 추가
2005.10.12	Update History 기록 시작
			대문자 소문자 구분없이 비교하는 Function 추가
			: BeEqual(...)
2005.12.07	지정된 글자수만 같은지 비교하는 BeEqual(..., int nLength) Function 추가.
			읽은 Word를 Class 자체에서 저장하여 활용할 수 있는 기능 추가
				BeEqual(const char *szStr);
				char szLastPickedWord[80];
2006.06.08	m_pTextBuffer 변수 추가
			Text 길이가 255 자 보다 크면 Buffer 메모리를 할당하여 사용하고
			255자 보다 작을 경우는 m_pTextBuffer 를 사용하도록 수정
2006.09.14	대소문자를 구분하여 비교하는 Function 추가
			: Comp(...)
2008.12.03	Enum String을 처리하는 기능 추가
			Function Name : GetEnumValue
			ENUM_STRING 배열을 구성하여 해당 String에 값을 얻는다.
			ENUM_STRING 배열의 마지막은 NULL String이어야 한다.
				ex)
				ENUM_STRING SampleEnum[4] =
				{
					"TYPE1", EN_TYPE1,
					"TYPE2", EN_TYPE2,
					"TYPE3", EN_TYPE3,
					""
				};
				nValue = GetEnumValue("TYPE2", SampleEnum);
				Assert(nValue == ENUM_TYPE2);
2008.12.17	IsNumeric(const char *szStr) Func 추가
			단어가 숫자인지 확인한다.
--------------------------------------------------------------------*/
