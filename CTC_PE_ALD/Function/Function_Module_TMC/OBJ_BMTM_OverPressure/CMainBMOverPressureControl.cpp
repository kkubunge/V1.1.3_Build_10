// Copyright (C) IPS Ltd.

#include "CMainBMOverPressureControl.h"



void CMainBMOverPressureControl::CloseOverPressureValve()
{
	/*int nCommStatus;
	if ("MAINBM" == get_ModuleName())
		WRITE_DIGITAL(MainBM_OverPresVvDO , CLOSE , &nCommStatus);	*/
}

void CMainBMOverPressureControl::OpenOverPressureValve()
{
	/*if (FALSE == IsVacuum())
		return;

	int nCommStatus;
	if ("MAINBM" == get_ModuleName())
		WRITE_DIGITAL(MainBM_OverPresVvDO , OPEN , &nCommStatus);*/
}

BOOL CMainBMOverPressureControl::Operate(string strParameter)
{
	BOOL bRet = FALSE;
	/*do {
		if ("CLOSE_OPRES_VALVE" == strParameter)
			CloseOverPressureValve();
		else if ("OPEN_OPRES_VALVE" == strParameter)
			OpenOverPressureValve();
		else
			break;

		bRet = TRUE;
	}while(0);*/
	return bRet;
}

BOOL CMainBMOverPressureControl::IsVacuum()
{
//	int nCommStatus;
	/*if ("MAINBM" == get_ModuleName()) {
		if (VAC_SENSOR == READ_DIGITAL(MainBM_ATMVACSensorXI , &nCommStatus))
			return TRUE;
	}*/
	
	return FALSE;
}
