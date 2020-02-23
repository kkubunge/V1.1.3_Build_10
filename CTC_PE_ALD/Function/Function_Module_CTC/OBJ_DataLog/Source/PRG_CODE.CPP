
#include "stdafx.h"
#include "TextParser.h"
#include "TextLogDll.h"
#include "DataLogger.h"
//------------------------------------------------------------------------------------------
// Initialize Function
#define PMC_DATALOG_CONFIG_FILE		".\\Function\\Function_Module_CTC\\OBJ_Datalog\\PMCDataLogConfig.ini"

typedef struct tagLOGGERLIST
{
	char szPMName[32];
	CDataLogger* pDataLogger;
	BOOL bLogStatus;
}LOGGERLIST;


LOGGERLIST g_LoggerList[5];
char       g_szFilePath[128];

int  g_nPMNumber = 0;
int  g_nPMCount = 0;
char g_szPMNumber[64];

BOOL g_bInit    = FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	//
		""
};


extern "C" void Screen_Nav_Info( int no )
{

}

extern "C" BOOL Program_Init_Code()
{
	
	return TRUE;
}

extern "C" void Program_Enter_Code()
{
	
}

/*
 *	2006.12.17 by ycchoi < 1.0.0.9 >
 *
 *	Chamber Condition Control Function에서 Dry Cleaning Recipe 종료시 마다 종료 Event를
 *	Manual Log Function에 Write해 준다. Event를 Parsng해서 해당 PM의 Logging을 종료한다.
 */
extern "C" void Event_Message_Received()
{
	string strEvent;
	strEvent = PROGRAM_EVENT_READ();

	printf(">>> MANUAL LOG EVENT Received[%s]\n", strEvent.c_str());

	// 2013.01.17 Manual Log Function Separated PM Part by hyuk
	// for(int i=0; i<g_nPMCount; i++)
	{
		int i = g_nPMNumber;
		if(strEvent.compare(0, 3, g_LoggerList[i].szPMName)==0)
		{
			g_LoggerList[i].pDataLogger->StopLogging();
			return;
		}
	}
}


void InitializePMList()
{
	for(int i = 0 ; i < 5 ; i++)
	{
		memset(g_LoggerList[i].szPMName , 0 , sizeof(g_LoggerList[i].szPMName));
		g_LoggerList[i].pDataLogger = NULL;
		g_LoggerList[i].bLogStatus  = FALSE;
	}	

	memset(g_szFilePath , 0 , sizeof(g_szFilePath));

	g_nPMCount = 0;
}


