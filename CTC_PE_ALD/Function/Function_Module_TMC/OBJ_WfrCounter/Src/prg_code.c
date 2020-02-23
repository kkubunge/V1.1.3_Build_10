/****************************************************************
 * System      : 200mm HDP-CVD
 * Subsystem   : TM Wafer Counter
 * Module      : TM Wafer Counter
 * File        : prg_code.c
 * Description : TM Wafer Counter
 * Author      : Yang/Heejeon
 * Dept.       : Institute of Intelligent System. S-T/F
 * Created     : 2003.06.17
 * Procedures defined : 
 * Procedures called  : 
 * Version     : 200mm HDP-CVD (Ver0.0)
 * Revision history
 *   date         rev. by            revised information
 * 2003.06.17     hjeon          - 1st release
 ****************************************************************/

#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include <Kutlstr.h>
#include "cimseqnc.h"
#include "iodefine.h"

/* define alarm numbers */
/* declare enumerations */
enum { NOYES_NO, NOYES_YES };

/* declare global variables */
BOOL	SIMULATION_MODE	= FALSE;
char    sFncName[10] = "WFRCNTR";

/* declare IO name string map table */
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "vWFR.Logging"	, _K_D_IO,	V_WAFER_LOGGING,	0 },	// vWFR.Logging
	{ "pdWFR.Count01"	, _K_D_IO,	PD_WAFER_COUNT01,	0 },	// pdWFR.Count01
	{ "pdWFR.Count02"	, _K_D_IO,	PD_WAFER_COUNT02,	0 },	// pdWFR.Count02
	{ "pdWFR.Count03"	, _K_D_IO,	PD_WAFER_COUNT03,	0 },	// pdWFR.Count03
	{ "pdWFR.Count04"	, _K_D_IO,	PD_WAFER_COUNT04,	0 },	// pdWFR.Count04
	{ "pdWFR.Count05"	, _K_D_IO,	PD_WAFER_COUNT05,	0 },	// pdWFR.Count05
	{ "pdWFR.Count06"	, _K_D_IO,	PD_WAFER_COUNT06,	0 },	// pdWFR.Count06
	{ "pdWFR.Count07"	, _K_D_IO,	PD_WAFER_COUNT07,	0 },	// pdWFR.Count07
	{ "pdWFR.Count08"	, _K_D_IO,	PD_WAFER_COUNT08,	0 },	// pdWFR.Count08
	{ "pdWFR.Count09"	, _K_D_IO,	PD_WAFER_COUNT09,	0 },	// pdWFR.Count09
	{ "pdWFR.Count10"	, _K_D_IO,	PD_WAFER_COUNT10,	0 },	// pdWFR.Count10
	{ "psWFR.Maint01"	, _K_S_IO,	PS_WAFER_MAINT01,	0 },	// psWFR.Maint01
	{ "psWFR.Maint02"	, _K_S_IO,	PS_WAFER_MAINT02,	0 },	// psWFR.Maint02
	{ "psWFR.Maint03"	, _K_S_IO,	PS_WAFER_MAINT03,	0 },	// psWFR.Maint03
	{ "psWFR.Maint04"	, _K_S_IO,	PS_WAFER_MAINT04,	0 },	// psWFR.Maint04
	{ "psWFR.Maint05"	, _K_S_IO,	PS_WAFER_MAINT05,	0 },	// psWFR.Maint05
	{ "psWFR.Maint06"	, _K_S_IO,	PS_WAFER_MAINT06,	0 },	// psWFR.Maint06
	{ "psWFR.Maint07"	, _K_S_IO,	PS_WAFER_MAINT07,	0 },	// psWFR.Maint07
	{ "psWFR.Maint08"	, _K_S_IO,	PS_WAFER_MAINT08,	0 },	// psWFR.Maint08
	{ "psWFR.Maint09"	, _K_S_IO,	PS_WAFER_MAINT09,	0 },	// psWFR.Maint09
	{ "psWFR.Maint10"	, _K_S_IO,	PS_WAFER_MAINT10,	0 },	// psWFR.Maint10
	"" // do not remove!
};

int tCountIOs[] = {
	PD_WAFER_COUNT01,
	PD_WAFER_COUNT02,
	PD_WAFER_COUNT03,
	PD_WAFER_COUNT04,
	PD_WAFER_COUNT05,
	PD_WAFER_COUNT06,
	PD_WAFER_COUNT07,
	PD_WAFER_COUNT08,
	PD_WAFER_COUNT09,
	PD_WAFER_COUNT10
};

int tMntStrIOs[] = {
	PS_WAFER_MAINT01,
	PS_WAFER_MAINT02,
	PS_WAFER_MAINT03,
	PS_WAFER_MAINT04,
	PS_WAFER_MAINT05,
	PS_WAFER_MAINT06,
	PS_WAFER_MAINT07,
	PS_WAFER_MAINT08,
	PS_WAFER_MAINT09,
	PS_WAFER_MAINT10
};

//---------------------------------------------------------------------------------------
/* query methods */

//---------------------------------------------------------------------------------------
/* set methods */
void setWaferCount(int nIO)
{
	int nComSts = 0;
	int nCnt = READ_DIGITAL(nIO, &nComSts);
	nCnt++;
	WRITE_DIGITAL(nIO, nCnt, &nComSts);
}

void setLogging(const int nSet)
{
	int nComSts = 0;
	WRITE_DIGITAL(V_WAFER_LOGGING, nSet, &nComSts);
}

int getStrIOLen(int nIO)
{
	char buf[256];
	int nComSts = 0;
	memset(buf, 0, sizeof(buf));
	READ_STRING(nIO, buf, &nComSts);
	return strlen(buf);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/*
  CountWafer function
*/
void CountWafer(void)
{
	int i = 0;
	setLogging(NOYES_NO);
	// increase wafer maint counter
	for (i=0; i < sizeof(tCountIOs)/sizeof(int); i++) {
		if ( getStrIOLen(tMntStrIOs[i]) > 0 )
			setWaferCount(PD_WAFER_COUNT01+i);
	}
	setLogging(NOYES_YES);
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/* Program_Main() */
Module_Status Program_Main(void)
{
	int nLen = 0, nCmd = 0;
	char *sPara = NULL;
	char sCmd[65], sArg[65];

	sPara = PROGRAM_PARAMETER_READ();
	nLen = strlen(sPara);
	sPara[nLen] = '\0'; // null assign (terminate)

	// check null function
	if (nLen < 1) {
		printf("ERROR: [%s] Function - Mode parameter is null\n", sFncName);
		return SYS_ABORTED;
	}

	// seperate argument with commmand string
	STR_SEPERATE(sPara , sCmd , sArg , 64 );

	if ( !strncmp(sPara, "COUNT", 5) ) CountWafer();
	return SYS_SUCCESS;
}

//---------------------------------------------------------------------------------------
/* Program_Init_Code() */
BOOL Program_Init_Code(void) { return TRUE; }
/* Program_Enter_Code() */
void Program_Enter_Code() { }



