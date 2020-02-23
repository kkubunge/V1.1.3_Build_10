// EventLogServer.h: interface for the CEventLogServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTLOGSERVER_H__E7E923F4_0298_4906_8197_71CCD90BAC10__INCLUDED_)
#define AFX_EVENTLOGSERVER_H__E7E923F4_0298_4906_8197_71CCD90BAC10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicIO.h"
#include "MonThread.h"
#include "SeqInterface.h"
class CEventLogServer  : public CSeqInterface, public CMonThread, public CDynamicIO
{
public:
	CEventLogServer();
	virtual ~CEventLogServer();

// Member Function
public:
	virtual BOOL Initialize();
	virtual Module_Status Main();
	virtual void Enter_Code();
	virtual void Event_Message_Received();

	BOOL ReadConfig();
	void ParseConfig(char *szData);
	void ParseModuleInfo(char *szData);			// 2013.11.18

	int GetLogType(char *szMessage);
	BOOL MakeProcLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog);
	BOOL MakeTransLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog);
	BOOL MakeFuncLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog);
	BOOL MakeLotLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog);
	BOOL MakeUserLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog);
	BOOL MakeCfgLogStr(char *szMessage, char *szLog, BOOL *bIsRunLog);

	void ConvertToMARSStd(char *szData);		// 2013.07.16

	int GetPMIdx(char *szDeviceID);
	int GetLPMIdx(char *szDeviceID);
	int GetWaferInfoFromPM(char *szWaferInfo, int nPMIdx);
	int GetWaferInfoFromBM(char *szWaferInfo, char *szModule, int nWaferCount);	// 2013.07.16
	BOOL IsPMRunning(int nPMIdx);
	BOOL IsLPMRunning();
	BOOL IsDevRunning(char *szDeviceID);
	BOOL IsProcRunning(int nPMIdx);
	int GetActType(char *szEventID);
//	void GetTransWaferInfo(char *szModule, int nActType, char *szArm, char *szTargetModule, int nSlot,
//							char *szWaferInfo, BOOL bDualArm=FALSE);
	void UpdateLPMInfo(int nLPMIdx);
	void ClearLPMInfo(int nLPMIdx);
	void UpdatePMInfo(int nPMIdx);		// 2013.07.29
	void ClearPMInfo(int nPMIdx);
	int GetRcpType(char *szEventID, char *szRecipeName);

	int GetWaferInfo(char *szWaferInfo, char *szModule, int nSlot, BOOL bDualArm=FALSE);
	int GetWaferSrcIdxFromPM(int nPMIdx, int nSlot, int *pnWaferNo);
	int GetWaferSrcIdx(char *szModule, int nSlot, int *pnWaferNo);
	void ChangeToStandardModuleName(char *szModuleName);
	int GetSlotInfo(char *szSlotString, BOOL *pbDualArm);
	void GetArmInfo(char *szModule, char *szArmInfo, char *szArm, BOOL bDualArm=FALSE);		// 2013.07.16

	// 2013.11.18
	BOOL GetModuleInfo(char *szModule, int *pnSlotCount);
	void BackupModuleWaferInfo(char *szModule, char *szWaferInfo);
	void GetWaferInfoFromModuleInfo(char *szModule, char *szWaferInfo);

	void SetPreModuleInfo(char *szModule, char *szFromDevice, char *szPreSlot, char *szEndEffect);
	void GetPreModuleInfo(char *szModule, char *szPreModuleInfo, char *szEndEffect, BOOL bDualArm);
	void SetLLWaferInfo(char *szModule, int nSlot, BOOL bDualArm);
	void GetLLWaferInfo(char *szWaferInfo, char *szModule, int nSlot, BOOL bDualArm);
	void RemoveLLWaferInfo(char *szModule, int nSlot, BOOL bDualArm);


// For Thread
protected:
	virtual void DoMonitor();

