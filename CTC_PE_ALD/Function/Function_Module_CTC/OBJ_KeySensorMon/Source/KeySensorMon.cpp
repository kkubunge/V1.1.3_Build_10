// LotFlowControl.cpp: implementation of the CLotFlowControl class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

//--------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//--------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "Timer.h"
#include "TextParser.h"
#include "KeySensorMon.h"


//--------------------------------------------------------------------
enum { EN_Alarm, EN_Normal };

enum {
	PMC_Unuse, PMC_Local, PMC_DisCon,  PMC_Standby, PMC_Maint,
	PMC_Uninit, PMC_EnablePM, PMC_CTCInUse, PMC_CTCDis, PMC_CTCDis2,
	PMC_CTCEnaPM, PMC_CTCDisHW, PMC_CTCDisHW2
};

enum { LOTSM_Conc, LOTSM_Cont, LOTSM_Seq, LOTSM_None };
enum { LOTC_None, LOTC_AutoLotPause };
enum { MANUAL, AUTO, LI_MAN } ;
enum { PM_PATH_NONE, PM_PATH_PM1, PM_PATH_PM2, PM_PATH_PM1_2 };

enum { EN_PM1, EN_PM2 };
enum { EN_TM1, EN_TM2 };
enum { PROSTS_Idle, PROSTS_Running, PROSTS_Aborted, PROSTS_SeqErr, PROSTS_LotPreEnd, PROSTS_LotPstEnd };
enum { PMFLC_None, PMFLC_Break };

enum { EN_OFF, EN_ON };

enum { LLVAC_ATM, LLVAC_Vac, LLVAC_GoATM, LLVAC_GoVac, LLVAC_Abort };

enum { RCP_STOP, RCP_RUNNING};

enum { WFR_NONE, WFR_PRESENT, WFR_SUCCESS, WFR_FAIL, WFR_PROCSSING};
enum { WFR_SRC_NONE, WFR_SRC_A, WFR_SRC_B, WFR_SRC_C, WFR_SRC_X, WFR_SRC_Y, WFR_SRC_D,};

enum { ATM, VACUUM, GoATM, GoVacuum, Vac_Abort};
enum { Idle_ATM, Idle_VACUUM, Run_GoATM, Run_GoVacuum, PumpAbort};

int g_KeySenosrEFEM_WaferSts_old[MAX_WFR_EFEM] = {0,0,0};
int g_KeySenosrLL_WaferSts_old[MAX_WFR_LL] = {0,0,0,0,0,0,0,0};
int g_KeySenosrTM_WaferSts_old[MAX_WFR_TM] = {0,0,0,0};

char g_szLotID[20];
char g_szPPID[20];
char g_szLotID_old[20];
char g_szPPID_old[20];

#define MAX_LPM				3
typedef struct _LPM_INFO
{
	char szLotID[80];
	char szPPID[80];
} LPM_INFO;

static LPM_INFO g_LPMInfo[MAX_LPM] =
{
	{ "", ""},
	{ "", ""},
	{ "", ""},	
};
/*
typedef struct _LPM_INFO_old
{
	char szLotID[80];
	char szPPID[80];
} LPM_INFO_old;

static LPM_INFO_old g_LPMInfo_old[MAX_LPM] =
{
	{ "", ""},
	{ "", ""},
	{ "", ""},	
};
*/
int g_nTMCnt = 0;
int g_nBM1Cnt = 0;
int g_nBM2Cnt = 0;
//--------------------------------------------------------------------			
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKeySensorMon::CKeySensorMon()
{
	int i;

	for(i = 0; i < MAX_WFR_EFEM; i++  )
	{
		m_KeySenosrEFEM[i].nszWaferRlt = 0;
		m_KeySenosrEFEM[i].nszWaferSrc = 0;
		m_KeySenosrEFEM[i].nszWaferSts = 0;
		strcpy(m_KeySenosrEFEM[i].szLotID,"");
		strcpy(m_KeySenosrEFEM[i].szPPID, "");
	}

	for(i = 0; i < MAX_WFR_LL; i++  )
	{
		m_KeySenosrLL[i].nszWaferRlt = 0;
		m_KeySenosrLL[i].nszWaferSrc = 0;
		m_KeySenosrLL[i].nszWaferSts = 0;
		strcpy(m_KeySenosrLL[i].szLotID,"");
		strcpy(m_KeySenosrLL[i].szPPID, "");
	}
	for(i = 0; i < MAX_WFR_TM; i++  )
	{
		m_KeySenosrTM[i].nszWaferRlt = 0;
		m_KeySenosrTM[i].nszWaferSrc = 0;
		m_KeySenosrTM[i].nszWaferSts = 0;
		strcpy(m_KeySenosrTM[i].szLotID,"");
		strcpy(m_KeySenosrTM[i].szPPID, "");
	}
	
	m_nLP1_PM1WaferCount = 0;
	m_nLP1_PM2WaferCount = 0;
	m_nLP1_PM3WaferCount = 0;

	m_nLP2_PM1WaferCount = 0;
	m_nLP2_PM2WaferCount = 0;
	m_nLP2_PM3WaferCount = 0;

	m_nLP3_PM1WaferCount = 0;
	m_nLP3_PM2WaferCount = 0;
	m_nLP3_PM3WaferCount = 0;

	
	memset(m_szLPM1PJID,0,sizeof(m_szLPM1PJID));
	memset(m_szLPM2PJID,0,sizeof(m_szLPM2PJID));
	memset(m_szLPM3PJID,0,sizeof(m_szLPM3PJID));

    memset(m_LP1_PM1WaferDCOPSUM,0,sizeof(m_LP1_PM1WaferDCOPSUM));
	memset(m_LP1_PM2WaferDCOPSUM,0,sizeof(m_LP1_PM2WaferDCOPSUM));
	memset(m_LP1_PM3WaferDCOPSUM,0,sizeof(m_LP1_PM3WaferDCOPSUM));

    memset(m_LP2_PM1WaferDCOPSUM,0,sizeof(m_LP2_PM1WaferDCOPSUM));
	memset(m_LP2_PM2WaferDCOPSUM,0,sizeof(m_LP2_PM2WaferDCOPSUM));
	memset(m_LP2_PM3WaferDCOPSUM,0,sizeof(m_LP2_PM3WaferDCOPSUM));

    memset(m_LP3_PM1WaferDCOPSUM,0,sizeof(m_LP3_PM1WaferDCOPSUM));
	memset(m_LP3_PM2WaferDCOPSUM,0,sizeof(m_LP3_PM2WaferDCOPSUM));
	memset(m_LP3_PM3WaferDCOPSUM,0,sizeof(m_LP3_PM3WaferDCOPSUM));
}

