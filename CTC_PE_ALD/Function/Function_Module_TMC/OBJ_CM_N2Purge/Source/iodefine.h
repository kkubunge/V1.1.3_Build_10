#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	/*
	IO_DRIVER_LODING,	

	//... 2017.06.21
	//... 2017.11.08 Update
	PRMD_N2Mode				,
	//	4 Nozzle
	PRMA_Pre_Nozzle1		,
	PRMA_Pre_Nozzle2		,
	PRMD_Pre_ExhNozzle		,
	PRMA_Pre_Time			,
	PRMA_TPre_Nozzle1		,
	PRMA_TPre_Nozzle2		,
	PRMD_TPre_ExhNozzle		,
	PRMA_TPre_Time			,
	PRMA_Proc_Nozzle1		,
	PRMA_Proc_Nozzle2		,
	PRMD_Proc_ExhNozzle		,
	PRMA_Proc_Time			,	
	PRMA_Post_Nozzle1		,
	PRMA_Post_Nozzle2		,
	PRMD_Post_ExhNozzle		,
	PRMA_Post_Time			,
	PRMA_PPost_Nozzle1		,
	PRMA_PPost_Nozzle2		,
	PRMD_PPost_ExhNozzle	,
	PRMA_PPost_Time			,
	//	2 Nozzle
	PRMA_Pre_2Nozzle1		,
	PRMD_Pre_2ExhNozzle		,
	PRMA_Pre_2Time			,
	PRMA_TPre_2Nozzle1		,
	PRMD_TPre_2ExhNozzle	,
	PRMA_TPre_2Time			,
	PRMA_Proc_2Nozzle1		,
	PRMD_Proc_2ExhNozzle	,
	PRMA_Proc_2Time			,	
	PRMA_Post_2Nozzle1		,
	PRMD_Post_2ExhNozzle	,
	PRMA_Post_2Time			,
	PRMA_PPost_2Nozzle1		,
	PRMD_PPost_2ExhNozzle	,
	PRMA_PPost_2Time		,
	//	Common
	PRMD_Man_Flow_Mode		,
	PRMA_Purge_Nozzle1		,
	PRMA_Purge_Nozzle2		,
	PRMA_Purge_ExhNozzle	,
	PRMA_Purge_PurgeTime	,
	PRMA_Purge_ElapseTime	,
	PRMA_Purge_IdleTime		,
	PRMA_N2_Chk_Delay		,
	PRMA_Nozzle1_WarnTol	,
	PRMA_Nozzle1_AbortTol	,
	PRMA_Nozzle1_ChkCnt		,
	PRMA_Nozzle2_WarnTol	,
	PRMA_Nozzle2_AbortTol	,
	PRMA_Nozzle2_ChkCnt		,
	PRMA_MainPrs_LowLmt		,
	PRMA_MainPrs_HighLmt	,
	PRMA_MainPrs_ChkCnt		,
	PRMD_TPre_Repeat		,
	PRMA_TPre_OnTime		,
	PRMA_TPre_OffTime		,
	//...
	PRMD_N2PrgStep,
	PLACEMENT_STS,
	PRESENT_STS,
	CLAMP_STS,

	N2_BLOW_START,
	ManPurgeStatus,
	N2PurgeSet,
	//...
	Request,
	Response,

*/
IO_DRIVER_LODING	,
PRMD_N2Mode			,
PRMA_Pre_Nozzle1		,
PRMA_Pre_Nozzle2		,
PRMD_Pre_ExhNozzle	,
PRMA_Pre_Time			,
PRMA_TPre_Nozzle1		,
PRMA_TPre_Nozzle2		,
PRMD_TPre_ExhNozzle	,
PRMA_TPre_Time		,
PRMA_Proc_Nozzle1		,
PRMA_Proc_Nozzle2		,
PRMD_Proc_ExhNozzle	,
PRMA_Proc_Time		,
PRMA_Post_Nozzle1		,
PRMA_Post_Nozzle2		,
PRMD_Post_ExhNozzle	,
PRMA_Post_Time		,
PRMA_PPost_Nozzle1	,
PRMA_PPost_Nozzle2	,
PRMD_PPost_ExhNozzle	,
PRMA_PPost_Time		,

PRMA_Pre_2Nozzle1		,
PRMD_Pre_2ExhNozzle	,
PRMA_Pre_2Time		,
PRMA_TPre_2Nozzle1	,
PRMD_TPre_2ExhNozzle	,
PRMA_TPre_2Time		,
PRMA_Proc_2Nozzle1	,
PRMD_Proc_2ExhNozzle	,
PRMA_Proc_2Time		,
PRMA_Post_2Nozzle1	,
PRMD_Post_2ExhNozzle	,
PRMA_Post_2Time		,
PRMA_PPost_2Nozzle1	,
PRMD_PPost_2ExhNozzle	,
PRMA_PPost_2Time		,


PRMD_Man_Flow_Mode	,
PRMA_Purge_Nozzle1	,
PRMA_Purge_Nozzle2	,
PRMA_Purge_ExhNozzle	,
PRMA_Purge_PurgeTime	,
PRMA_Purge_ElapseTime	,
PRMA_Purge_IdleTime	,
PRMA_N2_Chk_Delay		,
PRMA_Nozzle1_WarnTol	,
PRMA_Nozzle1_AbortTol	,
PRMA_Nozzle1_ChkCnt	,
PRMA_Nozzle2_WarnTol	,
PRMA_Nozzle2_AbortTol	,
PRMA_Nozzle2_ChkCnt	,
PRMA_MainPrs_LowLmt	,
PRMA_MainPrs_HighLmt	,
PRMA_MainPrs_ChkCnt	,
PRMD_TPre_Repeat		,
PRMA_TPre_OnTime		,
PRMA_TPre_OffTime		,


PRMD_N2PrgStep		,
PLACEMENT_STS			,
PRESENT_STS			,
CLAMP_STS				,

N2_BLOW_START			,
ManPurgeStatus		,
N2PurgeSet			,

Request				,
Response				,



} IOPointIndex;
//------------------------------------------------------------------------------------------

char szStepCmd[10][15] = {"CFG_N2_PRE1", "CFG_N2_PROC1", "CFG_N2_POST1", "CFG_N2_TPRE1", "CFG_N2_PPOST1",
						  "CFG_N2_PRE", "CFG_N2_PROC", "CFG_N2_POST", "CFG_N2_TPRE", "CFG_N2_PPOST"};

typedef enum {
	NotUse,	Nozzle2, Nozzle4, Auto
}N2MODE;
typedef enum { IdleMode_0, FoupMode_1 };
typedef enum { ON_0, OFF_1 };
typedef enum { OFF_0, ON_1 };
typedef enum { NONEXIST_0, EXIST_1 };
typedef enum { Unknown_0, Clamp_1, Unclamp_2, Error_3 };
typedef enum { Pre_0, Process_1, Post_2, Pre_3, PPost_4, NozzleBlow_5, Idle_6, Purge_7, Error_8};
//------------------------------------------------------------------------------------------

void SetPurgeParam(char *cmd, int ioNozzle1, int ioNozzle2, int ioExhNozzle, int ioPurgeTime);	
void SetN2Purge();
void SetN2ManPurge();
void SetN2ManStop();

//------------------------------------------------------------------------------------------
#endif


