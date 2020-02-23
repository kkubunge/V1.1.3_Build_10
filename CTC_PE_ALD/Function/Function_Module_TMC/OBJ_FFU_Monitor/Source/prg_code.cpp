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
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
} // End of [extern "C"]


#include "TextLogDll.h"
//------------------------------------------------------------------------------------------
typedef void(__cdecl * _THREAD_FUNC1)(void* lpParam);

#define ALARM_MCUL_STATUS					5501
#define FFU1_RPM_ALARM						5502
#define FFU2_RPM_ALARM						5503
#define	ALARM_EFEM_FFU_PRES_DIFF_ERROR		9722
#define	RETRY_CNT							3

BOOL	SIMULATION_MODE	= FALSE;

enum {SINSUNG, MARS};

IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"	    , _K_D_IO,			IO_Driver_Loding	    ,	0 },
	{ "TM.FFU1_StsDI"			, _K_D_IO,			FFU1_StsDI				,	0 },
	{ "TM.FFU2_StsDI"			, _K_D_IO,			FFU2_StsDI				,	0 },
	{ "TM.EFEMDiff_PresAI"		, _K_A_IO,			EFEMDiff_PresAI			,	0 },
	{ "TM.EFEMDiff_PresAI_Sin"	, _K_A_IO,			EFEMDiff_PresAI_Sin		,	0 },
	{ "TM.EFEMDiff_PresAI_Mars"	, _K_A_IO,			EFEMDiff_PresAI_Mars	,	0 },
	{ "PRMA_FFU_DIFFPRS_LIMIT"	, _K_A_IO,			PRMA_FFU_DIFFPRS_LIMIT	,	0 },
	{ "PRMA_FFU_RPM1"			, _K_A_IO,			PRMA_FFU_RPM1			,	0 },
	{ "PRMA_FFU_RPM2"			, _K_A_IO,			PRMA_FFU_RPM2			,	0 },
	{ "PRMD_FFU_MAKER"			, _K_D_IO,			PRMD_FFU_MAKER			,	0 },
	{ "TM.FFU1_RpmAI"			, _K_A_IO,			FFU1_RpmAI				,	0 },
	{ "TM.FFU2_RpmAI"			, _K_A_IO,			FFU2_RpmAI				,	0 },

	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void MCULAlmCheckThread()
{
	int CommStatus;
	int nFFU1Sts     = 0;
	int nFFU2Sts     = 0;
	int nErrorCount  = 0;
	int nErrorCount2 = 0;
	int nRPM1_Cnt    = 0;
    int nRPM2_Cnt    = 0;
	int nFFU_Maker   = 0;
	double R_PRES , S_PRES;
	int m_nPrsErrorCount = 0;
	double dbRPM1_Param  = 0;
	double dbRPM2_Param  = 0;
	double dbRPM1        = 0;
	double dbRPM2		 = 0;
	char   szbuf[256];
	char   szBuffer[256];
	memset(szbuf,0,sizeof(szbuf));
	memset(szBuffer,0,sizeof(szBuffer));

	while(TRUE)
	{
		if(SIMULATION_MODE) break;

		dbRPM1_Param  = READ_ANALOG(PRMA_FFU_RPM1, &CommStatus);
		dbRPM2_Param  = READ_ANALOG(PRMA_FFU_RPM2, &CommStatus);
		if(dbRPM1_Param > 0.001) nFFU1Sts = READ_DIGITAL(FFU1_StsDI , &CommStatus);
		else					 nFFU1Sts = 1;

		if(dbRPM2_Param > 0.001) nFFU2Sts = READ_DIGITAL(FFU2_StsDI , &CommStatus);
		else					 nFFU2Sts = 1;

		if(1 != nFFU1Sts)	// 1 = Normal
		{
			nErrorCount++;
		}
		else
		{
			nErrorCount = 0;
		}

		if(1 != nFFU2Sts)
		{
			nErrorCount2++;
		}
		else
		{
			nErrorCount2 = 0;
		}

		if(( nErrorCount >= RETRY_CNT ) || ( nErrorCount2 >= RETRY_CNT ))
		{
			printf("ffu1sts : %d, ffu2sts : %d, err : %d, err2 : %d\n", nFFU1Sts, nFFU2Sts, nErrorCount, nErrorCount2);
			ALARM_MANAGE(ALARM_MCUL_STATUS);
			nErrorCount = 0;
			nErrorCount2 = 0;
		}



		//RPM Check
		dbRPM1        = READ_ANALOG(FFU1_RpmAI, &CommStatus);
		dbRPM2        = READ_ANALOG(FFU2_RpmAI, &CommStatus);

		if( dbRPM1_Param - dbRPM1 > 0.1 )
		{
			if( ++nRPM1_Cnt > RETRY_CNT )
			{
				nRPM1_Cnt = 0;
				memset(szbuf,0,sizeof(szbuf));
				memset(szBuffer,0,sizeof(szBuffer));
				ALARM_MESSAGE_GET( FFU1_RPM_ALARM , szBuffer );
				sprintf(szbuf," (Intlk:%.0f, RPM:%.0f)", dbRPM1_Param, dbRPM1);
				strcat( szBuffer , szbuf );
				ALARM_MESSAGE_SET( FFU1_RPM_ALARM , szBuffer );
				if( ALM_PAUSED != ALARM_STATUS(FFU1_RPM_ALARM ))
				{
					ALARM_POST(FFU1_RPM_ALARM);
				}
			}
		}
		else
		{
			nRPM1_Cnt = 0;
		}

		if( dbRPM2_Param - dbRPM2 > 0.1 )
		{
			if( ++nRPM2_Cnt > RETRY_CNT )
			{
				nRPM2_Cnt = 0;
				memset(szbuf,0,sizeof(szbuf));
				memset(szBuffer,0,sizeof(szBuffer));
				ALARM_MESSAGE_GET( FFU2_RPM_ALARM , szBuffer );
				sprintf(szbuf," (Intlk:%.0f, RPM:%.0f)", dbRPM1_Param, dbRPM1);
				strcat( szBuffer , szbuf );
				ALARM_MESSAGE_SET( FFU2_RPM_ALARM , szBuffer );
				if(ALM_PAUSED != ALARM_STATUS(FFU2_RPM_ALARM ))
				{
					ALARM_POST(FFU2_RPM_ALARM);
				}
			}
		}
		else
		{
				nRPM2_Cnt = 0;
		}

		S_PRES = READ_ANALOG( PRMA_FFU_DIFFPRS_LIMIT  , &CommStatus );
		nFFU_Maker = READ_DIGITAL( PRMD_FFU_MAKER  , &CommStatus );
		if (nFFU_Maker == SINSUNG)
		{
			R_PRES = READ_ANALOG( EFEMDiff_PresAI_Sin , &CommStatus );
		}
		else if (nFFU_Maker == MARS)
		{
			R_PRES = READ_ANALOG( EFEMDiff_PresAI_Mars , &CommStatus );
		}
		WRITE_ANALOG(EFEMDiff_PresAI, R_PRES, &CommStatus);

		if ( S_PRES - R_PRES  > 0.01 ) {
			m_nPrsErrorCount++;
		} else m_nPrsErrorCount = 0;

		if (m_nPrsErrorCount >= RETRY_CNT){
			memset(szbuf,0,sizeof(szbuf));
			memset(szBuffer,0,sizeof(szBuffer));
			ALARM_MESSAGE_GET( ALARM_EFEM_FFU_PRES_DIFF_ERROR , szBuffer );
			sprintf(szbuf," (Intlk:%.0f, Prs:%.0f)", S_PRES, R_PRES);
			strcat( szBuffer , szbuf );
			ALARM_MESSAGE_SET( ALARM_EFEM_FFU_PRES_DIFF_ERROR , szBuffer );
			if(ALM_PAUSED != ALARM_STATUS(ALARM_EFEM_FFU_PRES_DIFF_ERROR ))
			{
				ALARM_POST(ALARM_EFEM_FFU_PRES_DIFF_ERROR);
			}
		}

		Sleep(5000);
	}
	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	Module_Status msRet = SYS_SUCCESS;

	Sleep(10000);

	printf("FFU Monitoring Start..\n");

	_beginthread((_THREAD_FUNC1) MCULAlmCheckThread, 0, NULL);

	return msRet;
}

//==========================================================================================
extern "C"
{
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no )
{

}

} // End of [extern "C"]
//==========================================================================================
