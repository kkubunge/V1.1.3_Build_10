#ifndef _READ_INI_
#define _READ_INI_


void Init_Buffer( void );
//void Make_Ini_Full_Path( char *pszPath );
//void Parsing_Data( char *pszData );
BOOL Process_Def_Data( char *pszData );
BOOL Read_Ini_File( char* pszFileName );

int	 GET_TOTAL_PAGE_INDEX();
void INC_TOTAL_PAGE_INDEX();
void RESET_TOTAL_PAGE_INDEX();


int	 GET_CURRENT_LINE();
void SET_CURRENT_LINE( int LineCount );
void INC_CURRENT_LINE();
void RESET_CURRENT_LINE();


#endif