CKeySensorMon::~CKeySensorMon()
{
}

//////////////////////////////////////////////////////////////////////
//...2015.06.10
Module_Status CKeySensorMon::Main()
{

	char *pszEd;
	char szEvent[256], szBuffer1[256], szBuffer2[256], szBuffer3[256], szBuffer4[256], szBuffer5[256], szBuffer6[256], szBuffer7[256], szBuffer8[256];
	char szPortNum[256], szTimeInfo[256], szModuleName[256], szWaferNum[256], szRecipeName[256], szCJName[256], szPJName[256];
	char szPRJobSts[256], szPJID[256], szPRJobCompleteSts[256];
	char szTemp1[256],szTemp2[256];
	int nIO_Status;	
	int nPM_StartIO,nPM_EndIO,nLPM_PM1StartIO,nLPM_PM1EndIO,nLPM_PM2StartIO,nLPM_PM2EndIO,nLPM_PM3StartIO,nLPM_PM3EndIO;
	int i = 0,j = 0, k = 0;
	double dbLPMDCOP = 0;
	Module_Status msRet = SYS_SUCCESS;

	memset(szEvent,			0,  sizeof(szEvent));
	memset(szBuffer1,		0,  sizeof(szBuffer1));
	memset(szBuffer2,		0,	sizeof(szBuffer2));
	memset(szBuffer3,		0,  sizeof(szBuffer3));
	memset(szBuffer4,		0,	sizeof(szBuffer4));
	memset(szBuffer5,		0,  sizeof(szBuffer5));
	memset(szBuffer6,		0,	sizeof(szBuffer6));
	memset(szBuffer7,		0,	sizeof(szBuffer7));
	memset(szBuffer8,		0,	sizeof(szBuffer8));
	memset(szPortNum,		0,	sizeof(szPortNum));
	memset(szTimeInfo,		0,	sizeof(szTimeInfo));
	memset(szModuleName,	0,	sizeof(szModuleName));
	memset(szWaferNum,		0,	sizeof(szWaferNum));
	memset(szRecipeName,	0,	sizeof(szRecipeName));
	memset(szCJName,		0,	sizeof(szCJName));
    memset(szPJName,		0,	sizeof(szPJName));
	memset(szPRJobSts,		0,	sizeof(szPRJobSts));
	memset(szPJID,			0,	sizeof(szPJID));
	memset(szTemp1,			0,	sizeof(szTemp1));
	memset(szTemp2,			0,	sizeof(szTemp2));
    memset(szPRJobCompleteSts,		0,	sizeof(szPRJobCompleteSts));
	//.....
	pszEd = PROGRAM_PARAMETER_READ();
	STR_SEPERATE( pszEd , szEvent , szBuffer1	, 255 );

	//.....
	if(STRCMP_L(szEvent, "TIME_PROCESS_END")) 
	{
		STR_SEPERATE_CHAR(szBuffer1, '|', szPortNum,	szBuffer2,	255);	
		STR_SEPERATE_CHAR(szBuffer2, '|', szTimeInfo,	szBuffer3,	255);	
		STR_SEPERATE_CHAR(szBuffer3, '|', szModuleName,	szBuffer4,	255);	
		STR_SEPERATE_CHAR(szBuffer4, '|', szWaferNum,	szBuffer5,	255);	
		STR_SEPERATE_CHAR(szBuffer5, '|', szRecipeName,	szBuffer6,	255);
		STR_SEPERATE_CHAR(szBuffer6, '|', szTemp1,		szBuffer7,	255);
		STR_SEPERATE_CHAR(szBuffer7, '|', szTemp2,		szBuffer8,	255);
		STR_SEPERATE_CHAR(szBuffer8, '|', szCJName,		szPJName,	255);
		//...
		if(STRCMP_L(szPortNum, "PORT1")) 
		{
			strcpy(m_szLPM1PJID,szPJName);
			if(STRCMP_L(szModuleName, "PM1")) {
				nPM_StartIO = PM1_RF1FreqAvrAI;
				nPM_EndIO	= PM1_HF2Z_ScanPwrAvr;
				for(i = nPM_StartIO,j = 0; i <= nPM_EndIO; i++,j++) 					
				{
					m_LP1_PM1WaferDCOPSUM[j] += READ_ANALOG(i,&nIO_Status);
				}
				m_nLP1_PM1WaferCount++;
			}
			else if( STRCMP_L( szModuleName, "PM2" )) {
				nPM_StartIO = PM2_RF1FreqAvrAI;
				nPM_EndIO	= PM2_HF2Z_ScanPwrAvr;
				for(i = nPM_StartIO,j = 0; i <= nPM_EndIO; i++,j++) 					
				{
					m_LP1_PM2WaferDCOPSUM[j] += READ_ANALOG(i,&nIO_Status);
				}
				m_nLP1_PM2WaferCount++;
			}
			else if( STRCMP_L( szModuleName, "PM3" )) {
				nPM_StartIO = PM3_RF1FreqAvrAI;
				nPM_EndIO	= PM3_HF2Z_ScanPwrAvr;
				for(i = nPM_StartIO,j = 0; i <= nPM_EndIO; i++,j++) 					
				{
					m_LP1_PM3WaferDCOPSUM[j] += READ_ANALOG(i,&nIO_Status);
				}
				m_nLP1_PM3WaferCount++;
			}
			else	printf("===> Error : TIME_PROCESS_End Process Module Number is not Valid.\n");
		}
		else if( STRCMP_L( szPortNum, "PORT2" )) 
		{
			strcpy(m_szLPM2PJID,szPJName);
			if(STRCMP_L(szModuleName, "PM1")) {
				nPM_StartIO = PM1_RF1FreqAvrAI;
				nPM_EndIO	= PM1_HF2Z_ScanPwrAvr;
				for(i = nPM_StartIO,j = 0; i <= nPM_EndIO; i++,j++) 					
				{
					m_LP2_PM1WaferDCOPSUM[j] += READ_ANALOG(i,&nIO_Status);
				}
				m_nLP2_PM1WaferCount++;
			}
			else if( STRCMP_L( szModuleName, "PM2" )) {
				nPM_StartIO = PM2_RF1FreqAvrAI;
				nPM_EndIO	= PM2_HF2Z_ScanPwrAvr;
				for(i = nPM_StartIO,j = 0; i <= nPM_EndIO; i++,j++) 					
				{
					m_LP2_PM2WaferDCOPSUM[j] += READ_ANALOG(i,&nIO_Status);
				}
				m_nLP2_PM2WaferCount++;
			}
			else if( STRCMP_L( szModuleName, "PM3" )) {
				nPM_StartIO = PM3_RF1FreqAvrAI;
				nPM_EndIO	= PM3_HF2Z_ScanPwrAvr;
				for(i = nPM_StartIO,j = 0; i <= nPM_EndIO; i++,j++) 					
				{
					m_LP2_PM3WaferDCOPSUM[j] += READ_ANALOG(i,&nIO_Status);
				}
				m_nLP2_PM3WaferCount++;
			}
			else	printf("===> Error : TIME_PROCESS_End Process Module Number is not Valid.\n");
		}
		else if( STRCMP_L( szPortNum, "PORT3" )) 
		{
			strcpy(m_szLPM3PJID,szPJName);
			if(STRCMP_L(szModuleName, "PM1")) {
				nPM_StartIO = PM1_RF1FreqAvrAI;
				nPM_EndIO	= PM1_HF2Z_ScanPwrAvr;
				for(i = nPM_StartIO,j = 0; i <= nPM_EndIO; i++,j++) 					
				{
					m_LP3_PM1WaferDCOPSUM[j] += READ_ANALOG(i,&nIO_Status);
				}
				m_nLP3_PM1WaferCount++;
			}
			else if( STRCMP_L( szModuleName, "PM2" )) {
				nPM_StartIO = PM2_RF1FreqAvrAI;
				nPM_EndIO	= PM2_HF2Z_ScanPwrAvr;
				for(i = nPM_StartIO,j = 0; i <= nPM_EndIO; i++,j++) 					
				{
					m_LP3_PM2WaferDCOPSUM[j] += READ_ANALOG(i,&nIO_Status);
				}
				m_nLP3_PM2WaferCount++;
			}
			else if( STRCMP_L( szModuleName, "PM3" )) {
				nPM_StartIO = PM3_RF1FreqAvrAI;
				nPM_EndIO	= PM3_HF2Z_ScanPwrAvr;
				for(i = nPM_StartIO,j = 0; i <= nPM_EndIO; i++,j++) 					
				{
					m_LP3_PM3WaferDCOPSUM[j] += READ_ANALOG(i,&nIO_Status);
				}
				m_nLP3_PM3WaferCount++;

			}
			else	printf("===> Error : TIME_PROCESS_End Process Module Number is not Valid.\n");
		}
		else	printf("===> Error : TIME_PROCESS_START Load Port Number is not Valid.\n");

	}
	else if( STRCMP_L( szEvent, "PROCESSJOB" )) 
	{
		STR_SEPERATE_CHAR( szBuffer1 , '|' , szPRJobSts			,  szBuffer2	, 255 );
		STR_SEPERATE_CHAR( szBuffer2 , '|' , szPJID				,  szBuffer3	, 255 );
		STR_SEPERATE_CHAR( szBuffer3 , '|' , szPRJobCompleteSts ,  szBuffer4	, 255 );	
		
		if(STRCMP_L( szPRJobSts, "FINISH" ))
		{
			if( STRCMP_L( m_szLPM1PJID, szPJID )) {	
				nLPM_PM1StartIO = PM1_RF1FreqLAvrAI;
				nLPM_PM1EndIO	= PM1_HF2Z_ScanPwrLAvr;
				for(i = nLPM_PM1StartIO,j = 0; i <= nLPM_PM1EndIO; i++,j++) 					
				{
					if(m_LP1_PM1WaferDCOPSUM[j] != 0) dbLPMDCOP = m_LP1_PM1WaferDCOPSUM[j]/m_nLP1_PM1WaferCount;
					else dbLPMDCOP = 0;
					WRITE_ANALOG(i,dbLPMDCOP,&nIO_Status);
					m_LP1_PM1WaferDCOPSUM[j] = 0;
				}
				m_nLP1_PM1WaferCount = 0;
				nLPM_PM2StartIO = PM2_RF1FreqLAvrAI;
				nLPM_PM2EndIO	= PM2_HF2Z_ScanPwrLAvr;
				for(i = nLPM_PM2StartIO,j = 0; i <= nLPM_PM2EndIO; i++,j++) 					
				{
					if(m_LP1_PM2WaferDCOPSUM[j] != 0) dbLPMDCOP = m_LP1_PM2WaferDCOPSUM[j]/m_nLP1_PM2WaferCount;
					else dbLPMDCOP = 0;
					WRITE_ANALOG(i,dbLPMDCOP,&nIO_Status);
					m_LP1_PM2WaferDCOPSUM[j] = 0;
				}
				m_nLP1_PM2WaferCount = 0;
				nLPM_PM3StartIO = PM3_RF1FreqLAvrAI;
				nLPM_PM3EndIO	= PM3_HF2Z_ScanPwrLAvr;
				for(i = nLPM_PM3StartIO,j = 0; i <= nLPM_PM3EndIO; i++,j++) 					
				{
					if(m_LP1_PM3WaferDCOPSUM[j] != 0) dbLPMDCOP = m_LP1_PM3WaferDCOPSUM[j]/m_nLP1_PM3WaferCount;
					else dbLPMDCOP = 0;
					WRITE_ANALOG(i,dbLPMDCOP,&nIO_Status);
					m_LP1_PM3WaferDCOPSUM[j] = 0;
				}
				m_nLP1_PM3WaferCount = 0;
			}
			else if( STRCMP_L( m_szLPM2PJID, szPJID )) {
				nLPM_PM1StartIO = PM1_RF1FreqLAvrAI;
				nLPM_PM1EndIO	= PM1_HF2Z_ScanPwrLAvr;
				for(i = nLPM_PM1StartIO,j = 0; i <= nLPM_PM1EndIO; i++,j++) 					
				{
					if(m_LP2_PM1WaferDCOPSUM[j] != 0) dbLPMDCOP = m_LP2_PM1WaferDCOPSUM[j]/m_nLP2_PM1WaferCount;
					else dbLPMDCOP = 0;
					WRITE_ANALOG(i,dbLPMDCOP,&nIO_Status);
					m_LP2_PM1WaferDCOPSUM[j] = 0;
				}
				m_nLP2_PM1WaferCount = 0;
				nLPM_PM2StartIO = PM2_RF1FreqLAvrAI;
				nLPM_PM2EndIO	= PM2_HF2Z_ScanPwrLAvr;
				for(i = nLPM_PM2StartIO,j = 0; i <= nLPM_PM2EndIO; i++,j++) 					
				{
					if(m_LP2_PM2WaferDCOPSUM[j] != 0) dbLPMDCOP = m_LP2_PM2WaferDCOPSUM[j]/m_nLP2_PM2WaferCount;
					else dbLPMDCOP = 0;
					WRITE_ANALOG(i,dbLPMDCOP,&nIO_Status);
					m_LP2_PM2WaferDCOPSUM[j] = 0;
				}
				m_nLP2_PM2WaferCount = 0;
				nLPM_PM3StartIO = PM3_RF1FreqLAvrAI;
				nLPM_PM3EndIO	= PM3_HF2Z_ScanPwrLAvr;
				for(i = nLPM_PM3StartIO,j = 0; i <= nLPM_PM3EndIO; i++,j++) 					
				{
					if(m_LP2_PM3WaferDCOPSUM[j] != 0) dbLPMDCOP = m_LP2_PM3WaferDCOPSUM[j]/m_nLP2_PM3WaferCount;
					else dbLPMDCOP = 0;
					WRITE_ANALOG(i,dbLPMDCOP,&nIO_Status);
					m_LP2_PM3WaferDCOPSUM[j] = 0;
				}
				m_nLP2_PM3WaferCount = 0;
			}
			else if( STRCMP_L( m_szLPM3PJID, szPJID )) {
				nLPM_PM1StartIO = PM1_RF1FreqLAvrAI;
				nLPM_PM1EndIO	= PM1_HF2Z_ScanPwrLAvr;
				for(i = nLPM_PM1StartIO,j = 0; i <= nLPM_PM1EndIO; i++,j++) 					
				{
					if(m_LP3_PM1WaferDCOPSUM[j] != 0) dbLPMDCOP = m_LP3_PM1WaferDCOPSUM[j]/m_nLP3_PM1WaferCount;
					else dbLPMDCOP = 0;
					WRITE_ANALOG(i,dbLPMDCOP,&nIO_Status);
					m_LP3_PM1WaferDCOPSUM[j] = 0;
				}
				m_nLP3_PM1WaferCount = 0;
				nLPM_PM2StartIO = PM2_RF1FreqLAvrAI;
				nLPM_PM2EndIO	= PM2_HF2Z_ScanPwrLAvr;
				for(i = nLPM_PM2StartIO,j = 0; i <= nLPM_PM2EndIO; i++,j++) 					
				{
					if(m_LP3_PM2WaferDCOPSUM[j] != 0) dbLPMDCOP = m_LP3_PM2WaferDCOPSUM[j]/m_nLP3_PM2WaferCount;
					else dbLPMDCOP = 0;
					WRITE_ANALOG(i,dbLPMDCOP,&nIO_Status);
					m_LP3_PM2WaferDCOPSUM[j] = 0;
				}
				m_nLP3_PM2WaferCount = 0;
				nLPM_PM3StartIO = PM3_RF1FreqLAvrAI;
				nLPM_PM3EndIO	= PM3_HF2Z_ScanPwrLAvr;
				for(i = nLPM_PM3StartIO,j = 0; i <= nLPM_PM3EndIO; i++,j++) 					
				{
					if(m_LP3_PM3WaferDCOPSUM[j] != 0) dbLPMDCOP = m_LP3_PM3WaferDCOPSUM[j]/m_nLP3_PM3WaferCount;
					else dbLPMDCOP = 0;
					WRITE_ANALOG(i,dbLPMDCOP,&nIO_Status);
					m_LP3_PM3WaferDCOPSUM[j] = 0;
				}

				m_nLP3_PM3WaferCount = 0;
			}
			else	printf("===> Error : Process Job Name is not Valid.\n");
		}
	}
	else	printf("===> Error : GEM_WaferZoneID_Monitor is not Valid.\n");
	

	return msRet;
}

