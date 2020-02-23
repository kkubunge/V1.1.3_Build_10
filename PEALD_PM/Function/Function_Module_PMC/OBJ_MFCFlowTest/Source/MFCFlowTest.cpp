// MFCFlowTest.cpp: implementation of the CMFCVerification class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

//==========================================================================================
extern "C"
{

//--------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//--------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
#include "CimSeqncKNFS.h"

} // End of [extern "C"]
//==========================================================================================

#include "TextParser.h"
#include "MsgBoxDll.h"
#include "TextLogDll_M.h"
#include "MFCFlowTest.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { OFF_0	, ON_1		  }; 
enum { V_CLOSE	, V_OPEN	  }; 
enum { NONE_0   , ERROR_1 ,  OK_2 };
enum { IDLE_0   , RUN_1       };
enum { NORMAL_0 , INTERLOCK_1 };

#define		ALARM_MFCFlowTest_FAIL			m_nALARM_MfcFlowFail
int			ALARM_START_POINTER = 0;

#define MFCFLOW_CFG_FILE_PATH	"Parameter\\PM_SEQ_PARM\\Gas_Calibration.ini"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMFCFlowTest::CMFCFlowTest()
{
	m_dbPumpTime = 30;			// sec
	m_dbAlarmTolerance = 1;		// %
	m_nALARM_MfcFlowFail = 2062;
}

CMFCFlowTest::~CMFCFlowTest()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

void CMFCFlowTest::Enter_Code()
{
}

BOOL CMFCFlowTest::Initialize()
{

	int CommStatus;
	char HostName[256];

	int ServerControlMode;
	int Port1;
	int Port2;
	int ExtendPort;
	int i;

	GET_MODULE_CONNECTION_INFO( HostName , &ServerControlMode , &Port1 , &Port2 , &ExtendPort );

	if ( (!ServerControlMode) && ( ExtendPort > 0 ) ) {
		_nINF_KNFS_SET_FILESAVE_SKIP_STATUS( SYSTEM_LOGSKIP_STATUS() );
		if ( _nINF_KNFS_CONNECTION_FOR_CLIENT( HostName , ExtendPort ) != 0 ) {
			printf( "_nINF_KNFS_CONNECTION_FOR_CLIENT ExtendPort Initialize Error\n" );
			return FALSE;
		}
	}

	CSeqInterface::Initialize();
	m_MFCValveSeq.Initialize();

	ReadConfig();

	for(i=0; i<MAX_MFC; i++)
	{
		WRITE_STRING(MFC01_Name+i, m_MFC_List[i].szName, &CommStatus);
	}

	return TRUE;
}

BOOL CMFCFlowTest::Deinitialize()
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
void CMFCFlowTest::MFC_FlowTest_Init()
{
	int CommStatus;

	WRITE_DIGITAL(INTLKS_CONTROL, 0 , &CommStatus);	
	WRITE_DIGITAL(MFC_FLOW_TEST, OFF_0, &CommStatus);
	//
	RUN_FUNCTION_FREE(ABORT_GAS, "");
	RUN_FUNCTION_FREE(VACUUM_CONTROL, "FAST_ROUGH");
}

