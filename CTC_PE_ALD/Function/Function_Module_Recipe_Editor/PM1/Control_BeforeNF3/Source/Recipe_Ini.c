#include <stdio.h>
#include <windows.h> 

#include "kutlStr.h"
#include "IniFile_Param.h"
#include "Prg_Param.h"
#include "Data_Type.h"
#include "TextParser_C.h"
#include "LotType.h"

RCP_MODE	g_dMode	=	RCP_MODE_UNKNOWN;

extern VAL_STATUS RCP_VAL_CFG[MAX_VAL_NO];
extern VAL_STATUS PRE_VAL_CFG[MAX_VAL_NO];
extern VAL_STATUS PST_VAL_CFG[MAX_VAL_NO];
extern VAL_STATUS IDL_VAL_CFG[MAX_VAL_NO];
extern VAL_STATUS CLN_VAL_CFG[MAX_VAL_NO];
extern VAL_STATUS PUG_VAL_CFG[MAX_VAL_NO];
extern VAL_STATUS LPR_VAL_CFG[MAX_VAL_NO];
extern VAL_STATUS LPS_VAL_CFG[MAX_VAL_NO];
extern int	g_Total_Valve_No;
extern BOOL g_bRF;
BOOL g_bHTE_USE; // 2014.04.03

int				g_MFCNo	=	0;
int				g_HeaterNo = 0;
char			g_szValveName[MAX_VAL_NO][MAX_VAL_NAME_LENGTH];				// Valve Name 변수
BOOL			g_bInterLock_Map[ MAX_VAL_NO ][ MAX_VAL_NO ];

MFC_RCP_DATA	g_MFCData[MFC_BUF_LENGTH];
TEMP_RCP_DATA   g_HtrList[MAX_HEATER_NO];

//---------------------------------------------------------------------------------------

void Init_Valve_Name( void )
{
	int		i;
	for ( i = 0 ; i < MAX_VAL_NO ; i++ )		sprintf(  g_szValveName[i], "V%d", i+1 );
}

void Init_Interlock_Map( void )
{
	int		i, j;
	for ( i = 0 ; i < MAX_VAL_NO ; i++ )
		for ( j = 0 ; j < MAX_VAL_NO ; j++ )		g_bInterLock_Map[ i ][ j ] = FALSE;
}

void Set_InterLock_Map( int dTarget_Valve, int dInterLock_Valve )
{
	if ( dTarget_Valve == dInterLock_Valve )		
		return;				// Deadlock return
	else
	{
//		printf("Valve InterLock : [%d] [%d]\n", dTarget_Valve, dInterLock_Valve );
		g_bInterLock_Map[ dTarget_Valve - 1 ][ dInterLock_Valve - 1 ] = TRUE;
		g_bInterLock_Map[ dInterLock_Valve - 1 ][ dTarget_Valve - 1 ] = TRUE;

	}
}

BOOL Can_Change_Status( int dTarget_Valve, VAL_STATUS* pValveStatus, int* Interlock_Valve )
{
	int			i;

	for ( i = 0 ; i < MAX_VAL_NO ; i ++ )
	{
		if (( g_bInterLock_Map[ dTarget_Valve ][ i ] ) &&	( pValveStatus[ i ] == VAL_OPEN) )
		{
			*Interlock_Valve = ( i + 1 );
			return FALSE;
		}
	}

	return TRUE;
}

void Print_All_InterLock( void )
{
	int		i,	j;

	for ( i = 0 ; i < MAX_VAL_NO ; i++ )
	{
		printf("[%02d] ", i );
		for ( j = 0 ; j < MAX_VAL_NO ; j++ )
		{
			printf("[%02d] ",  g_bInterLock_Map[i][j] );
		}
		printf("\n");
	}

}

void MFC_Buffer_Initialize( void )
{
	int		i, j;
	for( i = 0 ; i < MFC_BUF_LENGTH ; i++ )
	{
		g_MFCData[i].bUse	=	FALSE;
		strcpy( g_MFCData[i].szMFCRealName	, "" );
		strcpy( g_MFCData[i].szMFCDisplayName, "");
		g_MFCData[i].dbSetPoint			= 0;
		// g_MFCData[i].dbDefaultTolerance	= 0;
		// g_MFCData[i].dbDefaultTimeOut	= 0;
		g_MFCData[i].dbMin				= 0;
		g_MFCData[i].dbMax				= 0;

		for (j=0; j<5; j++) strcpy(g_MFCData[i].szEnum[j] , "");
	}
}

