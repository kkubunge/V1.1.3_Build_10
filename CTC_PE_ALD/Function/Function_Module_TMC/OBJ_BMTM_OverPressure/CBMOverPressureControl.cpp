// Copyright (C) IPS Ltd.

#include "CBMOverPressureControl.h"



void CBMOverPressureControl::CloseOverPressureValve()
{
	int nCommStatus;
	if ("BM1" == get_ModuleName())
		WRITE_DIGITAL(BM1_OverPresVvDO , CLOSE , &nCommStatus);
	else if ("BM2" == get_ModuleName())
		WRITE_DIGITAL(BM2_OverPresVvDO , CLOSE , &nCommStatus);
}

void CBMOverPressureControl::OpenOverPressureValve()
{
	if (FALSE == IsVacuum())
		return;
	int nCommStatus;
	if ("BM1" == get_ModuleName())
		WRITE_DIGITAL(BM1_OverPresVvDO , OPEN , &nCommStatus);
	else if ("BM2" == get_ModuleName())
		WRITE_DIGITAL(BM2_OverPresVvDO , OPEN , &nCommStatus);
}

BOOL CBMOverPressureControl::Operate(string strParameter)
{
	BOOL bRet = FALSE;
	do
	{
		if ("CLOSE_OPRES_VALVE" == strParameter)
			CloseOverPressureValve();
		else if ("OPEN_OPRES_VALVE" == strParameter)
			OpenOverPressureValve();
		else
			break;

		bRet = TRUE;
	}while(0);
	return bRet;
}

BOOL CBMOverPressureControl::IsVacuum()
{
	int nCommStatus;
	if ("BM1" == get_ModuleName())
	{
		if (VAC_SENSOR == READ_DIGITAL(BM1_ATMVACSensorXI , &nCommStatus))
			return TRUE;
	}
	else if ("BM2" == get_ModuleName())
	{
		if (VAC_SENSOR == READ_DIGITAL(BM2_ATMVACSensorXI , &nCommStatus))
			return TRUE;
	}
	return FALSE;
}