BOOL CMFCFlowTest::FastPumping()
{
	BOOL bRet = TRUE;

	if( RUN_FUNCTION_FREE(VACUUM_CONTROL, "FAST_ROUGH") != SYS_SUCCESS )
	{
		bRet = FALSE;
	}
	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CMFCFlowTest::MFCFlowTest()
{
	int nCommStatus, nFlowCount, nResult = 0;
	int nMFCIdx, GasSelect, nGasType, nIdx, nPumpTime, nDelayTime, nCheckTime;
	int Type , InDev , OutDev;
	double Min , Max;
	double dbReadPrs, dbBasePrs;
	char Buffer[1024], Buffer2[512], szTemp[16];
	char Result[32];
	int nSet, i;
	double dbFlow;
	int nEndResult = 0;

	int		nAvgCount	= 0;
	double	dbFlowSum	= 0;
	double	dbPrsSum	= 0;
	int		nFlwHuntCnt, nFlwHuntCntLimit;	//... Unit : Count
	double	dbFlwHuntTol = 0;				//... Unit : %
	double	dbSetFlow	= 0;				//... Unit : sccm

	memset(szTemp,	0, sizeof(szTemp));
	memset(Result,	0, sizeof(Result));
	memset(Buffer,	0, sizeof(Buffer));
	memset(Buffer2, 0, sizeof(Buffer2));

	do
	{
		//////////////////////////////////////////////////////////////////////////
		for(nMFCIdx = 0; nMFCIdx < MAX_MFC; nMFCIdx++)
		{
			//... Check Skip Case
			if(READ_DIGITAL(MFlow_MFC01 + nMFCIdx, &nCommStatus) == OFF_0)		continue;
			if(strcmp(m_MFC_List[nMFCIdx].szName, "-") == 0)					continue;

			//... All Gas Close
			if(RUN_FUNCTION_FREE(ABORT_GAS, "") == SYS_ABORTED)			return SYS_ABORTED;
			
			//... Read Pumping Time
			nPumpTime = (int) READ_ANALOG(MFC_PUMP_TIME, &nCommStatus);

			//... Initial Data
			WRITE_DIGITAL(MFlow_MFC01_Result + nMFCIdx, NONE_0, &nCommStatus);
			for(i = 0; i < MAX_CHECK_LIST; i++)
			{
				WRITE_ANALOG(MFlow_MFC01_SET1		+ (nMFCIdx * MAX_CHECK_LIST) + i, 0, &nCommStatus);
				WRITE_ANALOG(MFlow_MFC01_FLOW1		+ (nMFCIdx * MAX_CHECK_LIST) + i, 0, &nCommStatus);
				WRITE_ANALOG(MFlow_MFC01_ResultPrs1	+ (nMFCIdx * MAX_CHECK_LIST) + i, 0, &nCommStatus);
				//
				WRITE_DIGITAL(MFlow_MFC01_FlwErr1DM	+ (nMFCIdx * MAX_CHECK_LIST) + i, 0, &nCommStatus);
			}

			//------------------------------------------------------------------------------------------
			// MFC Flow Test : Gas Line Pumping
			//------------------------------------------------------------------------------------------

			_LOG("#==============================================================");
			_LOG("# MFC(%d) Flow Test Start   ", nMFCIdx+1);
			_LOG("#==============================================================");

			//... Read Max Flow
			nResult    = 0;
			nSet       = 0; // For IO Index
			nFlowCount = 1; // 20~100%			
			GasSelect  = nMFCIdx; 
			ANALOG_SHAPE(MFC01StPtAO + GasSelect, &Type, &InDev, &OutDev, &Min, &Max);

			do
			{
				//... Delay for Before ABORT_GAS
				if(! WAIT_SECONDS(2))				return SYS_ABORTED;

				//... Pumping MFC
				sprintf(Buffer, "MFC%02d_PUMP", GasSelect + 1);
				nIdx = m_MFCValveSeq.FindStepIdx(Buffer);
				m_MFCValveSeq.m_adbMFCSet[nIdx][0] = Max;
				if(TRUE != m_MFCValveSeq.StartValveSeq(Buffer, TRUE))			return SYS_ABORTED;

				//... Wait Pumping Time
				S_TIMER_READY();
				while(TRUE)
				{
					if(! WAIT_SECONDS(1))				return SYS_ABORTED;
					if(S_TIMER_CHECK(1, nPumpTime))		break;
				}

				//------------------------------------------------------------------------------------------
				// MFC Flow Test : Chamber Pumping & Base Pressure Check
				//------------------------------------------------------------------------------------------
				if(RUN_FUNCTION_FREE(ABORT_GAS, "") == SYS_ABORTED)					return SYS_ABORTED;							
				if(RUN_FUNCTION_FREE(VACUUM_CONTROL, "FAST_ROUGH") == SYS_ABORTED)	return SYS_ABORTED;
				
				//... Wait Pumping Time
				if(nPumpTime > 0)
				{
					S_TIMER_READY();
					while(TRUE) 
					{
						if(! WAIT_SECONDS(1))				return SYS_ABORTED;
						if(S_TIMER_CHECK(1, nPumpTime))		break;
					}
				}
				else
				{
					//... Delay for After ABORT_GAS
					if(! WAIT_SECONDS(2))				return SYS_ABORTED;
				}
				
				dbBasePrs  = READ_ANALOG(M651_Pressure, &nCommStatus);

				//... Read Parameter
				nDelayTime = (int) READ_ANALOG(MFC_CHECK_DELAY, &nCommStatus );
				nCheckTime = (int) READ_ANALOG(MFC_CHECK_TIME,	&nCommStatus );
				
				//------------------------------------------------------------------------------------------
				// MFC Flow Test : MFC Flow 
				//------------------------------------------------------------------------------------------

				nGasType   = 0;		//... Fixed Valve Sequence Real Gas
				memset(Result, 0, sizeof(Result));

				sprintf(Buffer, "MFC%02d_FLW_GAS%d", GasSelect+1, nGasType+1);
				nIdx = m_MFCValveSeq.FindStepIdx(Buffer);
				m_MFCValveSeq.m_adbMFCSet[nIdx][0] = (Max*20*nFlowCount/100);

				//... Set MFC & Gas Flow
				dbSetFlow = m_MFCValveSeq.m_adbMFCSet[nIdx][0];

				WRITE_ANALOG(MFlow_MFC01_SET1 + (nMFCIdx * MAX_CHECK_LIST) + nSet, dbSetFlow, &nCommStatus);				
				if(TRUE != m_MFCValveSeq.StartValveSeq(Buffer, TRUE))		return SYS_ABORTED;

				//... Wait Check Delay Time
				S_TIMER_READY ();
				while(TRUE)
				{
					if(! WAIT_SECONDS(0.5))					return SYS_ABORTED;
					if(S_TIMER_CHECK(1, nDelayTime))		break;
				}

				//////////////////////////////////////////////////////////////////////////
				//... Check Time for Average Data
				nAvgCount	= 0;
				dbFlowSum	= 0;
				dbPrsSum	= 0;

				nFlwHuntCnt	= 0;
				dbFlwHuntTol = READ_ANALOG(MFC_FL0W_TOL, &nCommStatus);

				S_TIMER_READY ();
				while(TRUE)
				{
					nAvgCount++;

					//////////////////////////////////////////////////////////////////////////
					//... Update MFC Flow
					dbFlow = READ_ANALOG(MFC01FlwAI + nMFCIdx, &nCommStatus );
					dbFlowSum = dbFlowSum + dbFlow;

					//... Exception Zero divide
					if(dbFlowSum > 0)	dbFlow = dbFlowSum / nAvgCount;
					else				dbFlow = 0;

					WRITE_ANALOG(MFlow_MFC01_FLOW1 + (nMFCIdx * MAX_CHECK_LIST) + nSet, dbFlow, &nCommStatus);

					//////////////////////////////////////////////////////////////////////////
					//... Update Partial Pressure
					dbReadPrs = READ_ANALOG(M651_Pressure, &nCommStatus);
					dbPrsSum = dbPrsSum + dbReadPrs;
					
					//... Exception Zero divide
					if(dbPrsSum > 0)	dbReadPrs = dbPrsSum / nAvgCount;
					else				dbReadPrs = 0;

					WRITE_ANALOG(MFlow_MFC01_ResultPrs1 + (nMFCIdx * MAX_CHECK_LIST) + nSet, dbReadPrs, &nCommStatus);

					//////////////////////////////////////////////////////////////////////////
					//... Check MFC Flow Hunting
					if(dbSetFlow > 0)
					{
						if(fabs(dbSetFlow - dbFlow) > dbSetFlow * dbFlwHuntTol * 0.01)
						{
							nFlwHuntCnt++;
							WRITE_DIGITAL(MFlow_MFC01_FlwErr1DM + (nMFCIdx * MAX_CHECK_LIST) + nSet, nFlwHuntCnt, &nCommStatus);
						}
					}

					if(! WAIT_SECONDS(1))					return SYS_ABORTED;
					if(S_TIMER_CHECK(1, nCheckTime))		break;
				}

				//////////////////////////////////////////////////////////////////////////
				// Check Flow Error Count
				nFlwHuntCntLimit	= READ_DIGITAL(MFlow_FlwErrLmtDM, &nCommStatus);

				if(nFlwHuntCnt >= nFlwHuntCntLimit)
				{
					nResult++;	
					sprintf(Result, "FAIL");
				}
				else 
				{
					sprintf(Result, "OK");
				}

				_LOG("#==================================================================");
				_LOG("# MFC(%d) Flow Test SET%d Result   ", nMFCIdx+1, nFlowCount);
				_LOG("# Set       Flow     : %8.3f     Read   Flow     : %8.3f  ",	dbSetFlow, dbFlow);				
				_LOG("# Read      Pressure : %8.3f     Base   Pressure : %8.3f  ",	dbReadPrs, dbBasePrs);
				_LOG("# Check     Result   : %s",									Result);
				_LOG("# Flow Hunting Count : %d        Hunting Tol Lmt : %8.3f  ",	nFlwHuntCnt, dbSetFlow * dbFlwHuntTol * 0.01);
				_LOG("#==================================================================");
				
				nSet++;
				nFlowCount++;

				//... Finished Check
				if(RUN_FUNCTION_FREE(ABORT_GAS, "") == SYS_ABORTED)					return SYS_ABORTED;
				if(RUN_FUNCTION_FREE(VACUUM_CONTROL, "FAST_ROUGH") == SYS_ABORTED)	return SYS_ABORTED;

			} while (nFlowCount < 6);

			//... Check Fail Case
			if(nResult > 0)
			{
				nEndResult = nEndResult + nResult;
				WRITE_DIGITAL(MFlow_MFC01_Result + nMFCIdx, ERROR_1, &nCommStatus);
				sprintf(szTemp, "MFC[%d] ", nMFCIdx + 1);
				strcat(Buffer2, szTemp);
			}
			else
			{
				WRITE_DIGITAL(MFlow_MFC01_Result + nMFCIdx, OK_2, &nCommStatus);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		
		if(nEndResult > 0)
		{
			sprintf(szTemp, "Flow Test Error");
			strcat(Buffer2, szTemp);
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer2);
		}

	} while(0);

	return SYS_SUCCESS;

}
//... 2018.06.24
Module_Status CMFCFlowTest::SET_MANUAL_MFC()
{
	int nCommStatus;
	int nMFCIdx = 0, GasSelect = 0, nGasType = 0, nIdx = 0;
	int Type = 0 , InDev = 0, OutDev = 0;
	double Min = 0 , Max = 0;
	char Buffer[1024];
	int nEndResult = 0;

	int		nAvgCount	= 0;
	double	dbFlowSum	= 0;
	double	dbPrsSum	= 0;
	double	dbSetFlow	= 0;				//... Unit : sccm

	memset(Buffer,	0, sizeof(Buffer));

	do
	{
		nGasType   = READ_DIGITAL(MANUAL_SET_MFC_SEQ,&nCommStatus);
		if(nGasType == NOTUSE_0)
		{
			sprintf( Buffer, "GAS Flow Mode [Not Use]" );
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer);
			return SYS_ABORTED;
		}
		//////////////////////////////////////////////////////////////////////////
		for(nMFCIdx = 0; nMFCIdx < MAX_MFC; nMFCIdx++)
		{
			//... Check Skip Case
			if(strcmp(m_MFC_List[nMFCIdx].szName, "-") == 0)					continue;
			
			dbSetFlow = READ_ANALOG(MANUAL_SET_MFC01+nMFCIdx,&nCommStatus);
			if(dbSetFlow < 0.1) continue;

			//... Read Max Flow
			GasSelect  = nMFCIdx; 
			ANALOG_SHAPE(MFC01StPtAO + GasSelect, &Type, &InDev, &OutDev, &Min, &Max);
			if(dbSetFlow >Max) dbSetFlow = Max;

			if(! WAIT_SECONDS(0.5))				return SYS_ABORTED;

			sprintf(Buffer, "MFC%02d_FLW_SEQ%d", GasSelect+1, nGasType);

			nIdx = m_MFCValveSeq.FindStepIdx(Buffer);

			//... Set MFC & Gas Flow
			m_MFCValveSeq.m_adbMFCSet[nIdx][0] = dbSetFlow;

			if(TRUE != m_MFCValveSeq.StartValveSeq(Buffer, TRUE))		return SYS_ABORTED;
			//... Wait Check Delay Time
		}
		//////////////////////////////////////////////////////////////////////////

	} while(0);

	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
Module_Status CMFCFlowTest::Main()
{
	int	CommStatus;
	char Buffer[1024];
	char *Ed;
	int i;
	int  MFC_Sel = 0 ;

	Ed = PROGRAM_PARAMETER_READ();

	int		nMFCListIdx = 0;

	double	dbPressure	= 0;

	//////////////////////////////////////////////////////////////////////////
	//... Select All MFC Flow Item
	if(STRCMP_L(Ed, "MANUAL_MFCFLOW"))
	{
		if( READ_DIGITAL(VRCP_RunningDI, &CommStatus) == RUN_1 )
		{
			sprintf( Buffer, "PM is Running, so it is not impossible MFC Flow Test" );
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer);
			return SYS_ABORTED;
		}
		
		if( READ_DIGITAL(INTLKS_CONTROL, &CommStatus) != 0) // Interlock All Enable == 0
		{
			WRITE_DIGITAL(INTLKS_CONTROL, 0 , &CommStatus);
			if(!WAIT_SECONDS(10)) return SYS_ABORTED;
		}
		
		if( READ_DIGITAL(PLC_GbIntlckDI, &CommStatus) == INTERLOCK_1 || READ_DIGITAL(SW_GbIntlckDM, &CommStatus) == INTERLOCK_1 )
		{
			sprintf( Buffer, "PM Interlock Occurred, so it is not impossible MFC Flow Test" );
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer);
			return SYS_ABORTED;
		}
						
		//------------------------------------------------------------------------------------------
		// PRE CHECK : Fast Rough
		//------------------------------------------------------------------------------------------
		WRITE_DIGITAL( MFC_FLOW_TEST, ON_1, &CommStatus ) ;
		
		if( FastPumping() == FALSE )
		{
			sprintf( Buffer, "Chamber Base Pumping Failed" );
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer);
			WRITE_DIGITAL(MFC_FLOW_TEST, OFF_0, &CommStatus);
			return SYS_ABORTED;
		}
				
		//------------------------------------------------------------------------------------------
		// MFC Flow Test 
		//------------------------------------------------------------------------------------------		
		if( SET_MANUAL_MFC() == SYS_ABORTED )
		{
			sprintf( Buffer, "Manual Flow Fail" );
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer);
			MFC_FlowTest_Init();
			WRITE_DIGITAL(MFC_FLOW_TEST, OFF_0, &CommStatus);	
			return SYS_ABORTED;
		}
		
	}
	else if(STRCMP_L(Ed, "MFCFLOW_ALL_SELECT"))
	{
		for(nMFCListIdx = 0; nMFCListIdx < MAX_MFC; nMFCListIdx++) 
		{
			if(strcmp(m_MFC_List[nMFCListIdx].szName, "-") == 0)	continue;	//... Not Define
			WRITE_DIGITAL(MFlow_MFC01 + nMFCListIdx, ON_1, &CommStatus);
		}

		//... Save GUI Parameter
		WRITE_DIGITAL(MFlow_MFC_ParmSaveDM, ON_1, &CommStatus);

		return SYS_SUCCESS;
	}
	//... Deselect All MFC Flow Item
	else if(STRCMP_L(Ed, "MFCFLOW_ALL_DESELECT"))
	{
		for(nMFCListIdx = 0; nMFCListIdx < MAX_MFC ; nMFCListIdx++) 
		{
			WRITE_DIGITAL(MFlow_MFC01 + nMFCListIdx, OFF_0, &CommStatus);
		}

		//... Save GUI Parameter
		WRITE_DIGITAL(MFlow_MFC_ParmSaveDM, ON_1, &CommStatus);

		return SYS_SUCCESS;
	}
	//... Run MFC Flow TEST
	else if(STRCMP_L(Ed, "MFC_FLOW_TEST")) 
	{
		//------------------------------------------------------------------------------------------
		// PRE CHECK : Valve Recipe Running, SW & PLC Global Interlock 
		//------------------------------------------------------------------------------------------
		if( READ_DIGITAL(VRCP_RunningDI, &CommStatus) == RUN_1 )
		{
			sprintf( Buffer, "PM is Running, so it is not impossible MFC Flow Test" );
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer);
			return SYS_ABORTED;
		}

		if( READ_DIGITAL(INTLKS_CONTROL, &CommStatus) != 0) // Interlock All Enable == 0
		{
			WRITE_DIGITAL(INTLKS_CONTROL, 0 , &CommStatus);
			if(!WAIT_SECONDS(10)) return SYS_ABORTED;
		}

		if( READ_DIGITAL(PLC_GbIntlckDI, &CommStatus) == INTERLOCK_1 || READ_DIGITAL(SW_GbIntlckDM, &CommStatus) == INTERLOCK_1 )
		{
			sprintf( Buffer, "PM Interlock Occurred, so it is not impossible MFC Flow Test" );
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer);
			return SYS_ABORTED;
		}

		i = 0 ;
		for( MFC_Sel = 0 ; MFC_Sel < MAX_MFC ; MFC_Sel ++ ) 
		{
			if( READ_DIGITAL( MFlow_MFC01 + MFC_Sel , &CommStatus ) == ON_1 )
			{
				i++ ;
			}
			else WRITE_DIGITAL( MFlow_MFC01 + MFC_Sel , OFF_0 , &CommStatus ) ;
		}

		if( i == 0 ) 
		{
			sprintf( Buffer, "No Select MFC, so it is not impossible to Flow Test the MFC" );
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer);
			return SYS_ABORTED;
		}

		//------------------------------------------------------------------------------------------
		// PRE CHECK : Fast Rough
		//------------------------------------------------------------------------------------------
		WRITE_DIGITAL( MFC_FLOW_TEST, ON_1, &CommStatus ) ;

		if( FastPumping() == FALSE )
		{
			sprintf( Buffer, "Chamber Base Pumping Failed" );
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer);
			WRITE_DIGITAL(MFC_FLOW_TEST, OFF_0, &CommStatus);
			return SYS_ABORTED;
		}

		//////////////////////////////////////////////////////////////////////////
		//... Update Start Pressure
		dbPressure = READ_ANALOG(M651_Pressure, &CommStatus);
		WRITE_ANALOG(MFlow_StartChmPrs, dbPressure, &CommStatus);

		dbPressure = READ_ANALOG(PumpPrsAI, &CommStatus);
		WRITE_ANALOG(MFlow_StartPumpPrs, dbPressure, &CommStatus);

		//------------------------------------------------------------------------------------------
		// MFC Flow Test 
		//------------------------------------------------------------------------------------------		
		if( MFCFlowTest() == SYS_ABORTED )
		{
			sprintf( Buffer, "Abort or Result NG" );
			ALARM_POST_WithMsg(ALARM_MFCFlowTest_FAIL, Buffer);
			MFC_FlowTest_Init();

			//... Save GUI Parameter
			WRITE_DIGITAL(MFlow_MFC_ParmSaveDM, ON_1, &CommStatus);

			return SYS_ABORTED;
		}
	}
	else 
	{
		printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
		return SYS_ABORTED;
	}

	if(!STRCMP_L(Ed, "MANUAL_MFCFLOW"))
	{
		RUN_FUNCTION_FREE ( ABORT_GAS , "" );
		RUN_FUNCTION_FREE ( VACUUM_CONTROL , "FAST_ROUGH" );
	}

	WRITE_DIGITAL( MFC_FLOW_TEST, OFF_0, &CommStatus ) ;

	//... Save GUI Parameter
	WRITE_DIGITAL(MFlow_MFC_ParmSaveDM, ON_1, &CommStatus);

	return SYS_SUCCESS;
}