void Heater_Buffer_Initialize( void )
{
	int		i;

	for( i = 0 ; i < MAX_HEATER_NO ; i++ )
	{
		g_HtrList[i].szHtrName[0] = 0;
		g_HtrList[i].bUse = FALSE;
	}
}

void Init_Buffer( void )
{
	MFC_Buffer_Initialize();
	Heater_Buffer_Initialize();
}

// 2004.08.23 by cskim, Modified
void Make_Ini_Full_Path( char *pszPath )
{
	if(strlen(g_szModule) > 0)
		sprintf( pszPath,"%s/%s/%s", INI_PATH, g_szModule, INI_FILE_NAME );
	else sprintf( pszPath,"%s/%s", INI_PATH, INI_FILE_NAME );
	return ;
}

BOOL STR_SEPERATE_BLANK( char* pszBuf, char* pszBuf1, char* pszBuf2, int nMax )
{
	int		i;
	int		dCount;
	int		dFirstLength, dSecondStart;
	BOOL	bFindFirstEnd	=	FALSE;

	dCount = strlen( pszBuf );
	if ( dCount < nMax )	nMax = dCount;

	for ( i = 0 ; i < nMax ; i++ )
	{
		if ( !bFindFirstEnd )
		{
			if (( pszBuf[ i ] == ' ' ) || ( pszBuf[i] == '\t' ) )
			{
				dFirstLength = i;
				bFindFirstEnd = TRUE;
			}
		}
		else
		{
			if (( pszBuf[ i ] != ' ' ) && ( pszBuf[i] != '\t' ) )
			{
				dSecondStart = i;
				strncpy( pszBuf1, pszBuf, dFirstLength );
				pszBuf1[ dFirstLength ] = '\0';
				strcpy( pszBuf2, &pszBuf[ dSecondStart ] );
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL REMOVE_STR_HEAD_BLANK( char* pszData )
{
	int		i;
	int		nMax;
	
	char	szData[1000];


	nMax = strlen( pszData );

	for ( i = 0 ; i < nMax ; i++ )
	{
		if (( pszData[i] != ' ' ) && ( pszData[i] != '\t' ))
		{
			strcpy( szData, &pszData[i]);
			strcpy( pszData, szData );
			return TRUE;
		}
	}

	return FALSE;
}


void RCP_MODE_SET( char* pszData ) 
{
	char	szReserveWord[255]	 = "";
	char	szConfigWord[255]	 = "";
	char	szBufCopy[255];

	strcpy( szBufCopy, pszData );

	STR_SEPERATE_CHAR( szBufCopy, ' ', szReserveWord, szConfigWord, 255 );

	if		( !strncmp ( szConfigWord, HEATER_BEGIN, strlen( HEATER_BEGIN )	) )			g_dMode = RCP_MODE_HEATER;
	else if ( !strncmp ( szConfigWord, MFC_BEGIN   , strlen( MFC_BEGIN )	) )			g_dMode = RCP_MODE_MFC;
	else if ( !strncmp ( szConfigWord, VALVE_BEGIN , strlen( VALVE_BEGIN )	) )			g_dMode = RCP_MODE_VALVE;
	else if ( !strncmp ( szConfigWord, EXT_CFG_BEGIN , strlen( EXT_CFG_BEGIN )	) )		g_dMode = RCP_MODE_EXT;
	else																				g_dMode = RCP_MODE_UNKNOWN;	
}

void Heater_Data_Parsing( char* pszData )
{
	int nHtrNo;
	int nIdx;

	tpSetText(pszData);
	if(tpGetInt(&nHtrNo, 0) == FALSE) return;
	nIdx = nHtrNo - 1;
	tpGetString(g_HtrList[nIdx].szHtrName, 0);
	g_HtrList[nIdx].bUse = TRUE;
}

//////////////////////////////////////////////////////////////////////////
void MFC_Data_Parsing( char* pszData )
{
	int		i;
	
	char	szTemp[10]	=	"";
	char	szRealName[255];
	char	szDisplayName[255];
	char	szBuffer[255];
	char	szSeperator[]	= " \t";
	char*	pszBuf;

	//double	dbSetPoint				=	0;
	//double	dbDefaultTolerance		=	0;
	double	dbDefaultTimeOut		=	0;
	double	dbMin					=	0;
	double	dbMax					=	0;

	UINT	uCounter		=	0;

	extern char *g_pszMfcOptEnum[];
	STR_SEPERATE_BLANK( pszData, szRealName, szBuffer, 50 );
	
	if		( szBuffer[0] == '-' )		return ;
	else if (  szBuffer[0] == '"' )		
	{
		STR_SEPERATE_CHAR( &szBuffer[1], '"', szDisplayName, szBuffer, 255 );
		REMOVE_STR_HEAD_BLANK( szBuffer );
		pszBuf = strtok( szBuffer, szSeperator );
		while ( pszBuf != NULL )
		{
			uCounter++;

			if ( strcmp( pszBuf, "-" ) )
			{
				if		( uCounter == 1 )		dbMin				=	atof( pszBuf ) ;
				else if ( uCounter == 2 )		dbMax				=	atof( pszBuf ) ;
				else if ( uCounter == 3 )		dbDefaultTimeOut	=	atof( pszBuf ) ;
				else if ( uCounter == 4 )		dbMin				=	atof( pszBuf ) ;
				else if ( uCounter == 5 )		dbMax				=	atof( pszBuf ) ;
				// else if ( uCounter == 8 )		strcpy( szEnum[0], pszBuf ) ;
				// else if ( uCounter == 9 )		strcpy( szEnum[1], pszBuf ) ;
				// else if ( uCounter == 10 )		strcpy( szEnum[2], pszBuf ) ;
				// else if ( uCounter == 11 )		strcpy( szEnum[3], pszBuf ) ;
				// else if ( uCounter == 12 )		strcpy( szEnum[4], pszBuf ) ;
			}

			pszBuf = strtok( NULL, szSeperator );
		}
		
		// 2006.08.03
		szRealName[49] = 0;
		szDisplayName[49] = 0;
		strcpy( g_MFCData[ g_MFCNo ].szMFCRealName		, szRealName	);
		strcpy( g_MFCData[ g_MFCNo ].szMFCDisplayName	, szDisplayName	);


		for ( i = 0 ; i < 9 ; i++ )
		{
			strcpy( g_MFCData[ g_MFCNo ].szEnum[i], g_pszMfcOptEnum[i] );
		}

		// g_MFCData[ g_MFCNo ].dbSetPoint				=	dbSetPoint;
		// g_MFCData[ g_MFCNo ].dbDefaultTolerance		=	dbDefaultTolerance;
		// g_MFCData[ g_MFCNo ].dbDefaultTimeOut		=	dbDefaultTimeOut;
		g_MFCData[ g_MFCNo ].dbMin					=	dbMin;
		g_MFCData[ g_MFCNo ].dbMax					=	dbMax;
		g_MFCData[ g_MFCNo ].bUse		=	TRUE;
		INCREASE_MFC_NO;
	}
}

void Print_MFC_Data( void )
{
	int		i;

	for ( i = 0 ; i < MFC_BUF_LENGTH ; i++ )
	{
		if ( g_MFCData[ i ].bUse )
		{
			printf("[%2d] %50s ", i , g_MFCData[ i ].szMFCRealName );
			printf("%50s" , g_MFCData[ i ].szMFCDisplayName );
			printf(" %5f ", g_MFCData[ i ].dbSetPoint );
			// printf(" %5f ", g_MFCData[ i ].dbDefaultTolerance );
			// printf(" %5f ", g_MFCData[ i ].dbDefaultTimeOut );
			printf(" %5f ", g_MFCData[ i ].dbMin );
			printf(" %5f ", g_MFCData[ i ].dbMax );
			printf("\n");
		}
	}
}

void RCP_VALVE_MASK_SET( RCP_TYPE udType, char* pszData )
{
	int			i;
	VAL_STATUS* pDest_Valve_Map;	
	int nLength;

	switch( udType )
	{
		case RCP_PRC :	pDest_Valve_Map = RCP_VAL_CFG;	break;
		case RCP_PRE :	pDest_Valve_Map = PRE_VAL_CFG;	break;
		case RCP_PST :	pDest_Valve_Map = PST_VAL_CFG;	break;
		case RCP_IDL :	pDest_Valve_Map = IDL_VAL_CFG;	break;
		case RCP_CLN :	pDest_Valve_Map = CLN_VAL_CFG;	break;
		case RCP_PUG :	pDest_Valve_Map = PUG_VAL_CFG;	break;
		case RCP_LPR :	pDest_Valve_Map = LPR_VAL_CFG;	break;
		case RCP_LPS :	pDest_Valve_Map = LPS_VAL_CFG;	break;
	}

	for ( i = 0 ; i < MAX_VAL_NO ; i++ )	pDest_Valve_Map[ i ] = VAL_NOUSE;
	
	nLength = strlen(pszData) - 2;
	if(nLength > g_Total_Valve_No) nLength = g_Total_Valve_No;

	for ( i = 0 ; i < g_Total_Valve_No ; i++ ) 
	{
    //2010.02.03
		if ( pszData[ i+1 ] == '1' )	 pDest_Valve_Map[ i ] = VAL_CLOSE;
        else if( pszData[ i+1 ] == '3' ) pDest_Valve_Map[ i ] = 3; //Recipe Config에 3으로 표시하는데 기존에서 추가된 Valve Bit만
        else{;}
	}
}


void InterLock_Set( int dValveNo, char* szOprInterLock2 )
{
	int		dValve	=	0;

	char*	pszBuf;
	char*	pszSeperator =",";

//	printf("Dest Data : %s\n", szOprInterLock2 );

	pszBuf = strtok( szOprInterLock2, pszSeperator );

	while( pszBuf != NULL )
	{
		dValve = atoi( pszBuf );
		Set_InterLock_Map( dValveNo, dValve );
		pszBuf = strtok( NULL, pszSeperator );
	}
}


void Analysis_Interlock( char* pszInterLock1, char* pszInterLock2 )
{
	 int  i   = 0;
	int  dCount  = 0;
	 int  dLength  = 0;
	int  dValve1  = 0;
 
	char ch;
	char szOprInterLock1[50];
	char szOprInterLock2[50];
 
	strcpy( szOprInterLock1, pszInterLock1 );
	strcpy( szOprInterLock2, pszInterLock2 );
 
	while( TRUE )
	{
		ch = szOprInterLock1[ dCount++ ];
		 
		if          ( ch == ' ' )
		{
			continue;
		}
		else if     ( ch == ','  )
		{
			strcpy( szOprInterLock2, pszInterLock2 );
			InterLock_Set( dValve1 , szOprInterLock2 );
			dValve1 = 0;
		}
		else if ( ch == '\0' )
		{
			strcpy( szOprInterLock2, pszInterLock2 );
			InterLock_Set( dValve1 , szOprInterLock2 );
			break;
		}
		else if ( ch != ' ' )
		{
			if (( ch >= '0' ) && ( ch <= '9' ))
			{
				dValve1 *= 10;
				dValve1 += ( ch - '0');
			}
			else				break;
		}
	}
	 
}

BOOL GetYesNo(const char *szYesNo)
{
	if(*szYesNo == 'Y' || *szYesNo == 'y' || *szYesNo == 'T'|| *szYesNo == 't' )
		return TRUE;
	return FALSE;
}
 
void Valve_Data_Parsing( char* pszData )
{
	int		dTempValCount;

	char	szBuffer[100]; 
	char	szData[150];	//2014.03.24
	char	szValNo[5];
	char	szValName[ MAX_VAL_NAME_LENGTH ];
	char	szInterLock1[50], szInterLock2[50];

	int		dValNo;

	STR_SEPERATE_BLANK( pszData, szBuffer, szData, 100 ) ;

	if	 ( strncmp( szBuffer, "TOTAL_VALVE_COUNT", strlen( "TOTAL_VALVE_COUNT" )) == 0 )
	{
		dTempValCount = atoi( szData );
		if(dTempValCount > MAX_VAL_NO) g_Total_Valve_No = MAX_VAL_NO;
		else g_Total_Valve_No = dTempValCount;
	}
	else if ( strncmp( szBuffer, "RF_USE_OPTION" , strlen( "RF_USE_OPTION"  ) ) == 0 )		g_bRF = GetYesNo(szData);
	else if ( strncmp( szBuffer, "RCP_VALVE_MASK", strlen( "RCP_VALVE_MASK" ) ) == 0 )		RCP_VALVE_MASK_SET( RCP_PRC, szData );
	else if ( strncmp( szBuffer, "PRE_VALVE_MASK", strlen( "PRE_VALVE_MASK" ) ) == 0 )		RCP_VALVE_MASK_SET( RCP_PRE, szData );
	else if ( strncmp( szBuffer, "PST_VALVE_MASK", strlen( "PST_VALVE_MASK" ) ) == 0 )		RCP_VALVE_MASK_SET( RCP_PST, szData );
	else if ( strncmp( szBuffer, "IDL_VALVE_MASK", strlen( "IDL_VALVE_MASK" ) ) == 0 )		RCP_VALVE_MASK_SET( RCP_IDL, szData );
	else if ( strncmp( szBuffer, "CLN_VALVE_MASK", strlen( "CLN_VALVE_MASK" ) ) == 0 )		RCP_VALVE_MASK_SET( RCP_CLN, szData );
	else if ( strncmp( szBuffer, "PUG_VALVE_MASK", strlen( "PUG_VALVE_MASK" ) ) == 0 )		RCP_VALVE_MASK_SET( RCP_PUG, szData );
	else if ( strncmp( szBuffer, "LPR_VALVE_MASK", strlen( "LPR_VALVE_MASK" ) ) == 0 )		RCP_VALVE_MASK_SET( RCP_LPR, szData );
	else if ( strncmp( szBuffer, "LPS_VALVE_MASK", strlen( "LPS_VALVE_MASK" ) ) == 0 )		RCP_VALVE_MASK_SET( RCP_LPS, szData );
	else if ( strncmp( szBuffer, "VAL_NAME"		 , strlen( "VAL_NAME" )		  ) == 0 )		
	{
		REMOVE_STR_HEAD_BLANK( szData );
		STR_SEPERATE_BLANK( szData, szValNo, szValName, 5 ) ;
		dValNo = atoi( szValNo );
		strcpy( g_szValveName[ dValNo - 1 ], szValName );
	}
	else if ( strncmp( szBuffer, "INTERLOCK", strlen( "INTERLOCK" )			  ) == 0 )
	{
		REMOVE_STR_HEAD_BLANK( szData );
		STR_SEPERATE_CHAR( szData, ':' ,szInterLock1, szInterLock2, 50 ) ;
		Analysis_Interlock( szInterLock1, szInterLock2 );
	}
}


//-----------------------------------------------------------------------------------
// 2004.11.05 by cskim

// 2005.03.02 by cskim
BOOL g_bValveStepCtrl = FALSE;

// 2005.05.20 by cskim, for CVD Editor
BOOL g_bHeaterSetUse = TRUE;

int g_nRF_MinPower = 0;
int g_nRF_MaxPower = 1250;
int g_nRF_MinPos = 0;
int g_nRF_MaxPos = 100;

//2014.03.31
int g_nRF2_MinPower = 0;
int g_nRF2_MaxPower = 1250;
int g_nRF2_MinPos = 0;
int g_nRF2_MaxPos = 100;
//2014.04.03
int g_nHTE_MinTemp = 0;
int g_nHTE_MaxTemp = 100;

int g_nStgHtrPos_Min = 100;
int g_nStgHtrPos_Max = 3000;
int g_nStgHtrPos_DecPnt = 1;
int g_nStgHtr_MinTemp = 0;
int g_nStgHtr_MaxTemp = 800;
int g_nRamp_MinTemp = 0;
int g_nRamp_MaxTemp = 100;

// 2005.12.26
BOOL g_bStgOffs_Use = FALSE;
double g_dbStgOffs_Min = -50;
double g_dbStgOffs_Max = 50;

int g_nTV_MinPos = 0;
int g_nTV_MaxPos = 100;
double g_dbTV_MinPres = 0.001;
double g_dbTV_MaxPres = 10;

double g_dbChm_MinPres = 0.001;
double g_dbChm_MaxPres = 10;

BOOL g_bPreTreat = FALSE;
BOOL g_bStepGroup = TRUE;
BOOL g_bRoughValve = TRUE;

// 2006.01.25
BOOL g_bThickness = FALSE;
//2009.07.06
BOOL g_bDMY = FALSE;
int g_bDMY_Step2 = 0;
double g_dbThickness_Min = 0; 
double g_dbThickness_Max = 20000;
//double g_dbThickness_Max = 10000;

// 2005.12.28
BOOL g_bStep_Time_Compen_Use = FALSE;
BOOL g_bValveStep_AllSave = TRUE;

double g_dbValveTime_Min = 0;
double g_dbValveTime_Max = 100;
int g_nValveTime_DecPnt = 2;

BOOL g_bRF_SetPointUse = FALSE;
BOOL g_bRF2_SetPointUse = FALSE; //2014.03.31
BOOL g_bPreventiveOptEdit = TRUE;
BOOL g_bSubItemEdit = TRUE;
BOOL g_bSubItem2Edit = FALSE;	// 2006.09.04

// 2013.04.02 Recipe Password Added by hyuk 
BOOL g_bRecipePW = FALSE;

// 2005.12.28
BOOL g_bMFC_RampUse = FALSE;

// 2005.05.24 by cskim, for Keep Last Condition
BOOL g_bKeepLastConEnable = FALSE;

// 2005.05.30 by cskim, for CVD Step Time & BacksideAr
BOOL g_bCVDStepTimeUse = FALSE;
BOOL g_bBacksideArUse = FALSE;

// 2007.02.08 by cskim
BOOL g_bOldRcpValveAutoEnDis = FALSE;

// 2005.05.11 by cskim, for CVD Editor
VRCP_SEQ_STRUCT g_VrcpSeq[MAX_VALVE_SEQ];
MFC_VLV_SEQ_STRUCT g_MfcVlv[MAX_MFC_NO];

// 2005.09.22 by cskim, for 112 Valve Recipe
int g_nTotalValveLineStep = 23;

// 2006.07.10 by cskim
double g_dbStepTimeLowLimit = 1.5;

void Init_Ext_Item()
{
	int i, j, k;

	// Init Mfc Valve List
	for(i=0; i<MAX_MFC_NO; i++)
	{
		g_MfcVlv[i].nEnumCount = 0;
		for(j=0; j<10; j++)
		{
			g_MfcVlv[i].pszEnum[j][0] = 0;
			g_MfcVlv[i].pnRcpLock[j] = 0;
			for(k=0; k<10; k++) g_MfcVlv[i].pnValve[j][k] = 0;
		}
	}

	// Init Valve Sequece Data
	for(i=0; i<MAX_VALVE_SEQ; i++)
	{
		g_VrcpSeq[i].szTypeName[0] = 0;
		g_VrcpSeq[i].nEnumCount = 0;
		for(j=0; j<10; j++)
		{
			g_VrcpSeq[i].pszEnum[j][0] = 0;
			g_VrcpSeq[i].pnRcpLock[j] = 0;
			for(k=0; k<10; k++) g_VrcpSeq[i].pnValve[j][k] = 0;
		}
	}
}

void VrcpSeq_Parsing(char *szValue)
{
	int nIdx = 0, nEnumIdx;
	int i, nValve;
	char szTypeName[20];

	tpSetText(szValue);
	tpGetWord(szTypeName, 0);
	if(0 == strcmp(szTypeName, "VRCP_TYPE1")) nIdx = 0;
	else if(0 == strcmp(szTypeName, "VRCP_TYPE2")) nIdx = 1;
	else if(0 == strcmp(szTypeName, "VRCP_TYPE3")) nIdx = 2;
	else if(0 == strcmp(szTypeName, "VRCP_TYPE4")) nIdx = 3;
	else return;

	if(g_VrcpSeq[nIdx].nEnumCount >= 10) return;
	strcpy(g_VrcpSeq[nIdx].szTypeName, szTypeName);
	nEnumIdx = g_VrcpSeq[nIdx].nEnumCount;
	tpGetWord(g_VrcpSeq[nIdx].pszEnum[nEnumIdx], 0);
	tpGetInt(& g_VrcpSeq[nIdx].pnRcpLock[nEnumIdx], 0);
	for(i=0; i<10; i++)
	{
		if(! tpGetInt(&nValve, 0)) break;
		g_VrcpSeq[nIdx].pnValve[nEnumIdx][i] = nValve;
	}
	g_VrcpSeq[nIdx].nEnumCount++;
}

void MfcValve_Parsing(char *szValue)
{
	int nMfc, nIdx=0, nEnumIdx;
	int i, nValve;

	tpSetText(szValue);
	tpGetInt(&nMfc, 0);
	nIdx = nMfc - 1;

	if(g_MfcVlv[nIdx].nEnumCount >= 10) return;
	nEnumIdx = g_MfcVlv[nIdx].nEnumCount;

	tpGetWord(g_MfcVlv[nIdx].pszEnum[nEnumIdx], 0);
	tpGetInt(& g_MfcVlv[nIdx].pnRcpLock[nEnumIdx], 0);
	for(i=0; i<10; i++)
	{
		if(! tpGetInt(&nValve, 0)) break;
		g_MfcVlv[nIdx].pnValve[nEnumIdx][i] = nValve;
	}
	g_MfcVlv[nIdx].nEnumCount++;
}

void Ext_Item_Parsing(const char *szData)
{
	char szItem[40];
	char szValue[100];

	tpSetText(szData);
	tpGetWord(szItem, 0);
	tpGetString(szValue, 0);

	if(0 == strcmp(szItem, "Valve_Step_Control"))      g_bValveStepCtrl = GetYesNo(szValue);
	else if(0 == strcmp(szItem, "RF_MinPower"))        g_nRF_MinPower = atoi(szValue);
	else if(0 == strcmp(szItem, "RF_MaxPower"))        g_nRF_MaxPower = atoi(szValue);
	else if(0 == strcmp(szItem, "RF_MinPosition"))     g_nRF_MinPos = atoi(szValue);
	else if(0 == strcmp(szItem, "RF_MaxPosition"))     g_nRF_MaxPos = atoi(szValue);
	//2014.03.31
	else if(0 == strcmp(szItem, "RF2_MinPower"))        g_nRF2_MinPower = atoi(szValue);
	else if(0 == strcmp(szItem, "RF2_MaxPower"))        g_nRF2_MaxPower = atoi(szValue);
	else if(0 == strcmp(szItem, "RF2_MinPosition"))     g_nRF2_MinPos = atoi(szValue);
	else if(0 == strcmp(szItem, "RF2_MaxPosition"))     g_nRF2_MaxPos = atoi(szValue);
	//2014.04.03
	else if(0 == strcmp(szItem, "HTE_MinTemp"))        g_nHTE_MinTemp = atoi(szValue);
	else if(0 == strcmp(szItem, "HTE_MaxTemp"))        g_nHTE_MaxTemp = atoi(szValue);
	else if(0 == strcmp( szItem, "HTE_USE_OPTION"))		g_bHTE_USE = GetYesNo(szValue); 

	else if(0 == strcmp(szItem, "StgHtrPos_Min"))      g_nStgHtrPos_Min = atoi(szValue);
	else if(0 == strcmp(szItem, "StgHtrPos_Max"))      g_nStgHtrPos_Max = atoi(szValue);
	else if(0 == strcmp(szItem, "StgHtrPos_DecPnt"))   g_nStgHtrPos_DecPnt = atoi(szValue);
	else if(0 == strcmp(szItem, "StgHtr_MinTemp"))     g_nStgHtr_MinTemp = atoi(szValue);
	else if(0 == strcmp(szItem, "StgHtr_MaxTemp"))     g_nStgHtr_MaxTemp = atoi(szValue);
	else if(0 == strcmp(szItem, "StgHtrRamp_MinTemp")) g_nRamp_MinTemp = atoi(szValue);
	else if(0 == strcmp(szItem, "StgHtrRamp_MaxTemp")) g_nRamp_MaxTemp = atoi(szValue);
	// 2005.12.26
	else if(0 == strcmp(szItem, "StgOffs_Use"))        g_bStgOffs_Use = GetYesNo(szValue);
	else if(0 == strcmp(szItem, "StgOffs_Min"))        g_dbStgOffs_Min = atof(szValue);
	else if(0 == strcmp(szItem, "StgOffs_Max"))        g_dbStgOffs_Max = atof(szValue);

	else if(0 == strcmp(szItem, "TV_MinPosition"))     g_nTV_MinPos = atoi(szValue);
	else if(0 == strcmp(szItem, "TV_MaxPosition"))     g_nTV_MaxPos = atoi(szValue);
	else if(0 == strcmp(szItem, "TV_MinPressure"))     g_dbTV_MinPres = atof(szValue);
	else if(0 == strcmp(szItem, "TV_MaxPressure"))     g_dbTV_MaxPres = atof(szValue);

	else if(0 == strcmp(szItem, "Chamber_MinPressure")) g_dbChm_MinPres = atof(szValue);
	else if(0 == strcmp(szItem, "Chamber_MaxPressure")) g_dbChm_MaxPres = atof(szValue);

	// 2005.12.28
	else if(0 == strcmp(szItem, "Step_Time_Compen_Use")) g_bStep_Time_Compen_Use = GetYesNo(szValue);
	else if(0 == strcmp(szItem, "ValveStep_AllSave"))    g_bValveStep_AllSave = GetYesNo(szValue);
	else if(0 == strcmp(szItem, "MFC_RampUse"))          g_bMFC_RampUse = GetYesNo(szValue);

	else if(0 == strcmp(szItem, "StgOffs_Use"))          g_bStgOffs_Use = GetYesNo(szValue);

	else if(0 == strcmp(szItem, "Rough_Valve_Ctrl"))     g_bRoughValve = GetYesNo(szValue);

	// 2006.01.25
	else if(0 == strcmp(szItem, "Thickness_Use"))        g_bThickness = GetYesNo(szValue);
	else if(0 == strcmp(szItem, "Step_Group_Use"))       g_bStepGroup = GetYesNo(szValue);

	else if(0 == strcmp(szItem, "ValveTime_Min"))        g_dbValveTime_Min = atof(szValue);
	else if(0 == strcmp(szItem, "ValveTime_Max"))        g_dbValveTime_Max = atof(szValue);
	else if(0 == strcmp(szItem, "ValveTime_DecPnt"))     g_nValveTime_DecPnt = atoi(szValue);
	// 2005.05.09 by cskim, Option 추가
	else if(0 == strcmp(szItem, "RF_SetPointUse"))       g_bRF_SetPointUse = GetYesNo(szValue);
	else if(0 == strcmp(szItem, "PreventiveOptEdit"))    g_bPreventiveOptEdit = GetYesNo(szValue);
	else if(0 == strcmp(szItem, "SubItemEdit"))          g_bSubItemEdit = GetYesNo(szValue);

	else if(0 == strcmp(szItem, "SubItem2Edit"))         g_bSubItem2Edit = GetYesNo(szValue);		// 2006.09.04
	// 2014.03.31
	else if(0 == strcmp(szItem, "RF2_SetPointUse"))       g_bRF2_SetPointUse = GetYesNo(szValue);
	else if(0 == strcmp(szItem, "MFC_VALVE"))            MfcValve_Parsing(szValue);
	else if(0 == strcmp(szItem, "VALVE_SEQ"))            VrcpSeq_Parsing(szValue);
	// 2005.05.20 by cskim, for CVD Editor
	else if(0 == strcmp(szItem, "HeaterSetUse"))         g_bHeaterSetUse = GetYesNo(szValue);
	// 2005.05.24 by cskim, for Keep Last Condition
	else if(0 == strcmp(szItem, "KeepLastConEnable"))    g_bKeepLastConEnable = GetYesNo(szValue);
	// 2005.05.30 by cskim, for CVD Step Time & BacksideAr
	else if(0 == strcmp(szItem, "CVDStepTimeUse"))       g_bCVDStepTimeUse = GetYesNo(szValue);
	else if(0 == strcmp(szItem, "BacksideArUse"))        g_bBacksideArUse = GetYesNo(szValue);
	// 2005.09.22 by cskim, for 112 Valve Recipe
	else if(0 == strcmp(szItem, "TOTAL_VALVE_LINE_STEP")) g_nTotalValveLineStep = atoi(szValue);
	// 2006.07.10 by cskim
	else if(0 == strcmp(szItem, "StepTimeLowLimit")) g_dbStepTimeLowLimit = atof(szValue);
	// 2007.02.08
	else if(0 == strcmp(szItem, "OldRcpValveAutoEnDis")) g_bOldRcpValveAutoEnDis = GetYesNo(szValue);

	// 2013.04.02 Recipe Password Added by hyuk 
	else if (0 == strcmp(szItem, "RecipePassword"))	g_bRecipePW = GetYesNo(szValue);
}

//-----------------------------------------------------------------------------------

void Parsing_Data( char *pszData )
{
	switch ( g_dMode )
	{
		case RCP_MODE_HEATER	:	Heater_Data_Parsing( pszData );		break;
		case RCP_MODE_MFC		:	MFC_Data_Parsing( pszData );		break;
		case RCP_MODE_VALVE		:	Valve_Data_Parsing( pszData );		break;
		case RCP_MODE_EXT		:	Ext_Item_Parsing( pszData );		break;
		default :											return;
	}
}

BOOL Process_Def_Data( char *pszData )
{
	char	pszCopy[255];
	UINT	uCounter		=	0;

	strcpy( pszCopy, pszData );

	if		( pszData[0] =='#' )		return TRUE;
	else if ( pszData[0] ==' ' )		return TRUE;
	else if ( pszData[0] =='$' ){		RCP_MODE_SET( pszData );	return TRUE; }
	else								Parsing_Data( pszData );

	return TRUE;

}

BOOL Read_Ini_File( void )
{
	char	ch= ' ';
	UINT	uBufCount;


	FILE*	fi;

	char	szIniPath[ MAX_PATH ];
	char	szLineBuf[ 1000 ];

	uBufCount	=	0;
	
	Make_Ini_Full_Path( szIniPath );
	Init_Valve_Name();						// Valve 이름 초기화 - 2004-04-26 김창수 대리 요구 사항
	Init_Buffer();
	Init_Interlock_Map();
	Init_Ext_Item();

	fi = fopen( szIniPath, "rt" );

	if ( fi == NULL )
	{
		printf("Process Recipe Read Error\n");
		return FALSE;
	}

	while (( ch = fgetc( fi ) ) != EOF )
	{
		if ( ch == '\n' ) 
		{
			szLineBuf[ uBufCount ] = '\0';
			Process_Def_Data( szLineBuf );
			uBufCount = 0;
		}
		else
		{
			szLineBuf[ uBufCount++ ] = ch;
		}
	}

	if ( uBufCount != 0 )
	{
		szLineBuf[ uBufCount ] = '\0';
		Process_Def_Data( szLineBuf );
		uBufCount = 0;
	}
		
	fclose( fi );

	return TRUE;
}