// TextParser_C.h: interface Text Parser for C.
// Create : 2004.03.02
// Last Update : 2004.04.02
// Programed by Changsu,Kim
//////////////////////////////////////////////////////////////////////

#ifndef BOOL
#define BOOL	int
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

// public:
extern void tpSetText(const char* pText);
extern BOOL tpMoveFirst();
extern BOOL tpMoveLast();
extern BOOL tpMoveNext();
extern BOOL tpMoveBack();
extern BOOL tpMoveTo(int nIndex);
extern BOOL tpMoveNextWord();
extern BOOL tpMoveBackWord();

extern char tpGetChar();
extern char tpPeekChar();
extern BOOL tpGetWord(char *pBuf, char chToken);	// Default Token = SPACE, TAB, NULL, RETURN, CR, LF
extern BOOL tpGetWordFromTk(char chToken, char *pBuf, char chTok_B);
extern BOOL tpGetString(char *pBuf, char chToken);
extern BOOL tpGetStringFromTk(char chToken, char *pBuf);
extern BOOL tpGetLine(char *pBuf, char chToken);
extern BOOL tpGetLineFromTk(char chToken, char *pBuf);
extern BOOL tpGetInt(int *pValue, char chToken);
extern BOOL tpGetIntFromTk(char chToken, int *pValue, char chTok_B);
extern BOOL tpGetLong(long *pValue, char chToken);
extern BOOL tpGetLongFromTk(char chToken, long *pValue, char chTok_B);
extern BOOL tpGetFloat(float *pValue, char chToken);
extern BOOL tpGetFloatFromTk(char chToken, float *pValue, char chTok_B);
extern BOOL tpGetHexa(unsigned int *pValue, char chToken);
extern BOOL tpGetHexaFromTk(char chToken, unsigned int *pValue, char chTok_B);
extern BOOL tpGetDate(int *pYear, int *pMonth, int *pDay, char chDiv, char chToken);
extern BOOL tpGetDateFromTk(char chToken, int *pYear, int *pMonth, int *pDay, char chDiv, char chTok_B);
extern BOOL tpGetBool(BOOL *pValue, char chToken);
extern BOOL tpGetBoolFromTk(char chToken, BOOL *pValue, char chTok_B);
extern BOOL tpToken(char chToken);
extern BOOL tpSkipLine();
extern BOOL tpIsEnd();

extern unsigned int tpHtoi(const char *szHexa);
extern char* tpUpperStr(char *szStr);