BOOL InitializeDataLogger()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead    [256];
	char szItem    [256];
	char szIOName  [32];
	char szIOType  [8];
	char szFilePath[128];
	char szRcpType[40];

	int  nIOType       = 0;
	int  nLogPeriod    = 1000;
	int  nDeletePeriod = 30;
	bool bLogOption    = FALSE;	
	char szPM[8]       = {0};
	bool abRcpLogEnable[9] = { false, true, false, false, false, true, false, true, false };

	InitializePMList();

	do {
		fp = fopen(PMC_DATALOG_CONFIG_FILE, "rt");
		if(fp == NULL)
		{
			printf("file %s open fail \n" , PMC_DATALOG_CONFIG_FILE);
			break;
		}
		if(NULL == fgets(szRead, 255, fp))
		{
			printf("file %s fgets fail \n" , PMC_DATALOG_CONFIG_FILE);
			break;
		}

		// printf("Reading --> %s \n" , szRead);
		do
		{
			CTextParser tp(szRead);
			tp.GetWord(szItem, 0);
			// printf("Reading --> %s \n" , szItem);

			if(szRead[0] == ';' || szRead[0] == '#' || szRead[0] == '\n' || szRead[0] == '\t')
			{
			}
			else if(0 == strcmp(szItem , "$_FILE_PATH"))
			{
				tp.GetString(szFilePath);
				strcpy(g_szFilePath , szFilePath);
				// printf("File Path %s \n" , g_szFilePath);
				//tp.TrimLeft(szFilePath);
			}
			else if(szItem[0] == '[')
			{
				tp.SetText(szItem);
				tp.GetChar();
				tp.GetString(szItem, ']');
				// printf("Reading --> %s \n" , szItem);

				if((strcmp("PM1", szItem) != 0) && (strcmp("PM2" , szItem) != 0) && (strcmp("PM3" , szItem) != 0)
				   //&& (strcmp("PM4" , szItem) != 0) && (strcmp("PM5" , szItem) != 0))
				   && (strcmp("PM4" , szItem) != 0) && (strcmp("PM5" , szItem) != 0) && (strcmp("PMC" , szItem) != 0) )		// for PMC version
				{
				   printf("Invalide PM configuration comming  \n");
				   return FALSE;
				}
				else
				{
					strcpy(szPM , szItem);
					// printf("Reading --> %s \n" , szPM);
				}
			}
			else if(0 == strcmp(szItem , "$_LOG_OPTION"))
			{
				tp.GetBool(&bLogOption , 0);
			}
			else if(0 == strcmp(szItem , "$_LOG_PERIOD"))
			{
				tp.GetInt(&nLogPeriod , 0);
				if(nLogPeriod < 100)
					nLogPeriod = 100;
			}
			else if(0 == strcmp(szItem , "$_FILE_DELETE"))
			{
				tp.GetInt(&nDeletePeriod , 0);
				if(nDeletePeriod <= 0)
					nDeletePeriod = 10;
			}
			// 2007.07.19
			else if(0 == strcmp(szItem , "$_LOG_ENABLE"))
			{
				tp.GetWord(szRcpType);
				if(0 == strcmp(szRcpType, "RT_ProcRcp")) tp.GetBool(&abRcpLogEnable[RT_ProcRcp]);
				else if(0 == strcmp(szRcpType, "RT_PreProRcp")) tp.GetBool(&abRcpLogEnable[RT_PreProRcp]);
				else if(0 == strcmp(szRcpType, "RT_PostProRcp")) tp.GetBool(&abRcpLogEnable[RT_PostProRcp]);
				else if(0 == strcmp(szRcpType, "RT_IdleRcp")) tp.GetBool(&abRcpLogEnable[RT_IdleRcp]);
				else if(0 == strcmp(szRcpType, "RT_CleanRcp")) tp.GetBool(&abRcpLogEnable[RT_CleanRcp]);
				else if(0 == strcmp(szRcpType, "RT_PurgeRcp")) tp.GetBool(&abRcpLogEnable[RT_PurgeRcp]);
				else if(0 == strcmp(szRcpType, "RT_LotPreRcp")) tp.GetBool(&abRcpLogEnable[RT_LotPreRcp]);
				else if(0 == strcmp(szRcpType, "RT_LotPostRcp")) tp.GetBool(&abRcpLogEnable[RT_LotPostRcp]);
			}
			else if(0 == strcmp(szItem , "$_IO_START"))
			{

				//Create Logger
				//g_LoggerList[g_nPMCount].pDataLogger = new CDataLogger(szPM , nLogPeriod , bLogOption ,nDeletePeriod, g_szFilePath);
				//memcpy(g_LoggerList[g_nPMCount].pDataLogger->m_abRcpLogEnable, abRcpLogEnable, sizeof(abRcpLogEnable));
				//strcpy(g_LoggerList[g_nPMCount].szPMName, strupr(szPM));

				// 2013.01.17 Manual Log Function Separated PM Part by hyuk
				strcpy(g_LoggerList[g_nPMNumber].szPMName, strupr(szPM));
				if(strcmp(strupr(szPM), g_szPMNumber) == 0)
				{

				g_LoggerList[g_nPMNumber].pDataLogger = new CDataLogger(szPM , nLogPeriod , bLogOption ,nDeletePeriod, g_szFilePath);
				memcpy(g_LoggerList[g_nPMNumber].pDataLogger->m_abRcpLogEnable, abRcpLogEnable, sizeof(abRcpLogEnable));

				printf("szPM : %s, nLogPeriod : %d, bLogOption : %d, nDeletePeriod : %d, FilePath : %s\n", szPM, nLogPeriod, bLogOption, nDeletePeriod, g_szFilePath);

					do
					{
						if(NULL == fgets(szRead, 255, fp))
						{
							printf("fgets fail --> %s \n" , szRead);
							break;
						}
						// printf("Reading --> %s \n" , szRead);

						CTextParser tp1(szRead);
						tp1.GetWord(szIOName, 0);
						
						if(szIOName[0] == ';' || szIOName[0] == '#' || szIOName[0] == '\n' || szIOName[0] == '\t' || szIOName[0] == NULL)
						{
						}
						else if(0 == strcmp(szIOName , "$_IO_END"))
						{
							g_nPMCount++;
							break;
						}
						else
						{
							tp1.GetWord(szIOType , 0);

							if(szIOType[0] == 'A')
							{
								nIOType = _K_A_IO;
							}
							else if(szIOType[0] == 'S')
							{
								nIOType = _K_S_IO;
							}
							else
							{
								nIOType = _K_D_IO;
							}

							//if(!g_LoggerList[g_nPMCount].pDataLogger->SetLogItem(szIOName , nIOType))
							if(!g_LoggerList[g_nPMNumber].pDataLogger->SetLogItem(szIOName , nIOType))
							{
								printf("Invalid Log Item %s Comming.. \n" , szIOName);
							}
							else
							{
								//printf("IO : %s.%s IO Type %s , Option %d , Log Period %d Define Success.. \n" , szPM , szIOName, szIOType , bLogOption , nLogPeriod);
							}
						}

					}while(feof(fp) == 0);
				}
			}

			if(NULL == fgets(szRead, 255, fp)) //end of file
				break;

		} while(feof(fp) == 0);

		fclose(fp);
		bRet = TRUE;
	} while(0);
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : %s", PMC_DATALOG_CONFIG_FILE);
	}
	return bRet;



}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main()
{
	// printf("Manual Data Log Function Entered\n");

	int nIOStatus;
	int nValue;

	char szParam[64];
	strcpy(szParam, PROGRAM_ARGUMENT_READ());

	if(szParam[2] == '1'){
		g_nPMNumber = 1;		
	}else if(szParam[2] == '2'){
		g_nPMNumber = 2;
	}else if(szParam[2] == '3'){
		g_nPMNumber = 3;
	}else if(szParam[2] == '4'){
		g_nPMNumber = 4;
	}else if(szParam[2] == '5'){
		g_nPMNumber = 5;

	}else{
		printf("Argument is invalid. So Manual Log Function is not created : %s\n", szParam);
		return SYS_ABORTED;
	}
	strcpy(g_szPMNumber, szParam);

	if(FALSE == g_bInit)
	{
		if(FALSE == InitializeDataLogger())
			return SYS_ABORTED;
		else
			g_bInit = TRUE;
	}

	//for(int i = 0 ; i < g_nPMCount ; i++)
	{
		int i = g_nPMNumber;

		if(g_LoggerList[i].pDataLogger->GetItemCount() <= 0)
		{
			delete g_LoggerList[i].pDataLogger;
			g_LoggerList[i].pDataLogger = NULL;
		}
		else
			g_LoggerList[i].pDataLogger->Initialize();
	}

	while(TRUE)
	{
		//Logging Monitoring Control
		WAIT_SECONDS(1.5);

		//for(int j = 0 ; j < g_nPMCount ; j++)
		{
			int j = g_nPMNumber;

			if(NULL != g_LoggerList[j].pDataLogger && g_LoggerList[j].pDataLogger->GetRunStsIOPoint() != -1)
			{
				nValue = _dREAD_DIGITAL(g_LoggerList[j].pDataLogger->GetRunStsIOPoint() , &nIOStatus);

				/*
				 *	2006.12.17 by ycchoi < 1.0.0.9 >
				 *	
				 *	Logging Stop 조건 변경
				 */
				// if(0 == nValue && g_LoggerList[j].bLogStatus == TRUE)
				if(0 == nValue && !g_LoggerList[j].pDataLogger->IsLoggingStartReady())
				{
//					printf("[%s] Stop Main Logging \n", g_LoggerList[j].szPMName);
					g_LoggerList[j].pDataLogger->StopLogging();
					g_LoggerList[j].bLogStatus = FALSE;
				}
				/*
				 *	2006.12.17 by ycchoi < 1.0.0.9 >
				 *
				 *	Logging Start 조건 변경
				 */
				// else if(1 == nValue && g_LoggerList[j].bLogStatus == FALSE)
				else if(1 == nValue && g_LoggerList[j].pDataLogger->IsLoggingStartReady())
				{
//					printf("[%s] Start Main Logging \n", g_LoggerList[j].szPMName);
					g_LoggerList[j].pDataLogger->StartLogging();
					g_LoggerList[j].bLogStatus = TRUE;
				}

			}
			else if(NULL != g_LoggerList[j].pDataLogger)
			{
				// printf("%s Log System will not execute properly due to invalid iopoint" , g_LoggerList[j].szPMName);
			}			
		}
	}	

	return SYS_SUCCESS;
}

