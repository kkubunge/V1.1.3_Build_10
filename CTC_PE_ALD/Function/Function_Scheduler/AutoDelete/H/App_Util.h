#ifndef _UTIL_MAIN__
#define _UTIL_MAIN__

#ifdef __cplusplus
extern "C"{
#endif

enum { 
	   TIME_HMS_COLON,   TIME_HMS_HYPEN,   TIME_HMS_SLASH,   TIME_HMS_CONTINUE,
	   TIME_HM_COLON,    TIME_HM_HYPEN,    TIME_HM_SLASH,    TIME_HM_CONTINUE,
	   TIME_HMSM_COLON , TIME_HMSM_HYPEN , TIME_HMSM_SLASH , TIME_HMSM_CONTINUE
};

enum {
		DATE_YYYYMMDD_SLASH,	DATE_YYMMDD_SLASH,   
		DATE_YYYYMMDD_HYPEN,	DATE_YYMMDD_HYPEN,
		DATE_YYYYMMDD_PERIOD,	DATE_YYMMDD_PERIOD, 
		DATE_YYYYMMDD_CONTINUE, DATE_YYMMDD_CONTINUE
};

enum { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT };

BOOL REMOVE_STR_HEAD_BLANK( char* pszData );
BOOL STR_SEPERATE_BLANK( char* pszBuf, char* pszBuf1, char* pszBuf2, int nMax );
BOOL STR_SEPERATE_BLANK2( char* pszBuf, char* pszBuf1, char* pszBuf2, int nMax );
void GetTimeData( char* szData, int dMode );
void GetDateData( char* szData, int dMode );
BOOL End_Slash_Check( char* szPath );
void SecToTime( double dbRes, char* pszTime );
void UPPER_STRING( char* szString );
void LOWER_STRING( char* szString );
BOOL DEBUG_LOG( char* szModuleName, char* szMsg );
BOOL REMOVE_STR_TAIL_BLANK( char* pszData );

DWORD GET_DATE_CODE( SYSTEMTIME* st );
BOOL IS_LEAP_YEAR( WORD wYear );
WORD ACCU_DATE_IN_THIS_YEAR( WORD wYear, WORD wMonth, WORD wDay );

BOOL IsFolderExist( char* szPath );
DWORD GET_TIME_CODE( SYSTEMTIME* st );


#ifdef __cplusplus
}
#endif

#endif