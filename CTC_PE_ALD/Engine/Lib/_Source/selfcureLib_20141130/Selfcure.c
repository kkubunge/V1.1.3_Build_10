#include <stdio.h>
#include <windows.h>
#include "k:\h\CimSeqnc.h"
#include "Selfcure.h"

#pragma comment(lib, "cimsqconio.lib")
#define	SELFCURE_FILEPATH_INI ".\\Parameter\\SelfCurePath.ini"

int    IO_ADD_READ_DIGITAL_NAME( LPSTR list , ... ) {
	va_list va;
	int nCommstatus;
	char name[64+1] = {0,};
	int nIOPoint;

	va_start( va , list );
	vsprintf( name , list , (LPSTR) va );
	va_end( va );
	nIOPoint = _FIND_FROM_STRING( _K_D_IO , name );

	if ( nIOPoint < 0 ) return -1;
	return _dREAD_DIGITAL( nIOPoint , &nCommstatus );
}

BOOL LOG_SELFCURE(char *szFileNameStatus, char *szLogBuf1, int nAlarmNo,  char *szLogBuf2, int nStsAlm, BOOL nAlmClear) {
	
	
	int	nReturnedSize = 0;
	char szReturnedString[64] = {0,};
	char szbuf[2048] = {0,};
	SYSTEMTIME time;
	FILE *fpLog = NULL;
	

	if ( nStsAlm > 0 ) {

		if ( nAlmClear == TRUE ) {
			if ( ALARM_STATUS(nStsAlm) == ALM_PAUSED ) {
				ALARM_CLEAR(nStsAlm);
				Sleep(2000);			//GUI Alarm Screen need clear update time
			}
		}

		sprintf(szbuf, "[%s] [%d] %s" , szLogBuf1, nAlarmNo, szLogBuf2 );
		ALARM_MESSAGE_SET(nStsAlm, szbuf);			
		ALARM_POST(nStsAlm);
	}

	
	if ( strlen(szFileNameStatus) == 0 ) {
		return FALSE;
	}
	
	//WRITE LOG	
	//Save to file
	//-----------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------
	fpLog = fopen(szFileNameStatus , "a+");
	if (NULL == fpLog) { Sleep(1000);  fopen(szFileNameStatus , "a+"); }
	
	if (NULL != fpLog) {
		GetLocalTime(&time);		
		sprintf(szbuf, "%04d%02d%02d %02d%02d%02d.%03d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		fprintf(fpLog , "[%s] [%s] [%d] %s\n" , szbuf, szLogBuf1, nAlarmNo, szLogBuf2 );
		printf("[%s] [%s] [%d] %s\n" , szbuf, szLogBuf1, nAlarmNo, szLogBuf2 );
	}
	else {
		printf("LOG_SELFCURE error can't open file[%s]\n" , szFileNameStatus );
		return FALSE;
	}
	
	if (NULL != fpLog) fclose(fpLog);
	
	return TRUE;
	//-----------------------------------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------------------
}


int SELFCURE_MANAGE( int nAlarmNo ) {
	int nSelfcureResult = 0;
	nSelfcureResult = SELFCURE_CHECK(nAlarmNo, TRUE);
	
	if ( nSelfcureResult == ALM_RETRY ) {
		return ALM_RETRY;
	}
	else {
		return ALARM_MANAGE(nAlarmNo);
	} 
}


int SELFCURE_MANAGE_FREE( int nAlarmNo ) {
	int nSelfcureResult = 0;
	nSelfcureResult = SELFCURE_CHECK(nAlarmNo, FALSE);
	
	if ( nSelfcureResult == ALM_RETRY ) {
		return ALM_RETRY;
	}
	else {
		return ALARM_MANAGE_FREE(nAlarmNo);
	} 
}

void SELFCURE_COUNT_INITIAL( ) {
	SELFCURE_CHECK( -1, 0 ) ;
}

int SELFCURE_CHECK( int nAlarmNo, BOOL bAbtChk ) {
	
	char szAppName[64] = {0,};
	char szKeyName[6][32] = { "ALARM_ID_LIST", "RECOVERY_METHOD", "RETRY_COUNT", "RETRY_INTERVAL_SECS", "RETRY_IGNORE_HOURS", "RECENT_RECOVERY_TIME" };
	char szReturnedString[64] = {0,};
	char szLogBuf1[64] = {0,}, szLogBuf2[64] = {0,}, szDateBuf[64] = {0,};
	int	nReturnedSize = 0, nRetryCountMax = 0, nIntervalSecs = 0, nIgnoreHours = 0;
	BOOL nReturnedResultforWrite = 0, bCheckIsDigit = 0;
	char szUpdateData[64] = {0,}, 	szAlmMsg[1024] = {0,};
	int	nIOPoint = 0, nCommstatus = 0, nValue[4];
	SYSTEMTIME time, time2;
	WORD wMinuteGap = 0;
	FILE *fp = NULL;
	int		i, nErrorCode = 0;
	static char szFileNameSetup[128] = {0,};
	static char szFileNameStatus[128] = {0,};
	static int	nCurrentAlmRetryCnt = 0;
	static int	nOldAlarmNo = -1;	            	//하나의 Exe에서  alarmNo 가 바뀌면,  nCurrentAlmRetryCnt 를 초기화할 수 있다.
	static int  nCMAlmMin = 0, nCMAlmMax = 0;
	static int  nASC_stsAlmNo = 0;
	static int  nASC_stsAlmPstMethod = 3;   //0(NotPost) 1(OnlyStart) 2(Start,Continue) 3(Start,Continue,Fail)(Default)
	static int  nASC_stsAlmPstDetail = 0;   //0(OnlyPost)(Default)  1(Detail Post)
	static int  nASC_stsAlmPstClear = 1;    //0(OnlyPost) 1(Clear->Post)(Default)

	
	if ( nAlarmNo == -1 ) {				//상위 Sequence 에서   nCurrentAlmRetryCnt 를 초기화할 수 있다.
		nOldAlarmNo = -1;
		nCurrentAlmRetryCnt = 0;
		return -1;
	}

	do {
		if ( strlen(szFileNameSetup) == 0 || strlen(szFileNameStatus) == 0 ) {		//path.ini 파일이 존재한다면, 최초 1회만 Open한다.
			nReturnedSize = GetPrivateProfileString("Setup", "FILENAME", "", szReturnedString, sizeof(szReturnedString)-1, SELFCURE_FILEPATH_INI );		
			if ( nReturnedSize <= 0 ) {
				nErrorCode = 1;
				break;
			}
			strcpy(szFileNameSetup, szReturnedString);
		
			nReturnedSize = GetPrivateProfileString("Status", "FILENAME", "", szReturnedString, sizeof(szReturnedString)-1, SELFCURE_FILEPATH_INI );		
			if ( nReturnedSize <= 0 ) {
				nErrorCode = 2;
				break;
			}		
			strcpy(szFileNameStatus, szReturnedString);

			nReturnedSize = GetPrivateProfileString("ALM_CM_RANGE", "RANGE_MIN", "", szReturnedString, sizeof(szReturnedString)-1, SELFCURE_FILEPATH_INI );		
			if ( nReturnedSize <= 0 ) {
				nErrorCode = 3;
				break;
			}		
			nCMAlmMin = atoi(szReturnedString);

			nReturnedSize = GetPrivateProfileString("ALM_CM_RANGE", "RANGE_MAX", "", szReturnedString, sizeof(szReturnedString)-1, SELFCURE_FILEPATH_INI );		
			if ( nReturnedSize <= 0 ) {
				nErrorCode = 4;
				break;
			}		
			nCMAlmMax = atoi(szReturnedString);

			//---> ADD 2014.04.25 by KSM
			nReturnedSize = GetPrivateProfileString("ASC", "ASC_ALARM", "", szReturnedString, sizeof(szReturnedString)-1, SELFCURE_FILEPATH_INI );		
			if ( nReturnedSize <= 0 ) {
				nErrorCode = 5;
				break;
			}		
			nASC_stsAlmNo = atoi(szReturnedString);


			nReturnedSize = GetPrivateProfileString("ASC", "ASC_ALMPOST_METHOD", "", szReturnedString, sizeof(szReturnedString)-1, SELFCURE_FILEPATH_INI );		
			if ( nReturnedSize <= 0 ) {
				//default value				
			}
			else {
				//0(NotPost) 1(OnlyStart) 2(Start,Continue) 3(Start,Continue,Fail)(Default)
				nASC_stsAlmPstMethod = atoi(szReturnedString);
				if ( nASC_stsAlmPstMethod <= 0 ) nASC_stsAlmPstMethod = 0;
				if ( nASC_stsAlmPstMethod >= 3 ) nASC_stsAlmPstMethod = 3;
			}

			nReturnedSize = GetPrivateProfileString("ASC", "ASC_ALMPOST_DETAIL", "", szReturnedString, sizeof(szReturnedString)-1, SELFCURE_FILEPATH_INI );		
			if ( nReturnedSize <= 0 ) {
				//default value
			}	
			else {
				//0(OnlyPost)(Default)  1(Detail Post)
				nASC_stsAlmPstDetail = atoi(szReturnedString);
				if ( nASC_stsAlmPstDetail <= 0 ) nASC_stsAlmPstDetail = 0;
				if ( nASC_stsAlmPstDetail >= 1 ) nASC_stsAlmPstDetail = 1;
			}

			nReturnedSize = GetPrivateProfileString("ASC", "ASC_ALMPOST_CLEAR", "", szReturnedString, sizeof(szReturnedString)-1, SELFCURE_FILEPATH_INI );		
			if ( nReturnedSize <= 0 ) {
				//default value
			}	
			else {
				//0(OnlyPost) 1(Clear->Post)(Default)
				nASC_stsAlmPstClear = atoi(szReturnedString);
				if ( nASC_stsAlmPstClear <= 0 ) nASC_stsAlmPstClear = 0;
				if ( nASC_stsAlmPstClear >= 1 ) nASC_stsAlmPstClear = 1;
			}
			//---> END 2014.04.25 by KSM

		}		


		fp = NULL;
		fp = fopen(szFileNameSetup, "r");

		if ( fp == NULL ) {
			sprintf( szLogBuf1, "%s", "PARAMETER_ERROR");
			sprintf( szLogBuf2, "NotOpen(%s)", szFileNameSetup);
			
			LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);				
			
			nErrorCode = 10;
			break;
		}
		else {
			fclose(fp);
		}
		
		sprintf( szAppName, "%d", nAlarmNo);
		//Get ALARM_ID_LIST
		//------------------------------------------------------------------------------------------------------------------------------------------
		memset(szReturnedString,0x00,sizeof(szReturnedString));
		nReturnedSize = GetPrivateProfileString(szAppName, szKeyName[0], "", szReturnedString, sizeof(szReturnedString)-1, szFileNameSetup );
		//printf("GetPrivateProfileString [%s] [%d] [%s]\n", szKeyName[0], nReturnedSize, szReturnedString);

		if ( nReturnedSize <= 0 ) {			//등록되지 않은 Alarm 이므로  Log 없이 break;
			nErrorCode = 11;
			break;
		}

		bCheckIsDigit = TRUE;
		for (i=0;i<nReturnedSize;i++) {
			if (!isdigit(szReturnedString[i])) bCheckIsDigit = FALSE;
		}

		if ( nReturnedSize <= 0 || bCheckIsDigit == FALSE || atoi(szReturnedString) != nAlarmNo  ) {
			sprintf( szLogBuf1, "%s", "PARAMETER_ERROR");
			sprintf( szLogBuf2, "%s(ErrorValue=%s)", szKeyName[0], szReturnedString);
			
			LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);				

			nErrorCode = 12;
			break;
		}
		//------------------------------------------------------------------------------------------------------------------------------------------


		//Get RECOVERY_METHOD
		//------------------------------------------------------------------------------------------------------------------------------------------
		memset(szReturnedString,0x00,sizeof(szReturnedString));
		nReturnedSize = GetPrivateProfileString(szAppName, szKeyName[1], "", szReturnedString, sizeof(szReturnedString)-1, szFileNameSetup );

		if ( nReturnedSize <= 0 || strcmp(szReturnedString, "RETRY") != 0 ) {
			nErrorCode = 13;
			break;
		}
		//------------------------------------------------------------------------------------------------------------------------------------------



		//Get RETRY_COUNT
		//------------------------------------------------------------------------------------------------------------------------------------------
		memset(szReturnedString,0x00,sizeof(szReturnedString));
		nReturnedSize = GetPrivateProfileString(szAppName, szKeyName[2], "", szReturnedString, sizeof(szReturnedString)-1, szFileNameSetup );
		//printf("GetPrivateProfileString [%s] [%d] [%s]\n", szKeyName[2], nReturnedSize, szReturnedString);
	
		bCheckIsDigit = TRUE;
		for (i=0;i<nReturnedSize;i++) {
			if (!isdigit(szReturnedString[i])) bCheckIsDigit = FALSE;
		}

		nRetryCountMax = atoi(szReturnedString);
		if ( nReturnedSize <= 0 || bCheckIsDigit == FALSE || nRetryCountMax < 1 || nRetryCountMax > 10 ) {
			sprintf( szLogBuf1, "%s", "PARAMETER_ERROR");
			sprintf( szLogBuf2, "%s(ErrorValue=%s)", szKeyName[2], szReturnedString);

			LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);				

			nErrorCode = 14;
			break;
		}
		//------------------------------------------------------------------------------------------------------------------------------------------


		//Get RETRY_INTERVAL_SECS
		//------------------------------------------------------------------------------------------------------------------------------------------
		memset(szReturnedString,0x00,sizeof(szReturnedString));
		nReturnedSize = GetPrivateProfileString(szAppName, szKeyName[3], "", szReturnedString, sizeof(szReturnedString)-1, szFileNameSetup );
		//printf("GetPrivateProfileString [%s] [%d] [%s]\n", szKeyName[3], nReturnedSize, szReturnedString);
		
		bCheckIsDigit = TRUE;
		for (i=0;i<nReturnedSize;i++) {
			if (!isdigit(szReturnedString[i])) bCheckIsDigit = FALSE;
		}

		nIntervalSecs = atoi(szReturnedString);
		if ( nReturnedSize <= 0 || bCheckIsDigit == FALSE || nIntervalSecs < 1 || nIntervalSecs > 3600 ) {
			sprintf( szLogBuf1, "%s", "PARAMETER_ERROR");
			sprintf( szLogBuf2, "%s(ErrorValue=%s)", szKeyName[3], szReturnedString);

			LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);				

			nErrorCode = 15;
			break;
		}
		//------------------------------------------------------------------------------------------------------------------------------------------

		//Get RETRY_IGNORE_HOURS
		//------------------------------------------------------------------------------------------------------------------------------------------
		memset(szReturnedString,0x00,sizeof(szReturnedString));
		nReturnedSize = GetPrivateProfileString(szAppName, szKeyName[4], "", szReturnedString, sizeof(szReturnedString)-1, szFileNameSetup );
		//printf("GetPrivateProfileString [%s] [%d] [%s]\n", szKeyName[4], nReturnedSize, szReturnedString);

		bCheckIsDigit = TRUE;
		for (i=0;i<nReturnedSize;i++) {
			if (!isdigit(szReturnedString[i])) bCheckIsDigit = FALSE;
		}

		nIgnoreHours = atoi(szReturnedString);
		if ( nReturnedSize <= 0 || bCheckIsDigit == FALSE || nIgnoreHours < 0 || nIgnoreHours > 24 ) {
			sprintf( szLogBuf1, "%s", "PARAMETER_ERROR");
			sprintf( szLogBuf2, "%s(ErrorValue=%s)", szKeyName[4], szReturnedString);

			LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);				

			nErrorCode = 16;
			break;
		}
		//------------------------------------------------------------------------------------------------------------------------------------------



		//Get RECENT_RECOVERY_TIME
		//------------------------------------------------------------------------------------------------------------------------------------------
		memset(szReturnedString,0x00,sizeof(szReturnedString));
		nReturnedSize = GetPrivateProfileString(szAppName, szKeyName[5], "", szReturnedString, sizeof(szReturnedString)-1, szFileNameSetup );
		//printf("GetPrivateProfileString [%s] [%d] [%s]\n", szKeyName[5], nReturnedSize, szReturnedString);

		if ( nReturnedSize <= 12 ) {
			sprintf( szLogBuf1, "%s", "PARAMETER_ERROR");
			sprintf( szLogBuf2, "%s(ErrorValue=%s)", szKeyName[4], szReturnedString);
			
			LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);
			
			nErrorCode = 17;
			break;
		}
		//------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------

		for ( i = 0 ; i < 4 ; i++ ) {			
			if ( i == 0 ) {
				nValue[i] = IO_ADD_READ_DIGITAL_NAME("CTC.MAIN_CONTROL");    //When Io point not found, return -1 .
			}
			else {
				nValue[i] = IO_ADD_READ_DIGITAL_NAME("CTC.MAIN_CONTROL%d", i+1);    //When Io point not found, return -1 .
			}				
		}




		if ( nValue[0] == -1 && nValue[1] == -1 && nValue[2] == -1 && nValue[3] == -1 ) {   // Case PM
			if ( IO_ADD_READ_DIGITAL_NAME("PM_COMMSTATUS") == 7  || IO_ADD_READ_DIGITAL_NAME("CommStsDM") == 7) {   //CTC Enable
				// Normal ( PM Enable )
			}
			else {
				nErrorCode = 31;
				break;
			}
		}
		else if ( nValue[0] != 1 && nValue[1] != 1 && nValue[2] != 1 && nValue[3] != 1 ) {		// CTCControl  :   0:Idle  1:Running  2:Pausing  Paused  Aborting  Disable  Waiting  Wait-HandOff

			if ( nAlarmNo >= nCMAlmMin && nAlarmNo <= nCMAlmMax ) {  // CM 은 OHT MODE 로 Check
				for ( i = 0 ; i < 4 ; i++ ) {			
					nValue[i] = IO_ADD_READ_DIGITAL_NAME("CTC.FA_STATUS_AGV_CM%d", i+1);    //When Io point not found, return -1 .
				}
				
				if ( nValue[0] != 1 && nValue[1] != 1 && nValue[2] != 1 && nValue[3] != 1 ) {		//  0:Manual  1:Auto
					nErrorCode = 32;
					break;
				}
				
				// Normal ( OHT Auto )

			}
			else {
				nErrorCode = 33;
				break;
			}
 		}
		// Normal ( Job Run )


		//-----------------------------(위치중요)-------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------------
		if ( nOldAlarmNo != nAlarmNo ) {		
			nOldAlarmNo = nAlarmNo;
			
			if ( nIgnoreHours > 0 ) nCurrentAlmRetryCnt = 0;		//하나의 While문안에서 nIgnoreHours 가 0 이면서,  2개 이상의 Self-cure 가 동작하는 경우 무한루프 문제 발생.
		}
		
		//Compare RETRY_IGNORE_HOURS 
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if ( nReturnedSize >= 12 && nIgnoreHours > 0 && nCurrentAlmRetryCnt == 0 ) {   //Time비교는 nCurrentAlmRetryCnt 이 0 일때만 수행한다. 
			memset(&time2, 0x00, sizeof(time2));
			
			memset(szDateBuf, 0x00, sizeof(szDateBuf));
			strncpy(szDateBuf, szReturnedString, 4);
			time2.wYear = atoi(szDateBuf);
			
			memset(szDateBuf, 0x00, sizeof(szDateBuf));
			strncpy(szDateBuf, szReturnedString+4, 2);
			time2.wMonth = atoi(szDateBuf);
			
			memset(szDateBuf, 0x00, sizeof(szDateBuf));
			strncpy(szDateBuf, szReturnedString+6, 2);
			time2.wDay = atoi(szDateBuf);
			
			memset(szDateBuf, 0x00, sizeof(szDateBuf));
			strncpy(szDateBuf, szReturnedString+8, 2);
			time2.wHour = atoi(szDateBuf);
			
			memset(szDateBuf, 0x00, sizeof(szDateBuf));
			strncpy(szDateBuf, szReturnedString+10, 2);
			time2.wMinute = atoi(szDateBuf);
			
			GetLocalTime(&time);		
			
			wMinuteGap = ( time.wYear   - time2.wYear  ) * 365 * 24 * 60 + 
				( time.wMonth  - time2.wMonth ) * 30 * 24 * 60 +
				( time.wDay    - time2.wDay   ) * 24 * 60 +
				( time.wHour   - time2.wHour  ) * 60 +
				( time.wMinute - time2.wMinute);
			
			//printf("AutoSelfcure : before date[%04d%02d%02d %02d%02d], current date[%04d%02d%02d %02d%02d], MinuteGap[%d]\n", time2.wYear, time2.wMonth, time2.wDay, time2.wHour, time2.wMinute, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, wMinuteGap);
			if ( nIgnoreHours * 60 > wMinuteGap )  {
				
				// 2014.03.17   Log 없음 : 지속적으로 Call 되는 Monitoring Function(AKRA Port Function) 의 경우 Log 계속 생성 문제 발생하여 Log 없음 ..(spec 에도 없음)  
				
				nErrorCode = 40;
				break;
			}
		}
		//------------------------------------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------
		//-----------------------------------------------------------------------------------------------------------------------
		if ( nRetryCountMax > nCurrentAlmRetryCnt ) {  //Run Self-cure


			//Set RECENT_RECOVERY_TIME  to SetupFile
			//-----------------------------------------------------------------------------------------------------------------------------
			// RECOVER_START, RECOVER_CONTINUE 시  모두 Update한다..  (2014.03.17 Spec 변경)
			GetLocalTime(&time);		
			sprintf(szUpdateData, "%04d%02d%02d%02d%02d%02d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );

			nReturnedResultforWrite = WritePrivateProfileString(szAppName, szKeyName[5], szUpdateData, szFileNameSetup );
			if ( nReturnedResultforWrite <= 0 ) {			//1회 retry
				printf("WritePrivateProfileString Retry : AppName[%s] Res[%d]\n", szAppName, nReturnedResultforWrite);
				Sleep(1000);
				nReturnedResultforWrite = WritePrivateProfileString(szAppName, szKeyName[5], szUpdateData, szFileNameSetup );
			}

			if ( nReturnedResultforWrite <= 0 ) {			//RETRY_IGNORE_HOURS 을 Update하지 못하면  매번 Retry 수행 위험 발생으로  break 처리한다.
				sprintf( szLogBuf1, "%s", "NO Update");
				sprintf( szLogBuf2, "%s(ErrorValue=%s)", szKeyName[5], szUpdateData);
				
				LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);
				printf("WritePrivateProfileString Fail : AppName[%s] Res[%d]\n", szAppName, nReturnedResultforWrite);

				nErrorCode = 41;
				break;
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			//-----------------------------------------------------------------------------------------------------------------------------


			//-----------------------------------------------------------------------------------------------------------------------------
			//-----------------------------------------------------------------------------------------------------------------------------
			if ( nCurrentAlmRetryCnt == 0 ) {
				sprintf( szLogBuf1, "%s", "RECOVER_START");
			}
			else {
				sprintf( szLogBuf1, "%s", "RECOVER_CONTINUE");
			}
			
			sprintf( szLogBuf2, "(%d/%d)Self-cure Running", nCurrentAlmRetryCnt+1, nRetryCountMax);

			if ( nASC_stsAlmPstDetail == 1 ) {
				ALARM_MESSAGE_GET(nAlarmNo, szAlmMsg);
				strcat(szLogBuf2, "(");
				strcat(szLogBuf2, szAlmMsg);
				strcat(szLogBuf2, ")");
			}
			
			if ( nCurrentAlmRetryCnt == 0 ) {
				if ( nASC_stsAlmPstMethod >= 1 ) {
					LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, nASC_stsAlmNo, (nASC_stsAlmPstClear == 1 ? TRUE : FALSE) );
				}
				else {
					LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);
				}
			}
			else {
				if ( nASC_stsAlmPstMethod >= 2 ) {
					LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, nASC_stsAlmNo, (nASC_stsAlmPstClear == 1 ? TRUE : FALSE));					
				}
				else {
					LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);				
					
				}
			}
			//-----------------------------------------------------------------------------------------------------------------------------
			//-----------------------------------------------------------------------------------------------------------------------------

			//---(WAIT)------------------------------------------------------------------------
			//---------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------
			if ( bAbtChk ) {
				if (!WAIT_SECONDS(nIntervalSecs)) {

					sprintf( szLogBuf1, "%s", "RECOVER_FAIL");
					sprintf( szLogBuf2, "Self-cure fail");
					
					if ( nASC_stsAlmPstDetail == 1 ) {
						ALARM_MESSAGE_GET(nAlarmNo, szAlmMsg);
						strcat(szLogBuf2, "(");
						strcat(szLogBuf2, szAlmMsg);
						strcat(szLogBuf2, ")");
					}

					if ( nASC_stsAlmPstMethod >= 3 ) {
						LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, nASC_stsAlmNo, (nASC_stsAlmPstClear == 1 ? TRUE : FALSE));	
					}
					else {
						LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);	
					}

					nErrorCode = 42;
					break;
				}
			}
			else {
				_sleep(nIntervalSecs*1000);
			}
			//---------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------

			
			//---------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------
			nCurrentAlmRetryCnt++;			//Retry Count 증가..

			return ALM_RETRY;
			//---------------------------------------------------------------------------------
			//---------------------------------------------------------------------------------
		}
		else {
			if ( nRetryCountMax == nCurrentAlmRetryCnt ) {	// 최종적으로 Alarm Post 된 상태에서 User 가 Retry 실행 시,  Fail Log가 중복되어 남을 수 있다. (1회로 제한)
				sprintf( szLogBuf1, "%s", "RECOVER_FAIL");
				sprintf( szLogBuf2, "Self-cure fail");
							
				if ( nASC_stsAlmPstDetail == 1 ) {
					ALARM_MESSAGE_GET(nAlarmNo, szAlmMsg);
					strcat(szLogBuf2, "(");
					strcat(szLogBuf2, szAlmMsg);
					strcat(szLogBuf2, ")");
				}

				if ( nASC_stsAlmPstMethod >= 3 ) {
					LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, nASC_stsAlmNo, (nASC_stsAlmPstClear == 1 ? TRUE : FALSE));	
				}
				else {
					LOG_SELFCURE(szFileNameStatus, szLogBuf1, nAlarmNo,  szLogBuf2, FALSE, FALSE);	
				}
			}
			
			nErrorCode = 99;
			break;
		}

	}while(0);


	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
	nCurrentAlmRetryCnt = 9999;   //무한루프 방지..
	printf("SELFCURE_CHECK AlarmNo[%d], BreakCode[%d]\n", nAlarmNo, nErrorCode);
	return -1;
	//----------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------
}
