#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{
#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]
//==========================================================================================

#include "AlarmDefine.h"
#include "EnumDefine.h"
#include "ValveSeq.h"		

CValveSeq	g_ValveSeq;
char APP_NAME[] = "AbortGas";

BOOL	g_bLastCmd_SwapPurge = FALSE;

//------------------------------------------------------------------------------------------

IO_Name_String_Map IO_STR_Map_Table[] = {

	{ "FC_AbortGas"			, _K_D_IO ,		FC_AbortGas,		0 } ,
	{ "PROCBGRUN_CONTROL"	, _K_F_IO ,		PROCBGRUN_CONTROL,	0 } ,
	{ "MFC_CONTROL"			, _K_F_IO ,		MFC_CONTROL,		0 } ,

	{ "RoughValveDI"		, _K_D_IO ,		RoughValveDI,		0 } ,
	{ "ChmPrssDI1"			, _K_D_IO ,		ChmPrssDI2,			0 } ,
	{ "SW_GbIntlckDM"		, _K_D_IO ,		SW_GbIntlckDM,		0 } ,
	{ "PLC_GbIntlckDI"		, _K_D_IO ,		PLC_GbIntlckDI,		0 } ,

	{ "DPRunDI"				, _K_D_IO ,		DPRunDI,			0 } ,
	{ "RFPwOnDO"			, _K_D_IO ,		RFONOFF1,			0 } ,
	{ "RFPwOn2DO"			, _K_D_IO ,		RFONOFF2,			0 } ,
	{ "PRESS_CONTROL"		, _K_F_IO ,		PRESS_CONTROL,		0 } ,

	{ "PRMD_SwpPug_Opt"		, _K_D_IO ,		PRMD_SwpPug_Opt,	0 } ,
	{ "PRMA_SwpPug_Flw"		, _K_A_IO ,		PRMA_SwpPug_Flw,	0 } ,

	{ "MFC01FlwAI"			, _K_A_IO	,	MFC01Flw,			0 },
	{ "MFC02FlwAI"			, _K_A_IO	,	MFC02Flw,			0 },
	{ "MFC03FlwAI"			, _K_A_IO	,	MFC03Flw,			0 },
	{ "MFC04FlwAI"			, _K_A_IO	,	MFC04Flw,			0 },
	{ "MFC05FlwAI"			, _K_A_IO	,	MFC05Flw,			0 },
	{ "MFC06FlwAI"			, _K_A_IO	,	MFC06Flw,			0 },
	{ "MFC07FlwAI"			, _K_A_IO	,	MFC07Flw,			0 },
	{ "MFC08FlwAI"			, _K_A_IO	,	MFC08Flw,			0 },
	{ "MFC09FlwAI"			, _K_A_IO	,	MFC09Flw,			0 },
	{ "MFC10FlwAI"			, _K_A_IO	,	MFC10Flw,			0 },
	{ "MFC11FlwAI"			, _K_A_IO	,	MFC11Flw,			0 },
	{ "MFC12FlwAI"			, _K_A_IO	,	MFC12Flw,			0 },
	{ "MFC13FlwAI"			, _K_A_IO	,	MFC13Flw,			0 },
	{ "MFC14FlwAI"			, _K_A_IO	,	MFC14Flw,			0 },
	{ "MFC15FlwAI"			, _K_A_IO	,	MFC15Flw,			0 },
	{ "MFC16FlwAI"			, _K_A_IO	,	MFC16Flw,			0 },
	{ "MFC17FlwAI"			, _K_A_IO	,	MFC17Flw,			0 },
	{ "MFC18FlwAI"			, _K_A_IO	,	MFC18Flw,			0 },
	{ "MFC19FlwAI"			, _K_A_IO	,	MFC19Flw,			0 },
	{ "MFC20FlwAI"			, _K_A_IO	,	MFC20Flw,			0 },
	{ "MFC21FlwAI"			, _K_A_IO	,	MFC21Flw,			0 },
	{ "MFC22FlwAI"			, _K_A_IO	,	MFC22Flw,			0 },
	{ "MFC23FlwAI"			, _K_A_IO	,	MFC23Flw,			0 },
	{ "MFC24FlwAI"			, _K_A_IO	,	MFC24Flw,			0 },
	{ "MFC25FlwAI"			, _K_A_IO	,	MFC25Flw,			0 },

	""
};

