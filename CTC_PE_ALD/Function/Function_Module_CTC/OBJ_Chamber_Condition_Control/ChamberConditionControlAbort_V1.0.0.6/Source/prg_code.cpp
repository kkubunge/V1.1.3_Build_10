// Copyright (C) IPS Ltd.

#include "stdafx.h"
#include "dyn_iodefine.h"
#include "TextLogDll.h"		// 2006.07.26

#include "Timer.h"			// 2012.04.12

#define IDLE_RE_STOP_TIME	180

HANDLE g_hServerEvent_PM[5] = {NULL, NULL, NULL, NULL, NULL};
HANDLE g_hClientEvent_PM[5] = {NULL, NULL, NULL, NULL, NULL};
BOOL g_bPM_Enable[5] = {FALSE, FALSE, FALSE, FALSE, FALSE};

IO_Name_String_Map IO_STR_Map_Table[] = {
	""
};

extern "C" void Screen_Nav_Info( int no )
{
	if(no == -1)
	{
		_gCLOSE_LOG();	// 2006.07.26
	}
}

extern "C" BOOL Program_Init_Code()
{
	int i;
	_gINIT_LOG();	// 2006.07.26
	dyn_InitIOList();
	for(i=0; i<5; i++)
	{
		if(dyn_IOValid(PM1_PROCESS_RUN_STATUS+i)
			&& dyn_IOValid(PM1_PRO_IdleRcpStsDM+i)) g_bPM_Enable[i] = TRUE;
	}
	return TRUE;
}

extern "C" void Event_Message_Received()
{
}

extern "C" void Program_Enter_Code()
{
	char szServerTemp[100] = "";
	char szClientTemp[100] = "";
	for (int nCount = 0; nCount < 5; nCount++)
	{
		sprintf(szServerTemp, "EVENT_FOR_CHAMBER_CONDITION_CONTROL_SERVER_PM%1d", nCount+1);
		sprintf(szClientTemp, "EVENT_FOR_CHAMBER_CONDITION_CONTROL_CLIENT_PM%1d", nCount+1);
		g_hServerEvent_PM[nCount] = ::CreateEvent(NULL, TRUE, FALSE, szServerTemp);
		g_hClientEvent_PM[nCount] = ::CreateEvent(NULL, TRUE, FALSE, szClientTemp);
		ResetEvent(g_hServerEvent_PM[nCount]);
		ResetEvent(g_hClientEvent_PM[nCount]);
	}
}