// Member Variable
public:
	typedef struct _PROCESS_LOG_STRU
	{
		char szDate[20];
		char szTime[20];
		char szDeviceID[20];
		char szLogType[20];
		char szEventID[20];
		char  szStatus[20];
		char szMaterialID[80];				// 2013.07.16
		char szSlotNo[20];;
		char szLotID[20];
		char szRecipeID[40];
		int  nStepNum;
		char szData[80];
	} PROCESS_LOG_STRU;
	PROCESS_LOG_STRU m_ProcessLog;

	typedef struct _TRANSFER_LOG_STRU
	{
		char szDate[20];
		char szTime[20];
		char szDeviceID[20];
		char szLogType[20];
		char szEventID[20];
		char szStatus[20];
		char szMaterialID[20];
		char szMaterialType[20];
		char szFromDevice[20];
		char szFromSlot[80];			// 2013.07.16
		char szToDevice[20];
		char szToSlot[80];				// 2013.07.16
		char szData[80];
	} TRANSFER_LOG_STRU;
	TRANSFER_LOG_STRU m_TransferLog;

	typedef struct _FUNCTION_LOG_STRU
	{
		char szDate[20];
		char szTime[20];
		char szDeviceID[20];
		char szLogType[20];
		char szEventID[20];
		char szStatus[20];
		char szMaterialID[80];			// 2013.07.16
		char szMaterialType[20];
		char szData[80];
	} FUNCTION_LOG_STRU;
	FUNCTION_LOG_STRU m_FunctionLog;

	typedef struct _LOT_EVENT_LOG_STRU
	{
		char szDate[20];
		char szTime[20];
		char szDeviceID[20];
		char szLogType[20];
		char szEventID[20];
		char szLotID[20];				// 2013.07.30
		char szFlowRcpID[40];
		char szFlowInfo[256];
		char szCarrierID[20];
		char szData[80];
	} LOT_EVENT_LOG_STRU;
	LOT_EVENT_LOG_STRU m_LotEventLog;

	typedef struct _CFG_EVENT_LOG_STRU
	{
		char szDate[20];
		char szTime[20];
		char szDeviceID[20];
		char szLogType[20];
		char szEventID[20];
		char szData[80];
	} CFG_EVENT_LOG_STRU;
	CFG_EVENT_LOG_STRU m_CfgEventLog;

	// LPM Info
	#define MAX_LPM		4
	#define MAX_PM		10					// 2013.07.01
	typedef struct _LPM_INFO_STRU
	{
		BOOL bUse;
		int nLPMStatus;
		char szDeviceID[20];
		char szFlowRcpID[40];
		char szFlowInfo[256];
		char szCarrierID[20];
		char szMapInfo[40];
		char szLotID[20];
		BOOL abPM_Flow[MAX_PM];
	} LPM_INFO_STRU;
	LPM_INFO_STRU m_LPMInfo[MAX_LPM];

	// PM Info
	typedef struct _PM_INFO_STRU
	{
		BOOL bUse;
		int nCommStatus;
		char szDeviceID[20];
		char szWaferInfo[80];					// 2013.07.16
		char szLotID[20];
		char szRecipeID[40];
		char szSlotInfo[20];
		BOOL bIsProcRunning;
		int nRcpType;							// 2013.07.29
	} PM_INFO_STRU;
	PM_INFO_STRU m_PMInfo[MAX_PM];

	// Global Configuration
	char m_szRunLogPath[256];
	char m_szMccLogPath[256];
	int	m_nKeepingDays;
	int m_nPMSlotCount;
	// 2013.07.01
	char m_szRunLogFile[80];
	char m_szEquipNameIO[40];

	// 2013.06.03
	int m_nDelimiter;
	char m_szDelimiter[8];
	enum {D_TAB, D_SPACE};
};

#endif // !defined(AFX_EVENTLOGSERVER_H__E7E923F4_0298_4906_8197_71CCD90BAC10__INCLUDED_)