//------------------------------------------------------------------------------------------
enum { VLV_CLOSE, VLV_OPEN };
enum { EN_NORMAL, EN_INTERLOCK };
enum { RV_UNKNOWN, RV_OPEN, RV_CLOSE, RV_ERROR };
enum { SN_TRANS, SN_ATM, SN_VACUUM, SN_ERROR };
enum { EN_OFF, EN_ON };
enum { CO_CLOSE, CO_OPEN };
enum { SltVv_OPEN, SltVv_Close };

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int READ_DIGITAL_COMMCHECK(char * pszFtnName, int nIO_Enum, int * pnCommStatus)
{	
	int nRetData = -1;
	int nTempData = -1;	
	do
	{
		nTempData = READ_DIGITAL(nIO_Enum, pnCommStatus);
		if( FALSE == *pnCommStatus )
		{			
			ALARM_POST(DigitCommErr);			
			break;
		}
		nRetData = nTempData;
	}while(0);
	return nRetData;
}

//////////////////////////////////////////////////////////////////////////
void WRITE_DIGITAL_COMMCHECK(char * pszFtnName, int nIO_Enum, int nWrite_Value, int *pnCommStatus )
{	
	do
	{
		WRITE_DIGITAL(nIO_Enum, nWrite_Value, pnCommStatus);
		if( FALSE == *pnCommStatus )
		{			
			ALARM_POST(DigitCommErr);			
			break;
		}
		
	}while(0);
	return;
}

