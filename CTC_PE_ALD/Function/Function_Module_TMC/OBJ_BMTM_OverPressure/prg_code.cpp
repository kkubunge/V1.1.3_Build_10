#include "StdAfx.h"
#include "COverPressureControl.h"
#include "CBMOverPressureControl.h"
#include "CMainBMOverPressureControl.h"
#include "CTMOverPressureControl.h"
//Version History//////////////////////////////////////////////////////////////////////////
//Date : 2008.03.18
//       Modified from Wuxi Bluetain
//       Main BM IO선언부 및 Over Pressure Control부 제거
//       


COverPressureControl* g_pCOverPressureControl = NULL;

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "PRMD_OVER_PRES_OPT"			, _K_D_IO	, PRMD_OVER_PRES_OPT			, 0 } ,

	{ "TM.OverPresVvDO"				, _K_D_IO	, TM_OverPresVvDO				, 0 } ,
//	{ "TM.SlowRoughVvDO"			, _K_D_IO	, TM_SlowRoughVvDO				, 0 } ,
//	{ "TM.FastRoughVvDO"			, _K_D_IO	, TM_FastRoughVvDO				, 0 } ,
	{ "TM.RoughVvXO"				, _K_D_IO	, TM_RoughVvXO					, 0 } ,
	{ "TM.ATMVACSensorXI"			, _K_D_IO	, TM_ATMVACSensorXI				, 0 } ,

	{ "BM1.OverPresVvDO"			, _K_D_IO	, BM1_OverPresVvDO				, 0 } ,
	{ "BM1.ATMVACSensorXI"			, _K_D_IO	, BM1_ATMVACSensorXI			, 0 } ,

	{ "BM2.OverPresVvDO"			, _K_D_IO	, BM2_OverPresVvDO				, 0 } ,
	{ "BM2.ATMVACSensorXI"			, _K_D_IO	, BM2_ATMVACSensorXI			, 0 } ,

	//{ "BM.OverPresVvDO"			, _K_D_IO	, MainBM_OverPresVvDO			, 0 } ,
	//{ "BM.ATMVACSensorXI"			, _K_D_IO	, MainBM_ATMVACSensorXI			, 0 } ,
	
	{ "TM.TM_UPC_Set"				, _K_A_IO	, TM_UPC_Set					, 0 } ,
	{ "PRMA_UPC_SET_FLOW"			, _K_A_IO	, PRMA_UPC_SET_FLOW				, 0 } ,

	


	""
};

extern "C" void Screen_Nav_Info( int no )
{
}

extern "C" BOOL Program_Init_Code()
{
	return TRUE;
}

extern "C" void Event_Message_Received()
{
}

extern "C" void Program_Enter_Code()
{
	string strArgument = PROGRAM_ARGUMENT_READ();

	if ("TM" == strArgument) {
		g_pCOverPressureControl = new CTMOverPressureControl();
		g_pCOverPressureControl->set_ModuleName(strArgument);
	} else if (("BM1" == strArgument) || ("BM2" == strArgument)) {
		g_pCOverPressureControl = new CBMOverPressureControl();
		g_pCOverPressureControl->set_ModuleName(strArgument);
	} /*else if ("MAINBM" == strArgument) {
		g_pCOverPressureControl = new CMainBMOverPressureControl();
		g_pCOverPressureControl->set_ModuleName(strArgument);
	}*/
}

Module_Status Program_Main()
{
	Module_Status nRet = SYS_ABORTED;
	do {
		if (NULL == g_pCOverPressureControl) {
			printf( "---> BMTM_OverPressure Control Error - NULL pointer\n");
			break;
		}

		int nCommStatus;
		if (ENABLE == READ_DIGITAL(PRMD_OVER_PRES_OPT, &nCommStatus)) {
			if (FALSE == g_pCOverPressureControl->Operate(PROGRAM_PARAMETER_READ()))
				break;
		}

		nRet = SYS_SUCCESS;
	}while(0);
	return nRet;
}
