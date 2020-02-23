#ifndef __DATA_TYPE__
#define __DATA_TYPE__

#define	MAX_LOG_ITEM	100

typedef struct 
{
	char	szLogFilePath[MAX_PATH];
	DWORD	dwDelCondition;	
	BOOL	IsFolder;
	
	// char	szDelCondition[10];
} LOG_ITEM;


#endif