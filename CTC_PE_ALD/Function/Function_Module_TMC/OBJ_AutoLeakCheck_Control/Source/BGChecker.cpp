// BGChecker.cpp: implementation of the CBGChecker class.
// 2008.04.02 by mgsong
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BGChecker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBGChecker::CBGChecker()
{
	m_dwBGChecker = 0;
	m_hBGCheckerHandle = NULL;
	m_bCM1Run = m_bCM2Run = m_bCM3Run = FALSE;
}

CBGChecker::~CBGChecker()
{
	KillBGChecker();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : RunBGChecker
//Date         : 2008.04.04
//Description  :
//Arguments    :
//Return Value :
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CBGChecker::RunBGChecker(BOOL bCM1Run , BOOL bCM2Run , BOOL bCM3Run)
{
	bool bRet = false;

	DWORD dwThreadID = 0;
	m_bCM1Run = bCM1Run;
	m_bCM2Run = bCM2Run;
	m_bCM3Run = bCM3Run;

	m_hBGCheckerHandle = CreateThread(NULL, 0, BGCheckerThread, this, 0, &dwThreadID);
	if(NULL != m_hBGCheckerHandle)
		bRet = true;	

	return bRet;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : KillBGChecker
//Date         : 2008.04.04
//Description  :
//Arguments    :
//Return Value :
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool CBGChecker::KillBGChecker()
{
	DWORD dwTimeOut = 0;
	if (m_hBGCheckerHandle) {
		::TerminateThread(m_hBGCheckerHandle , 0);

		DWORD dwExitCode = STILL_ACTIVE;
		BOOL  BExitReturn = GetExitCodeThread(m_hBGCheckerHandle , &dwExitCode);
		if (STILL_ACTIVE == dwExitCode) {	
			while (STILL_ACTIVE == dwExitCode) {
				Sleep(50);
				dwTimeOut += 50;
				BExitReturn = GetExitCodeThread(m_hBGCheckerHandle , &dwExitCode);	
				if (FALSE == BExitReturn) break;

				if (dwTimeOut >= 30000) break;
			}
			CloseHandle(m_hBGCheckerHandle);
			m_hBGCheckerHandle = NULL;			
		} else {
			CloseHandle(m_hBGCheckerHandle);
			m_hBGCheckerHandle = NULL;		
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : BGCheckerThread
//Date         : 2008.04.04
//Description  :
//Arguments    :
//Return Value :
//////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD CBGChecker::BGCheckerThread(LPVOID pVoid)
{
	CBGChecker* pBGChecker = (CBGChecker*) pVoid;	
	while (1) {
		pBGChecker->CheckAndControlLot();
		Sleep(100);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function     : CheckAndControlLot
//Date         : 2008.04.04
//Description  :
//Arguments    :
//Return Value :
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CBGChecker::CheckAndControlLot()
{
//	printf("BGChecker Checking... \n");
	int  nCM1Status , nCM2Status , nCM3Status;
	int  nIOStatus;
	char schdBuffer[128];
	memset(schdBuffer , 0, sizeof(schdBuffer));
	
	nCM1Status = READ_DIGITAL(MAIN_CONTROL  , &nIOStatus );
	nCM2Status = READ_DIGITAL(MAIN_CONTROL2 , &nIOStatus );
	nCM3Status = READ_DIGITAL(MAIN_CONTROL3 , &nIOStatus );

	//CM1 Check
	if ((nCM1Status == Running) && (FALSE == m_bCM1Run)) {
		sprintf(schdBuffer , "PAUSE_F PORT1");
		WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
	} else if ((nCM1Status == Waiting)) {
		sprintf(schdBuffer , "PAUSE_F PORT1");
		WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
	} else {
		//may first running & nCM1Status == TRUE
		//but now running lot completed so reset flag because there is possibility of restart lot
		nCM1Status = FALSE;
	}

	//CM2 Check
	if ((nCM2Status == Running) && (FALSE == m_bCM2Run)) {
		sprintf(schdBuffer , "PAUSE_F PORT2");
		WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
	} else if ((nCM2Status == Waiting)) {
		sprintf(schdBuffer , "PAUSE_F PORT2");
		WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
	} else {
		//may first running & nCM2Status == TRUE
		//but now running lot completed so reset flag because there is possibility of restart lot
		nCM2Status = FALSE;
	}


	//CM3 Check
	if ((nCM3Status == Running) && (FALSE == m_bCM3Run)) {
		sprintf(schdBuffer , "PAUSE_F PORT3");
		WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
	} else if ((nCM3Status == Waiting)) {
		sprintf(schdBuffer , "PAUSE_F PORT3");
		WRITE_FUNCTION_EVENT(F_SCHEDULER , schdBuffer);
	} else {
		//may first running & nCM3Status == TRUE
		//but now running lot completed so reset flag because there is possibility of restart lot
		nCM3Status = FALSE;
	}
}