void CKeySensorMon::Enter_Code()
{
	char szParam[256];

	strcpy(szParam, PROGRAM_ARGUMENT_READ());
}

BOOL CKeySensorMon::Initialize()
{
	BOOL bRet;
	bRet = CSeqInterface::Initialize();

	CreateThread();
	return bRet;
}

void CKeySensorMon::Check_EFEM_KeySensor(int nMAX_WFR_EFEM)
{
	int i;
	int nIOStatus;

	for(i = 0; i < nMAX_WFR_EFEM; i++)
	{
		m_KeySenosrEFEM[i].nszWaferSts = READ_DIGITAL(CTC_FA_Wafer_Status + i,&nIOStatus);
		m_KeySenosrEFEM[i].nszWaferSrc = READ_DIGITAL(CTC_FA_Wafer_Source + i,&nIOStatus) + 1;

		switch(m_KeySenosrEFEM[i].nszWaferSrc)
		{
			case WFR_SRC_A:
				strcpy(m_KeySenosrEFEM[i].szLotID,g_LPMInfo[WFR_SRC_A-1].szLotID); 
				strcpy(m_KeySenosrEFEM[i].szPPID,g_LPMInfo[WFR_SRC_A-1].szPPID);
				break;
			case WFR_SRC_B:
				strcpy(m_KeySenosrEFEM[i].szLotID,g_LPMInfo[WFR_SRC_B-1].szLotID); 
				strcpy(m_KeySenosrEFEM[i].szPPID,g_LPMInfo[WFR_SRC_B-1].szPPID);
				break;
			case WFR_SRC_C:
				strcpy(m_KeySenosrEFEM[i].szLotID,g_LPMInfo[WFR_SRC_C-1].szLotID); 
				strcpy(m_KeySenosrEFEM[i].szPPID,g_LPMInfo[WFR_SRC_C-1].szPPID);
				break;
			default:
				strcpy(m_KeySenosrEFEM[i].szLotID,""); 
				strcpy(m_KeySenosrEFEM[i].szPPID,"");
				break;
		}
	}

	for(i = 0; i < nMAX_WFR_EFEM; i++)
	{
		if(g_KeySenosrEFEM_WaferSts_old[i] == m_KeySenosrEFEM[i].nszWaferSts) continue;
		if(m_KeySenosrEFEM[i].nszWaferSts == 0)
		{
			WRITE_DIGITAL(FA_Wafer_SourceDM + i,WFR_SRC_NONE,&nIOStatus);
			WRITE_STRING(FA_Wafer_PPIDSM + i,"",&nIOStatus);
			WRITE_STRING(FA_Wafer_LOTIDSM + i,"",&nIOStatus);
		}
		else
		{	
			WRITE_DIGITAL(FA_Wafer_SourceDM + i,m_KeySenosrEFEM[i].nszWaferSrc,&nIOStatus);
			WRITE_STRING(FA_Wafer_PPIDSM + i,m_KeySenosrEFEM[i].szPPID,&nIOStatus);
			WRITE_STRING(FA_Wafer_LOTIDSM + i,m_KeySenosrEFEM[i].szLotID,&nIOStatus);
		}
		g_KeySenosrEFEM_WaferSts_old[i] = m_KeySenosrEFEM[i].nszWaferSts;
	}
}