BOOL CMFCFlowTest::ReadConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle;

	do {
		fp = fopen(MFCFLOW_CFG_FILE_PATH, "rt");
		if(fp == NULL) break;
		if(NULL == fgets(szRead, 255, fp)) break;
		do {
			if(szRead[0] == '[')
			{
				CTextParser tp(szRead);
				tp.GetChar();
				tp.GetString(szItem, ']');

				if(strcmp("MFC List", szItem) == 0)						nTitle = 1;
				else if(strcmp("Chamber Config", szItem) == 0)			nTitle = 2;
				else nTitle = 0;

				if(nTitle > 0) do
				{
					if(NULL == fgets(szRead, 255, fp)) break;
					if(szRead[0] == '[') break;
					if(szRead[0] != ';' && szRead[0] != '#') switch(nTitle)
					{
					case 1: ParseMFCList(szRead); break;
					case 2: ParseChamberConfig(szRead); break;
					}
				} while(feof(fp) == 0);
				else if(NULL == fgets(szRead, 255, fp)) break;
			}
			else if(NULL == fgets(szRead, 255, fp)) break;
		} while(feof(fp) == 0);
		fclose(fp);
		bRet = TRUE;
	} while(0);
	if(bRet == FALSE)
	{
		printf("-----> Config File Reading Failed : [%s]\n", MFCFLOW_CFG_FILE_PATH);
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//... 2015.02.21 Bug Fixed MFC Number Parsing
void CMFCFlowTest::ParseMFCList(char *szData)
{
	int nMFCNo	= 0;
	int nMFCIdx = 0;

	CTextParser txtP;
	txtP.SetText(szData);

	txtP.GetInt(&nMFCNo);

	//... Check Valid Range
	if(0 < nMFCNo && nMFCNo <= MAX_MFC)
	{
		//... Changed Index Array
		nMFCIdx = nMFCNo - 1;
		txtP.GetWord(m_MFC_List[nMFCIdx].szName);		
	}
}

void CMFCFlowTest::ParseChamberConfig(char *szData)
{
	CTextParser txtP;
	txtP.SetText(szData);
	txtP.GetWord();

	if(txtP.BeEqual("ALARM_MfcFlowFail")) txtP.GetInt('=', &m_nALARM_MfcFlowFail);
}