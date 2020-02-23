// MFCVerification.cpp: implementation of the CMFCVerification class.
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
#include "MFCVerification.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { OFF_0	, ON_1		}; 
enum { V_CLOSE	, V_OPEN	}; 
enum { NOpen	,	FClose	} ; 
enum { RealGas_0	,  PurgeAr_1	,	NormalN2_2,		RealGas_FinMax_3};
enum { Auto_0		,  Manual_1	};
enum { M_PRESS		,  M_TIME	};

enum { Chuck_NotHome_0 , Chuck_Home_1 };
enum { M_REF		,  M_LFM	};
enum { R_IDLE		,  R_REF	,  R_LFM	};
enum { CHUCK_LEFT , CHUCK_RIGHT , CHUCK_ALL } ;
enum { Idle_0	  , Running_1	, Aborted_2 } ;
enum { Normal	  , Abort		}; // Abort Check

enum { GasNormal	,	GasPurge		,	GasDivFlow	};  
enum { PLC_INT_UNLOCK, PLC_INT_LOCK}; //... 2016.02.16

#define RESULT_ERROR_VALUE	-9999

#define		 ALARM_MFC_VERIFY_FAIL			m_nALARM_MfcVerFail
#define		 ALARM_HITEMP_MFC_VERIFY_FAIL	2182

int			ALARM_START_POINTER = 0;

#define		ABORTGAS_WAIT_TIME	5

#define MFCV_CFG_FILE_PATH	"Parameter\\PM_SEQ_PARM\\Gas_Calibration.ini"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMFCVerification::CMFCVerification()
{
	m_dbChamberVol = 44.2;		// liter
	m_dbPressCheckErr = 0.3;	// torr
	m_dbEndPressCheck = 9.8;	// torr

	m_dbPumpTime = 30;			// sec
	m_dbPurgeTime = 30;			// sec
	m_dbStableTime = 30;			// sec

	m_nALARM_MfcVerFail = 2061;
	m_nBaseMFCNo1 = 1;           //... 2016.02.16
	m_nBaseMFCNo2 = 9;
	m_dbCompareTemp	= 20;
	m_bGetBaseVerfiyStartBit = FALSE;
}

CMFCVerification::~CMFCVerification()
{

}

//////////////////////////////////////////////////////////////////////
// Member Function

void CMFCVerification::Enter_Code()
{
}

BOOL CMFCVerification::Initialize()
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
    //... 2016.02.16 view MFC Cal Parameter
	printf("ChamberVol : %.2f, PressCheckErr : %.2f, EndPressCheck : %.2f, PumpTime : %.2f, PurgeTime : %.2f, StableTime : %.2f, ALARM_MfcVerFail : %d\n", 
		m_dbChamberVol, m_dbPressCheckErr, m_dbEndPressCheck, m_dbPumpTime, m_dbPurgeTime, m_dbStableTime,m_nALARM_MfcVerFail );

	for(i=0; i<MAX_MFC; i++)
	{
		WRITE_STRING(MFC01_Name+i, m_MFC_List[i].szName, &CommStatus);
	}

	CreateThread();

	return TRUE;
}

BOOL CMFCVerification::Deinitialize()
{
	return TRUE;
}