void CKeySensorMon::Check_LL_KeySensor(int nMAX_WFR_LL)
{
	int i;
	int nIOStatus;

	for(i = 0; i < nMAX_WFR_LL; i++)
	{
		m_KeySenosrLL[i].nszWaferSts = READ_DIGITAL(CTC_BM1_Wafer_Status + i,&nIOStatus);
		m_KeySenosrLL[i].nszWaferSrc = READ_DIGITAL(CTC_BM1_Wafer_Source + i,&nIOStatus) + 1;

		switch(m_KeySenosrEFEM[i].nszWaferSrc)
		{
			case WFR_SRC_A:
				strcpy(m_KeySenosrLL[i].szLotID,g_LPMInfo[WFR_SRC_A-1].szLotID); 
				strcpy(m_KeySenosrLL[i].szPPID,g_LPMInfo[WFR_SRC_A-1].szPPID);
				break;
			case WFR_SRC_B:
				strcpy(m_KeySenosrLL[i].szLotID,g_LPMInfo[WFR_SRC_B-1].szLotID); 
				strcpy(m_KeySenosrLL[i].szPPID,g_LPMInfo[WFR_SRC_B-1].szPPID);
				break;
			case WFR_SRC_C:
				strcpy(m_KeySenosrLL[i].szLotID,g_LPMInfo[WFR_SRC_C-1].szLotID); 
				strcpy(m_KeySenosrLL[i].szPPID,g_LPMInfo[WFR_SRC_C-1].szPPID);
				break;
			default:
				strcpy(m_KeySenosrLL[i].szLotID,""); 
				strcpy(m_KeySenosrLL[i].szPPID,"");
				break;
		}
	}

	for(i = 0; i < nMAX_WFR_LL; i++)
	{
		if(g_KeySenosrLL_WaferSts_old[i] == m_KeySenosrLL[i].nszWaferSts) continue;
		if(m_KeySenosrLL[i].nszWaferSts == 0)
		{
			WRITE_DIGITAL(BM1_Wafer_SourceDM + i,WFR_SRC_NONE,&nIOStatus);

			WRITE_STRING(BM1_Wafer_PPIDSM + i,"",&nIOStatus);
			WRITE_STRING(BM1_Wafer_LOTIDSM + i,"",&nIOStatus);
		}
		else
		{	
			WRITE_DIGITAL(BM1_Wafer_SourceDM + i,m_KeySenosrLL[i].nszWaferSrc,&nIOStatus);

			WRITE_STRING(BM1_Wafer_PPIDSM + i,m_KeySenosrLL[i].szPPID,&nIOStatus);
			WRITE_STRING(BM1_Wafer_LOTIDSM + i,m_KeySenosrLL[i].szLotID,&nIOStatus);
		}
		g_KeySenosrLL_WaferSts_old[i] = m_KeySenosrLL[i].nszWaferSts;
	}
}
void CKeySensorMon::Check_TM_KeySensor(int nMAX_WFR_TM)
{
	int i;
	int nIOStatus;


	for(i = 0; i < nMAX_WFR_TM; i++)
	{
		m_KeySenosrTM[i].nszWaferSts = READ_DIGITAL(CTC_TA_Wafer_Status + i,&nIOStatus);
		m_KeySenosrTM[i].nszWaferSrc = READ_DIGITAL(CTC_TA_Wafer_Source + i,&nIOStatus) + 1;

		switch(m_KeySenosrTM[i].nszWaferSrc)
		{
			case WFR_SRC_A:
				strcpy(m_KeySenosrTM[i].szLotID,g_LPMInfo[WFR_SRC_A-1].szLotID); 
				strcpy(m_KeySenosrTM[i].szPPID,g_LPMInfo[WFR_SRC_A-1].szPPID);
				break;
			case WFR_SRC_B:
				strcpy(m_KeySenosrTM[i].szLotID,g_LPMInfo[WFR_SRC_B-1].szLotID); 
				strcpy(m_KeySenosrTM[i].szPPID,g_LPMInfo[WFR_SRC_B-1].szPPID);
				break;
			case WFR_SRC_C:
				strcpy(m_KeySenosrTM[i].szLotID,g_LPMInfo[WFR_SRC_C-1].szLotID); 
				strcpy(m_KeySenosrTM[i].szPPID,g_LPMInfo[WFR_SRC_C-1].szPPID);
				break;
			default:
				strcpy(m_KeySenosrTM[i].szLotID,""); 
				strcpy(m_KeySenosrTM[i].szPPID,"");
				break;
		}
	}

	for(i = 0; i < nMAX_WFR_TM; i++)
	{
		if(g_KeySenosrTM_WaferSts_old[i] == m_KeySenosrTM[i].nszWaferSts) continue;
		if(m_KeySenosrTM[i].nszWaferSts == 0)
		{
			WRITE_DIGITAL(TA_Wafer_SourceDM + i,WFR_SRC_NONE,&nIOStatus);

			WRITE_STRING(TA_Wafer_PPIDSM + i,"",&nIOStatus);
			WRITE_STRING(TA_Wafer_LOTIDSM + i,"",&nIOStatus);
		}
		else
		{	
			WRITE_DIGITAL(TA_Wafer_SourceDM + i,m_KeySenosrTM[i].nszWaferSrc,&nIOStatus);

			WRITE_STRING(TA_Wafer_PPIDSM + i,m_KeySenosrTM[i].szPPID,&nIOStatus);
			WRITE_STRING(TA_Wafer_LOTIDSM + i,m_KeySenosrTM[i].szLotID,&nIOStatus);
		}
		g_KeySenosrTM_WaferSts_old[i] = m_KeySenosrTM[i].nszWaferSts;
	}
}
BOOL CKeySensorMon::Check_LPM_INFO( int nMAX_LPM)
{
	int i;
	int nCount = 0;
	int nIOStatus;
	BOOL bRlt = TRUE;
	for(i = 0; i < nMAX_LPM; i++)
	{
		READ_STRING(CTC_RECIPE_FILE + i, g_LPMInfo[i].szPPID,&nIOStatus);
		READ_STRING(CTC_MID_NAME + i, g_LPMInfo[i].szLotID,&nIOStatus);
		//strcpy(g_LPMInfo_old[i].szPPID,g_LPMInfo[i].szPPID);
		//strcpy(g_LPMInfo_old[i].szLotID,g_LPMInfo[i].szLotID);

		if(STRCMP_L(g_LPMInfo[i].szLotID,""))
		{
			nCount++;
		}
		//printf("[Debug] ====>Check_LPM_INFO %d %d %s %s\n",i,nCount,g_LPMInfo[i].szPPID,g_LPMInfo[i].szLotID);
	}
	if(nCount >= nMAX_LPM) bRlt = FALSE;
	return bRlt;
}

