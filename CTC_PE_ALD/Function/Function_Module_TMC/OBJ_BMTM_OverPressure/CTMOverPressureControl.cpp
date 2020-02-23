// Copyright (C) IPS Ltd.

#include "CTMOverPressureControl.h"



void CTMOverPressureControl::CloseOverPressureValve()
{
	int nCommStatus;
	WRITE_DIGITAL(TM_OverPresVvDO , CLOSE , &nCommStatus);
}

void CTMOverPressureControl::CloseRoughingValve()
{
	int nCommStatus;
	WRITE_DIGITAL(TM_RoughVvXO , CLOSE , &nCommStatus);
}

void CTMOverPressureControl::OpenOverPressureValve()
{
	if (FALSE == IsVacuum())
		return;
	int nCommStatus, nUPCFlow;

	//2014.03.15 MAHA BB UPC Contorl Added by hyuk	
	nUPCFlow = READ_ANALOG(PRMA_UPC_SET_FLOW, &nCommStatus);
	WRITE_ANALOG(TM_UPC_Set, nUPCFlow, &nCommStatus );
	//2014.03.15 MAHA BB UPC Contorl Added by hyuk	

	WRITE_DIGITAL(TM_OverPresVvDO , OPEN , &nCommStatus);
}

void CTMOverPressureControl::OpenRoughingValve()
{
	if (FALSE == IsVacuum())
		return;
	int nCommStatus;
	WRITE_DIGITAL(TM_RoughVvXO	   , OPEN , &nCommStatus);
}

BOOL CTMOverPressureControl::Operate(string strParameter)
{
	BOOL bRet = FALSE;
	do
	{
		if ("OPEN_OPRES_VALVE" == strParameter)
			OpenOverPressureValve();
		else if ("CLOSE_OPRES_VALVE" == strParameter)
			CloseOverPressureValve();
		else
		{
			string strRunString = ExtractRunString(strParameter);
			if ("CLOSE_ROUGH" == strRunString)
				CloseRoughingValve();
			else if ("OPEN_ROUGH" == strRunString)
				OpenRoughingValve();
			else if ("SKIP" == strRunString) bRet = TRUE;	// BM Gate Open/Close시 TM은 하지 않도록 추가 kcr
			else
				break;
		}

		bRet = TRUE;
	}while(0);
	return bRet;
}

BOOL CTMOverPressureControl::IsVacuum()
{
	int nCommStatus;
	if (VAC_SENSOR == READ_DIGITAL(TM_ATMVACSensorXI , &nCommStatus))
		return TRUE;

	return FALSE;
}

string CTMOverPressureControl::ExtractRunString(string strParameter)
{
	string strRunString = "";
	char RunStr[41]="", Temp[41]="", StationStr[41]="", TempStr[41]="";

	STR_SEPERATE( (char*)strParameter.c_str() , RunStr , Temp , 40 );
	STR_SEPERATE( Temp , StationStr , TempStr , 40 );
	string strStation = StationStr;

	if ((strStation == "A_PM1") || (strStation == "A_PM2") || (strStation == "A_PM3") || (strStation == "A_PM4") || (strStation == "A_PM5") || (strStation == "A_PM6") ||
		(strStation == "B_PM1") || (strStation == "B_PM2") || (strStation == "B_PM3") || (strStation == "B_PM4") || (strStation == "B_PM5") || (strStation == "B_PM6") ||
		(strStation == "S_PM1") || (strStation == "S_PM2") || (strStation == "S_PM3") || (strStation == "S_PM4") || (strStation == "S_PM5") || (strStation == "S_PM6") ||
		(strStation == "T_PM1") || (strStation == "T_PM2") || (strStation == "T_PM3") || (strStation == "T_PM4") || (strStation == "T_PM5") || (strStation == "T_PM6") ||
		(strStation == "PM1")   || (strStation == "PM2")   || (strStation == "PM3")   || (strStation == "PM4")   || (strStation == "PM5")   || (strStation == "PM6")   ||
		(strStation == "P1")    || (strStation == "P2")    || (strStation == "P3")    || (strStation == "P4")    || (strStation == "P5")    || (strStation == "P6"))
	{
		strRunString = RunStr;
	}
	else strRunString="SKIP";

	return strRunString;
}
