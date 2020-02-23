#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <KutlAnal.h>
#include <kutlgui.h>
#include <kutlStr.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "..\resource.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	""
};

int m_nKeepDays = 30;
int m_nKeepMonths = 12;
int m_nDeleteRangeDays = 90;
int m_nDeleteRangeMonths = 12;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status LastDateByDays1( SYSTEMTIME *pstrtSysTime, int nPDays ) {
	int nYear=0, nMonth=0, nDay=0, nLastMonth=0;

	nYear = pstrtSysTime->wYear;
	nMonth = pstrtSysTime->wMonth;
	nDay = pstrtSysTime->wDay;

	nDay -= nPDays;
	if(nDay < 1) {
		nLastMonth = nMonth - 1;
		if(nLastMonth < 1) nLastMonth = 12;
		switch(nLastMonth) {
		case 4: case 6: case 9: case 11: nDay += 30; break;
		case 2: nDay += 29; break;
		default : nDay += 30; break;
		}
		nMonth--;
		if(nMonth < 1) {
			nMonth += 12;
			nYear--;
		}
	}

	pstrtSysTime->wYear = nYear;
	pstrtSysTime->wMonth = nMonth;
	pstrtSysTime->wDay = nDay;

//	printf("=====LastDateByDays-1>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status LastDateByDays2( SYSTEMTIME *pstrtSysTime, int nPDays ) {
	int nYear=0, nMonth=0, nDay=0, nLastMonth=0;

	nYear = pstrtSysTime->wYear;
	nMonth = pstrtSysTime->wMonth;
	nDay = pstrtSysTime->wDay;

	nDay -= nPDays;
	if(nDay < 1) {
		nLastMonth = nMonth - 1;
		if(nLastMonth < 1) nLastMonth = 12;
		switch(nLastMonth) {
		case 4: case 6: case 9: case 11: nDay += 30; break;
		case 2: nDay += 29; break;
		default : nDay += 30; break;
		}
		nMonth--;
		if(nMonth < 1) {
			nMonth += 12;
			nYear--;
		}
	}

	pstrtSysTime->wYear = nYear;
	pstrtSysTime->wMonth = nMonth;
	pstrtSysTime->wDay = nDay;

//	printf("=====LastDateByDays-2>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status LastDateByDays3( SYSTEMTIME *pstrtSysTime, int nPDays ) {
	int nYear=0, nMonth=0, nDay=0, nLastMonth=0;

	nYear = pstrtSysTime->wYear;
	nMonth = pstrtSysTime->wMonth;
	nDay = pstrtSysTime->wDay;

	nDay -= nPDays;
	if(nDay < 1) {
		nLastMonth = nMonth - 1;
		if(nLastMonth < 1) nLastMonth = 12;
		switch(nLastMonth) {
		case 4: case 6: case 9: case 11: nDay += 30; break;
		case 2: nDay += 29; break;
		default : nDay += 30; break;
		}
		nMonth--;
		if(nMonth < 1) {
			nMonth += 12;
			nYear--;
		}
	}

	pstrtSysTime->wYear = nYear;
	pstrtSysTime->wMonth = nMonth;
	pstrtSysTime->wDay = nDay;

//	printf("=====LastDateByDays-3>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status LastDateByMonth4( SYSTEMTIME *pstrtSysTime, int nPMonths ) {
	int nYear=0, nMonth=0, nDay=0, nLastMonth=0;

	nYear = pstrtSysTime->wYear;
	nMonth = pstrtSysTime->wMonth;
	nDay = pstrtSysTime->wDay;

	nMonth -= nPMonths;
	while(nMonth < 1) {
		nMonth += 12;
		nYear--;
	}

	pstrtSysTime->wYear = nYear;
	pstrtSysTime->wMonth = nMonth;
	pstrtSysTime->wDay = nDay;

//	printf("=====LastDateByDays-4>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status LastDateByMonth4_1( SYSTEMTIME *pstrtSysTime, int nPDays ) {
	int nYear=0, nMonth=0, nDay=0, nLastMonth=0;

	nYear = pstrtSysTime->wYear;
	nMonth = pstrtSysTime->wMonth;
	nDay = pstrtSysTime->wDay;

	nDay -= nPDays;
	if(nDay < 1) {
		nLastMonth = nMonth - 1;
		if(nLastMonth < 1) nLastMonth = 12;
		switch(nLastMonth) {
		case 4: case 6: case 9: case 11: nDay += 30; break;
		case 2: nDay += 29; break;
		default : nDay += 30; break;
		}
		nMonth--;
		if(nMonth < 1) {
			nMonth += 12;
			nYear--;
		}
	}

	pstrtSysTime->wYear = nYear;
	pstrtSysTime->wMonth = nMonth;
	pstrtSysTime->wDay = nDay;

//	printf("=====LastDateByDays-4_1>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);

	return SYS_SUCCESS;
}
Module_Status LastDateByDays5( SYSTEMTIME *pstrtSysTime, int nPDays ) {
	int nYear=0, nMonth=0, nDay=0, nLastMonth=0;

	nYear = pstrtSysTime->wYear;
	nMonth = pstrtSysTime->wMonth;
	nDay = pstrtSysTime->wDay;

	nDay -= nPDays;
	if(nDay < 1) {
		nLastMonth = nMonth - 1;
		if(nLastMonth < 1) nLastMonth = 12;
		switch(nLastMonth) {
		case 4: case 6: case 9: case 11: nDay += 30; break;
		case 2: nDay += 29; break;
		default : nDay += 30; break;
		}
		nMonth--;
		if(nMonth < 1) {
			nMonth += 12;
			nYear--;
		}
	}

	pstrtSysTime->wYear = nYear;
	pstrtSysTime->wMonth = nMonth;
	pstrtSysTime->wDay = nDay;

//	printf("=====LastDateByDays-5>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void GetLogFileName1(char* szFilePath, SYSTEMTIME *pstrtSysTime) {
	char szBuf[256];
//	sprintf(szBuf, "D:\\IPS-CTC\\_Log\\PIOViewer\\~$_%dy%02dm%02dd.log", pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	sprintf(szBuf, "f:\\datalog\\_Log\\PIOViewer\\~$_%dy%02dm%02dd.log", pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	strcpy(szFilePath, szBuf);

//	printf("=====GetLogFileName-1>>>>> [File = %s]\n",szFilePath);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void GetLogFileName2(char* szFilePath, SYSTEMTIME *pstrtSysTime) {
	char szBuf[256];
//	sprintf(szBuf, "D:\\IPS-CTC\\_Log\\PIOViewer\\PIO%d-%02d-%02d.log", pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	sprintf(szBuf, "f:\\datalog\\_Log\\PIOViewer\\PIO%d-%02d-%02d.log", pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	strcpy(szFilePath, szBuf);

//	printf("=====GetLogFileName-2>>>>> [File = %s]\n",szFilePath);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void GetLogFileName3(char* szFilePath, SYSTEMTIME *pstrtSysTime) {
	char szBuf[256];
//	sprintf(szBuf, "D:\\IPS-CTC\\_Log\\LPMMonitor\\~$_%dy%02dm%02dd.log", pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	sprintf(szBuf, "f:\\datalog\\_Log\\LPMMonitor\\~$_%dy%02dm%02dd.log", pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	strcpy(szFilePath, szBuf);

//	printf("=====GetLogFileName-3>>>>> [File = %s]\n",szFilePath);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void GetLogFileName4(char* szFilePath, SYSTEMTIME *pstrtSysTime) {
	char szBuf[256];
//	sprintf(szBuf, "D:\\IPS-CTC\\_RecipeHistory\\RH%d-%02d.log", pstrtSysTime->wYear, pstrtSysTime->wMonth);
	sprintf(szBuf, "f:\\_RecipeHistory\\RH%d-%02d.log", pstrtSysTime->wYear, pstrtSysTime->wMonth);
	strcpy(szFilePath, szBuf);

//	printf("=====GetLogFileName-4>>>>> [File = %s]\n",szFilePath);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void GetLogFileName5(char* szFilePath, SYSTEMTIME *pstrtSysTime) {
	char szBuf[256];
//	sprintf(szBuf, "D:\\IPS-CTC\\_JobHistory\\JH%d%02d%02d.log", pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	sprintf(szBuf, "f:\\_JobHistory\\JH%d%02d%02d.log", pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	strcpy(szFilePath, szBuf);

//	printf("=====GetLogFileName-5>>>>> [File = %s]\n",szFilePath);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

void DeleteLogFile1(SYSTEMTIME *pstrtSysTime) {
	char szFilePath[256];
	GetLogFileName1(szFilePath, pstrtSysTime);

//	printf("=====DeleteLogFile->FilePath-1>>>>> [File = %s]\n",szFilePath);
//	printf("=====DeleteLogFile->Date-1>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	
	DeleteFile(szFilePath);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void DeleteLogFile2(SYSTEMTIME *pstrtSysTime) {
	char szFilePath[256];
	GetLogFileName2(szFilePath, pstrtSysTime);

//	printf("=====DeleteLogFile->FilePath-2>>>>> [File = %s]\n",szFilePath);
//	printf("=====DeleteLogFile->Date-2>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	
	DeleteFile(szFilePath);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void DeleteLogFile3(SYSTEMTIME *pstrtSysTime) {
	char szFilePath[256];
	GetLogFileName3(szFilePath, pstrtSysTime);

//	printf("=====DeleteLogFile->FilePath-3>>>>> [File = %s]\n",szFilePath);
//	printf("=====DeleteLogFile->Date-3>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	
	DeleteFile(szFilePath);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void DeleteLogFile4(SYSTEMTIME *pstrtSysTime) {
	char szFilePath[256];
	GetLogFileName4(szFilePath, pstrtSysTime);

//	printf("=====DeleteLogFile->FilePath-4>>>>> [File = %s]\n",szFilePath);
//	printf("=====DeleteLogFile->Date-4>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	
	DeleteFile(szFilePath);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void DeleteLogFile5(SYSTEMTIME *pstrtSysTime) {
	char szFilePath[256];
	GetLogFileName5(szFilePath, pstrtSysTime);

//	printf("=====DeleteLogFile->FilePath-5>>>>> [File = %s]\n",szFilePath);
//	printf("=====DeleteLogFile->Date-5>>>>> [%dy%02dm%02dd]\n",pstrtSysTime->wYear, pstrtSysTime->wMonth, pstrtSysTime->wDay);
	
	DeleteFile(szFilePath);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Thread_Carrier_EXC_Data_Monitor() {
	SYSTEMTIME pstrtSysTime;
	int i;

	while(1) {
		_sleep(10000);  //... 2016.03.03 
		GetLocalTime(&pstrtSysTime);
		LastDateByDays1(&pstrtSysTime, m_nKeepDays);
		for( i=0; i<m_nDeleteRangeDays; i++ ) {
			LastDateByDays1(&pstrtSysTime, 1);
			DeleteLogFile1(&pstrtSysTime);
			if(!WAIT_SECONDS(10)) break;
		}
		m_nDeleteRangeDays = 2;
		WAIT_SECONDS(20*60*60);
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Thread_PIO_Data_Monitor() {
	SYSTEMTIME pstrtSysTime;
	int i;

	while(1) {
		_sleep(10000);	//... 2016.03.03
		GetLocalTime(&pstrtSysTime);
		LastDateByDays2(&pstrtSysTime, m_nKeepDays);
		for( i=0; i<m_nDeleteRangeDays; i++ ) {
			LastDateByDays2(&pstrtSysTime, 1);
			DeleteLogFile2(&pstrtSysTime);
			if(!WAIT_SECONDS(10)) break;
		}
		m_nDeleteRangeDays = 2;
		WAIT_SECONDS(20*60*60);
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Thread_LPMMonitor_Data_Monitor() {
	SYSTEMTIME pstrtSysTime;
	int i;

	while(1) {
		_sleep(10000);	//... 2016.03.03
		GetLocalTime(&pstrtSysTime);
		LastDateByDays3(&pstrtSysTime, m_nKeepDays);
		for( i=0; i<m_nDeleteRangeDays; i++ ) {
			LastDateByDays3(&pstrtSysTime, 1);
			DeleteLogFile3(&pstrtSysTime);
			if(!WAIT_SECONDS(10)) break;
		}
		m_nDeleteRangeDays = 2;
		WAIT_SECONDS(20*60*60);
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Thread_RecipeHystory_Data_Monitor() {
	SYSTEMTIME pstrtSysTime;
	int i;

	while(1) {
		_sleep(10000);	//... 2016.03.03
		GetLocalTime(&pstrtSysTime);
		LastDateByMonth4(&pstrtSysTime, m_nKeepMonths);
		for( i=0; i<m_nDeleteRangeDays; i++ ) {
			LastDateByMonth4_1(&pstrtSysTime, 1);
			DeleteLogFile4(&pstrtSysTime);
			if(!WAIT_SECONDS(10)) break;
		}
		m_nDeleteRangeDays = 2;
		WAIT_SECONDS(25*20*60*60);
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Thread_JobHystory_Data_Monitor() {
	SYSTEMTIME pstrtSysTime;
	int i;

	while(1) {
		_sleep(10000);	//... 2016.03.03
		GetLocalTime(&pstrtSysTime);
		LastDateByDays5(&pstrtSysTime, m_nKeepDays);
		for( i=0; i<m_nDeleteRangeDays; i++ ) {
			LastDateByDays5(&pstrtSysTime, 1);
			DeleteLogFile5(&pstrtSysTime);
			if(!WAIT_SECONDS(10)) break;
		}
		m_nDeleteRangeDays = 2;
		WAIT_SECONDS(20*60*60);
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
//	printf("*^^*====++++====++++====++++====++++====*^^*\n");
	_beginthread( (void*)Thread_Carrier_EXC_Data_Monitor, 0, NULL);
	_beginthread( (void*)Thread_PIO_Data_Monitor, 0, NULL);
//	_beginthread( (void*)Thread_LPMMonitor_Data_Monitor, 0, NULL);
	_beginthread( (void*)Thread_RecipeHystory_Data_Monitor, 0, NULL);
	_beginthread( (void*)Thread_JobHystory_Data_Monitor, 0, NULL);
	return SYS_SUCCESS;	
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
