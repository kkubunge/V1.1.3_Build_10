#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_DRIVER_LODING,	

	//... 2017.06.21
	//... 2017.11.08 Update
	PRMD_N2Mode				,
	//	4 Nozzle
	// Front MFC
	PRMA_Pre_Nozzle1		,
	PRMA_TPre_Nozzle1		,
	PRMA_Proc_Nozzle1		,
	PRMA_Post_Nozzle1		,
	PRMA_PPost_Nozzle1		,
	// Rear MFC
	PRMA_Pre_Nozzle2		,
	PRMA_TPre_Nozzle2		,
	PRMA_Proc_Nozzle2		,
	PRMA_Post_Nozzle2		,
	PRMA_PPost_Nozzle2		,
	// Exhaust Valve
	PRMD_Pre_ExhNozzle		,
	PRMD_TPre_ExhNozzle		,
	PRMD_Proc_ExhNozzle		,
	PRMD_Post_ExhNozzle		,
	PRMD_PPost_ExhNozzle	,
	// Purge Time
	PRMA_Pre_Time			,	
	PRMA_TPre_Time			,	
	PRMA_Proc_Time			,	
	PRMA_Post_Time			,	
	PRMA_PPost_Time			,
	//	2 Nozzle
	// Front MFC
	PRMA_Pre_2Nozzle1		,
	PRMA_TPre_2Nozzle1		,
	PRMA_Proc_2Nozzle1		,
	PRMA_Post_2Nozzle1		,
	PRMA_PPost_2Nozzle1		,
	// Exhaust Valve
	PRMD_Pre_2ExhNozzle		,
	PRMD_TPre_2ExhNozzle	,
	PRMD_Proc_2ExhNozzle	,
	PRMD_Post_2ExhNozzle	,
	PRMD_PPost_2ExhNozzle	,
	// Purge Time
	PRMA_Pre_2Time			,	
	PRMA_TPre_2Time			,	
	PRMA_Proc_2Time			,	
	PRMA_Post_2Time			,	
	PRMA_PPost_2Time		,
	// MFC Warn/Abort Band
	PRMA_Nozzle1_WarnTol	,
	PRMA_Nozzle1_AbortTol	,
	PRMA_Nozzle1_ChkCnt		,
	PRMA_Nozzle2_WarnTol	,
	PRMA_Nozzle2_AbortTol	,
	PRMA_Nozzle2_ChkCnt		,
	// Main Pressure Warn/Abort Band
	PRMA_MainPrs_LowLmt		,
	PRMA_MainPrs_HighLmt	,
	PRMA_MainPrs_ChkCnt		,
	// Real Air I/O
	PRMA_N2Flow1			,
	PRMA_N2Flow2			,
	PRMA_N2MainPrs			,
	PRMA_N2ExhPrs			,
	PRMA_OxygenDensity		,
	// Real I/O
	DOOR_CLOSE				,
	UNDOCK_STS				,
	CLAMP_STS				,
	DOCK_STS				,
	PLACEMENT_STS			,
	PRESENT_STS				,
	FOUP_EXIST_STS			,
	// Status I/O
	N2_PRE_START			,
	N2_TPRE_START			,
	N2_PROC_START			,
	N2_POST_START			,
	N2_PPOST_START			,
	// ..
	N2_PRE_END				,
	N2_TPRE_END				,
	N2_PROC_END				,
	N2_POST_END				,
	N2_PPOST_END			,
	// ..
	N2_PRE_STOP				,
	N2_TPRE_STOP			,
	N2_PROC_STOP			,
	N2_POST_STOP			,
	N2_PPOST_STOP 			,
	// ..
	N2_PRE_WARNING 			,
	N2_TPRE_WARNING			,
	N2_PROC_WARNING			,
	N2_POST_WARNING			,
	N2_PPOST_WARNING		,
	// ..
	N2_BLOW_START			,
	N2_BLOW_END				,
	N2_BLOW_STOP			,
	N2_BLOW_WARNING 		,
	N2_PURGE_START			,
	N2_PURGE_END			,
	N2_PURGE_STOP			,
	N2_PURGE_WARNING		,
	// ..
	PRMD_N2FOUPSts			,
	PRMA_PRM_CHECK_DELAY	,
	PRMA_Purge_ElapseTime	,
	// ..
	PRMA_N2PrgStep			,
	RESET_CTRL				,
	PRMD_MODE_STATUS		,
	//...
	Request,
	Response,
	PurgeEventID,
	
} IOPointIndex;

typedef struct  
{
	int nStepStartIO;
	int nStepEndIO;
	int nStepStopIO;
	int nStepWarnIO;
} N2LPStsIO;

typedef struct
{
	double dbFrontMFCSet;
	double dbRearMFCSet;   
	double dbPurgeTime;
	double dbFrontMFCWarnTol;
	double dbFrontMFCAbortTol;
	double dbRearMFCWarnTol;
	double dbRearMFCAbortTol;  
	int nFrontErrorCnt;
	int nRearErrorCnt;
} N2LPInfo;

//------------------------------------------------------------------------------------------
typedef enum { Nozzle2_0, Nozzle4_1, Unknown_2 }N2MODE;
typedef enum { Unknown_0, Absent_1, Nozzle2_2, Nozzle4_3 };
typedef enum { NONEXIST_0, EXIST_1 };
typedef enum { OFF_0, ON_1 };
typedef enum { None_0, Reset_1 };
typedef enum { IdleMode_0, FoupMode_1 };
typedef enum { Pre_Step_0, TPre_Step_1, Proc_Step_2, Post_Step_3, PPost_Step_4 };
typedef enum { Pre_0, Process_1, Post_2, TPre_3, PPost_4, NozzleBlow_5, Idle_6, Purge_7, Error_8};
typedef enum { NotUse_0, Disable_1, Maint_2, Standby_3 };

char szStepCmd[10][15] = {"CFG_N2_PRE1", "CFG_N2_PROC1", "CFG_N2_POST1", "CFG_N2_TPRE1", "CFG_N2_PPOST1",
						  "CFG_N2_PRE", "CFG_N2_PROC", "CFG_N2_POST", "CFG_N2_TPRE", "CFG_N2_PPOST"};
N2LPStsIO m_pN2LPStsIO[5];
N2LPInfo m_pN2LPInfo[2][5];
char m_szModuleName[10] = "\0";
int  m_nEventInfo = 0;
BOOL m_bIsMonitorStart = FALSE;
BOOL SIMULATION_MODE = FALSE;


//------------------------------------------------------------------------------------------
void Read_Parameter();
void IsMFCMonOK(int nFoupSts);
void IsMainPressureOK();
BOOL IsProcessStart();
void IsProcComplete();
int	 WhatIsFoup();
void AlarmMessageAttach(int AlarmID, char *AlarmAttach);

static DWORD WINAPI StartMonitor(LPVOID pVoid);
static DWORD WINAPI StartPrsMonitor(LPVOID pVoid);

//------------------------------------------------------------------------------------------
#endif


