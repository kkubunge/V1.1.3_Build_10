///////////////////////////////////////////////////////////////////////////////
// AbortProcess

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

//------------------------------------------------------------------------------------------

#define FUNC_NAME		"ALDValveControl"

// #define MAX_ALD_VALVE	10		//... For MAHA-DPT
#define MAX_ALD_VALVE		13		//... 2015.04.28 Add ALD Valve 3 EA (V11, V12, V17)
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {

	{ "PRO_RunStsDM"		,	_K_D_IO	,	PRO_RunStsDM,			0 } ,
	{ "SW_GbIntlckDM"		,	_K_D_IO	,	SW_GbIntlckDM,			0 } ,
	{ "PLC_GbIntlckDI"		,	_K_D_IO	,	PLC_GbIntlckDI,			0 } ,

	//... 2015.02.28 ALD Valve Verification
	{ "M_AVlv_ParmSaveDM"	,	_K_D_IO	,	M_AVlv_ParmSaveDM,		0 } ,
	{ "M_AVlv_ResultDM"		,	_K_D_IO	,	M_AVlv_ResultDM,		0 } ,

	{ "M_AVlv_CycTgtCntAM"	,	_K_A_IO	,	M_AVlv_CycTgtCntAM,		0 } ,
	{ "M_AVlv_CycCurCntAM"	,	_K_A_IO	,	M_AVlv_CycCurCntAM,		0 } ,

	//////////////////////////////////////////////////////////////////////////
	//... 2015.04.28 Fixed ALD Valve 13 EA (Top 10 EA, Canister 3 EA)
	{ "M_AVlv01_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv01_OP_MinTmAM,	0 } ,
	{ "M_AVlv02_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv02_OP_MinTmAM,	0 } ,
	{ "M_AVlv03_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv03_OP_MinTmAM,	0 } ,
	{ "M_AVlv04_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv04_OP_MinTmAM,	0 } ,
	{ "M_AVlv05_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv05_OP_MinTmAM,	0 } ,
	{ "M_AVlv06_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv06_OP_MinTmAM,	0 } ,
	{ "M_AVlv07_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv07_OP_MinTmAM,	0 } ,
	{ "M_AVlv08_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv08_OP_MinTmAM,	0 } ,
	{ "M_AVlv09_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv09_OP_MinTmAM,	0 } ,
	{ "M_AVlv10_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv10_OP_MinTmAM,	0 } ,
	{ "M_AVlv11_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv11_OP_MinTmAM,	0 } ,
	{ "M_AVlv12_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv12_OP_MinTmAM,	0 } ,
	{ "M_AVlv13_OP_MinTmAM"	,	_K_A_IO	,	M_AVlv13_OP_MinTmAM,	0 } ,
	//
	{ "M_AVlv01_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv01_OP_MaxTmAM,	0 } ,
	{ "M_AVlv02_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv02_OP_MaxTmAM,	0 } ,
	{ "M_AVlv03_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv03_OP_MaxTmAM,	0 } ,
	{ "M_AVlv04_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv04_OP_MaxTmAM,	0 } ,
	{ "M_AVlv05_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv05_OP_MaxTmAM,	0 } ,
	{ "M_AVlv06_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv06_OP_MaxTmAM,	0 } ,
	{ "M_AVlv07_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv07_OP_MaxTmAM,	0 } ,
	{ "M_AVlv08_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv08_OP_MaxTmAM,	0 } ,
	{ "M_AVlv09_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv09_OP_MaxTmAM,	0 } ,
	{ "M_AVlv10_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv10_OP_MaxTmAM,	0 } ,
	{ "M_AVlv11_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv11_OP_MaxTmAM,	0 } ,
	{ "M_AVlv12_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv12_OP_MaxTmAM,	0 } ,
	{ "M_AVlv13_OP_MaxTmAM"	,	_K_A_IO	,	M_AVlv13_OP_MaxTmAM,	0 } ,
	//
	{ "M_AVlv01_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv01_OP_AvgTmAM,	0 } ,
	{ "M_AVlv02_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv02_OP_AvgTmAM,	0 } ,
	{ "M_AVlv03_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv03_OP_AvgTmAM,	0 } ,
	{ "M_AVlv04_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv04_OP_AvgTmAM,	0 } ,
	{ "M_AVlv05_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv05_OP_AvgTmAM,	0 } ,
	{ "M_AVlv06_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv06_OP_AvgTmAM,	0 } ,
	{ "M_AVlv07_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv07_OP_AvgTmAM,	0 } ,
	{ "M_AVlv08_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv08_OP_AvgTmAM,	0 } ,
	{ "M_AVlv09_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv09_OP_AvgTmAM,	0 } ,
	{ "M_AVlv10_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv10_OP_AvgTmAM,	0 } ,
	{ "M_AVlv11_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv11_OP_AvgTmAM,	0 } ,
	{ "M_AVlv12_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv12_OP_AvgTmAM,	0 } ,
	{ "M_AVlv13_OP_AvgTmAM"	,	_K_A_IO	,	M_AVlv13_OP_AvgTmAM,	0 } ,
	//
	{ "M_AVlv01_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv01_CL_MinTmAM,	0 } ,
	{ "M_AVlv02_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv02_CL_MinTmAM,	0 } ,
	{ "M_AVlv03_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv03_CL_MinTmAM,	0 } ,
	{ "M_AVlv04_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv04_CL_MinTmAM,	0 } ,
	{ "M_AVlv05_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv05_CL_MinTmAM,	0 } ,
	{ "M_AVlv06_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv06_CL_MinTmAM,	0 } ,
	{ "M_AVlv07_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv07_CL_MinTmAM,	0 } ,
	{ "M_AVlv08_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv08_CL_MinTmAM,	0 } ,
	{ "M_AVlv09_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv09_CL_MinTmAM,	0 } ,
	{ "M_AVlv10_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv10_CL_MinTmAM,	0 } ,
	{ "M_AVlv11_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv11_CL_MinTmAM,	0 } ,
	{ "M_AVlv12_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv12_CL_MinTmAM,	0 } ,
	{ "M_AVlv13_CL_MinTmAM"	,	_K_A_IO	,	M_AVlv13_CL_MinTmAM,	0 } ,
	//
	{ "M_AVlv01_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv01_CL_MaxTmAM,	0 } ,
	{ "M_AVlv02_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv02_CL_MaxTmAM,	0 } ,
	{ "M_AVlv03_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv03_CL_MaxTmAM,	0 } ,
	{ "M_AVlv04_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv04_CL_MaxTmAM,	0 } ,
	{ "M_AVlv05_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv05_CL_MaxTmAM,	0 } ,
	{ "M_AVlv06_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv06_CL_MaxTmAM,	0 } ,
	{ "M_AVlv07_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv07_CL_MaxTmAM,	0 } ,
	{ "M_AVlv08_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv08_CL_MaxTmAM,	0 } ,
	{ "M_AVlv09_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv09_CL_MaxTmAM,	0 } ,
	{ "M_AVlv10_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv10_CL_MaxTmAM,	0 } ,
	{ "M_AVlv11_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv11_CL_MaxTmAM,	0 } ,
	{ "M_AVlv12_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv12_CL_MaxTmAM,	0 } ,
	{ "M_AVlv13_CL_MaxTmAM"	,	_K_A_IO	,	M_AVlv13_CL_MaxTmAM,	0 } ,
	//
	{ "M_AVlv01_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv01_CL_AvgTmAM,	0 } ,
	{ "M_AVlv02_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv02_CL_AvgTmAM,	0 } ,
	{ "M_AVlv03_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv03_CL_AvgTmAM,	0 } ,
	{ "M_AVlv04_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv04_CL_AvgTmAM,	0 } ,
	{ "M_AVlv05_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv05_CL_AvgTmAM,	0 } ,
	{ "M_AVlv06_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv06_CL_AvgTmAM,	0 } ,
	{ "M_AVlv07_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv07_CL_AvgTmAM,	0 } ,
	{ "M_AVlv08_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv08_CL_AvgTmAM,	0 } ,
	{ "M_AVlv09_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv09_CL_AvgTmAM,	0 } ,
	{ "M_AVlv10_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv10_CL_AvgTmAM,	0 } ,
	{ "M_AVlv11_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv11_CL_AvgTmAM,	0 } ,
	{ "M_AVlv12_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv12_CL_AvgTmAM,	0 } ,
	{ "M_AVlv13_CL_AvgTmAM"	,	_K_A_IO	,	M_AVlv13_CL_AvgTmAM,	0 } ,
	
	//... Real ALD Valve
	{ "V97ValveDO"			,	_K_D_IO	,	V97ValveDO,				0 } ,
	{ "V98ValveDO"			,	_K_D_IO	,	V98ValveDO,				0 } ,
	{ "V100ValveDO"			,	_K_D_IO	,	V100ValveDO,			0 } ,
	{ "V101ValveDO"			,	_K_D_IO	,	V101ValveDO,			0 } ,
	{ "V102ValveDO"			,	_K_D_IO	,	V102ValveDO,			0 } ,
	{ "V105ValveDO"			,	_K_D_IO	,	V105ValveDO,			0 } ,
	{ "V106ValveDO"			,	_K_D_IO	,	V106ValveDO,			0 } ,
	{ "V108ValveDO"			,	_K_D_IO	,	V108ValveDO,			0 } ,
	{ "V109ValveDO"			,	_K_D_IO	,	V109ValveDO,			0 } ,
	{ "V110ValveDO"			,	_K_D_IO	,	V110ValveDO,			0 } ,
	{ "V11ValveDO"			,	_K_D_IO	,	V11ValveDO,				0 } ,
	{ "V12ValveDO"			,	_K_D_IO	,	V12ValveDO,				0 } ,
	{ "V17ValveDO"			,	_K_D_IO	,	V17ValveDO,				0 } ,
	//
	{ "V97ValveDI"			,	_K_D_IO	,	V97ValveDI,				0 } ,
	{ "V98ValveDI"			,	_K_D_IO	,	V98ValveDI,				0 } ,
	{ "V100ValveDI"			,	_K_D_IO	,	V100ValveDI,			0 } ,
	{ "V101ValveDI"			,	_K_D_IO	,	V101ValveDI,			0 } ,
	{ "V102ValveDI"			,	_K_D_IO	,	V102ValveDI,			0 } ,
	{ "V105ValveDI"			,	_K_D_IO	,	V105ValveDI,			0 } ,
	{ "V106ValveDI"			,	_K_D_IO	,	V106ValveDI,			0 } ,
	{ "V108ValveDI"			,	_K_D_IO	,	V108ValveDI,			0 } ,
	{ "V109ValveDI"			,	_K_D_IO	,	V109ValveDI,			0 } ,
	{ "V110ValveDI"			,	_K_D_IO	,	V110ValveDI,			0 } ,
	{ "V11ValveDI"			,	_K_D_IO	,	V11ValveDI,				0 } ,
	{ "V12ValveDI"			,	_K_D_IO	,	V12ValveDI,				0 } ,
	{ "V17ValveDI"			,	_K_D_IO	,	V17ValveDI,				0 } ,
	//
	{ "V97VlvOpenTmAI"		,	_K_A_IO	,	V97VlvOpenTmAI,			0 } ,
	{ "V98VlvOpenTmAI"		,	_K_A_IO	,	V98VlvOpenTmAI,			0 } ,
	{ "V100VlvOpenTmAI"		,	_K_A_IO	,	V100VlvOpenTmAI,		0 } ,
	{ "V101VlvOpenTmAI"		,	_K_A_IO	,	V101VlvOpenTmAI,		0 } ,
	{ "V102VlvOpenTmAI"		,	_K_A_IO	,	V102VlvOpenTmAI,		0 } ,
	{ "V105VlvOpenTmAI"		,	_K_A_IO	,	V105VlvOpenTmAI,		0 } ,
	{ "V106VlvOpenTmAI"		,	_K_A_IO	,	V106VlvOpenTmAI,		0 } ,
	{ "V108VlvOpenTmAI"		,	_K_A_IO	,	V108VlvOpenTmAI,		0 } ,
	{ "V109VlvOpenTmAI"		,	_K_A_IO	,	V109VlvOpenTmAI,		0 } ,
	{ "V110VlvOpenTmAI"		,	_K_A_IO	,	V110VlvOpenTmAI,		0 } ,
	{ "V11VlvOpenTmAI"		,	_K_A_IO	,	V11VlvOpenTmAI,			0 } ,
	{ "V12VlvOpenTmAI"		,	_K_A_IO	,	V12VlvOpenTmAI,			0 } ,
	{ "V17VlvOpenTmAI"		,	_K_A_IO	,	V17VlvOpenTmAI,			0 } ,
	//
	{ "V97VlvCloseTmAI"		,	_K_A_IO	,	V97VlvCloseTmAI,		0 } ,
	{ "V98VlvCloseTmAI"		,	_K_A_IO	,	V98VlvCloseTmAI,		0 } ,
	{ "V100VlvCloseTmAI"	,	_K_A_IO	,	V100VlvCloseTmAI,		0 } ,
	{ "V101VlvCloseTmAI"	,	_K_A_IO	,	V101VlvCloseTmAI,		0 } ,
	{ "V102VlvCloseTmAI"	,	_K_A_IO	,	V102VlvCloseTmAI,		0 } ,
	{ "V105VlvCloseTmAI"	,	_K_A_IO	,	V105VlvCloseTmAI,		0 } ,
	{ "V106VlvCloseTmAI"	,	_K_A_IO	,	V106VlvCloseTmAI,		0 } ,
	{ "V108VlvCloseTmAI"	,	_K_A_IO	,	V108VlvCloseTmAI,		0 } ,
	{ "V109VlvCloseTmAI"	,	_K_A_IO	,	V109VlvCloseTmAI,		0 } ,
	{ "V110VlvCloseTmAI"	,	_K_A_IO	,	V110VlvCloseTmAI,		0 } ,
	{ "V11VlvCloseTmAI"		,	_K_A_IO	,	V11VlvCloseTmAI,		0 } ,
	{ "V12VlvCloseTmAI"		,	_K_A_IO	,	V12VlvCloseTmAI,		0 } ,
	{ "V17VlvCloseTmAI"		,	_K_A_IO	,	V17VlvCloseTmAI,		0 },
	//... 2016.04.04
	{ "LeakChkStsDM"		,	_K_D_IO,	LeakChkStsDM,			0 },
	{ "M_AVlv_IntCheckDM"	,	_K_D_IO,	M_AVlv_IntCheckDM,		0 },
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// Global Variable

//////////////////////////////////////////////////////////////////////////
BOOL CheckInterlock()
{
	BOOL	bRet		= FALSE;
	int		nIOStatus	= 0;

	enum { ePROC_STOP, ePROC_RUN };
	enum { eALMSTS_NORMAL, eALMSTS_ALARM };
	enum { eOFFON_OFF, eOFFON_ON };

	do 
	{
		//... Check Recipe Running
		if(ePROC_STOP != READ_DIGITAL(PRO_RunStsDM, &nIOStatus))		break;

		//... Check SW Interlock
		if(eALMSTS_NORMAL != READ_DIGITAL(SW_GbIntlckDM, &nIOStatus)
			&&eOFFON_ON == READ_DIGITAL(M_AVlv_IntCheckDM, &nIOStatus))	break;

		//... Check PLC Interlock
		if(eALMSTS_NORMAL != READ_DIGITAL(PLC_GbIntlckDI, &nIOStatus)
			&&eOFFON_ON == READ_DIGITAL(M_AVlv_IntCheckDM, &nIOStatus))	break;

		bRet = TRUE;
	} while (0);

	return	bRet;
}

//////////////////////////////////////////////////////////////////////////
//... 2016.04.04
Module_Status DynamicLK()
{
	Module_Status msRet = SYS_ABORTED;

	int		nIOStatus	= 0;
	int		nTargetCnt	= 0;
	int		nCurrentCnt	= 0;
	int		i			= 0;

	double	dbOpenSumTime[MAX_ALD_VALVE];
	double	dbOpenMinTime[MAX_ALD_VALVE];
	double	dbOpenMaxTime[MAX_ALD_VALVE];

	double	dbCloseSumTime[MAX_ALD_VALVE];
	double	dbCloseMinTime[MAX_ALD_VALVE];
	double	dbCloseMaxTime[MAX_ALD_VALVE];

	double	dbCurTime	= 0;
	double	dbAvrTime	= 0;

	enum { eVLV_CLOSE, eVLV_OPEN };
	enum { eOFFON_OFF, eOFFON_ON };
	enum { eRESULT_NONE, eRESULT_ERROR, eRESULT_OK };
	enum { eALMSTS_NORMAL, eALMSTS_ALARM };

	//////////////////////////////////////////////////////////////////////////
	//... Check Target Cycle Count
	nTargetCnt = (int) READ_ANALOG(M_AVlv_CycTgtCntAM, &nIOStatus);
	if(nTargetCnt < 1)		return SYS_SUCCESS;

	//////////////////////////////////////////////////////////////////////////
	//... Init ALD Valve Time
	for(i = 0; i < MAX_ALD_VALVE; i++)
	{
		//... Open Info
		dbOpenSumTime[i] = 0;
		dbOpenMinTime[i] = 0;
		dbOpenMaxTime[i] = 0;
		
		WRITE_ANALOG(M_AVlv01_OP_MinTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_AVlv01_OP_MaxTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_AVlv01_OP_AvgTmAM + i, 0, &nIOStatus);

		//... Close Info
		dbCloseSumTime[i] = 0;
		dbCloseMinTime[i] = 0;
		dbCloseMaxTime[i] = 0;

		WRITE_ANALOG(M_AVlv01_CL_MinTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_AVlv01_CL_MaxTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_AVlv01_CL_AvgTmAM + i, 0, &nIOStatus);
	}

	//... Init Result Status
	WRITE_DIGITAL(M_AVlv_ResultDM, eRESULT_NONE, &nIOStatus);

	//////////////////////////////////////////////////////////////////////////
	//... Run ALD Valve Verification	
	do 
	{
		//... Check Cycle Count
		nCurrentCnt++;
		if(nCurrentCnt > nTargetCnt)
		{
			msRet = SYS_SUCCESS;
			break;		//... Completed
		}

		//... Update Current Count
		WRITE_ANALOG(M_AVlv_CycCurCntAM, nCurrentCnt, &nIOStatus);

		for(i = 0; i < MAX_ALD_VALVE; i++)
		{
			//////////////////////////////////////////////////////////////////////////
			//... 2016.04.04 For Dynamic Leak Check 
			//... ALD Valve On/Off Operate while Gas Line Leak Check is Running
			//... To Protect Valve Interlock
			/* V97       V100        V102     V105      V108       V110     V11 */ 
			if(i == 0 || i == 2 || i == 4 || i == 5 || i == 7 || i == 9 || i == 10) continue;

			//////////////////////////////////////////////////////////////////////////
			//... Check Interlock
			//... Check SW Interlock
			if(eALMSTS_NORMAL != READ_DIGITAL(SW_GbIntlckDM, &nIOStatus))	break;

			//... Check PLC Interlock
			if(eALMSTS_NORMAL != READ_DIGITAL(PLC_GbIntlckDI, &nIOStatus))	break;

			//////////////////////////////////////////////////////////////////////////
			//... Set Valve OPEN
			WRITE_DIGITAL(V97ValveDO + i, eVLV_OPEN, &nIOStatus);
			if(! WAIT_SECONDS(1))		break;

			//... Check Valve OPEN
			if(eVLV_OPEN != READ_DIGITAL(V97ValveDI + i, &nIOStatus))
			{
				//... TODO. ALARM_POST
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Update OPEN Min/Max Time
			dbCurTime = READ_ANALOG(V97VlvOpenTmAI + i, &nIOStatus);
			if(dbCurTime < dbOpenMinTime[i] || nCurrentCnt == 1)
			{
				dbOpenMinTime[i] = dbCurTime;
				WRITE_ANALOG(M_AVlv01_OP_MinTmAM + i, dbCurTime, &nIOStatus);
			}
			if(dbCurTime > dbOpenMaxTime[i] || nCurrentCnt == 1)
			{
				dbOpenMaxTime[i] = dbCurTime;
				WRITE_ANALOG(M_AVlv01_OP_MaxTmAM + i, dbCurTime, &nIOStatus);
			}
			
			//... Update OPEN Avg Time
			dbOpenSumTime[i] = dbOpenSumTime[i] + dbCurTime;
			if(dbOpenSumTime[i] > 0)	dbAvrTime = dbOpenSumTime[i] / nCurrentCnt;
			else						dbAvrTime = 0;
			WRITE_ANALOG(M_AVlv01_OP_AvgTmAM + i, dbAvrTime, &nIOStatus);

			//////////////////////////////////////////////////////////////////////////
			//... Set Valve CLOSE
			WRITE_DIGITAL(V97ValveDO + i, eVLV_CLOSE, &nIOStatus);
			if(! WAIT_SECONDS(1))		break;
			
			//... Check Valve CLOSE
			if(eVLV_CLOSE != READ_DIGITAL(V97ValveDI + i, &nIOStatus))
			{
				//... TODO. ALARM_POST
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Update CLOSE Min/Max Time
			dbCurTime = READ_ANALOG(V97VlvCloseTmAI + i, &nIOStatus);
			if(dbCurTime < dbCloseMinTime[i] || nCurrentCnt == 1)
			{
				dbCloseMinTime[i] = dbCurTime;
				WRITE_ANALOG(M_AVlv01_CL_MinTmAM + i, dbCurTime, &nIOStatus);
			}
			if(dbCurTime > dbCloseMaxTime[i] || nCurrentCnt == 1)
			{
				dbCloseMaxTime[i] = dbCurTime;
				WRITE_ANALOG(M_AVlv01_CL_MaxTmAM + i, dbCurTime, &nIOStatus);
			}
			
			//... Update CLOSE Avg Time
			dbCloseSumTime[i] = dbCloseSumTime[i] + dbCurTime;
			
			if(dbCloseSumTime[i] > 0)	dbAvrTime = dbCloseSumTime[i] / nCurrentCnt;
			else						dbAvrTime = 0;
			WRITE_ANALOG(M_AVlv01_CL_AvgTmAM + i, dbAvrTime, &nIOStatus);

			WRITE_DIGITAL(V97ValveDO + i, eVLV_OPEN, &nIOStatus);

			if(/*Error*/ 13 == READ_DIGITAL(LeakChkStsDM,&nIOStatus)
				||/*Completed*/12 == READ_DIGITAL(LeakChkStsDM,&nIOStatus))
			{
				break;
			}
		}
	} while (1);	// Unlimited Loop

	if(/*Error*/ 13 == READ_DIGITAL(LeakChkStsDM,&nIOStatus)
		||/*Completed*/12 == READ_DIGITAL(LeakChkStsDM,&nIOStatus))
	{
		//... ALD All Valve CLOSE
		for(i = 0; i < MAX_ALD_VALVE; i++)
		{
			WRITE_DIGITAL(V97ValveDO + i, eVLV_CLOSE, &nIOStatus);		
		}	
	}

	//... Update Result
	if(SYS_SUCCESS == msRet)		WRITE_DIGITAL(M_AVlv_ResultDM, eRESULT_OK,	  &nIOStatus);
	else							WRITE_DIGITAL(M_AVlv_ResultDM, eRESULT_ERROR, &nIOStatus);

	//... Toggle Save Parameter
	WRITE_DIGITAL(M_AVlv_ParmSaveDM, eOFFON_ON, &nIOStatus);

	return msRet;
}
//////////////////////////////////////////////////////////////////////////
Module_Status CycleTest()
{
	Module_Status msRet = SYS_ABORTED;

	int		nIOStatus	= 0;
	int		nTargetCnt	= 0;
	int		nCurrentCnt	= 0;
	int		i			= 0;

	double	dbOpenSumTime[MAX_ALD_VALVE];
	double	dbOpenMinTime[MAX_ALD_VALVE];
	double	dbOpenMaxTime[MAX_ALD_VALVE];

	double	dbCloseSumTime[MAX_ALD_VALVE];
	double	dbCloseMinTime[MAX_ALD_VALVE];
	double	dbCloseMaxTime[MAX_ALD_VALVE];

	double	dbCurTime	= 0;
	double	dbAvrTime	= 0;

	enum { eVLV_CLOSE, eVLV_OPEN };
	enum { eOFFON_OFF, eOFFON_ON };
	enum { eRESULT_NONE, eRESULT_ERROR, eRESULT_OK };

	//////////////////////////////////////////////////////////////////////////
	//... Check Target Cycle Count
	nTargetCnt = (int) READ_ANALOG(M_AVlv_CycTgtCntAM, &nIOStatus);
	if(nTargetCnt < 1)		return SYS_SUCCESS;

	//////////////////////////////////////////////////////////////////////////
	//... Init ALD Valve Time
	for(i = 0; i < MAX_ALD_VALVE; i++)
	{
		//... Open Info
		dbOpenSumTime[i] = 0;
		dbOpenMinTime[i] = 0;
		dbOpenMaxTime[i] = 0;
		
		WRITE_ANALOG(M_AVlv01_OP_MinTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_AVlv01_OP_MaxTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_AVlv01_OP_AvgTmAM + i, 0, &nIOStatus);

		//... Close Info
		dbCloseSumTime[i] = 0;
		dbCloseMinTime[i] = 0;
		dbCloseMaxTime[i] = 0;

		WRITE_ANALOG(M_AVlv01_CL_MinTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_AVlv01_CL_MaxTmAM + i, 0, &nIOStatus);
		WRITE_ANALOG(M_AVlv01_CL_AvgTmAM + i, 0, &nIOStatus);
	}

	//... Init Result Status
	WRITE_DIGITAL(M_AVlv_ResultDM, eRESULT_NONE, &nIOStatus);

	//////////////////////////////////////////////////////////////////////////
	//... Run ALD Valve Verification	
	do 
	{
		//... Check Cycle Count
		nCurrentCnt++;
		if(nCurrentCnt > nTargetCnt)
		{
			msRet = SYS_SUCCESS;
			break;		//... Completed
		}

		//... Update Current Count
		WRITE_ANALOG(M_AVlv_CycCurCntAM, nCurrentCnt, &nIOStatus);

		for(i = 0; i < MAX_ALD_VALVE; i++)
		{
			//////////////////////////////////////////////////////////////////////////
			//... Check Interlock
			if(FALSE == CheckInterlock())
			{
				//... TODO. ALARM_POST
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Set Valve OPEN
			WRITE_DIGITAL(V97ValveDO + i, eVLV_OPEN, &nIOStatus);
			if(! WAIT_SECONDS(1))		break;

			//... Check Valve OPEN
			if(eVLV_OPEN != READ_DIGITAL(V97ValveDI + i, &nIOStatus))
			{
				//... TODO. ALARM_POST
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Update OPEN Min/Max Time
			dbCurTime = READ_ANALOG(V97VlvOpenTmAI + i, &nIOStatus);
			if(dbCurTime < dbOpenMinTime[i] || nCurrentCnt == 1)
			{
				dbOpenMinTime[i] = dbCurTime;
				WRITE_ANALOG(M_AVlv01_OP_MinTmAM + i, dbCurTime, &nIOStatus);
			}
			if(dbCurTime > dbOpenMaxTime[i] || nCurrentCnt == 1)
			{
				dbOpenMaxTime[i] = dbCurTime;
				WRITE_ANALOG(M_AVlv01_OP_MaxTmAM + i, dbCurTime, &nIOStatus);
			}
			
			//... Update OPEN Avg Time
			dbOpenSumTime[i] = dbOpenSumTime[i] + dbCurTime;
			if(dbOpenSumTime[i] > 0)	dbAvrTime = dbOpenSumTime[i] / nCurrentCnt;
			else						dbAvrTime = 0;
			WRITE_ANALOG(M_AVlv01_OP_AvgTmAM + i, dbAvrTime, &nIOStatus);

			//////////////////////////////////////////////////////////////////////////
			//... Set Valve CLOSE
			WRITE_DIGITAL(V97ValveDO + i, eVLV_CLOSE, &nIOStatus);
			if(! WAIT_SECONDS(1))		break;
			
			//... Check Valve CLOSE
			if(eVLV_CLOSE != READ_DIGITAL(V97ValveDI + i, &nIOStatus))
			{
				//... TODO. ALARM_POST
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//... Update CLOSE Min/Max Time
			dbCurTime = READ_ANALOG(V97VlvCloseTmAI + i, &nIOStatus);
			if(dbCurTime < dbCloseMinTime[i] || nCurrentCnt == 1)
			{
				dbCloseMinTime[i] = dbCurTime;
				WRITE_ANALOG(M_AVlv01_CL_MinTmAM + i, dbCurTime, &nIOStatus);
			}
			if(dbCurTime > dbCloseMaxTime[i] || nCurrentCnt == 1)
			{
				dbCloseMaxTime[i] = dbCurTime;
				WRITE_ANALOG(M_AVlv01_CL_MaxTmAM + i, dbCurTime, &nIOStatus);
			}
			
			//... Update CLOSE Avg Time
			dbCloseSumTime[i] = dbCloseSumTime[i] + dbCurTime;
			
			if(dbCloseSumTime[i] > 0)	dbAvrTime = dbCloseSumTime[i] / nCurrentCnt;
			else						dbAvrTime = 0;
			WRITE_ANALOG(M_AVlv01_CL_AvgTmAM + i, dbAvrTime, &nIOStatus);
		}
	} while (1);	// Unlimited Loop

	//////////////////////////////////////////////////////////////////////////
	//... ALD All Valve CLOSE
	for(i = 0; i < MAX_ALD_VALVE; i++)
	{
		WRITE_DIGITAL(V97ValveDO + i, eVLV_CLOSE, &nIOStatus);		
	}

	//... Update Result
	if(SYS_SUCCESS == msRet)		WRITE_DIGITAL(M_AVlv_ResultDM, eRESULT_OK,	  &nIOStatus);
	else							WRITE_DIGITAL(M_AVlv_ResultDM, eRESULT_ERROR, &nIOStatus);

	//... Toggle Save Parameter
	WRITE_DIGITAL(M_AVlv_ParmSaveDM, eOFFON_ON, &nIOStatus);

	return msRet;
}
//==========================================================================================
extern "C"
{

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	Module_Status msRet = SYS_ERROR;
	int		nParLen = 0;
	char	*pParam;
	char	szCmd[41], szSubParam[256];

	printf("Entered %s\n", FUNC_NAME);

	do {
		pParam = PROGRAM_PARAMETER_READ();
		nParLen = strlen(pParam);

		STR_SEPERATE(pParam , szCmd, szSubParam, 255);

		if(strcmp(szCmd, "CYCLE_TEST") == 0)		msRet = CycleTest();
		else if(strcmp(szCmd, "DYN_ALDTEST") == 0)	msRet = DynamicLK();
		else
		{
			printf("Invalid Parameter in %s\n", FUNC_NAME);
			msRet = SYS_ERROR;
		}

	} while(0);

	printf("Leaved %s[%d]\n", FUNC_NAME, msRet);

	return msRet;
}

} // End of [extern "C"]
//==========================================================================================