//////////////////////////////////////////////////////////////////////////
BOOL StablePurge()
{
	int nIOStatus;

	if( READ_DIGITAL(RoughValveDI, &nIOStatus) == RV_OPEN &&    
		READ_DIGITAL(ChmPrssDI2, &nIOStatus) == SN_VACUUM && 
		READ_DIGITAL(DPRunDI, &nIOStatus) == EN_ON && 
		READ_DIGITAL(PLC_GbIntlckDI, &nIOStatus) == EN_NORMAL && 
		READ_DIGITAL(SW_GbIntlckDM, &nIOStatus) == EN_NORMAL)
	{
		g_ValveSeq.StartValveSeq("STABLE_PURGE");
	}
	else
	{
		g_ValveSeq.StartValveSeq("ABORT_GAS");
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL WaferSwapPurge()
{
	int nIOStatus;

	double	dbMFCFlow	= 0;
	int		nVlvStepIdx	= 0;

	//... Check Swap Purge On
	if(EN_OFF == READ_DIGITAL(PRMD_SwpPug_Opt, &nIOStatus))		return TRUE; 
	
	if((READ_DIGITAL(RoughValveDI, &nIOStatus) == RV_OPEN ) &&     //2014.06.04
		READ_DIGITAL(ChmPrssDI2, &nIOStatus) == SN_VACUUM && 
		READ_DIGITAL(DPRunDI, &nIOStatus) == EN_ON && 
		READ_DIGITAL(PLC_GbIntlckDI, &nIOStatus) == EN_NORMAL)
	{
		dbMFCFlow = READ_ANALOG(PRMA_SwpPug_Flw, &nIOStatus);
		if(dbMFCFlow > 0)
		{	//... 
			nVlvStepIdx = g_ValveSeq.FindStepIdx("SWAP_PURGE");
			g_ValveSeq.m_adbMFCSet[nVlvStepIdx][0] = dbMFCFlow;
			g_ValveSeq.m_adbMFCSet[nVlvStepIdx][1] = dbMFCFlow;
			g_ValveSeq.StartValveSeq("SWAP_PURGE");
		}
	}
	else
	{
		g_ValveSeq.StartValveSeq("ABORT_GAS", TRUE);
		return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL IsMFCFlow()
{	
	int		nVlvStepIdx	= 0;
	int		nIOStatus;
	int		nMFC_No_1 = 0;
	int		nMFC_No_2 = 0;
	int		nMFC_No_3 = 0;
	double	dbMFCFlowAO	= 0;
	double	dbMFCFlowAI_1 = 0;
	double	dbMFCFlowAI_2 = 0;
	
	dbMFCFlowAO = READ_ANALOG(PRMA_SwpPug_Flw, &nIOStatus);
	nVlvStepIdx = g_ValveSeq.FindStepIdx("SWAP_PURGE");
		
	--nMFC_No_1 = g_ValveSeq.m_anMFCNo[nVlvStepIdx][0];	
	--nMFC_No_2 = g_ValveSeq.m_anMFCNo[nVlvStepIdx][1];

	dbMFCFlowAI_1 = READ_ANALOG(MFC01Flw+nMFC_No_1, &nIOStatus);	
	if (dbMFCFlowAI_1 < dbMFCFlowAO * 1.1 && dbMFCFlowAI_1 > dbMFCFlowAO * 0.9)	return FALSE;

	dbMFCFlowAI_2 = READ_ANALOG(MFC01Flw+nMFC_No_2, &nIOStatus);	
	if (dbMFCFlowAI_2 < dbMFCFlowAO * 1.1 && dbMFCFlowAI_2 > dbMFCFlowAO * 0.9)	return FALSE;

	return TRUE;
}

//==========================================================================================
extern "C"
{

//////////////////////////////////////////////////////////////////////////
Module_Status Program_Main() {
	char pszLocFtnName[] = "Program_Main";
	char pszFtnName[256];
	Module_Status msRet = SYS_ERROR;
	int  nIOResult = 0;
	int  nData     = -1;
	int	 i = 0;
	char *pParam;
	char szCmd[41], szSubParam[256];

	sprintf(pszFtnName, "%s::%s", APP_NAME, pszLocFtnName);
	printf("Entered %s [%s]\n", APP_NAME, PROGRAM_PARAMETER_READ());
	
	do 
	{
		nData = READ_DIGITAL_COMMCHECK(pszFtnName, FC_AbortGas, &nIOResult);
		if(FALSE == nIOResult)  break;
		
		if(EO_OFF == nData)   
		{
			msRet = SYS_SUCCESS;
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		pParam = PROGRAM_PARAMETER_READ();
		STR_SEPERATE(pParam , szCmd, szSubParam, 255 );

		//... RF OFF
		WRITE_DIGITAL_COMMCHECK(pszFtnName, RFONOFF1, 0, &nIOResult);
		WRITE_DIGITAL_COMMCHECK(pszFtnName, RFONOFF2, 0, &nIOResult);
	
		//... MFC Control Abort
		RUN_FUNCTION_ABORT(MFC_CONTROL);

		//... Command Control
		if     (STRCMP_L(szCmd, "STABLE_PURGE"))	StablePurge();		
		else if(STRCMP_L(szCmd, "SWAP_PURGE"))
		{
			if(TRUE == IsMFCFlow())  //... => Mfc flow 됐는지 확인해서 Set하는 구조로 변경하면 됨
			{
				if(FALSE == WaferSwapPurge())
				{	//... Case. Failed Swap Purge
					//g_bLastCmd_SwapPurge = FALSE;
					msRet = SYS_ERROR;
					break;
				}
			}
		}
		else if(STRCMP_L(szCmd, "M_SWAP_PURGE"))	WaferSwapPurge();	//... For Manual Swap Purge
		else
		{
		    RUN_FUNCTION_ABORT(PRESS_CONTROL);
			g_ValveSeq.StartValveSeq("ABORT_GAS", TRUE);
		}

		msRet = SYS_SUCCESS;

	} while(0);

	//... Change Last Command Swap Purge
	//if(SYS_SUCCESS == msRet)
	//{	//... Protect Duplication Call
	//	if(STRCMP_L(szCmd, "SWAP_PURGE"))		g_bLastCmd_SwapPurge = TRUE;
	//	else									g_bLastCmd_SwapPurge = FALSE;
	//}

	printf("Leave %s [%d]\n", APP_NAME, msRet);

	return msRet;
}

//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {

	g_ValveSeq.Initialize("AbortGas");

	return TRUE;
}

void Event_Message_Received() {
}


} // End of [extern "C"]
//==========================================================================================