Module_Status Program_Main()
{ 
	Module_Status nRet = SYS_SUCCESS;
	string strArgument = PROGRAM_PARAMETER_READ();
	int nCommStatus;
	int nPos, nStation, nIdx;
	BOOL bInsideLoop = TRUE;		// 2012.05.02

	CTimer tmr;						// 2012.04.12

	_LOG("[%s] Started", strArgument.c_str());		// 2006.07.26

	do {
		// added by ycchoi 2006.07.19
		// BM에 대한 Parameter가 들어 오면 무조건 SYS_SUCCESS를 Return하도록 한다.
		nPos = strArgument.find("BM");
		if(nPos != -1)	{ nRet = SYS_SUCCESS; break; }

		nPos = strArgument.find("PM");
		if(nPos == -1) return SYS_ERROR;
		nStation = atoi((strArgument.substr(nPos+2, 1)).c_str());
		if(nStation < 1 || nStation > 5) { nRet = SYS_SUCCESS; break; }
		nIdx = nStation - 1;
		if(g_bPM_Enable[nIdx] == FALSE) { nRet = SYS_SUCCESS; break; }

		if (-1 != strArgument.find("WAIT_TILL_RUN_STOPPED_"))
		{
			if (__SEQUENCE_RUNNING == dyn_READ_DIGITAL(PM1_PROCESS_RUN_STATUS+(nIdx), &nCommStatus))
			{
				// 2012.04.12
				do {	
					dyn_WRITE_DIGITAL(PM1_PRO_IdleRcpStsDM+(nIdx), __IDLE_STOP, &nCommStatus);
					tmr.StartTimer();		// 2012.04.12
					do	{
						if (FALSE == WAIT_SECONDS(0.1))
						{
							printf("# Waiting module idle aborted by user\n");
							bInsideLoop = FALSE;
							break;
						}
						
						// 2012.04.12
						if(tmr.GetProgressTime(UNIT_SECOND) > IDLE_RE_STOP_TIME)
						{
							WAIT_SECONDS(3);
							break;
						}

					} while(__SEQUENCE_RUNNING == dyn_READ_DIGITAL(PM1_PROCESS_RUN_STATUS+(nIdx), &nCommStatus));
					if (FALSE == bInsideLoop)
					{
						nRet = SYS_ABORTED;
						break;
					}
				} while(__SEQUENCE_RUNNING == dyn_READ_DIGITAL(PM1_PROCESS_RUN_STATUS+(nIdx), &nCommStatus));
			}
		}
		else if (-1 != strArgument.find("ONLINE_"))
		{
			int nPM_CommStatus = dyn_READ_DIGITAL(CTC_PM1_CommStatus+nIdx, &nCommStatus);
			if (nPM_CommStatus == 3 || nPM_CommStatus == 7) // 3:Standby, 7:CTCInUse
			{
				do {	// 2012.05.02
					printf("# Chamber Control Abort Start : %s , Station %d\n", strArgument.c_str(), nStation);
					SetEvent(g_hServerEvent_PM[nIdx]);
					tmr.StartTimer();						// 2012.05.02
					do
					{
						DWORD dwState = WaitForSingleObject(g_hClientEvent_PM[nIdx], 900);
						if (dwState == WAIT_OBJECT_0)
						{
							bInsideLoop = FALSE;			// 2012.05.02
							_LOG("-----> [PM%d] Idle Stop Success", nIdx+1);
							break;
						}
						else if (dwState == WAIT_TIMEOUT)
						{
							if ((SYS_SUCCESS == nRet) && (FALSE == WAIT_SECONDS(0.1)))
							{
								printf("# WaitForSingleObject is aborted by user\n");
								nRet = SYS_ABORTED;
								bInsideLoop = FALSE;		// 2012.05.02
								_LOG("-----> [PM%d] WaitForSingleObject is aborted by user", nIdx+1);
								break;
							}
						}
						else
						{
							printf("# WaitForSingleObject is abandoned\n");
							nRet = SYS_ABORTED;
							bInsideLoop = FALSE;			// 2012.05.02
							_LOG("-----> [PM%d] WaitForSingleObject is abandoned", nIdx+1);
							break;
						}

						// 2012.05.02
						if(tmr.GetProgressTime(UNIT_SECOND) > IDLE_RE_STOP_TIME)
						{
							WAIT_SECONDS(3);
							_LOG("-----> [PM%d] Idle Stop Failed so Retry", nIdx+1);
							break;
						}
					}while(1);
					
					if(bInsideLoop)
					{
						if(! WAIT_SECONDS(3)) break;
					}
				} while(bInsideLoop);
				printf("# Chamber Control Abort Stopped : %s , Station %d\n", strArgument.c_str(), nStation);
				ResetEvent(g_hClientEvent_PM[nIdx]);
			}
		}
		else if (-1 != nPos)
		{
			printf("# Chamber Control Abort Start : %s , Station %d\n", strArgument.c_str(), nStation);
			_LOG("# Chamber Control Abort Start : %s , Station %d", strArgument.c_str(),nStation);
			SetEvent(g_hServerEvent_PM[nIdx]);
			do
			{
				DWORD dwState = WaitForSingleObject(g_hClientEvent_PM[nIdx], 900);
				if (dwState == WAIT_OBJECT_0)
				{
					break;
				}
				else if (dwState == WAIT_TIMEOUT)
				{
					if ((SYS_SUCCESS == nRet) && (FALSE == WAIT_SECONDS(0.1)))
					{
						printf("# WaitForSingleObject is aborted by user\n");
						nRet = SYS_ABORTED;
					}
				}
				else
				{
					printf("# WaitForSingleObject is abandoned\n");
					nRet = SYS_ABORTED;
				}
			}while(1);
			printf("# Chamber Control Abort Stopped : %s , Station %d\n", strArgument.c_str(), nStation);
			ResetEvent(g_hClientEvent_PM[nIdx]);
		}
	} while(0);

	// 2006.07.26
	if(nRet == SYS_SUCCESS)	_LOG("[%s] Finished(SYS_SUCCESS)", strArgument.c_str());
	else if(nRet == SYS_ERROR) _LOG("[%s] Finished(SYS_ERROR)", strArgument.c_str());
	else if(nRet == SYS_ABORTED) _LOG("[%s] Finished(SYS_ABORTED)", strArgument.c_str());
	else _LOG("[%s] Finished(%d)", strArgument.c_str(), nRet);
	return nRet;
}