Module_Status CMFCVerification::Main()
{
	int	CommStatus;
	char *Ed;
	int  MFC_Sel = 0 ;
	int nGasType;
	Ed = PROGRAM_PARAMETER_READ();

	nGasType = READ_DIGITAL(M_CAL_GasType_SELECT, &CommStatus);
	WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , OFF_0 , &CommStatus );

	if ( STRCMP_L ( Ed , "MFC_ALL_SELECT" ) )
	{
		for ( MFC_Sel = 0 ; MFC_Sel < MAX_MFC ; MFC_Sel ++ ) 
		{
			if(strcmp(m_MFC_List[MFC_Sel].szName, "-") == 0) continue;
			if(strcmp(m_MFC_List[MFC_Sel].szCalGasName[nGasType], "-") == 0) continue;

			WRITE_DIGITAL ( M_CAL_GAS00 + MFC_Sel , ON_1 , &CommStatus ) ;
		}

		return SYS_SUCCESS ;
	}
	else if ( STRCMP_L ( Ed , "MFC_ALL_DESELECT" ) )
	{
		for ( MFC_Sel = 0 ; MFC_Sel < MAX_MFC ; MFC_Sel ++ ) 
		{
			WRITE_DIGITAL ( M_CAL_GAS00 + MFC_Sel , OFF_0 , &CommStatus ) ;
		}
		return SYS_SUCCESS ;
	}
	else if ( STRCMP_L ( Ed , "CALIBRATION" ) ) 
	{
		return MFC_Verification(nGasType);			
	}
	else if ( STRCMP_L ( Ed , "TEST" ) )
	{
		return MFC_VERIFY_LOGGING ( );
	}
	else {
		printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
		return SYS_ABORTED;
	}

	WRITE_DIGITAL ( INTLKS_CONTROL , 0 , &CommStatus );

	WRITE_DIGITAL ( M_CAL_GAS_SELECT_RESULT     , 18 , &CommStatus ); // 18 is None

	//...	2016.02.16 Abort Gas, Vacuum Control Added
	RUN_FUNCTION_FREE ( ABORT_GAS , "" );
	RUN_FUNCTION_FREE ( VACUUM_CONTROL , "FAST_ROUGH" );

	return SYS_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Module_Status CMFCVerification::MFC_Verification(int nGasType)
{
	Module_Status msRet = SYS_SUCCESS;
	int	CommStatus;
	int GasSelect ;
	double PumpTime , CurrentPress1 , CurrentPress2 , ElapsedTime , PurgeTime, Standard_ChamberTemp;
	double GasSet , TargetPress , StartPress;
	char Buffer[1024];
	double Coversion = 1;
	int Type , InDev , OutDev;
	double Min , Max;
	int i = 0, F_Result ;
	double	Verifi_Result_Factor_Final = 0;
	BOOL	Verifi_Result_Factor_Final_Result = TRUE;
	int  MFC_Sel = 0 ;
	//... 2016.02.16
	int nManualFlow = -1;
	int nBaseMFC = 0;
	double dbBaseTemp = 0;
	double dbBaseTemp2 = 0;

	//... 2016.02.16 High Temperature MFC Verification Option Check
	if(READ_DIGITAL( HIGH_MFC_CAL   , &CommStatus ) == ON_1 ) m_bGetBaseVerfiyStartBit = TRUE;

	//////////////////////////////////////////////////////////////////////////
	//... PRE CHECK
	//////////////////////////////////////////////////////////////////////////
	for ( MFC_Sel = 0 ; MFC_Sel < MAX_MFC ; MFC_Sel ++ ) 
	{
		if ( READ_DIGITAL ( M_CAL_GAS00 + MFC_Sel , &CommStatus ) == ON_1
			&& 	strcmp(m_MFC_List[MFC_Sel].szCalGasName[nGasType], "-") != 0)
		{
			i = i ++ ;
		}
		else WRITE_DIGITAL ( M_CAL_GAS00 + MFC_Sel , OFF_0 , &CommStatus ) ;
	}

	if ( i == 0 ) 
	{
		sprintf ( Buffer , "No Select MFC , so it is not impossible to Calibrate the MFC" );
		ALARM_POST_WithMsg(ALARM_MFC_VERIFY_FAIL, Buffer);

		WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , OFF_0 , &CommStatus ) ;
		return SYS_ABORTED;
	}

    //... To Check MFC Verification Function Status (Reference to Control monitor Function)
	WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , ON_1 , &CommStatus ) ;

	MFC_Sel = 0 ;

	//////////////////////////////////////////////////////////////////////////
	//... PLC Interlock Off (it is possible to flow Gas with Rough Valve Close)
	//////////////////////////////////////////////////////////////////////////

	//... 2016.02.16
	if(SYS_SUCCESS != PLCInterlockControl(PLC_INT_UNLOCK)) 
	{
		sprintf ( Buffer , "PLC Interlock Auto Unlock Fail");

		ALARM_POST_WithMsg(ALARM_MFC_VERIFY_FAIL, Buffer);

		WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , OFF_0 , &CommStatus ) ;

		printf("[MFC verification] PLC Interlock Unlock Check Fail\n");
		return SYS_ABORTED;
	}
	//...


	//////////////////////////////////////////////////////////////////////////
	//... Verification Start
	//////////////////////////////////////////////////////////////////////////
	while ( TRUE )
	{
		if ( READ_DIGITAL ( M_CAL_GAS00 + MFC_Sel , &CommStatus ) == OFF_0 )
		{
			MFC_Sel = MFC_Sel ++ ;
			if ( MFC_Sel >= 18 )
			{
				break ;
			}
			continue ;
		}
		else
		{
			if ( MFC_Sel >= 18 ) break ;
			GasSelect = MFC_Sel;
			WRITE_DIGITAL ( M_CAL_GAS_SELECT , MFC_Sel , &CommStatus );
			WRITE_STRING( M_CAL_GAS_Name , m_MFC_List[GasSelect].szName , &CommStatus );
			printf("#============================================\n");
			printf("#  MFC(%s) VERIFICATION START   \n",m_MFC_List[GasSelect].szName );
			printf("#============================================\n");

		}
		PumpTime = CurrentPress1 = CurrentPress2 = ElapsedTime = PurgeTime = 0;
		GasSet = TargetPress = StartPress = 0;
		
		//... 2015.02.16 Manual MFC Set 
		if ( READ_DIGITAL ( M_CAL_AutoMan_SELECT    	, &CommStatus ) == Manual_1  &&
			 READ_ANALOG ( Manual_MFC1_SET + GasSelect , &CommStatus ) == 0             )
		{
			sprintf ( Buffer , "MFC Set is Zero, so it is not impossible to Calibrate the MFC" );
			ALARM_POST_WithMsg(ALARM_MFC_VERIFY_FAIL, Buffer);
			WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , OFF_0 , &CommStatus ) ;
			return SYS_ABORTED;
		}

		
		//... 2016.02.16 Z-Motion Homing Sensor Check	
		if ( READ_DIGITAL ( CHUCK_HOME_STAT_A, &CommStatus ) != Chuck_Home_1  || 
			    READ_DIGITAL ( CHUCK_HOME_STAT_B, &CommStatus ) != Chuck_Home_1 )
		{
			sprintf ( Buffer , "Stage Heater is NotHome, so it is not impossible to Calibrate the MFC" );
			ALARM_POST_WithMsg(ALARM_MFC_VERIFY_FAIL, Buffer);
			WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , OFF_0 , &CommStatus ) ;
			return SYS_ABORTED;
		}
		
		if ( READ_DIGITAL( HIGH_MFC_CAL   , &CommStatus ) != ON_1 )
		{
			Standard_ChamberTemp = READ_ANALOG ( M_CAL_CHAMBER_TEMP   , &CommStatus );
			if ( READ_ANALOG ( Eu1_CurrTemp    	, &CommStatus ) > Standard_ChamberTemp  ||
				 READ_ANALOG ( Eu2_CurrTemp    	, &CommStatus ) > Standard_ChamberTemp    )
			{
				sprintf ( Buffer , "Stage Heater Temp is High, so it is not impossible to Calibrate the MFC" );
				ALARM_POST_WithMsg(ALARM_MFC_VERIFY_FAIL, Buffer);
				WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , OFF_0 , &CommStatus ) ;
				return SYS_ABORTED;
			}
		}
		//---> ADD 2008/11/30  by KSM  : GUI IO Clear
		for ( i = 0 ; i < MFC_COUNT ; i++ ) { 
			WRITE_ANALOG ( M_CALI_MFC01_FACTOR + i , 0 , &CommStatus );
		}
			
		for ( i = 0 ; i < 5 ; i++ ) { 
			WRITE_ANALOG ( M_CAL_MFC_SET_1 + i       ,  0 ,  &CommStatus ); 
			WRITE_ANALOG ( M_CAL_MFC_ACT_FLOW_1 + i  ,  0 ,  &CommStatus ); 
			WRITE_ANALOG ( M_CAL_MFC_FACTOR_1 + i    ,  0 ,  &CommStatus );
		}		

		WRITE_ANALOG ( M_CAL_TOTAL_ACT_FLOW , 0 , &CommStatus );
		WRITE_ANALOG ( M_CAL_MFC_FACTOR_AVG , 0 ,  &CommStatus );
		WRITE_DIGITAL ( M_CAL_GAS_SELECT_RESULT     , 18 , &CommStatus ); // 18 is None
		WRITE_DIGITAL ( M_CAL_GasType_SELECT_RESULT ,  0 , &CommStatus ); 

		//---> END 2008/11/30  by KSM
		for ( i = 0 ; i < 5 ; i ++ )
		{
			WRITE_ANALOG  ( M_CAL_MFC00_SET_1 + i + (GasSelect*16) , 0 , &CommStatus ) ;
			WRITE_ANALOG  ( M_CAL_MFC00_ACT_FLOW_1 + i + (GasSelect*16) , 0 , &CommStatus ) ;
			WRITE_ANALOG  ( M_CAL_MFC00_FACTOR_1 + i + (GasSelect*16) , 0 , &CommStatus ) ;
		}
		WRITE_ANALOG  ( M_CAL_MFC00_FACTOR_AVG + (GasSelect*16) , 0 , &CommStatus ) ;
		//]


		//---> ADD 2008/11/30  by KSM
		if ( READ_DIGITAL ( M_CAL_AutoMan_SELECT	, &CommStatus ) == Manual_1 )
		{
			//... 2016.02.16 Manual MFC Set
			nManualFlow = (int) READ_ANALOG ( Manual_MFC1_SET + GasSelect    , &CommStatus );
			Verification_Control( GasSelect, i, nManualFlow);
			
			//[ 2010/03/04 Manual로 MFC Verification 시에도 Logging되도록 수정 ( BY LJH )
			WRITE_DIGITAL( M_CAL_GAS_SELECT_RESULT      , GasSelect , &CommStatus );
			WRITE_ANALOG ( M_CAL_MFC_SET_1			    , nManualFlow,  &CommStatus ); 
			WRITE_ANALOG ( M_CAL_MFC_ACT_FLOW_1			, READ_ANALOG ( M_CAL_TOTAL_ACT_FLOW            , &CommStatus ),  &CommStatus ); 
			WRITE_ANALOG ( M_CAL_MFC_FACTOR_1			, READ_ANALOG ( M_CALI_MFC01_FACTOR + GasSelect , &CommStatus ),  &CommStatus );
			MFC_VERIFY_LOGGING () ;
			//]
		}
		else //... Auto Select
		{
			// For Result GUI
			
			WRITE_DIGITAL ( M_CAL_GAS_SELECT_RESULT     , GasSelect , &CommStatus );
			WRITE_DIGITAL ( M_CAL_GasType_SELECT_RESULT , READ_DIGITAL ( M_CAL_GasType_SELECT	, &CommStatus ) , &CommStatus );

			ANALOG_SHAPE  ( MFC01StPtAO + GasSelect , &Type , &InDev , &OutDev , &Min , &Max );

			//... Calculate MFC Flow Rate 
			CalMFCFlowRate((int)Max);
			
			WRITE_ANALOG ( M_CAL_STABLE_TIME_SET  , m_dbStableTime  , &CommStatus );	

			//... 2016.02.16
			if ( READ_DIGITAL( HIGH_MFC_CAL   , &CommStatus ) == ON_1 
				&& m_bGetBaseVerfiyStartBit == TRUE)
			{
				do 
				{
					//... Start Leak Check Sequence
					if(ON_1 == READ_DIGITAL(M_CAL_LeakChkDM,&CommStatus))
					{ 
						F_Result = RUN_FUNCTION(VACUUM_CONTROL,"LEAK_CHECk");
						if(F_Result != SYS_SUCCESS) break;
					}
					printf("[MFC Verification] Base MFC%d Check Start\n",m_nBaseMFCNo1);
					//... Base#1 MFC Flow before High Temp of MFC Verification   
					nBaseMFC = m_nBaseMFCNo1;
					nBaseMFC -= 1 ; 
					F_Result = Get_Base_Verify(nBaseMFC, 3, -1);
					if(F_Result != SYS_SUCCESS) break;
					//... Read Calculated Chamber Temp After Base#1 MFC Flow
					dbBaseTemp = READ_ANALOG(M_CAL_CHAMBER_TEMP_Ret,&CommStatus);
					printf("[MFC Verification] Base MFC%d Check end[Temp %.2f]\n",m_nBaseMFCNo1,dbBaseTemp);
					
					printf("[MFC Verification] Base MFC%d Check Start\n",m_nBaseMFCNo2);
					//... Base#2 MFC Flow before High Temp of MFC Verification   
					nBaseMFC = m_nBaseMFCNo2;
					nBaseMFC -= 1 ; 
					F_Result = Get_Base_Verify(nBaseMFC, 3, -1);
					if(F_Result != SYS_SUCCESS) break;
					//... Read Calculated Chamber Temp After Base#2 MFC Flow
					dbBaseTemp2 = READ_ANALOG(M_CAL_CHAMBER_TEMP_Ret,&CommStatus);
					printf("[MFC Verification] Base MFC%d Check end[Temp %.2f]\n",m_nBaseMFCNo2,dbBaseTemp2);

					//... Compare Calculated sBase Temp#1 to Temp#2 
					//... if differential temperature is 20 C, Interlock occur
					dbBaseTemp -= dbBaseTemp2;

					if( m_dbCompareTemp < fabs(dbBaseTemp))
					{
                        //.. TO.DO Alarm Post
						sprintf ( Buffer , "Base (MFC%d),(MFC%d) Temp(Int%.3f Real%.3f)", m_nBaseMFCNo1, m_nBaseMFCNo2 ,m_dbCompareTemp, dbBaseTemp );
						ALARM_POST_WithMsg(ALARM_HITEMP_MFC_VERIFY_FAIL, Buffer);
						F_Result = SYS_ABORTED;
			 			break;
					}

				} while (0);
				m_bGetBaseVerfiyStartBit = FALSE;
			}

			if ( F_Result == SYS_ABORTED )
			{
				if(SYS_SUCCESS!=PLCInterlockControl(PLC_INT_LOCK))
				{
					printf("[MFC verification] PLC Interlock Lock Check Fail\n");
				}
				return SYS_ABORTED;
			}
			//...
			for ( i = 0 ; i < 5 ; i++ )
			{
				if ( i == 0 )
				{
					WRITE_ANALOG ( M_CAL_PUMP_TIME_SET  , m_dbPumpTime  , &CommStatus );
					WRITE_ANALOG ( M_CAL_PURGE_TIME_SET , m_dbPurgeTime , &CommStatus );
				}
				else
				{
					WRITE_ANALOG ( M_CAL_PUMP_TIME_SET  , 0  , &CommStatus );
					WRITE_ANALOG ( M_CAL_PURGE_TIME_SET , 0  , &CommStatus );
				}

				WRITE_DIGITAL ( M_CAL_LoopCount  , i+1 , &CommStatus ); 

			//####################################################################
				F_Result = Verification_Control( GasSelect, i, -1 );
			//####################################################################

				if ( F_Result == SYS_ABORTED )
				{

					//...	2016.02.16 Abort Gas, Vacuum Control Added
					RUN_FUNCTION_FREE ( ABORT_GAS , "" );
					RUN_FUNCTION_FREE ( VACUUM_CONTROL , "FAST_ROUGH" );

					WRITE_DIGITAL ( INTLKS_CONTROL , 0 , &CommStatus );
					WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , OFF_0 , &CommStatus ) ;
					 //... 2016.02.16
					if(SYS_SUCCESS != PLCInterlockControl(PLC_INT_LOCK))
					{
						printf("[MFC verification] PLC Interlock Lock Check Fail\n");
					}
					//...
					return SYS_ABORTED;
				}					
				WRITE_ANALOG ( M_CAL_MFC_SET_1 + i       , m_Step_MFCSet[i]		, &CommStatus );	   //... GasSet 2012.08.17
				WRITE_ANALOG ( M_CAL_MFC_ACT_FLOW_1 + i  , READ_ANALOG ( M_CAL_TOTAL_ACT_FLOW            , &CommStatus ),  &CommStatus );  //Total Flow
				WRITE_ANALOG ( M_CAL_MFC_FACTOR_1 + i    , READ_ANALOG ( M_CALI_MFC01_FACTOR + GasSelect , &CommStatus ),  &CommStatus );  //Factor
			}

			Verifi_Result_Factor_Final_Result = TRUE;

			Verifi_Result_Factor_Final = 0;

			for ( i = 0 ; i < 5 ; i++ )
			{
				Verifi_Result_Factor_Final   += READ_ANALOG ( M_CAL_MFC_FACTOR_1 + i , &CommStatus );

				if ( READ_ANALOG ( M_CAL_MFC_FACTOR_1 + i , &CommStatus ) == RESULT_ERROR_VALUE )   // RESULT_ERROR_VALUE = -9999
				{
					Verifi_Result_Factor_Final_Result = FALSE;
				}
			}

			if ( Verifi_Result_Factor_Final_Result )
			{
				Verifi_Result_Factor_Final   = Verifi_Result_Factor_Final   / 5 ;
			}
			else
			{
				Verifi_Result_Factor_Final   = RESULT_ERROR_VALUE ;
			}

			WRITE_ANALOG ( M_CAL_MFC_FACTOR_AVG , Verifi_Result_Factor_Final  , &CommStatus );

			printf(" Final  Verifi_Result_Factor_Final [%f]\n", Verifi_Result_Factor_Final);
		
			MFC_VERIFY_LOGGING () ;
		}
		MFC_Sel = MFC_Sel ++ ;
	}
	//////////////////////////////////////////////////////////////////////////
	//... PLC Interlock On
	//////////////////////////////////////////////////////////////////////////

	//... 2016.02.16
	if(SYS_SUCCESS != PLCInterlockControl(PLC_INT_LOCK)) 
	{
		WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , OFF_0 , &CommStatus ) ;
		printf("[MFC verification] PLC Interlock Lock Check Fail\n");
		return SYS_ABORTED;
	}
	RUN_FUNCTION_FREE ( ABORT_GAS , "" );

	WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , OFF_0 , &CommStatus ) ;

	//... 2016.02.16 Manual MFC Set
	for ( i = 0 ; i < MFC_COUNT ; i++ ) { 
		WRITE_ANALOG ( Manual_MFC1_SET + i , 0 , &CommStatus );
	}
	return msRet;
}	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//... 2016.02.16  Get Chamber Temp for High Temp MFC Verification
Module_Status CMFCVerification::Get_Base_Verify(int GasSelect, int nMfcSetIdx, int nManualFlow)
{
	Module_Status F_Result; 
	int CommStatus;

	WRITE_ANALOG ( M_CAL_PUMP_TIME_SET  , m_dbPumpTime  , &CommStatus );
	WRITE_ANALOG ( M_CAL_PURGE_TIME_SET , m_dbPurgeTime , &CommStatus );
	
	F_Result = Verification_Control( GasSelect, nMfcSetIdx, nManualFlow);  

	if ( F_Result == SYS_ABORTED )
	{

		RUN_FUNCTION_FREE ( ABORT_GAS , "" );
		RUN_FUNCTION_FREE ( VACUUM_CONTROL , "FAST_ROUGH" );

		WRITE_DIGITAL ( INTLKS_CONTROL , 0 , &CommStatus );

		WRITE_DIGITAL ( AUTO_MFC_VERIFY_CTR , OFF_0 , &CommStatus ) ;
	}	
					
	return F_Result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//... 2016.02.16 PLC Auto Unlock
Module_Status CMFCVerification::PLCInterlockControl(int nInterlockSts)
{
	int nCommStatus;
	Module_Status msRet = SYS_SUCCESS;
	printf("[MFC verification] PLC Interlock %d Control Start\n",nInterlockSts);
	do 
	{
		if(nInterlockSts == PLC_INT_UNLOCK)
		{
			WRITE_DIGITAL ( PLCInkUnlockStsDO , ON_1 , &nCommStatus ) ;

			if(!WAIT_SECONDS(2))
			{
				msRet = SYS_ABORTED;
				break;
			}

			WRITE_DIGITAL ( PLCInkUnlockStsDO , OFF_0 , &nCommStatus ) ;
		}
		else //nInterlockSts == PLC_INT_LOCK
		{
			WRITE_DIGITAL ( PLCIntLockDO , ON_1 , &nCommStatus ) ;

			if(!WAIT_SECONDS(2))
			{
				msRet = SYS_ABORTED;
				break;
			}

			WRITE_DIGITAL ( PLCIntLockDO , OFF_0 , &nCommStatus ) ;
		}
	} while(0);

	if(msRet == SYS_ABORTED)
	{
		printf("[MFC verification] PLC Interlock %d Control Fail\n",nInterlockSts);
	}
	else
	{
		printf("[MFC verification] PLC Interlock %d Control Success\n",nInterlockSts);
	}
	return msRet;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMFCVerification::CalMFCFlowRate(int nMax)
{
	int CommStatus;

	if ( READ_DIGITAL( M_CAL_GasType_SELECT , &CommStatus ) == NormalN2_2 ) 	//...Verification Test For IPS manufacture Part Team
	{
		if ( nMax <= 1000 )
		{
			m_Step_MFCSet[0] = 100;
			m_Step_MFCSet[1] = 100;
			m_Step_MFCSet[2] = 100;
			m_Step_MFCSet[3] = 200;
			m_Step_MFCSet[4] = 300;
			
			WRITE_DIGITAL ( M_CAL_MODE_SELECT	  , M_TIME , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 1  , &CommStatus );
			WRITE_ANALOG  ( M_CAL_ELAPSE_TIME_SET , 20 , &CommStatus );
		} 
		else if ( nMax <= 5000 )
		{
			m_Step_MFCSet[0] = 1000;
			m_Step_MFCSet[1] = 1000;
			m_Step_MFCSet[2] = 1000;
			m_Step_MFCSet[3] = 1500;
			m_Step_MFCSet[4] = 1500;	
			
			WRITE_DIGITAL ( M_CAL_MODE_SELECT , M_PRESS , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 2.5  , &CommStatus );
		}
		else
		{
			m_Step_MFCSet[0] = 3000;
			m_Step_MFCSet[1] = 3000;
			m_Step_MFCSet[2] = 3000;
			m_Step_MFCSet[3] = 3000;
			m_Step_MFCSet[4] = 3000;	

			WRITE_DIGITAL ( M_CAL_MODE_SELECT , M_PRESS , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 2.5  , &CommStatus );
			WRITE_ANALOG  ( M_CAL_TARGET_PRESS    , 9 , &CommStatus );
		}
	}
	else if(READ_DIGITAL( M_CAL_GasType_SELECT , &CommStatus ) == RealGas_0
			||READ_DIGITAL( M_CAL_GasType_SELECT , &CommStatus ) == PurgeAr_1) 
	{
		if ( nMax <= 1000 )
		{
			m_Step_MFCSet[0] = 100;
			m_Step_MFCSet[1] = 100;
			m_Step_MFCSet[2] = 100;
			m_Step_MFCSet[3] = 200;
			m_Step_MFCSet[4] = 300;
			
			WRITE_DIGITAL ( M_CAL_MODE_SELECT	  , M_TIME , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 1  , &CommStatus );
			WRITE_ANALOG  ( M_CAL_ELAPSE_TIME_SET , 20 , &CommStatus );
		} 
		else if ( nMax <= 2000 )
		{
			m_Step_MFCSet[0] = 1000;
			m_Step_MFCSet[1] = 1000;
			m_Step_MFCSet[2] = 1000;
			m_Step_MFCSet[3] = 1500;
			m_Step_MFCSet[4] = 1500;	
			
			WRITE_DIGITAL ( M_CAL_MODE_SELECT , M_PRESS , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 2  , &CommStatus );
			WRITE_ANALOG  ( M_CAL_TARGET_PRESS    , 9 , &CommStatus );
		}
		else if ( nMax <= 5000 )
		{
			m_Step_MFCSet[0] = 1000;
			m_Step_MFCSet[1] = 1000;
			m_Step_MFCSet[2] = 1000;
			m_Step_MFCSet[3] = 1500;
			m_Step_MFCSet[4] = 2500;	
			
			WRITE_DIGITAL ( M_CAL_MODE_SELECT , M_PRESS , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 2 , &CommStatus );
			WRITE_ANALOG  ( M_CAL_TARGET_PRESS    , 9 , &CommStatus );
		}
		else   // nMax  > 5000
		{
			m_Step_MFCSet[0] = 3000;
			m_Step_MFCSet[1] = 3000;
			m_Step_MFCSet[2] = 3000;
			m_Step_MFCSet[3] = 3500;
			m_Step_MFCSet[4] = 4000;	
			
			WRITE_DIGITAL ( M_CAL_MODE_SELECT , M_PRESS , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 2  , &CommStatus );
			WRITE_ANALOG  ( M_CAL_TARGET_PRESS    , 9 , &CommStatus );
		}
	}
	else if(READ_DIGITAL( M_CAL_GasType_SELECT , &CommStatus ) == RealGas_FinMax_3)
	{
		if ( nMax <= 1000 )
		{
			m_Step_MFCSet[0] = 100;
			m_Step_MFCSet[1] = 100;
			m_Step_MFCSet[2] = 200;
			m_Step_MFCSet[3] = 300;
			m_Step_MFCSet[4] = nMax;
			
			WRITE_DIGITAL ( M_CAL_MODE_SELECT	  , M_TIME , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 1  , &CommStatus );
			WRITE_ANALOG  ( M_CAL_ELAPSE_TIME_SET , 20 , &CommStatus );
		} 
		else if ( nMax <= 2000 )
		{
			m_Step_MFCSet[0] = 1000;
			m_Step_MFCSet[1] = 1000;
			m_Step_MFCSet[2] = 1000;
			m_Step_MFCSet[3] = 1500;
			m_Step_MFCSet[4] = nMax;	
			
			WRITE_DIGITAL ( M_CAL_MODE_SELECT , M_PRESS , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 2  , &CommStatus );
			WRITE_ANALOG  ( M_CAL_TARGET_PRESS    , 9 , &CommStatus );
		}
		else if ( nMax <= 5000 )
		{
			m_Step_MFCSet[0] = 1000;
			m_Step_MFCSet[1] = 1000;
			m_Step_MFCSet[2] = 1500;
			m_Step_MFCSet[3] = 2500;
			m_Step_MFCSet[4] = nMax;	
			
			WRITE_DIGITAL ( M_CAL_MODE_SELECT , M_PRESS , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 2 , &CommStatus );
			WRITE_ANALOG  ( M_CAL_TARGET_PRESS    , 9 , &CommStatus );
		}
		else
		{
			m_Step_MFCSet[0] = 3000;
			m_Step_MFCSet[1] = 3000;
			m_Step_MFCSet[2] = 3500;
			m_Step_MFCSet[3] = 4000;
			m_Step_MFCSet[4] = nMax;	
			
			WRITE_DIGITAL ( M_CAL_MODE_SELECT , M_PRESS , &CommStatus );
			WRITE_ANALOG  ( M_CAL_START_PRESS	  , 2  , &CommStatus );
			WRITE_ANALOG  ( M_CAL_TARGET_PRESS    , 9 , &CommStatus );
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//... 2016.02.16 Add Manual Set
Module_Status CMFCVerification::Verification_Control(int GasSelect, int nMfcSetIdx, int nManualFlow)
{

	int	CommStatus;
	int ModeSelect , GaugeSelect, GaugeIo/*, Chuck_Htr_Model*/, i ;
	double PumpTime , CurrentPress1 , CurrentPress2 , ElapsedTime , PurgeTime , StableTime , CheckTime;
	double GasSet , TargetPress , StartPress;
	int Type , InDev , OutDev;
	double Min , Max;
	double ChamberVol , ChamberTemp , Data1 , Data2 , TotalFlow , MfcFactor;
	double Coversion = 1;
	double ErrorRate = 0;
	char   Buffer[256];

	double dbTempRet = 0; //... 2016.02.16
	int GasTarget[MFC_COUNT] = {0};

	int nIdx, nGasType;

	BOOL	CheckResult = TRUE;


	PumpTime = GasSet = TargetPress = CurrentPress2 = ElapsedTime = 0;
	WRITE_ANALOG ( M_CAL_PURGE_ELAPSED  , 0 , &CommStatus );
	WRITE_ANALOG ( M_CAL_PUMP_ELAPSED   , 0 , &CommStatus );
	WRITE_ANALOG ( M_CAL_ELAPSED_TIME   , 0 , &CommStatus );
	WRITE_ANALOG ( M_CAL_FINAL_PRESS    , 0 , &CommStatus );
	WRITE_ANALOG ( M_CAL_ELAPSED_TIME	, 0 , &CommStatus );
	WRITE_ANALOG ( M_CAL_STABLE_ELAPSED , 0 , &CommStatus );
	WRITE_ANALOG ( M_CAL_TOTAL_ACT_FLOW , 0 , &CommStatus );

	for ( i = 0 ; i < MFC_COUNT ; i++ ) { 
		WRITE_ANALOG ( M_CALI_MFC01_FACTOR + i , 0 , &CommStatus );
	}
	
	//////////////////////////////////////////////////////////////////////////
	//... Gas Select
	GaugeSelect = READ_DIGITAL ( M_CAL_GAUGE_SELECT , &CommStatus );
	if ( GaugeSelect == 0 ) GaugeIo = M651_Pressure;
	else GaugeIo = M651_Pressure;
	
	nGasType = READ_DIGITAL(M_CAL_GasType_SELECT, &CommStatus); 
	Coversion = m_MFC_List[GasSelect].dbConvFactor[nGasType];  // 2012.08.17

	printf(" MFC Verification  Conversion FACTOR [%.3f], UseGasType[%d]\n", Coversion, READ_DIGITAL ( M_CAL_GasType_SELECT	, &CommStatus ) );

	//////////////////////////////////////////////////////////////////////////
	//... 0. Standby

	//...	2016.02.16 Abort Gas, Vacuum Control Added
	if ( RUN_FUNCTION_FREE ( ABORT_GAS , "" ) == SYS_ABORTED )
	{
		return SYS_ABORTED;
	}

	if ( RUN_FUNCTION_FREE ( VACUUM_CONTROL , "FAST_ROUGH" ) == SYS_ABORTED ) {
		return SYS_ABORTED;
	}

	if(!WAIT_SECONDS(ABORTGAS_WAIT_TIME)) _LOG("MFC Verification Aborted : %d", __LINE__);
    //...

	//////////////////////////////////////////////////////////////////////////
	//... 1. Purge before Calibration

	PurgeTime = READ_ANALOG ( M_CAL_PURGE_TIME_SET , &CommStatus );
	if ( PurgeTime > 0 )
	{

		ANALOG_SHAPE  ( MFC01StPtAO + GasSelect , &Type , &InDev , &OutDev , &Min , &Max );
		WRITE_ANALOG  ( M_CAL_PURGE_GAS_SET		, Max    , &CommStatus );   // For GUI Display

		GasTarget[GasSelect] = (int)Max;

		sprintf(Buffer, "MFC%02d_PURGE", GasSelect+1);
		nIdx = m_MFCValveSeq.FindStepIdx(Buffer);
		m_MFCValveSeq.m_adbMFCSet[nIdx][0] = Max;
		if(TRUE != m_MFCValveSeq.StartValveSeq(Buffer))
		{
			return SYS_ABORTED;
		}

		S_TIMER_READY ();
		while ( TRUE ) {
			if ( !WAIT_SECONDS ( 0.5 ) ) 
			{
				return SYS_ABORTED;
			}

			WRITE_ANALOG ( M_CAL_PURGE_ELAPSED , S_TIMER_ELAPSED () , &CommStatus );
			if ( S_TIMER_CHECK ( 1 , PurgeTime )) break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//... 2. Base Pumping 

	//...	2016.02.16 Abort Gas, Vacuum Control Added
	if ( RUN_FUNCTION_FREE ( ABORT_GAS , "" ) == SYS_ABORTED ) 
	{
		return SYS_ABORTED;
	}

	//... Initial MFC Set Point
	GasTarget[GasSelect] = -2; 	

	PumpTime = READ_ANALOG ( M_CAL_PUMP_TIME_SET , &CommStatus );
	if ( PumpTime > 0 )
	{

		if ( RUN_FUNCTION_FREE ( VACUUM_CONTROL , "FAST_ROUGH" ) == SYS_ABORTED )
		{
			return SYS_ABORTED;
		}

		if(!WAIT_SECONDS(ABORTGAS_WAIT_TIME)) _LOG("MFC Verification Aborted : %d", __LINE__);

		sprintf(Buffer, "MFC%02d_PUMP", GasSelect+1);
		nIdx = m_MFCValveSeq.FindStepIdx(Buffer);
		m_MFCValveSeq.m_adbMFCSet[nIdx][0] = Max;
		if(TRUE != m_MFCValveSeq.StartValveSeq(Buffer))
		{
			return SYS_ABORTED;
		}

		S_TIMER_READY ();
		while ( TRUE ) {
			if ( !WAIT_SECONDS ( 0.5 ) ) 
			{
				return SYS_ABORTED;
			}

			WRITE_ANALOG ( M_CAL_PUMP_ELAPSED , S_TIMER_ELAPSED () , &CommStatus );
			if ( S_TIMER_CHECK ( 1 , PumpTime )) break;
		}
		WRITE_ANALOG  ( MFC01StPtAO + GasSelect	, 0 , &CommStatus );
		_sleep ( 1000 );
	}

	//////////////////////////////////////////////////////////////////////////
	//... 3. Gas Flow with Purge Line for Calibration

    //... 2016.02.16 Delete SW IO control For Interlock Off
	//WRITE_DIGITAL ( INTLKS_CONTROL , 3 , &CommStatus );

	//... 2016.02.16 Add MFC Flow at Manual Select 
	if (nManualFlow != -1){
		GasSet = nManualFlow;
	}else{
		GasSet = m_Step_MFCSet[nMfcSetIdx];		
	}

	//...2012.08.17  Apply to Coversion Fact
	GasTarget[GasSelect] =  (int)(GasSet * Coversion);		

	printf("GasSelect : %d, nGasType : %d, GasTarget : %d\n", GasSelect, nGasType, GasTarget[GasSelect]);

	sprintf(Buffer, "MFC%02d_FLW_GAS%d", GasSelect+1, nGasType+1);
	nIdx = m_MFCValveSeq.FindStepIdx(Buffer);
	m_MFCValveSeq.m_adbMFCSet[nIdx][0] = GasTarget[GasSelect];	//2012.08.17 

	if(TRUE != m_MFCValveSeq.StartValveSeq(Buffer))
	{
		return SYS_ABORTED;
	}

	StableTime = READ_ANALOG ( M_CAL_STABLE_TIME_SET , &CommStatus );
	S_TIMER_READY ();
	while ( TRUE ) {
		if ( !WAIT_SECONDS ( 0.5 ) ) 
		{
			return SYS_ABORTED;
		}

		WRITE_ANALOG ( M_CAL_STABLE_ELAPSED , S_TIMER_ELAPSED () , &CommStatus );

		if ( S_TIMER_CHECK ( 1 ,  StableTime )) break;
	}

    //... Check Pressure Range After Complete Stable Sequence 
	StartPress = READ_ANALOG ( M_CAL_START_PRESS   , &CommStatus );
	if (  READ_ANALOG ( GaugeIo , &CommStatus ) > StartPress
			&& FALSE == m_bSimulMode) 
	{


		//...	2016.02.16 Abort Gas, Vacuum Control Added
		RUN_SET_FUNCTION ( ABORT_GAS  , "" );

		//[ 2009/10/28 MFC Verification Auto Scenario 개발 ( BY LJH )
		sprintf ( Buffer , "Start Pressure is High ( Set[%.3f], Actual[%.3f] )", StartPress , READ_ANALOG ( GaugeIo , &CommStatus ) );
		ALARM_POST_WithMsg(ALARM_MFC_VERIFY_FAIL, Buffer);
		//]
		return SYS_ABORTED;
	}

	if(TRUE != m_MFCValveSeq.StartValveSeq("PUMPCLOSE"))
	{
		return SYS_ABORTED;
	}
	//...

	//... Check Start Pressure
	printf(" StartPress Check\n");

	StartPress = READ_ANALOG ( M_CAL_START_PRESS , &CommStatus );

	if ( m_bSimulMode ) 
	{
		WRITE_ANALOG ( GaugeIo , 0.030 , &CommStatus );
	}

	while ( TRUE ) 
	{
		CurrentPress1 = READ_ANALOG ( GaugeIo , &CommStatus );

		if ( m_bSimulMode ) {
			WRITE_ANALOG ( GaugeIo , StartPress , &CommStatus );
		}

		if ( CurrentPress1 >= StartPress ) break;

		_sleep(20);
		if ( MANAGER_ABORT() )
		{
			WRITE_DIGITAL ( INTLKS_CONTROL , 0 , &CommStatus );
			return SYS_ABORTED;
		}
	}

	if ( CurrentPress1 > StartPress + m_dbPressCheckErr )    // PRESSURE_CHECK_ERROR = 0.3
	{
		CheckResult = FALSE;
	}
	//...

	//... Check End Pressure & Gas Flow Stop
	printf(" EndPress Check\n");
	
	ModeSelect = READ_DIGITAL ( M_CAL_MODE_SELECT , &CommStatus );
	TargetPress = READ_ANALOG ( M_CAL_TARGET_PRESS , &CommStatus );
	CheckTime = READ_ANALOG ( M_CAL_ELAPSE_TIME_SET , &CommStatus );

	ElapsedTime = 0;
	S_TIMER_READY ();
	while ( TRUE ) 
	{
		CurrentPress2 = READ_ANALOG ( GaugeIo , &CommStatus );
		
		if ( m_bSimulMode ) 
		{
			CurrentPress2 = ElapsedTime * 1.987;
			WRITE_ANALOG ( GaugeIo , CurrentPress2 , &CommStatus );
		}

		ElapsedTime = S_TIMER_ELAPSED ();
		WRITE_ANALOG ( M_CAL_ELAPSED_TIME , ElapsedTime , &CommStatus );

		if ( ModeSelect == 0 ) 
		{
			if ( CurrentPress2 >= TargetPress ) break;
		}
		else 
		{
			if ( S_TIMER_CHECK ( 1 , CheckTime ) ) break;
		}

		_sleep(20);
		if ( MANAGER_ABORT() )
		{
			WRITE_DIGITAL ( INTLKS_CONTROL , 0 , &CommStatus );
			return SYS_ABORTED;
		}
	}
	WRITE_ANALOG  ( M_CAL_FINAL_PRESS , CurrentPress2 , &CommStatus );
	
	//....

	if ( ModeSelect == 0 )
	{
		if ( CurrentPress2 > TargetPress + m_dbPressCheckErr )    // PRESSURE_CHECK_ERROR = 0.3
		{
			CheckResult = FALSE;
		}
	}
	else {
		if ( CurrentPress2 > m_dbEndPressCheck )    
		{
			CheckResult = FALSE;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//... 4. Calculation of Calibration Factor

	ChamberVol = m_dbChamberVol;

    //.... 2016.02.16
	if(READ_DIGITAL(HIGH_MFC_CAL,&CommStatus) != ON_1)
	{
	    ChamberTemp = ( READ_ANALOG ( Eu1_CurrTemp 	, &CommStatus ) +  READ_ANALOG ( Eu2_CurrTemp , &CommStatus ) ) / 2 ;
	}
	else
	{
		ChamberTemp = READ_ANALOG ( M_CAL_CHAMBER_TEMP_Ret , &CommStatus ); 
	}	

	Data1 = ( CurrentPress2 - CurrentPress1 ) * 1000 / ElapsedTime;
	
	Data2 = ( ChamberVol * 60 ) / ( 82.0544 * 760 * (ChamberTemp + 273)); //... convert C Temp to F temp
	
	TotalFlow = Data1 * Data2 * 22400;

	MfcFactor = TotalFlow / GasSet;

	printf ( "\n" );
	printf ( "Chamber Volume : [%.3f]\n" , ChamberVol );
	printf ( "Chamber Temp   : [%.3f]\n" , ChamberTemp );
	printf ( "Start Pressure : [%.3f]\n" , CurrentPress1 );
	printf ( "Final Pressure : [%.3f]\n" , CurrentPress2 );
	printf ( "Pressure Diff  : [%.3f]\n" , ( CurrentPress2 - CurrentPress1 ) );
	printf ( "Elapsed Time   : [%.3f]\n" , ElapsedTime );
	printf ( "GasSet Sccm    : [%.3f]\n" , GasSet );

	printf ( "[%.3f][%f] : Flow [%.3f] : Factor [%.3f]\n" , Data1 , Data2 , TotalFlow , MfcFactor );

	if ( CheckResult )
	{
		WRITE_ANALOG ( M_CAL_TOTAL_ACT_FLOW , TotalFlow , &CommStatus );
		WRITE_ANALOG ( M_CALI_MFC01_FACTOR + GasSelect , MfcFactor , &CommStatus );
	}
	else
	{
		WRITE_ANALOG ( M_CAL_TOTAL_ACT_FLOW			   , RESULT_ERROR_VALUE , &CommStatus );
		WRITE_ANALOG ( M_CALI_MFC01_FACTOR + GasSelect , RESULT_ERROR_VALUE , &CommStatus );
	}

	//... 2016.02.16
	Data1 = ( CurrentPress2 - CurrentPress1 ) * 1000 / ElapsedTime;
	Data2 = ( ChamberVol * 60 ) / ( 82.0544 * 760 * GasSet);
	dbTempRet = (Data1 * Data2 * 22400) - 273;
	printf ( "Chamber Temp_Ret   : [%.3f]\n" , dbTempRet );
	WRITE_ANALOG ( M_CAL_CHAMBER_TEMP_Ret , dbTempRet , &CommStatus );
    //...

	//////////////////////////////////////////////////////////////////////////
	//... 5. Chamber Standby
	if ( RUN_FUNCTION_FREE ( ABORT_GAS  , "" ) == SYS_ABORTED )
	{
		WRITE_DIGITAL ( INTLKS_CONTROL , 0 , &CommStatus );
		return SYS_ABORTED;
	}
	
	if ( RUN_FUNCTION_FREE ( VACUUM_CONTROL , "FAST_ROUGH"  ) == SYS_ABORTED ) {
		WRITE_DIGITAL ( INTLKS_CONTROL , 0 , &CommStatus );
		return SYS_ABORTED;
	}
    //...
	WRITE_DIGITAL ( INTLKS_CONTROL , 0 , &CommStatus );
	//...

	_sleep(2000);

	//////////////////////////////////////////////////////////////////////////

	return SYS_SUCCESS;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//... IO Check Thread For Manual MFC Cal 
void CMFCVerification::DoMonitor()
{
	int nIOStatus;
	int i;
	int szGas_Sel[MAX_MFC];
	int nChangeGas;

	for ( i = 0 ; i< MAX_MFC ; i++ ) 
	{
		WRITE_DIGITAL ( M_CAL_GAS00 + i , 0 , &nIOStatus ) ; // 0 : OFF / 1: ON
		szGas_Sel[i] = 0;

	}

	while(WaitSec(1))
	{
		if(READ_DIGITAL(M_CAL_AutoMan_SELECT,&nIOStatus) == Manual_1)
		{
			for ( i = 0 ; i< MAX_MFC ; i++ ) 
			{
				if ( szGas_Sel[i] != READ_DIGITAL ( M_CAL_GAS00 + i , &nIOStatus ) ) nChangeGas = i ;
			}
			
			for ( i = 0 ; i < MAX_MFC ; i++ )
			{
				if ( i == nChangeGas )
				{
					szGas_Sel[i] = READ_DIGITAL ( M_CAL_GAS00 + i , &nIOStatus ) ;
					continue ;
				}
				WRITE_DIGITAL ( M_CAL_GAS00 + i , 0 , &nIOStatus ) ; // 0 : OFF / 1: ON
				szGas_Sel[i] = 0 ;
			}
		}
		else //... Auto
		{
			Sleep(6000);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Module_Status CMFCVerification::MFC_VERIFY_LOGGING()
{
	int	CommStatus , i  ;
	int M_CAL_GAS_SEL , M_CAL_GAS_TYPE ;
	char M_CAL_GAS_TYPE_CHAR[4][256] = { "RealGas"	,  "Purge_Ar" , "N2" , "RealGasFinMax"} ;
	double M_CAL_MFC_SET[5] , M_CAL_MFC_ACT[5] , M_CAL_MFC_FACTOR[5] , M_CAL_FACTOR_AVG ;
	SYSTEMTIME st;

    _LOG("-------------------------------------------------------------------------------");

	for ( i=0 ; i<5 ; i++ )
	{
		M_CAL_MFC_SET[i] = 0 ;
		M_CAL_MFC_ACT[i] = 0 ;
		M_CAL_MFC_FACTOR[i] = 0 ;
	}

	M_CAL_GAS_SEL  = READ_DIGITAL ( M_CAL_GAS_SELECT_RESULT , &CommStatus  ) ;
	M_CAL_GAS_TYPE = READ_DIGITAL ( M_CAL_GasType_SELECT , &CommStatus ) ;

	for ( i = 0 ; i < 5 ; i ++ )
	{
		M_CAL_MFC_SET[i] = READ_ANALOG ( M_CAL_MFC_SET_1 + i , &CommStatus ) ;
		M_CAL_MFC_ACT[i] = READ_ANALOG ( M_CAL_MFC_ACT_FLOW_1 + i  , &CommStatus ) ;
		M_CAL_MFC_FACTOR[i] = READ_ANALOG ( M_CAL_MFC_FACTOR_1 + i , &CommStatus ) ;
	}
	
	M_CAL_FACTOR_AVG = READ_ANALOG ( M_CAL_MFC_FACTOR_AVG , &CommStatus ) ;

	GetLocalTime( &st );

	_LOG( "* DATE : %04d/%02d/%02d %02d:%02d:%02d\n" , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond );
	_LOG( "* SELECT GAS : MFC%d-%s ( GAS TYPE : %s )\n", M_CAL_GAS_SEL+1, m_MFC_List[M_CAL_GAS_SEL].szName ,  M_CAL_GAS_TYPE_CHAR [M_CAL_GAS_TYPE] ) ;
	_LOG( "* MFC_SET     VERIFIED_FLOW     CAL_FACTOR\n");
	for ( i = 0 ; i < 5 ; i ++ )
	{
		_LOG( "* %.2f     %.2f		%.2f\n" , M_CAL_MFC_SET[i] , M_CAL_MFC_ACT[i] , M_CAL_MFC_FACTOR[i]);
	}
	_LOG( "* MFC_VERIFIY CAL_FACTOR AVG : %.2f \n\n\n", M_CAL_FACTOR_AVG) ;

	WRITE_DIGITAL ( M_CAL_GasType00 + M_CAL_GAS_SEL , M_CAL_GAS_TYPE , &CommStatus ) ;

	for ( i = 0 ; i < 5 ; i ++ )
	{
		WRITE_ANALOG  ( M_CAL_MFC00_SET_1 + i + (M_CAL_GAS_SEL*16) , M_CAL_MFC_SET[i] , &CommStatus ) ;
		WRITE_ANALOG  ( M_CAL_MFC00_ACT_FLOW_1 + i + (M_CAL_GAS_SEL*16) , M_CAL_MFC_ACT[i] , &CommStatus ) ;
		WRITE_ANALOG  ( M_CAL_MFC00_FACTOR_1 + i + (M_CAL_GAS_SEL*16) , M_CAL_MFC_FACTOR[i] , &CommStatus ) ;
	}
	WRITE_ANALOG  ( M_CAL_MFC00_FACTOR_AVG + (M_CAL_GAS_SEL*16) , M_CAL_FACTOR_AVG , &CommStatus ) ;

    _LOG("-------------------------------------------------------------------------------");
	return SYS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CMFCVerification::ReadConfig()
{
	BOOL bRet = FALSE;
	FILE *fp;
	char szRead[256];
	char szItem[256];
	int nTitle;

	do {
		fp = fopen(MFCV_CFG_FILE_PATH, "rt");
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
		printf("-----> Config File Reading Failed : [%s]\n", MFCV_CFG_FILE_PATH);
	}
	else
	{	//... ParseChamberConfig
		printf("Vol[%.2f] PrsChkErr[%.2f] EndPrs[%.2f] Pump[%.2f] Purge[%.2f] Stable[%.2f] Alarm[%d]\n", 
			m_dbChamberVol, m_dbPressCheckErr, m_dbEndPressCheck, m_dbPumpTime, m_dbPurgeTime, m_dbStableTime,m_nALARM_MfcVerFail );
	}

	return bRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMFCVerification::ParseMFCList(char *szData)
{
	int nMFCNo, nMFCIdx;

	CTextParser txtP;
	txtP.SetText(szData);

	while(txtP.GetInt(&nMFCNo))
	{
		if(nMFCNo<1 || nMFCNo>MAX_MFC) continue;
		nMFCIdx = nMFCNo - 1;
		txtP.GetWord(m_MFC_List[nMFCIdx].szName);

		txtP.GetWord(m_MFC_List[nMFCIdx].szCalGasName[0]);
		txtP.GetDbFloat(&m_MFC_List[nMFCIdx].dbConvFactor[0]);

		txtP.GetWord(m_MFC_List[nMFCIdx].szCalGasName[1]);
		txtP.GetDbFloat(&m_MFC_List[nMFCIdx].dbConvFactor[1]);

		txtP.GetWord(m_MFC_List[nMFCIdx].szCalGasName[2]);
		txtP.GetDbFloat(&m_MFC_List[nMFCIdx].dbConvFactor[2]);

		txtP.GetWord(m_MFC_List[nMFCIdx].szCalGasName[3]);
		txtP.GetDbFloat(&m_MFC_List[nMFCIdx].dbConvFactor[3]);


		printf("MFC[%d] Name[%s] CalGas1[%s] Factor1[%.3f] CalGas2[%s] Factor2[%.3f] CalGas3[%s] Factor3[%.3f]\n", 
			nMFCIdx, m_MFC_List[nMFCIdx].szName, 
			m_MFC_List[nMFCIdx].szCalGasName[0], m_MFC_List[nMFCIdx].dbConvFactor[0], 
			m_MFC_List[nMFCIdx].szCalGasName[1], m_MFC_List[nMFCIdx].dbConvFactor[1], 
			m_MFC_List[nMFCIdx].szCalGasName[2], m_MFC_List[nMFCIdx].dbConvFactor[2]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMFCVerification::ParseChamberConfig(char *szData)
{
	CTextParser txtP;
	txtP.SetText(szData);
	txtP.GetWord();

	if(txtP.BeEqual("ChamberVol"))				txtP.GetDbFloat('=', &m_dbChamberVol);
	else if(txtP.BeEqual("PressCheckErr"))		txtP.GetDbFloat('=', &m_dbPressCheckErr);
	else if(txtP.BeEqual("EndPressCheck"))		txtP.GetDbFloat('=', &m_dbEndPressCheck);
	else if(txtP.BeEqual("PumpTime"))			txtP.GetDbFloat('=', &m_dbPumpTime);
	else if(txtP.BeEqual("PurgeTime"))			txtP.GetDbFloat('=', &m_dbPurgeTime);
	else if(txtP.BeEqual("StableTime"))			txtP.GetDbFloat('=', &m_dbStableTime);
	else if(txtP.BeEqual("ALARM_MfcVerFail"))	txtP.GetInt('=', &m_nALARM_MfcVerFail);
	else if(txtP.BeEqual("BaseCalMFCNo1"))		txtP.GetInt('=', &m_nBaseMFCNo1);
	else if(txtP.BeEqual("BaseCalMFCNo2"))		txtP.GetInt('=', &m_nBaseMFCNo2);
	else if(txtP.BeEqual("CompareTemp"))		txtP.GetDbFloat('=', &m_dbCompareTemp);
	printf("ChmVol[%.2f] PrsChkErr[%.2f] EndPrsChk[%.2f] PumpTm[%.2f] PurgTm[%.2f] StbTm[%.2f] Alm[%d] MFC[%d,%d] CTemp[%.2f]\n", 
		m_dbChamberVol, m_dbPressCheckErr, m_dbEndPressCheck, m_dbPumpTime, m_dbPurgeTime, m_dbStableTime,m_nALARM_MfcVerFail,m_nBaseMFCNo1,m_nBaseMFCNo2,m_dbCompareTemp);
}