void CKeySensorMon::Check_TMPumpStsMon()
{
	int nIOStatus;
	int nVacSts1;	
	
	nVacSts1 = READ_DIGITAL(TM_VacuumStatusDM,&nIOStatus);

	switch(nVacSts1)
	{
		case ATM:
			WRITE_STRING(Key_TMVacuumStepSM,"",&nIOStatus);
			g_nTMCnt = 0;
			break;	
		case GoATM:
			do{
				if( g_nTMCnt == 0){
					if(READ_ANALOG(TM_CGChmPrsAI,&nIOStatus) < READ_ANALOG(PRMA_TM_SVENT_PRES,&nIOStatus)){
						WRITE_STRING(Key_TMVacuumStepSM,"SlowVent",&nIOStatus);
						g_nTMCnt++;
					}
					break;
				}
				if( g_nTMCnt == 1){
					if(READ_ANALOG(TM_CGChmPrsAI,&nIOStatus) > READ_ANALOG(PRMA_TM_SVENT_PRES,&nIOStatus)){
						WRITE_STRING(Key_TMVacuumStepSM,"FastVent",&nIOStatus);
						g_nTMCnt++;
					}
					break;
				}
			}while(0);
			break;
		case VACUUM:
			WRITE_STRING(Key_TMVacuumStepSM,"",&nIOStatus);
			g_nTMCnt = 0;
			break;
		case GoVacuum:
			do{
				if( g_nTMCnt == 0){
					if(READ_ANALOG(TM_CGChmPrsAI,&nIOStatus) > READ_ANALOG(PRMA_TM_SPUMP_PRES,&nIOStatus)){
						WRITE_STRING(Key_TMVacuumStepSM,"SlowPump",&nIOStatus);
						g_nTMCnt++;
					}
					break;
				}
				if( g_nTMCnt == 1){
					if(READ_ANALOG(TM_CGChmPrsAI,&nIOStatus) < READ_ANALOG(PRMA_TM_SPUMP_PRES,&nIOStatus)){
						WRITE_STRING(Key_TMVacuumStepSM,"FastPump",&nIOStatus);
						g_nTMCnt++;
					}
					break;
				}
			}while(0);
			break;
		default:
			WRITE_STRING(Key_TMVacuumStepSM,"",&nIOStatus);
			g_nTMCnt = 0;
			break;
	}
}

