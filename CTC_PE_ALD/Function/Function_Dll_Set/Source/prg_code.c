//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#include "iodefine.h"
#include "IPS_Log.h"
#include "Kutlanal.h"
//------------------------------------------------------------------------------------------
#define INVALID_LOT_PRERCP_TYPE			40
#define INVALID_LOTPRE_OPTION           41
#define LOT_PRE_START_TIMEOUT           42

int gPreviousBeforeLotPreType = 0;
int gPreviousLotPreType = 0;
int gCurrentLotPreType = 0;
int gRunningLotPreType = 0;
int gCM1LotPreType = 0;
int gCM2LotPreType = 0;
int gCM3LotPreType = 0;

void OptionalIOCheck();

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "e:SCHEDULER"					, _K_S_IO	,	SCHEDULER_S			,	0	} ,
	{ "v:SCHEDULER"					, _K_D_IO	,	SCHEDULER_D			,	0	} ,
	{ "CTC.PM1_PreRecipeUseA"		, _K_D_IO	,	PM1_PreRecipeUseA	,	0	} ,
	{ "CTC.PM2_PreRecipeUseA"		, _K_D_IO	,	PM2_PreRecipeUseA	,	0	} ,
	{ "CTC.PM3_PreRecipeUseA"		, _K_D_IO	,	PM3_PreRecipeUseA	,	0	} ,
	{ "CTC.PM4_PreRecipeUseA"		, _K_D_IO	,	PM4_PreRecipeUseA	,	0	} ,
	{ "CTC.PM5_PreRecipeUseA"		, _K_D_IO	,	PM5_PreRecipeUseA	,	0	} ,
	{ "CTC.PM1_PreRecipeA"			, _K_S_IO	,	PM1_PreRecipeA		,	0	} ,
	{ "CTC.PM2_PreRecipeA"			, _K_S_IO	,	PM2_PreRecipeA		,	0	} ,
	{ "CTC.PM3_PreRecipeA"			, _K_S_IO	,	PM3_PreRecipeA		,	0	} ,
	{ "CTC.PM4_PreRecipeA"			, _K_S_IO	,	PM4_PreRecipeA		,	0	} ,
	{ "CTC.PM5_PreRecipeA"			, _K_S_IO	,	PM5_PreRecipeA		,	0	} ,
	{ "CTC.PM1_PreRecipeUseB"		, _K_D_IO	,	PM1_PreRecipeUseB	,	0	} ,
	{ "CTC.PM2_PreRecipeUseB"		, _K_D_IO	,	PM2_PreRecipeUseB	,	0	} ,
	{ "CTC.PM3_PreRecipeUseB"		, _K_D_IO	,	PM3_PreRecipeUseB	,	0	} ,
	{ "CTC.PM4_PreRecipeUseB"		, _K_D_IO	,	PM4_PreRecipeUseB	,	0	} ,
	{ "CTC.PM5_PreRecipeUseB"		, _K_D_IO	,	PM5_PreRecipeUseB	,	0	} ,
	{ "CTC.PM1_PreRecipeB"			, _K_S_IO	,	PM1_PreRecipeB		,	0	} ,
	{ "CTC.PM2_PreRecipeB"			, _K_S_IO	,	PM2_PreRecipeB		,	0	} ,
	{ "CTC.PM3_PreRecipeB"			, _K_S_IO	,	PM3_PreRecipeB		,	0	} ,
	{ "CTC.PM4_PreRecipeB"			, _K_S_IO	,	PM4_PreRecipeB		,	0	} ,
	{ "CTC.PM5_PreRecipeB"			, _K_S_IO	,	PM5_PreRecipeB		,	0	} ,
	{ "CTC.PM1_PreRecipeUseC"		, _K_D_IO	,	PM1_PreRecipeUseC	,	0	} ,
	{ "CTC.PM2_PreRecipeUseC"		, _K_D_IO	,	PM2_PreRecipeUseC	,	0	} ,
	{ "CTC.PM3_PreRecipeUseC"		, _K_D_IO	,	PM3_PreRecipeUseC	,	0	} ,
	{ "CTC.PM4_PreRecipeUseC"		, _K_D_IO	,	PM4_PreRecipeUseC	,	0	} ,
	{ "CTC.PM5_PreRecipeUseC"		, _K_D_IO	,	PM5_PreRecipeUseC	,	0	} ,
	{ "CTC.PM1_PreRecipeC"			, _K_S_IO	,	PM1_PreRecipeC		,	0	} ,
	{ "CTC.PM2_PreRecipeC"			, _K_S_IO	,	PM2_PreRecipeC		,	0	} ,
	{ "CTC.PM3_PreRecipeC"			, _K_S_IO	,	PM3_PreRecipeC		,	0	} ,
	{ "CTC.PM4_PreRecipeC"			, _K_S_IO	,	PM4_PreRecipeC		,	0	} ,
	{ "CTC.PM5_PreRecipeC"			, _K_S_IO	,	PM5_PreRecipeC		,	0	} ,
	{ "CTC.PM1_PreRecipeUseD"		, _K_D_IO	,	PM1_PreRecipeUseD	,	0	} ,
	{ "CTC.PM2_PreRecipeUseD"		, _K_D_IO	,	PM2_PreRecipeUseD	,	0	} ,
	{ "CTC.PM3_PreRecipeUseD"		, _K_D_IO	,	PM3_PreRecipeUseD	,	0	} ,
	{ "CTC.PM4_PreRecipeUseD"		, _K_D_IO	,	PM4_PreRecipeUseD	,	0	} ,
	{ "CTC.PM5_PreRecipeUseD"		, _K_D_IO	,	PM5_PreRecipeUseD	,	0	} ,
	{ "CTC.PM1_PreRecipeD"			, _K_S_IO	,	PM1_PreRecipeD		,	0	} ,
	{ "CTC.PM2_PreRecipeD"			, _K_S_IO	,	PM2_PreRecipeD		,	0	} ,
	{ "CTC.PM3_PreRecipeD"			, _K_S_IO	,	PM3_PreRecipeD		,	0	} ,
	{ "CTC.PM4_PreRecipeD"			, _K_S_IO	,	PM4_PreRecipeD		,	0	} ,
	{ "CTC.PM5_PreRecipeD"			, _K_S_IO	,	PM5_PreRecipeD		,	0	} ,
	{ "CTC.PM1_PreRecipeUseE"		, _K_D_IO	,	PM1_PreRecipeUseE	,	0	} ,
	{ "CTC.PM2_PreRecipeUseE"		, _K_D_IO	,	PM2_PreRecipeUseE	,	0	} ,
	{ "CTC.PM3_PreRecipeUseE"		, _K_D_IO	,	PM3_PreRecipeUseE	,	0	} ,
	{ "CTC.PM4_PreRecipeUseE"		, _K_D_IO	,	PM4_PreRecipeUseE	,	0	} ,
	{ "CTC.PM5_PreRecipeUseE"		, _K_D_IO	,	PM5_PreRecipeUseE	,	0	} ,
	{ "CTC.PM1_PreRecipeE"			, _K_S_IO	,	PM1_PreRecipeE		,	0	} ,
	{ "CTC.PM2_PreRecipeE"			, _K_S_IO	,	PM2_PreRecipeE		,	0	} ,
	{ "CTC.PM3_PreRecipeE"			, _K_S_IO	,	PM3_PreRecipeE		,	0	} ,
	{ "CTC.PM4_PreRecipeE"			, _K_S_IO	,	PM4_PreRecipeE		,	0	} ,
	{ "CTC.PM5_PreRecipeE"			, _K_S_IO	,	PM5_PreRecipeE		,	0	} ,
	{ "CTC.LotPreOption"			, _K_D_IO	,	LotPreOption		,	0	} ,	
	{ "CTC.MAIN_CONTROL"			, _K_D_IO	,	CM1_Control			,	0	} ,
	{ "CTC.MAIN_CONTROL2"			, _K_D_IO	,	CM2_Control			,	0	} ,
	{ "CTC.MAIN_CONTROL3"			, _K_D_IO	,	CM3_Control			,	0	} ,
	{ "CTC.N2PURGE_CONTROL"			, _K_D_IO	,	N2Purge_Control		,	0	} ,	
	{ "CTC.ROBOT_CONTROL"			, _K_D_IO	,	Robot_Control		,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
BOOL GetRecipeName(char* chRecipeNameIn, char* chRecipeNameOut)
{
	BOOL bRet				= FALSE;
	int  nIndex				= 0;
	int  nRecipeName		= 0;
	int  nResults			= 0;
	char chRecipe[256]		= "";
	char chRecipeOut[256]	= "";

	nRecipeName = strlen(chRecipeNameIn);
	if(nRecipeName > 0)
	{
		strcpy(chRecipe,chRecipeNameIn);
		do
		{
			if(':' == chRecipe[nIndex])
				break;
			nIndex++;
		}while(nIndex < nRecipeName);

		strncpy(chRecipeOut,chRecipe,nIndex);	
		strcpy(chRecipeNameOut,chRecipeOut);

		bRet = TRUE;		
	}
	else
	{
		strcpy(chRecipeNameOut,"");
		bRet = FALSE;
	}

	return bRet;
}

int GetLotPreOption(char *szPortInfo)
{
	int nRunSts  = 0;
	int nRunSts2 = 0;
	int nRunStsTmp = 0;
	int nLotPreOpt = 7;
	int CommStatus = 0;
	char pszFtnName[256] = "";

	if(strcmp(szPortInfo,"CM1") == 0 || strcmp(szPortInfo,"CM2") == 0 || strcmp(szPortInfo,"CM3") == 0)
	{
		if(strcmp(szPortInfo,"CM1") == 0)
		{
			nRunSts  = READ_DIGITAL( CM2_Control , &CommStatus);
			nRunSts2 = READ_DIGITAL( CM3_Control , &CommStatus);
		}
		else if(strcmp(szPortInfo,"CM2") == 0)
		{
			nRunSts  = READ_DIGITAL( CM1_Control , &CommStatus);
			nRunSts2 = READ_DIGITAL( CM3_Control , &CommStatus);
		}
		else if(strcmp(szPortInfo,"CM3") == 0)
		{
			nRunSts  = READ_DIGITAL( CM1_Control , &CommStatus);
			nRunSts2 = READ_DIGITAL( CM2_Control , &CommStatus);
		}

		if((nRunSts  == 1 || nRunSts  == 2 || nRunSts  == 3 || nRunSts  == 4 || nRunSts  == 6) || // Previous Lot Running
		   (nRunSts2 == 1 || nRunSts2 == 2 || nRunSts2 == 3 || nRunSts2 == 4 || nRunSts2 == 6) )
		{
			do
			{
				
				if( (nRunSts  == 1 || nRunSts  == 2 || nRunSts  == 3 || nRunSts  == 4 || nRunSts  == 6) && // In case of 2 previous lots running
					(nRunSts2 == 1 || nRunSts2 == 2 || nRunSts2 == 3 || nRunSts2 == 4 || nRunSts2 == 6) )
				{
					if((gPreviousLotPreType == gCurrentLotPreType) && (gPreviousBeforeLotPreType == gCurrentLotPreType) && (gPreviousBeforeLotPreType == gPreviousLotPreType))
					{
						nLotPreOpt = 3;
						break;
					}
					else
					{
						nLotPreOpt = 7;
						break;
					}
				}

				if(strcmp(szPortInfo,"CM1") == 0)
				{
					nRunStsTmp = READ_DIGITAL(CM2_Control,&CommStatus);
					if(nRunStsTmp == 1 || nRunStsTmp == 2 || nRunStsTmp == 3 || nRunStsTmp == 4 || nRunStsTmp == 6)
					{
						gRunningLotPreType = gCM2LotPreType;
						LOG(pszFtnName, ELT_File, "[FUNCTION] gRunningLotPreType of CM2 is %d" , gRunningLotPreType);
					}
					nRunStsTmp = READ_DIGITAL(CM3_Control,&CommStatus);
					if(nRunStsTmp == 1 || nRunStsTmp == 2 || nRunStsTmp == 3 || nRunStsTmp == 4 || nRunStsTmp == 6)
					{
						gRunningLotPreType = gCM3LotPreType;
						LOG(pszFtnName, ELT_File, "[FUNCTION] gRunningLotPreType of CM3 is %d" , gRunningLotPreType);
					}
				}
				else if(strcmp(szPortInfo,"CM2") == 0)
				{
					nRunStsTmp = READ_DIGITAL(CM3_Control,&CommStatus);
					if(nRunStsTmp == 1 || nRunStsTmp == 2 || nRunStsTmp == 3 || nRunStsTmp == 4 || nRunStsTmp == 6)
					{
						gRunningLotPreType = gCM3LotPreType;
						LOG(pszFtnName, ELT_File, "[FUNCTION] gRunningLotPreType of CM3 is %d" , gRunningLotPreType);
					}
					nRunStsTmp = READ_DIGITAL(CM1_Control,&CommStatus);
					if(nRunStsTmp == 1 || nRunStsTmp == 2 || nRunStsTmp == 3 || nRunStsTmp == 4 || nRunStsTmp == 6)
					{
						gRunningLotPreType = gCM1LotPreType;
						LOG(pszFtnName, ELT_File, "[FUNCTION] gRunningLotPreType of CM1 is %d" , gRunningLotPreType);
					}
				}
				else if(strcmp(szPortInfo,"CM3") == 0)
				{
					nRunStsTmp = READ_DIGITAL(CM1_Control,&CommStatus);
					if(nRunStsTmp == 1 || nRunStsTmp == 2 || nRunStsTmp == 3 || nRunStsTmp == 4 || nRunStsTmp == 6)
					{
						gRunningLotPreType = gCM1LotPreType;
						LOG(pszFtnName, ELT_File, "[FUNCTION] gRunningLotPreType of CM1 is %d" , gRunningLotPreType);
					}
					nRunStsTmp = READ_DIGITAL(CM2_Control,&CommStatus);
					if(nRunStsTmp == 1 || nRunStsTmp == 2 || nRunStsTmp == 3 || nRunStsTmp == 4 || nRunStsTmp == 6)
					{
						gRunningLotPreType = gCM2LotPreType;
						LOG(pszFtnName, ELT_File, "[FUNCTION] gRunningLotPreType of CM2 is %d" , gRunningLotPreType);
					}
				}

				if(gRunningLotPreType == gCurrentLotPreType)
				{
					nLotPreOpt = 3;
					LOG(pszFtnName, ELT_File, "[FUNCTION] gRunningLotPreType is %d" , gRunningLotPreType);
				}

			}while(0);
		}
	}
	return nLotPreOpt;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	char Buffer[64]				= "";
	char szLotPre[256]			= "";
	char szParmReceived[256]    = "";
	char szSpecialData[256]		= "";
	char szPortInfo[256]		= "";
	char pszFtnName[256]		= "";	
	char szLotPreFull[256]		= "";
	char PreRecipeName[256]		= "";
	char PreRecipeNameReal[256] = "";
	char szPramRead[256] = "";
	char szEmpty[256]    = "";
	char *pdest = NULL;
	int CommStatus = 0;
	int nLotPreOpt = 0;
	int nRcpType = -1;
	int data = 0; 
	int nPos = 0;
	int i = 0;
	int PreRecipeioindex = 0;
	int PreRecipeioindexU = 0;
	int nCounter = 0;
	
	LOG(pszFtnName, ELT_File, "////////// Lot Pre Pass Sequence Entered ////////////////////");
	printf( "[FUNCTION] Lot Special Data is %s\n" , PROGRAM_PARAMETER_READ() );

	/*
	strcpy(szPramRead,PROGRAM_PARAMETER_READ());
	LOG(pszFtnName, ELT_File, "[FUNCTION] Lot Special Data is %s" , szPramRead);
	if(strlen(szPramRead) < 1 || strcmp(szEmpty,szPramRead) == 0)
	{
		ALARM_POST(INVALID_LOT_PRERCP_TYPE); // Alarm Post
		return SYS_ABORTED;
	}
	*/

	strcpy(szParmReceived,PROGRAM_PARAMETER_READ()); // 2:CM1
	printf(" ///////////////// => szParmReceived =>%s \n",szParmReceived);
	
	nPos = strcspn(szParmReceived,":");
	printf(" ///////////////// => nPos:%d \n",nPos);

	if(nPos < 1 || strlen(szParmReceived) < 1)
	{
		ALARM_POST(INVALID_LOT_PRERCP_TYPE); // Alarm Post
		return SYS_ABORTED;
	}

	strncpy(szSpecialData,szParmReceived,nPos);
	printf(" ///////////////// szSpecialData=>%s \n",szSpecialData);
	
	if(strcmp(szSpecialData,"0") != 0 && strcmp(szSpecialData,"1") != 0 && strcmp(szSpecialData,"2") != 0 &&
	   strcmp(szSpecialData,"3") != 0 && strcmp(szSpecialData,"4") != 0 && strcmp(szSpecialData,"5") != 0 )
	{
		ALARM_POST(INVALID_LOT_PRERCP_TYPE); // Alarm Post
		return SYS_ABORTED;
	}

    pdest = strstr( szParmReceived, ":" );
	if(pdest != NULL)
	{
		strcpy(szPortInfo,pdest+1);
	}

	//nRcpType = atoi(PROGRAM_PARAMETER_READ());
	nRcpType = atoi(szSpecialData);

	gPreviousBeforeLotPreType = gPreviousLotPreType;
	gPreviousLotPreType = gCurrentLotPreType;
	gCurrentLotPreType = nRcpType;

	LOG(pszFtnName, ELT_File, "[FUNCTION] gPreviousBeforeLotPreType Data is %d" , gPreviousBeforeLotPreType);
	LOG(pszFtnName, ELT_File, "[FUNCTION] gPreviousLotPreType Data is %d" , gPreviousLotPreType);	
	LOG(pszFtnName, ELT_File, "[FUNCTION] gCurrentLotPreType Data is %d" , gCurrentLotPreType);	

	switch( nRcpType ) {
	case 0 :
		PreRecipeioindexU = PM1_PreRecipeUseA;
		PreRecipeioindex  = PM1_PreRecipeA;
		break;
	case 1 :
		PreRecipeioindexU = PM1_PreRecipeUseA;
		PreRecipeioindex  = PM1_PreRecipeA;
		break;
	case 2 :
		PreRecipeioindexU = PM1_PreRecipeUseB;
		PreRecipeioindex  = PM1_PreRecipeB;
		break;
	case 3 :
		PreRecipeioindexU = PM1_PreRecipeUseC;
		PreRecipeioindex  = PM1_PreRecipeC;
		break;
	case 4 :
		PreRecipeioindexU = PM1_PreRecipeUseD;
		PreRecipeioindex  = PM1_PreRecipeD;
		break;
	case 5 :
		PreRecipeioindexU = PM1_PreRecipeUseE;
		PreRecipeioindex  = PM1_PreRecipeE;
		break;
	default:
		ALARM_POST(INVALID_LOT_PRERCP_TYPE); // Alarm Post
		return SYS_ABORTED;	
		break;
	}

	//nLotPreOpt = READ_DIGITAL( LotPreOption , &CommStatus );
	printf(" /////////////////  szPortInfo=>%s \n",szPortInfo);

	///////// Add CM# LotPre Type ////////////////////////////////////////
	if(0 == strcmp(szPortInfo,"CM1"))
	{
		gCM1LotPreType = nRcpType;
		LOG(pszFtnName, ELT_File, "[FUNCTION] gCM1LotPreType is %d" , gCM1LotPreType);	
	}
	else if(0 == strcmp(szPortInfo,"CM2"))
	{
		gCM2LotPreType = nRcpType;
		LOG(pszFtnName, ELT_File, "[FUNCTION] gCM2LotPreType is %d" , gCM2LotPreType);
	}
	else if(0 == strcmp(szPortInfo,"CM3"))
	{
		gCM3LotPreType = nRcpType;
		LOG(pszFtnName, ELT_File, "[FUNCTION] gCM3LotPreType is %d" , gCM3LotPreType);
	}
	///////// Add CM# LotPre Type ////////////////////////////////////////

	nLotPreOpt = GetLotPreOption(szPortInfo); // Force Lot Pre Option
	LOG(pszFtnName, ELT_File, "[FUNCTION] GetLotPreOption(szPortInfo) is %d" , nLotPreOpt);	
	switch( nLotPreOpt ) 
	{
		case 0 :
			sprintf( szLotPre , "");
			break;
		case 1 :
			sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY");
			break;
		case 2 :
			sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY2");
			break;
		case 3 :
			sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY3");
			break;
		case 4 :
			sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY4");
			break;
		case 5 :
			sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY5");
			break;
		case 6 :
			sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY6");
			break;
		case 7 :
			sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY7");
			break;
		case 8 :
			sprintf( szLotPre , "DEFAULT_RECIPE_CHANGE READY8");
			break;
		default:
			ALARM_POST(INVALID_LOTPRE_OPTION); // Alarm Post
			return SYS_ABORTED;	
	}
	
	if(nRcpType > 0 && nRcpType < 6)
	{
		for ( i = 0 ; i < 5 ; i++ ) 
		{
			strcpy(Buffer,"");
			strcpy(PreRecipeName,"");
			strcpy(PreRecipeNameReal,"");
			
			data = READ_DIGITAL( PreRecipeioindexU + i , &CommStatus );
			READ_STRING( PreRecipeioindex + i , PreRecipeName , &CommStatus );

			//GetRecipeName(PreRecipeName,PreRecipeNameReal);
			strcpy(PreRecipeNameReal,PreRecipeName);

			// 2005.03.22 It is added for partial blocking.  It'll be removed after full test 
			if(( 2 == nLotPreOpt ) || ( 4 == nLotPreOpt) || ( 6 == nLotPreOpt) || ( 8 == nLotPreOpt)) data = 0;
			// 2005.03.22 It is added for partial blocking.  It'll be removed after full test

			if ( data == 0 ) {
				sprintf( Buffer , "|PM%d|" , i + 1 ); // In case of Not Use
			}
			else {
				  sprintf( Buffer , "|PM%d|%s" , i + 1 , PreRecipeNameReal );
			}

			strcpy(szLotPreFull,szLotPre); 
			strcat(szLotPreFull,Buffer); // Add Lot Pre Option && |PM#|PreRecipeName

			//////////////////////////////////////////////////////////
			LOG(pszFtnName, ELT_File, "Lot Pre RcpType %d ",nRcpType );
			LOG(pszFtnName, ELT_File, szLotPreFull);
			//_sleep(120);
			Sleep(10);
			//////////////////////////////////////////////////////////

			WRITE_STRING( SCHEDULER_S , szLotPreFull , &CommStatus );

			nCounter = 0;
			while( TRUE ) 
			{
				if ( READ_DIGITAL( SCHEDULER_D , &CommStatus ) != SYS_RUNNING ) break;
				_sleep(10);
				nCounter += 10;
				if(nCounter > 10000)
				{
					ALARM_POST(LOT_PRE_START_TIMEOUT); // Alarm Post
					return SYS_ABORTED;	
				}
			}
		}			
	}
	else
	{
		for ( i = 0 ; i < 5 ; i++ ) 
		{
			strcpy(Buffer,"");
			strcpy(szLotPre,"DEFAULT_RECIPE_CHANGE READY2");
			strcpy(PreRecipeName,"");
			strcpy(PreRecipeNameReal,"");

			READ_STRING( PreRecipeioindex + i , PreRecipeName , &CommStatus );

			//GetRecipeName(PreRecipeName,PreRecipeNameReal);
			strcpy(PreRecipeNameReal,PreRecipeName);

			sprintf( Buffer , "|PM%d|" , i + 1 ); // In case of Not Use
			
			strcpy(szLotPreFull,szLotPre); 
			strcat(szLotPreFull,Buffer); // Add Lot Pre Option && |PM#|PreRecipeName
			
			//////////////////////////////////////////////////////////
			LOG(pszFtnName, ELT_File, "Lot Pre RcpType %d ",nRcpType );
			LOG(pszFtnName, ELT_File, szLotPreFull);
			_sleep(120);
			//////////////////////////////////////////////////////////

			WRITE_STRING( SCHEDULER_S , szLotPreFull , &CommStatus );
			nCounter = 0;
			while( TRUE ) 
			{
				if ( READ_DIGITAL( SCHEDULER_D , &CommStatus ) != SYS_RUNNING ) break;
				_sleep(10);
				nCounter += 10;
				if(nCounter > 10000)
				{
					ALARM_POST(LOT_PRE_START_TIMEOUT); // Alarm Post
					return SYS_ABORTED;	
				}
			}
		}			
	}

/*
	data = 0;   <=================
//	0  :	Thread.Normal
//	1  :	Depand.Normal
//	2  :	Thread(R).Normal
//	3  :	Thread.Balance
//	4  :	Depand.Balance
//	5  :	Thread(R).Balance
//	6  :	Thread.Normal.LT
//	7  :	Depand.Normal.LT
//	8  :	Thread(R).Norml.LT
//	9  :	Thread.Balance.LT
//	10 :	Depand.Balance.LT
//	11 :	Thread(R).Balan.LT
	sprintf( Buffer , "SET_PREPOST_PROCESS_DEPAND %d" , data );

    WRITE_STRING( SCHEDULER_S , Buffer , &CommStatus );
	while( TRUE ) {
		if ( READ_DIGITAL( SCHEDULER_D , &CommStatus ) != SYS_RUNNING ) break;
		_sleep(1);
	}
*/
	LOG(pszFtnName, ELT_File, "////////// Lot Pre Pass Sequence Leaved ////////////////////");
	printf(" ====>  Function_Dll_Set Returned <==== \n");
	return SYS_SUCCESS;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {

	printf( "Dll_Set.Function check Option IO\n" );
	OptionalIOCheck();

	return TRUE;
}

void OptionalIOCheck()
{
	BOOL FileEnd = TRUE;
	int Line, ReadCnt, CommStatus = 0;
	FILE *fpt = NULL;
	char Buffer[256] = {0,};
	char chr_return1[256] = {0,};
	char chr_return2[256] = {0,};
		
	if ( ( fpt = fopen( "System/Equipment_Set.cfg", "r+t") ) == NULL ) 
	{
		printf( "Cannot Find Equipment_Set.cfg File\nSelect Equipment Model Again, Please!!" );
		MessageBox( GetTopWindow( NULL ) , "Cannot Find Equipment_Set.cfg File!!\nSelect Equipment Model Again, Please!!" , "ERROR" , MB_ICONHAND );
	}
	else
	{
		for ( Line = 1; FileEnd; Line++ )
		{
			FileEnd = Get_Line_String_From_File( fpt, Buffer, &ReadCnt );
			if ( ReadCnt > 0 )
			{
				if ( !Get_Data_From_String( Buffer , chr_return1 , 0 ) ) 
				{
					printf( "Unknown Error 1 - [%s]-[%d]\n" , Buffer , Line );
				}
				else
				{
					if (STRCMP_L( chr_return1 , "ATM_COMM_TYPE:" ) )
					{
						if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) )
						{
							printf( "Unknown Error 2 - [%s]-[%d]\n" , Buffer , Line);
						}
						else
						{
							if( atoi(chr_return2) == 1 )
							{
								WRITE_DIGITAL( Robot_Control , RS_No_0 , &CommStatus );
								printf("#--------------------------------\n");
								printf("#-ATM_COMM_TYPE : RS232_NO ALIGN-\n");
								printf("#--------------------------------\n");
							}
							else if( atoi(chr_return2) == 2 )
							{
								WRITE_DIGITAL( Robot_Control , RS_Align_1 , &CommStatus );
								printf("#--------------------------------\n");
								printf("#--ATM_COMM_TYPE : RS232_ALIGN---\n");
								printf("#--------------------------------\n");
							}
							else if( atoi(chr_return2) == 3 )
							{
								WRITE_DIGITAL( Robot_Control , ENT_No_2 , &CommStatus );
								printf("#--------------------------------\n");
								printf("#--ATM_COMM_TYPE : RS232_ALIGN---\n");
								printf("#--------------------------------\n");
							}
							else if( atoi(chr_return2) == 4 )
							{
								WRITE_DIGITAL( Robot_Control , ENT_Align_3 , &CommStatus );
								printf("#--------------------------------\n");
								printf("#--ATM_COMM_TYPE : RS232_ALIGN---\n");
								printf("#--------------------------------\n");
							}
						}
					}
					else if (STRCMP_L( chr_return1 , "N2PURGE_USE:" ) )
					{
						if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) )
						{
							printf( "Unknown Error 2 - [%s]-[%d]\n" , Buffer , Line);
						}
						else
						{
							if( atoi(chr_return2) == 1 )
							{
								WRITE_DIGITAL( N2Purge_Control , Off_1 , &CommStatus );
								printf("#--------------------------------\n");
								printf("#-ATM_COMM_TYPE : N2 Purge Use---\n");
								printf("#--------------------------------\n");
							}
							else if( atoi(chr_return2) == 2 )
							{
								WRITE_DIGITAL( N2Purge_Control , On_0 , &CommStatus );
								printf("#--------------------------------\n");
								printf("#-ATM_COMM_TYPE : N2 Purge No Use\n");
								printf("#--------------------------------\n");
							}
						}
					}
				}
			}
		}
		fclose( fpt );
	}
}