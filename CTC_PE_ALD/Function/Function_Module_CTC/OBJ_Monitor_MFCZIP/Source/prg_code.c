#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <conio.h>
//#include <afxcmn.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
#include "kutletc.h"
#include "kutlanal.h"
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------

#define	FILE_PATH_SIZE	256

enum { OFF_0	,	ON_1	};  
enum { STOP_0	,	RUN_1	};  
enum { Unuse , Local , DisConn , Conn , DisableSW , DisableHW , EnablePM , CTCEna , CTCDisY , CTCDisN , CTCEnaPM , CTCDisHWY , CTCDisHWN , EnAbort , DisAbort , DisHWAbort , EnPMAbort } ; 

int g_nPMA_Status = STOP_0;
int g_nPMB_Status = STOP_0;
int g_nPMC_Status = STOP_0;

char g_szEQ_Name[128] ="";

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
		
	{ "INFO_MFCPATH_NAME"		, _K_S_IO	,	INFO_MFCPATH_NAME		,	0	} ,	
	{ "EQ_NAME"					, _K_S_IO	,	EQ_NAME					,	0	} ,	

	{ "PM1.PRO_RunStsDM"		, _K_D_IO	,	PM1_Process_Run			,	0	},
	{ "PM2.PRO_RunStsDM"		, _K_D_IO	,	PM2_Process_Run			,	0	},
	{ "PM3.PRO_RunStsDM"		, _K_D_IO	,	PM3_Process_Run			,	0	},

	{ "PM1.PRMD_MFC_Diagnosis"	, _K_D_IO	,	PM1_PRMD_MFC_Diagnosis	,	0	},
	{ "PM2.PRMD_MFC_Diagnosis"	, _K_D_IO	,	PM2_PRMD_MFC_Diagnosis	,	0	},
	{ "PM3.PRMD_MFC_Diagnosis"	, _K_D_IO	,	PM3_PRMD_MFC_Diagnosis	,	0	},
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void ThrdZipMonitor_PM1(void *Dummy  )
{
	int m_nProcessToggle = 0, m_nRet = 0, CommStatus;
	WORD m_wStart_Year, m_wStart_Month, m_wStart_Day;
	
	char m_szCurrFilePath[FILE_PATH_SIZE] = {0,};
	char m_szFileZipName[FILE_PATH_SIZE] = {0,};	
	char m_szFileZipPath[FILE_PATH_SIZE] = {0,};
	char m_szBuffer[FILE_PATH_SIZE] = {0,};	
	char m_szBuffer2[FILE_PATH_SIZE] = {0,};	
	
	FILE *fp = NULL;
	
	STARTUPINFO si = {0,};
	PROCESS_INFORMATION pi;
	
	while( TRUE )
	{	
		_sleep(300); 
		
		if(READ_DIGITAL(PM1_Process_Run, &CommStatus) == RUN_1  && m_nProcessToggle == OFF_0 && ON_1 == READ_DIGITAL(PM1_PRMD_MFC_Diagnosis,&CommStatus))
		{ 
			m_nProcessToggle = ON_1;
			g_nPMA_Status = ON_1;
			
		}
		if(READ_DIGITAL(PM1_Process_Run, &CommStatus) == STOP_0 && m_nProcessToggle == ON_1)
		{
			m_nProcessToggle = OFF_0; //중복 실행을 막기 위한 Toggle I/O
			fp = fopen("Parameter/PresentLogName_PM1.log", "r" ); //최근 마지막으로 진행 한 Log.
			if(fp == NULL)
			{
				fclose(fp);
				printf("[OBJ_Monitor_MFCZIP PM1] Log Read Fail\n");
				continue;
			}
			printf("[debug] == > Step1    %d    %s\n",sizeof(m_szFileZipName),fp);
			fgets( m_szFileZipName, sizeof(m_szFileZipName), fp );
			fclose(fp);
			printf("[debug] == > Step2\n");
			//Run 종료 후, 기존에 남겻 던 Log 읽기.
			STR_SEPERATE_CHAR( m_szFileZipName , '-' , m_szBuffer2	 , m_szBuffer , 4999 ); // Serial
			m_wStart_Year = atoi(m_szBuffer2);
			STR_SEPERATE_CHAR( m_szBuffer , '-' , m_szBuffer2	 , m_szBuffer , 4999 ); // Serial
			m_wStart_Month = atoi(m_szBuffer2);
			STR_SEPERATE_CHAR( m_szBuffer , '-' , m_szBuffer2	 , m_szBuffer , 4999 ); // Serial
			m_wStart_Day = atoi(m_szBuffer2);
			printf("[debug] == > Step3\n");
			sprintf(m_szCurrFilePath, "F:\\MARS\\PartsLog\\PM1\\%04d%02d%02d\\%s.log", m_wStart_Year , m_wStart_Month, m_wStart_Day, m_szFileZipName); 
			sprintf(m_szFileZipPath, "7z a %s.zip %s", m_szCurrFilePath , m_szCurrFilePath);	
			printf("[debug] == > Step4\n");
			m_nRet = CreateProcess(NULL, m_szFileZipPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); // 응용 프로세스 진행.
			printf("[debug] == > Step5\n");
			if(m_nRet)   //정상적으로 실행되면 Retrun TRUE 반환
			{
				printf("WaitForSingleObject Start\n");
				WaitForSingleObject(pi.hProcess, 0xffffffff);    // 해당 핸들이 종료될때까지 대기
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				printf("WaitForSingleObject End\n");
			}				
			remove(m_szCurrFilePath); //압축 완료 후, ProcessLog 삭제.
			_sleep(500);
			g_nPMA_Status = STOP_0;
		}
		
	}
	_endthread ();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void ThrdZipMonitor_PM2(void *Dummy  )
{
	int m_nProcessToggle = 0, m_nRet = 0, CommStatus;
	WORD m_wStart_Year, m_wStart_Month, m_wStart_Day;
	
	char m_szCurrFilePath[FILE_PATH_SIZE] = {0,};
	char m_szFileZipName[FILE_PATH_SIZE] = {0,};	
	char m_szFileZipPath[FILE_PATH_SIZE] = {0,};
	char m_szBuffer[FILE_PATH_SIZE] = {0,};	
	char m_szBuffer2[FILE_PATH_SIZE] = {0,};	
	
	FILE *fp = NULL;
	
	STARTUPINFO si = {0,};
	PROCESS_INFORMATION pi;
	
	while( TRUE )
	{	
		_sleep(300); 
		
		if(READ_DIGITAL(PM2_Process_Run, &CommStatus) == RUN_1  && m_nProcessToggle == OFF_0 && ON_1 == READ_DIGITAL(PM2_PRMD_MFC_Diagnosis,&CommStatus))
		{ 
			m_nProcessToggle = ON_1;
			g_nPMB_Status = ON_1;
			
		}
		if(READ_DIGITAL(PM2_Process_Run, &CommStatus) == STOP_0 && m_nProcessToggle == ON_1)
		{
			m_nProcessToggle = OFF_0; //중복 실행을 막기 위한 Toggle I/O
			fp = fopen("Parameter/PresentLogName_PM2.log", "r" ); //최근 마지막으로 진행 한 Log. 
			if(fp == NULL)
			{
				fclose(fp);
				printf("[OBJ_Monitor_MFCZIP PM2] Log Read Fail\n");
				continue;
			}
			fgets( m_szFileZipName, sizeof(m_szFileZipName), fp );
			fclose(fp);
			
			//Run 종료 후, 기존에 남겻 던 Log 읽기.
			STR_SEPERATE_CHAR( m_szFileZipName , '-' , m_szBuffer2	 , m_szBuffer , 4999 ); // Serial
			m_wStart_Year = atoi(m_szBuffer2);
			STR_SEPERATE_CHAR( m_szBuffer , '-' , m_szBuffer2	 , m_szBuffer , 4999 ); // Serial
			m_wStart_Month = atoi(m_szBuffer2);
			STR_SEPERATE_CHAR( m_szBuffer , '-' , m_szBuffer2	 , m_szBuffer , 4999 ); // Serial
			m_wStart_Day = atoi(m_szBuffer2);
			
			sprintf(m_szCurrFilePath, "F:\\MARS\\PartsLog\\PM2\\%04d%02d%02d\\%s.log", m_wStart_Year , m_wStart_Month, m_wStart_Day, m_szFileZipName); 
			sprintf(m_szFileZipPath, "7z a %s.zip %s", m_szCurrFilePath , m_szCurrFilePath);	
			
			m_nRet = CreateProcess(NULL, m_szFileZipPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); // 응용 프로세스 진행.
			if(m_nRet)   //정상적으로 실행되면 Retrun TRUE 반환
			{
				printf("WaitForSingleObject Start\n");
				WaitForSingleObject(pi.hProcess, 0xffffffff);    // 해당 핸들이 종료될때까지 대기
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				printf("WaitForSingleObject End\n");
			}				
			remove(m_szCurrFilePath); //압축 완료 후, ProcessLog 삭제.
			_sleep(500);
			g_nPMB_Status = STOP_0;
		}
		
	}
	_endthread ();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void ThrdZipMonitor_PM3(void *Dummy  )
{
	int m_nProcessToggle = 0, m_nRet = 0, CommStatus;
	WORD m_wStart_Year, m_wStart_Month, m_wStart_Day;
	
	char m_szCurrFilePath[FILE_PATH_SIZE] = {0,};
	char m_szFileZipName[FILE_PATH_SIZE] = {0,};	
	char m_szFileZipPath[FILE_PATH_SIZE] = {0,};
	char m_szBuffer[FILE_PATH_SIZE] = {0,};	
	char m_szBuffer2[FILE_PATH_SIZE] = {0,};	
	
	FILE *fp = NULL;
	
	STARTUPINFO si = {0,};
	PROCESS_INFORMATION pi;
	
	while( TRUE )
	{	
		_sleep(300); 
		
		if(READ_DIGITAL(PM3_Process_Run, &CommStatus) == RUN_1  && m_nProcessToggle == OFF_0 && ON_1 == READ_DIGITAL(PM3_PRMD_MFC_Diagnosis,&CommStatus))
		{ 
			m_nProcessToggle = ON_1;
			g_nPMC_Status = ON_1;
			
		}
		if(READ_DIGITAL(PM3_Process_Run, &CommStatus) == STOP_0 && m_nProcessToggle == ON_1)
		{
			m_nProcessToggle = OFF_0; //중복 실행을 막기 위한 Toggle I/O
			fp = fopen("Parameter/PresentLogName_PM3.log", "r" ); //최근 마지막으로 진행 한 Log. 
			if(fp == NULL)
			{
				fclose(fp);
				printf("[OBJ_Monitor_MFCZIP PM3] Log Read Fail\n");
				continue;
			}
			fgets( m_szFileZipName, sizeof(m_szFileZipName), fp );
			fclose(fp);
			
			//Run 종료 후, 기존에 남겻 던 Log 읽기.
			STR_SEPERATE_CHAR( m_szFileZipName , '-' , m_szBuffer2	 , m_szBuffer , 4999 ); // Serial
			m_wStart_Year = atoi(m_szBuffer2);
			STR_SEPERATE_CHAR( m_szBuffer , '-' , m_szBuffer2	 , m_szBuffer , 4999 ); // Serial
			m_wStart_Month = atoi(m_szBuffer2);
			STR_SEPERATE_CHAR( m_szBuffer , '-' , m_szBuffer2	 , m_szBuffer , 4999 ); // Serial
			m_wStart_Day = atoi(m_szBuffer2);
			
			sprintf(m_szCurrFilePath, "F:\\MARS\\PartsLog\\PM3\\%04d%02d%02d\\%s.log", m_wStart_Year , m_wStart_Month, m_wStart_Day, m_szFileZipName); 
			sprintf(m_szFileZipPath, "7z a %s.zip %s", m_szCurrFilePath , m_szCurrFilePath);	
			
			m_nRet = CreateProcess(NULL, m_szFileZipPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); // 응용 프로세스 진행.
			if(m_nRet)   //정상적으로 실행되면 Retrun TRUE 반환
			{
				printf("WaitForSingleObject Start\n");
				WaitForSingleObject(pi.hProcess, 0xffffffff);    // 해당 핸들이 종료될때까지 대기
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				printf("WaitForSingleObject End\n");
			}				
			remove(m_szCurrFilePath); //압축 완료 후, ProcessLog 삭제.
			_sleep(500);
			g_nPMC_Status = STOP_0;
		}
		
	}
	_endthread ();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void ThrdZipMonitor_PM1_TOTAL(void *Dummy  )
{
	int  m_nRet = 0;
	WORD m_wStart_Year, m_wStart_Month, m_wStart_Day; //현재 Thread 시작 날짜 저장.
	
	char m_szZipPath[FILE_PATH_SIZE] = "" ;	
	char m_szZipName[FILE_PATH_SIZE] = "" ;	
	char m_szMakeZipPath[FILE_PATH_SIZE] = "" ;	
	
	STARTUPINFO si = {0,};
	PROCESS_INFORMATION pi;
	SYSTEMTIME SysTime;
	
	//현재 시작 날짜 저장.
	GetLocalTime(&SysTime);	
	m_wStart_Day   = SysTime.wDay;
	m_wStart_Month = SysTime.wMonth;
	m_wStart_Year  = SysTime.wYear;
	//]

	while( TRUE )
	{
		_sleep(300);
		GetLocalTime(&SysTime);	
		if( m_wStart_Day != SysTime.wDay && g_nPMA_Status == STOP_0 ) //저장된 날짜와 시스템 시간이 다르면.
		{	

			sprintf ( m_szZipName, "F:\\MARS\\PartsLog\\PM1\\%04d-%02d-%02d_%s_Sensor" ,  m_wStart_Year , m_wStart_Month, m_wStart_Day , g_szEQ_Name);
			sprintf ( m_szZipPath, "F:\\MARS\\PartsLog\\PM1\\%04d%02d%02d",  m_wStart_Year , m_wStart_Month, m_wStart_Day );
			sprintf ( m_szMakeZipPath, "7z a %s.zip %s\\*", m_szZipName , m_szZipPath);
		
			m_nRet = CreateProcess(NULL, m_szMakeZipPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); // 응용 프로세스 진행 후킹.
		
			if(m_nRet)   //정상적으로 실행되면 Retrun TRUE 반환
			{
				printf("WaitForSingleObject Start\n");
				WaitForSingleObject(pi.hProcess, 0xffffffff);    // 해당 핸들이 종료될때까지 대기
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				printf("WaitForSingleObject End\n");
			}		

			GetLocalTime(&SysTime);	
			m_wStart_Day   = SysTime.wDay;
			m_wStart_Month = SysTime.wMonth;
			m_wStart_Year  = SysTime.wYear;		
		}
	}
	_endthread ();
}


void ThrdZipMonitor_PM2_TOTAL(void *Dummy  )
{
	int  m_nRet = 0;
	WORD m_wStart_Year, m_wStart_Month, m_wStart_Day; //현재 Thread 시작 날짜 저장.
	
	char m_szZipPath[FILE_PATH_SIZE] = "" ;	
	char m_szZipName[FILE_PATH_SIZE] = "" ;	
	char m_szMakeZipPath[FILE_PATH_SIZE] = "" ;	
	
	STARTUPINFO si = {0,};
	PROCESS_INFORMATION pi;
	SYSTEMTIME SysTime;
	
	//현재 시작 날짜 저장.
	GetLocalTime(&SysTime);	
	m_wStart_Day   = SysTime.wDay;
	m_wStart_Month = SysTime.wMonth;
	m_wStart_Year  = SysTime.wYear;
	//]
	
	while( TRUE )
	{
		_sleep(300);
		GetLocalTime(&SysTime);	
		if( m_wStart_Day != SysTime.wDay && g_nPMB_Status == STOP_0 ) //저장된 날짜와 시스템 시간이 다르면.
		{	
			sprintf ( m_szZipName, "F:\\MARS\\PartsLog\\PM2\\%04d-%02d-%02d_%s_Sensor",  m_wStart_Year , m_wStart_Month, m_wStart_Day , g_szEQ_Name);
			sprintf ( m_szZipPath, "F:\\MARS\\PartsLog\\PM2\\%04d%02d%02d",  m_wStart_Year , m_wStart_Month, m_wStart_Day );
			sprintf ( m_szMakeZipPath, "7z a %s.zip %s\\*", m_szZipName , m_szZipPath);
			
			m_nRet = CreateProcess(NULL, m_szMakeZipPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); // 응용 프로세스 진행 후킹.
			
			if(m_nRet)   //정상적으로 실행되면 Retrun TRUE 반환
			{
				printf("WaitForSingleObject Start\n");
				WaitForSingleObject(pi.hProcess, 0xffffffff);    // 해당 핸들이 종료될때까지 대기
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				printf("WaitForSingleObject End\n");
			}		
			
			GetLocalTime(&SysTime);	
			m_wStart_Day   = SysTime.wDay;
			m_wStart_Month = SysTime.wMonth;
			m_wStart_Year  = SysTime.wYear;		
		}
	}
	_endthread ();

}

void ThrdZipMonitor_PM3_TOTAL(void *Dummy  )
{
	int  m_nRet = 0;
	WORD m_wStart_Year, m_wStart_Month, m_wStart_Day; //현재 Thread 시작 날짜 저장.
	
	char m_szZipPath[FILE_PATH_SIZE] = "" ;	
	char m_szZipName[FILE_PATH_SIZE] = "" ;	
	char m_szMakeZipPath[FILE_PATH_SIZE] = "" ;	
	
	STARTUPINFO si = {0,};
	PROCESS_INFORMATION pi;
	SYSTEMTIME SysTime;
	
	//현재 시작 날짜 저장.
	GetLocalTime(&SysTime);	
	m_wStart_Day   = SysTime.wDay;
	m_wStart_Month = SysTime.wMonth;
	m_wStart_Year  = SysTime.wYear;
	//]
	
	while( TRUE )
	{
		_sleep(300);
		GetLocalTime(&SysTime);	
		if( m_wStart_Day != SysTime.wDay && g_nPMC_Status == STOP_0 ) //저장된 날짜와 시스템 시간이 다르면.
		{	
			sprintf ( m_szZipName, "F:\\MARS\\PartsLog\\PM3\\%04d-%02d-%02d_%s_Sensor",  m_wStart_Year , m_wStart_Month, m_wStart_Day , g_szEQ_Name);
			sprintf ( m_szZipPath, "F:\\MARS\\PartsLog\\PM3\\%04d%02d%02d",  m_wStart_Year , m_wStart_Month, m_wStart_Day );
			sprintf ( m_szMakeZipPath, "7z a %s.zip %s\\*", m_szZipName , m_szZipPath);
			
			m_nRet = CreateProcess(NULL, m_szMakeZipPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); // 응용 프로세스 진행 후킹.
			
			if(m_nRet)   //정상적으로 실행되면 Retrun TRUE 반환
			{
				printf("WaitForSingleObject Start\n");
				WaitForSingleObject(pi.hProcess, 0xffffffff);    // 해당 핸들이 종료될때까지 대기
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				printf("WaitForSingleObject End\n");
			}		
			GetLocalTime(&SysTime);	
			m_wStart_Day   = SysTime.wDay;
			m_wStart_Month = SysTime.wMonth;
			m_wStart_Year  = SysTime.wYear;		
		}
	}
	_endthread ();
}

void ThrdZipMonitor_EVENT(void *Dummy  )
{
	FILE *fp = NULL;
	char szCurrFilePath[FILE_PATH_SIZE] = "";
	char szOldFilePath[FILE_PATH_SIZE] = "" ;	
	char szZipPath[1024] = "";
	int m_nRet = 0, CommStatus;
	STARTUPINFO si = {0,};
	PROCESS_INFORMATION pi;

	WORD Old_day, Curr_day;
	SYSTEMTIME SysTime;

	GetLocalTime(&SysTime);
	Old_day = SysTime.wDay;

	while( TRUE )
	{
		if( ON_1 == READ_DIGITAL(PM1_PRMD_MFC_Diagnosis,&CommStatus) || ON_1 == READ_DIGITAL(PM2_PRMD_MFC_Diagnosis,&CommStatus)
			|| ON_1 == READ_DIGITAL(PM3_PRMD_MFC_Diagnosis,&CommStatus)  )
		{
			
			GetLocalTime(&SysTime);	
			Curr_day = SysTime.wDay;

			if( Old_day != Curr_day )
			{			
				sprintf( szCurrFilePath , "F:\\Mars\\PartsLog\\%04d-%02d-%02d_%s_PartInfo.log" , SysTime.wYear , SysTime.wMonth, SysTime.wDay , g_szEQ_Name );
				if ( ( fp = fopen( szCurrFilePath , "r" ) ) == NULL ) 
				{
					fp = fopen( szCurrFilePath , "w" );
					fprintf( fp , "MFC:GAS SYMBOL, GAS NO, FULLSCALE, GAIN, OFFSET A, OFFSET B\n" );
					fclose(fp);	
				}
				else fclose(fp);			
				sprintf(szZipPath, "7z a %s.zip %s", szOldFilePath, szOldFilePath);
				m_nRet = CreateProcess(NULL, szZipPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi); // 응용 프로세스 진행.
				if(m_nRet)   //정상적으로 실행되면 Retrun TRUE 반환
				{
					printf("WaitForSingleObject Start\n");
					WaitForSingleObject(pi.hProcess, 0xffffffff);    // 해당 핸들이 종료될때까지 대기
					CloseHandle(pi.hProcess);
					CloseHandle(pi.hThread);
					printf("WaitForSingleObject End\n");
				}					
				remove(szOldFilePath); //압축 완료 된 후, 이전 Log 파일 삭제.
				Old_day = Curr_day;
				strcpy(szOldFilePath, szCurrFilePath);			
			}
		
			else
			{		
				if ( ( fp = fopen( szOldFilePath , "r" ) ) == NULL ) 
				{
					sprintf( szOldFilePath , "F:\\Mars\\PartsLog\\%04d-%02d-%02d_%s_PartInfo.log" , SysTime.wYear , SysTime.wMonth, SysTime.wDay , g_szEQ_Name );
					fp = fopen( szOldFilePath , "w" );
					fprintf( fp , "MFC:GAS SYMBOL, GAS NO, FULLSCALE, GAIN, OFFSET A, OFFSET B\n" );
					fclose(fp);	
				}
				else fclose(fp);						
			}
		}
		_sleep(300);
	}
	_endthread ();

}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {

	int CommStatus;

	READ_STRING ( EQ_NAME  , g_szEQ_Name  , &CommStatus );

	_beginthread ( ThrdZipMonitor_PM1	, 0 , NULL ) ;  //  
	_beginthread ( ThrdZipMonitor_PM2	, 0 , NULL ) ;  //
	_beginthread ( ThrdZipMonitor_PM3	, 0 , NULL ) ;  //
	
	_beginthread ( ThrdZipMonitor_PM1_TOTAL	, 0 , NULL ) ;  // 
	_beginthread ( ThrdZipMonitor_PM2_TOTAL	, 0 , NULL ) ;  // 
	_beginthread ( ThrdZipMonitor_PM3_TOTAL	, 0 , NULL ) ;  // 
	
	_beginthread ( ThrdZipMonitor_EVENT	, 0 , NULL ) ;  // 

	while ( TRUE ) 	_sleep ( 1000 ) ;	

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}