void CKeySensorMon::Check_LLPumpStsMon()
{
	int nIOStatus;
	int nVacSts1,nVacSts2;	


	nVacSts1 = READ_DIGITAL(BM1_VacuumStatusDM,&nIOStatus);
	nVacSts2 = READ_DIGITAL(BM2_VacuumStatusDM,&nIOStatus);
	//printf("[Debug] ====>Check_LLStepInfo g_nBM1Cnt%d g_nBM2Cnt%d\n",g_nBM1Cnt,g_nBM2Cnt);		
	switch(nVacSts1)
	{
		case ATM:
			WRITE_STRING(Key_BM1VacuumStepSM,"",&nIOStatus);
			g_nBM1Cnt = 0;
			break;	
		case GoATM:
			do{
				if( g_nBM1Cnt == 0){
					if(READ_ANALOG(BM1_CGChmPrsAI,&nIOStatus) < READ_ANALOG(PRMA_BM1_SVENT_PRES,&nIOStatus)){
						WRITE_STRING(Key_BM1VacuumStepSM,"SlowVent",&nIOStatus);
						g_nBM1Cnt++;
					}
					break;
				}
				if( g_nBM1Cnt == 1){
					if(READ_ANALOG(BM1_CGChmPrsAI,&nIOStatus) > READ_ANALOG(PRMA_BM1_SVENT_PRES,&nIOStatus)){
						WRITE_STRING(Key_BM1VacuumStepSM,"FastVent",&nIOStatus);
						g_nBM1Cnt++;
					}
					break;
				}
			}while(0);
			break;
		case VACUUM:
			WRITE_STRING(Key_BM1VacuumStepSM,"",&nIOStatus);
			g_nBM1Cnt = 0;
			break;
		case GoVacuum:
			do{
				if( g_nBM1Cnt == 0){
					if(READ_ANALOG(BM1_CGChmPrsAI,&nIOStatus) > READ_ANALOG(PRMA_BM1_SPUMP_PRES,&nIOStatus)){
						WRITE_STRING(Key_BM1VacuumStepSM,"SlowPump",&nIOStatus);
						g_nBM1Cnt++;
					}
					break;
				}
				if( g_nBM1Cnt == 1){
					if(READ_ANALOG(BM1_CGChmPrsAI,&nIOStatus) < READ_ANALOG(PRMA_BM1_SPUMP_PRES,&nIOStatus)){
						WRITE_STRING(Key_BM1VacuumStepSM,"FastPump",&nIOStatus);
						g_nBM1Cnt++;
					}
					break;
				}
			}while(0);
			break;
		default:
			WRITE_STRING(Key_BM1VacuumStepSM,"",&nIOStatus);
			g_nBM1Cnt = 0;
			break;
	}

	switch(nVacSts2)
	{
		case ATM:
			WRITE_STRING(Key_BM2VacuumStepSM,"",&nIOStatus);
			g_nBM2Cnt = 0;
			break;	
		case GoATM:
			do{
				if( g_nBM2Cnt == 0){
					if(READ_ANALOG(BM2_CGChmPrsAI,&nIOStatus) < READ_ANALOG(PRMA_BM2_SVENT_PRES,&nIOStatus)){
						WRITE_STRING(Key_BM2VacuumStepSM,"SlowVent",&nIOStatus);
						g_nBM2Cnt++;
					}
					break;
				}
				if( g_nBM2Cnt == 1){
					if(READ_ANALOG(BM2_CGChmPrsAI,&nIOStatus) > READ_ANALOG(PRMA_BM2_SVENT_PRES,&nIOStatus)){
						WRITE_STRING(Key_BM2VacuumStepSM,"FastVent",&nIOStatus);
						g_nBM2Cnt++;
					}
					break;
				}
			}while(0);
			break;
		case VACUUM:
			WRITE_STRING(Key_BM2VacuumStepSM,"",&nIOStatus);
			g_nBM2Cnt = 0;
			break;
		case GoVacuum:
			do{
				if( g_nBM2Cnt == 0){
					if(READ_ANALOG(BM2_CGChmPrsAI,&nIOStatus) > READ_ANALOG(PRMA_BM2_SPUMP_PRES,&nIOStatus)){
						WRITE_STRING(Key_BM2VacuumStepSM,"SlowPump",&nIOStatus);
						g_nBM2Cnt++;
					}
					break;
				}
				if( g_nBM2Cnt == 1){
					if(READ_ANALOG(BM2_CGChmPrsAI,&nIOStatus) < READ_ANALOG(PRMA_BM2_SPUMP_PRES,&nIOStatus)){
						WRITE_STRING(Key_BM2VacuumStepSM,"FastPump",&nIOStatus);
						g_nBM2Cnt++;
					}
					break;
				}
			}while(0);
			break;
		default:
			WRITE_STRING(Key_BM2VacuumStepSM,"",&nIOStatus);
			g_nBM2Cnt = 0;
			break;
	}

}
//////////////////////////////////////////////////////////////////////
// Member Function
//////////////////////////////////////////////////////////////////////
// Monitor Thread Function

void CKeySensorMon::DoMonitor()
{
	while(1)
	{
		_sleep(500);
		if(FALSE == Check_LPM_INFO(MAX_LPM)) continue;
		Check_LLPumpStsMon();
		Check_TMPumpStsMon();

		Check_EFEM_KeySensor(MAX_WFR_EFEM);
		Check_LL_KeySensor(MAX_WFR_LL);
		Check_TM_KeySensor(MAX_WFR_TM);
	}
}
