#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
#include "IPS_Log.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define	MAX_CASSETTE	3
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { RUN };
enum { OFF , ON   };
enum { SIM , REAL };
enum { ABSENT , PRESENT };
enum { EMPTY  , FULL    };
enum { UNKNOWN1, CLAMP	, UNCLAMP  , ERROR1		};
enum { UNKNOWN2, IN1    , OUT1	   , ERROR2		};
enum { UNKNOWN3, UPPED  , DOWNED   , ERROR3		};
enum { NotUse , Enable  , Disable2 , DisableHW , EnablePM };
enum { Idle   , Running , Pausing  , Paused    , Aborting , Disable , Waiting , WaitHandOff };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
char *EVENT_STRING_FOUP_CLAMP			= "EVENT_ON LL_CLAMP_COMPLETE PORT%d";
char *EVENT_STRING_FOUP_UNCLAMP			= "EVENT_ON LL_UNCLAMP_COMPLETE PORT%d";
char *EVENT_STRING_FOUP_DOCK			= "EVENT_ON LL_DOCKING_COMPLETE PORT%d";
char *EVENT_STRING_FOUP_UNDOCK			= "EVENT_ON LL_UNDOCKING_COMPLETE PORT%d";
char *EVENT_STRING_DOOR_OPEN			= "EVENT_ON LL_OPEN_COMPLETE PORT%d";
char *EVENT_STRING_DOOR_CLOSE			= "EVENT_ON LL_CLOSE_COMPLETE PORT%d";
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL g_REFRESH_MESSAGE[3] = {FALSE, FALSE, FALSE};
BOOL SIMULATION_MODE	= FALSE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"			, _K_D_IO	,	IO_DRIVER_LOAD		,	0	} ,

	{ "e:SCHEDULER"					, _K_S_IO	,	SCHEDULER			,	0	} ,

	{ "CTC.MAIN_STATUS"				, _K_D_IO	,	MAIN_STATUS			,	0	} ,
	{ "CTC.MAIN_STATUS2"			, _K_D_IO	,	MAIN_STATUS2		,	0	} ,
	{ "CTC.MAIN_STATUS3"			, _K_D_IO	,	MAIN_STATUS3		,	0	} ,

	{ "CTC.MAIN_CONTROL"			, _K_D_IO	,	MAIN_CONTROL		,	0	} ,
	{ "CTC.MAIN_CONTROL2"			, _K_D_IO	,	MAIN_CONTROL2		,	0	} ,
	{ "CTC.MAIN_CONTROL3"			, _K_D_IO	,	MAIN_CONTROL3		,	0	} ,

	{ "CTC.FA_STATUS_MDL_CM1"		, _K_D_IO	,	MDL_STATUS			,	0	} ,
	{ "CTC.FA_STATUS_MDL_CM2"		, _K_D_IO	,	MDL_STATUS2			,	0	} ,
	{ "CTC.FA_STATUS_MDL_CM3"		, _K_D_IO	,	MDL_STATUS3			,	0	} ,

	{ "LPMA.Switch2LoadDI"			, _K_D_IO	,	CM1_LOAD_BUTTON		,	0	} ,
	{ "LPMB.Switch2LoadDI"			, _K_D_IO	,	CM2_LOAD_BUTTON		,	0	} ,
	{ "LPMC.Switch2LoadDI"			, _K_D_IO	,	CM3_LOAD_BUTTON		,	0	} ,

	{ "LPMA.Switch1UnloadDI"		, _K_D_IO	,	CM1_UNLOAD_BUTTON	,	0	} ,
	{ "LPMB.Switch1UnloadDI"		, _K_D_IO	,	CM2_UNLOAD_BUTTON	,	0	} ,
	{ "LPMC.Switch1UnloadDI"		, _K_D_IO	,	CM3_UNLOAD_BUTTON	,	0	} ,
	
	{ "LPMA.F_ProperlyPlaceDI"		, _K_D_IO	,	CM1_CASS_STATUS		,	0	} ,
	{ "LPMB.F_ProperlyPlaceDI"		, _K_D_IO	,	CM2_CASS_STATUS		,	0	} ,
	{ "LPMC.F_ProperlyPlaceDI"		, _K_D_IO	,	CM3_CASS_STATUS		,	0	} ,

	{ "LPMA.ClampUnclampXI"			, _K_D_IO	,	CM1_CLAMP_STATUS	,	0	} ,
	{ "LPMB.ClampUnclampXI"			, _K_D_IO	,	CM2_CLAMP_STATUS	,	0	} ,
	{ "LPMC.ClampUnclampXI"			, _K_D_IO	,	CM3_CLAMP_STATUS	,	0	} ,

	{ "LPMA.ShuttleInOutXI"			, _K_D_IO	,	CM1_DOCK_STATUS		,	0	} ,
	{ "LPMB.ShuttleInOutXI"			, _K_D_IO	,	CM2_DOCK_STATUS		,	0	} ,
	{ "LPMC.ShuttleInOutXI"			, _K_D_IO	,	CM3_DOCK_STATUS		,	0	} ,
	
	{ "LPMA.DoorUpDownXI"			, _K_D_IO	,	CM1_OPEN_STATUS		,	0	} ,
	{ "LPMB.DoorUpDownXI"			, _K_D_IO	,	CM2_OPEN_STATUS		,	0	} ,
	{ "LPMC.DoorUpDownXI"			, _K_D_IO	,	CM3_OPEN_STATUS		,	0	} ,

	{ "CM1.AlarmStatus"				, _K_D_IO	,	CM1_AARM_Status		,	0	} ,
	{ "CM2.AlarmStatus"				, _K_D_IO	,	CM2_AARM_Status		,	0	} ,
	{ "CM3.AlarmStatus"				, _K_D_IO	,	CM3_AARM_Status		,	0	} ,

	{ "LPMA.Lamp5OnDIO"				, _K_D_IO	,	CM1_ALARM_LAMP		,	0	} ,
	{ "LPMB.Lamp5OnDIO"				, _K_D_IO	,	CM2_ALARM_LAMP		,	0	} ,
	{ "LPMC.Lamp5OnDIO"				, _K_D_IO	,	CM3_ALARM_LAMP		,	0	} ,

	{ "CTC.FA_STATUS_AGV_CM1"		, _K_D_IO	,	CM1_AMHS_Status		,	0	} ,
	{ "CTC.FA_STATUS_AGV_CM2"		, _K_D_IO	,	CM2_AMHS_Status		,	0	} ,
	{ "CTC.FA_STATUS_AGV_CM3"		, _K_D_IO	,	CM3_AMHS_Status		,	0	} ,

	{ "LPMA.Lamp4OnDIO"				, _K_D_IO	,	CM1_AUTO_LAMP		,	0	} ,
	{ "LPMB.Lamp4OnDIO"				, _K_D_IO	,	CM2_AUTO_LAMP		,	0	} ,
	{ "LPMC.Lamp4OnDIO"				, _K_D_IO	,	CM3_AUTO_LAMP		,	0	} ,

	{ "GEM.CM1_LPReserveStatus"		, _K_D_IO	,	CM1_RESERVE_Status	,	0	} ,
	{ "GEM.CM2_LPReserveStatus"		, _K_D_IO	,	CM2_RESERVE_Status	,	0	} ,
	{ "GEM.CM3_LPReserveStatus"		, _K_D_IO	,	CM3_RESERVE_Status	,	0	} ,

	{ "LPMA.Lamp3OnDIO"				, _K_D_IO	,	CM1_RESERVE_LAMP	,	0	} ,
	{ "LPMB.Lamp3OnDIO"				, _K_D_IO	,	CM2_RESERVE_LAMP	,	0	} ,
	{ "LPMC.Lamp3OnDIO"				, _K_D_IO	,	CM3_RESERVE_LAMP	,	0	} ,

	{ "CM1.LOADING_SIGNAL"			, _K_D_IO	,	CM1_LOADING_SIGNAL	,	0	} ,
	{ "CM2.LOADING_SIGNAL"			, _K_D_IO	,	CM2_LOADING_SIGNAL	,	0	} ,
	{ "CM3.LOADING_SIGNAL"			, _K_D_IO	,	CM3_LOADING_SIGNAL	,	0	} ,

	{ "LPMA.Lamp1OnDIO"				, _K_D_IO	,	CM1_LOAD_LAMP		,	0	} ,
	{ "LPMB.Lamp1OnDIO"				, _K_D_IO	,	CM2_LOAD_LAMP		,	0	} ,
	{ "LPMC.Lamp1OnDIO"				, _K_D_IO	,	CM3_LOAD_LAMP		,	0	} ,

	{ "CM1.UNLOADING_SIGNAL"		, _K_D_IO	,	CM1_UNLOADING_SIGNAL,	0	} ,
	{ "CM2.UNLOADING_SIGNAL"		, _K_D_IO	,	CM2_UNLOADING_SIGNAL,	0	} ,
	{ "CM3.UNLOADING_SIGNAL"		, _K_D_IO	,	CM3_UNLOADING_SIGNAL,	0	} ,

	{ "LPMA.Lamp2OnDIO"				, _K_D_IO	,	CM1_UNLOAD_LAMP		,	0	} ,
	{ "LPMB.Lamp2OnDIO"				, _K_D_IO	,	CM2_UNLOAD_LAMP		,	0	} ,
	{ "LPMC.Lamp2OnDIO"				, _K_D_IO	,	CM3_UNLOAD_LAMP		,	0	} ,
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { THREAD_STOP, THREAD_RUN, THREAD_FAILED };
enum { eUnknown,	eClamp,	eUnclamp,	eError };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Clear_PortData( int Port ) {
}

void PortMonitor(int nPort, int *pThreadSts)
{
	char Buffer[256];
	int CommStatus , i, j , Count = 0 , Status, Status2;
	int PROCESSING = -1, PROCESS_LAMP = -1 , CASS_LAMP = -1;
	int CASS , CASS_OLD;
	int MDL , MDL_OLD;
	int MAINSTS , END_LAMP;
	int BTN_L , BTN_U;
	int BTN_L_OLD , BTN_U_OLD;
	int CLAMP_S , CLAMP_S_OLD;
	int DOCK_S , DOCK_S_OLD;
	int OPEN_S , OPEN_S_OLD;
	char szTitle[80], szMsg[80];
	int nClampSts = 0;

	char pszFtnName[256] = ""; // jelee added 2007.12.13
	i = nPort - 1;
	_sleep(5000);
	CLAMP_S_OLD = READ_DIGITAL( CM1_CLAMP_STATUS + i, &CommStatus );
	DOCK_S_OLD  = READ_DIGITAL( CM1_DOCK_STATUS + i, &CommStatus );
	OPEN_S_OLD  = READ_DIGITAL( CM1_OPEN_STATUS + i, &CommStatus );

	while ( *pThreadSts == THREAD_RUN ) {
		// data initial
		g_REFRESH_MESSAGE[i] = FALSE;
		//======================================================================================================
		END_LAMP = OFF; BTN_L_OLD = OFF; BTN_U_OLD = OFF;
		// port initial
		CASS = READ_DIGITAL( CM1_CASS_STATUS + i, &CommStatus );
		switch( READ_DIGITAL( MDL_STATUS  + i, &CommStatus ) ) {
		case Enable : case EnablePM :	MDL = MDL_OLD = Enable; break;
		default:						MDL = MDL_OLD = NotUse; break;
		}
		if ( MDL == Enable ) {

			LOG(pszFtnName, ELT_File, " [1] Data Initial Port Monitor LPM[%d]", i);// jelee added 2007.12.13
			sprintf(szTitle, "CM[%d] Initializing . . .", i+1);
			sprintf(szMsg, "Confirm that CM[%d] Cassette Will Be Unloaded From Now !", i+1);
			MessageBox(NULL, szMsg, szTitle, MB_OK | MB_SYSTEMMODAL | MB_ICONWARNING);
			if ( CASS == ABSENT ) {
				LOG(pszFtnName, ELT_File, " [2] Data Initial Port Monitor LPM[%d] => Load", i);// jelee added 2007.12.13
				sprintf( Buffer , "MAKE_LOAD_REQUEST_CF CM%d" , i + 1 );
				WRITE_STRING( SCHEDULER , Buffer , &CommStatus ); //최정호 과장 요청사항(2005.11.21)
				CASS_OLD = ABSENT;
			}
			else { // because unknown states.., foup present
				LOG(pszFtnName, ELT_File, " [3] Data Initial Port Monitor LPM[%d] => Unload", i);// jelee added 2007.12.13
				sprintf( Buffer , "MAKE_UNLOAD_REQUEST_F CM%d" , i + 1 );
			 	WRITE_STRING( SCHEDULER , Buffer , &CommStatus ); //최정호 과장 요청사항(2005.11.21)
				CASS_OLD = PRESENT;
			}
		}
		
		//---------------------------------------------------------------------------------------------------
		// initial waiting
		//---------------------------------------------------------------------------------------------------
		_sleep(10000);

		while ( !g_REFRESH_MESSAGE[i] ) {
			// if(nPort == 3) printf("-----> CM%d : Sleeping Started\n", nPort);
			_sleep( 500 );
			// if(nPort == 3) printf("-----> CM%d : Status Check Started\n", nPort);
			//======================================================================================================
			// status check
//			LOG(pszFtnName, ELT_File, " [4] Status Check Port Monitor LPM[%d]", i);// jelee added 2007.12.13
			MAINSTS = READ_DIGITAL( MAIN_STATUS + i , &CommStatus );
			//------------------------------------------------------
			switch( READ_DIGITAL( MDL_STATUS + i , &CommStatus ) ) {
			case Enable : case EnablePM :	MDL = Enable; break;
			default:						MDL = NotUse; break;
			}
			//... 2015.09.06
			nClampSts = READ_DIGITAL( CM1_CLAMP_STATUS + i, &CommStatus );
			if(nClampSts == eClamp)
			{
				if(ON != READ_DIGITAL(CM1_LOAD_BUTTON + i,&CommStatus)) WRITE_DIGITAL( CM1_LOAD_BUTTON    + i ,ON, &CommStatus );
				if(OFF != READ_DIGITAL(CM1_LOAD_BUTTON + i,&CommStatus))WRITE_DIGITAL( CM1_UNLOAD_BUTTON  + i ,OFF, &CommStatus );
			}
			else if(nClampSts == eUnclamp)
			{
				if(OFF != READ_DIGITAL(CM1_LOAD_BUTTON + i,&CommStatus)) WRITE_DIGITAL( CM1_LOAD_BUTTON    + i ,OFF, &CommStatus );
				if(ON != READ_DIGITAL(CM1_UNLOAD_BUTTON + i,&CommStatus))WRITE_DIGITAL( CM1_UNLOAD_BUTTON  + i ,ON, &CommStatus );
			}
			//... 
			CASS  = READ_DIGITAL( CM1_CASS_STATUS	+ i , &CommStatus );
			BTN_U = READ_DIGITAL( CM1_UNLOAD_BUTTON  + i , &CommStatus );
			BTN_L = READ_DIGITAL( CM1_LOAD_BUTTON    + i , &CommStatus );
			//======================================================================================================
			PROCESS_LAMP = OFF;
			j = 0;
			PROCESSING = READ_DIGITAL( MAIN_CONTROL + i , &CommStatus );
			switch( PROCESSING ) {
			case Running: case Pausing:	
			case Paused: case Aborting:
				PROCESS_LAMP = ON; // At Least 1 port running
				break;
			default:
				break;
			}
			//======================================================================================================
			if ( CASS == ABSENT ) j = EMPTY;
			if ( j == EMPTY ) { // At Least 1 Load Port Empty..
				if ( CASS_LAMP != ON ) {
					CASS_LAMP = ON;
					// signal lamp action
				}
			}
			else { // Without any Load Ports Empty( Load Port Full )
				if ( CASS_LAMP != OFF ) {
					CASS_LAMP = OFF;
					// signal lamp action
				}
			}
			//======================================================================================================
			switch( MAINSTS ) { // CTC.MAIN_STATUS1,2
			case 18 : case 19 : // Before Unload Action..
				if ( ( END_LAMP == OFF ) && ( CASS == PRESENT ) ) { // if CM1 || CM2 => Cassette Present, End Buzzer On..==> Check More Status
					END_LAMP = ON;
					// signal lamp action
				}
				break;
			default :
				END_LAMP = OFF;
				break;
			}
			//======================================================================================================
			if ( ( END_LAMP == ON ) && ( CASS == ABSENT ) ) { // if Cass Unloaded..
				// signal lamp action
			}
			//======================================================================================================
			// Start Carrier Monitoring
			// if(nPort == 3) printf("-----> CM%d : Carrier Monitoring Started\n", nPort);
			if ( CASS != CASS_OLD ) {
				CASS_OLD = CASS;
				if ( CASS == ABSENT ) Clear_PortData( i );
			}
			if ( MDL == Enable ) { // Module Status Enable
				if ( CASS == PRESENT ) { // Cassette Present
					if ( MDL_OLD == NotUse ) { // Unknown states..
						LOG(pszFtnName, ELT_File, " [5] Unknown Status Port Monitor LPM[%d]", i);// jelee added 2007.12.13
						sprintf( Buffer , "MAKE_UNLOAD_REQUEST_F CM%d" , i + 1 );
						WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
					}
					else { 
						if ( BTN_L == ON ) {
							if ( BTN_L_OLD == OFF ) {
								BTN_L_OLD = ON;
								switch( MAINSTS ) {
								case 0 : // Ready
								case 1 : // After Load Request
									break;
								}
							}
						}
						else {
							BTN_L_OLD = OFF;
						}

						if ( BTN_U == ON ) { // UnLoad Button Pressed
							if ( BTN_U_OLD == OFF ) {
								BTN_U_OLD = ON;
								switch( MAINSTS ) {
//									case 0 : // Ready
								case 2 : // LOAD COMPLETE and WAITING 
//									case 3 : // After Loading
//									case 5 : case 6 : // After Mapping
//									case 15 : case 16 : case 17 : // After Running
//									case 20 : case 21 : case 22 : // After Unloading
									LOG(pszFtnName, ELT_File, " [6] Load Complete and Waiting Port Monitor LPM[%d]", i);// jelee added 2007.12.13
									sprintf( Buffer , "MAKE_UNLOAD_REQUEST_F CM%d" , i + 1 );
									WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
									break;
								case 18 : case 19 : // Unloading
									break;
								default : // 1(Loading) , 4(Mapping) , 7-14(Running)
									break;
								}
							}
						}
						else { // UnLoad Button Not Pressed
							BTN_U_OLD = OFF;
							/// when abort, reunload request
							if ( MDL == Enable && MDL_OLD == Enable && BTN_U == 0 && BTN_U_OLD == 0 ) {
								switch( MAINSTS ) {
								case 3 : // When Loading, case Load Request Abort( After Foup Present )
								case 21 : case 22 : // WAITING UNLOAD  ( 21 ==> Unload Reject )
									LOG(pszFtnName, ELT_File, " [7] Waiting Unload Port Monitor LPM[%d]", i);// jelee added 2007.12.13
									sprintf( Buffer , "MAKE_UNLOAD_REQUEST_F CM%d" , i + 1 );
									WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
									break;
								}
							}
						}
					}
				} // End of Cassette Present
				else { // MDL == ENABLE , CASS == ABSENT
					if ( MDL_OLD == NotUse ) {
						LOG(pszFtnName, ELT_File, " [8] MDL == Enalb, CASS = Absent Port Monitor LPM[%d]", i);// jelee added 2007.12.13
						sprintf( Buffer , "MAKE_LOAD_REQUEST_CF CM%d" , i + 1 );
						WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
					}
					/// when abort, reload request
					else { // MDL == ENABLE , CASS = ABSENT 
						if ( MDL == Enable && MDL_OLD == Enable && BTN_L == 0 && BTN_L_OLD == 0 ) {
							switch( MAINSTS ) {
							case 2 : // LOAD COMPLETE AND WAITING
							case 3 : // ABORTED WHEN LOADING..
							case 21 : case 22 : // ABORTED WHEN UNLOADING..( After FOUP Remove )
								LOG(pszFtnName, ELT_File, " [9] Aborted When Unloading Port Monitor LPM[%d]", i);// jelee added 2007.12.13
								sprintf( Buffer , "MAKE_LOAD_REQUEST_CF CM%d" , i + 1 );
								WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
								break;
							}
						}
					}
				}
				MDL_OLD = Enable;
			} // End of Module Status Enable
			else { // MDL == DISABLE.
				MDL_OLD = NotUse;
				//
				if ( CASS == ON ) {
					if ( BTN_U == ON ) {
						if ( BTN_U_OLD == OFF ) {
							BTN_U_OLD = ON;
							switch( MAINSTS ) {
							case 0 : // Ready
							case 2 : case 3 : // After Loading
							case 5 : case 6 : // After Mapping
							case 15 : case 16 : case 17 : // After Running
//									sprintf( Buffer , "MAKE_UNLOAD_REQUEST_F CM%d|NOLOOP" , i + 1 );
//									WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
								break;
							case 18 : case 19 : // Unloading
								break;
							default : // 1(Loading) , 4(Mapping) , 7-14(Running)
								break;
							}
						}
					}
					else { // MDL == DISABLE , CASS == ABSENT..
						BTN_U_OLD = 0;
						/// when abort, reunload request
						if ( MDL == NotUse && MDL_OLD == NotUse && BTN_U == 0 && BTN_U_OLD == 0 ) {
							switch( MAINSTS ) {
							case 21: case 22 : // WAITING UNLOAD..
								LOG(pszFtnName, ELT_File, " [10] Waiting Unload Port Monitor LPM[%d]", i);// jelee added 2007.12.13
								sprintf( Buffer , "MAKE_UNLOAD_REQUEST_F CM%d" , i + 1 );
								WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
								break;
							}
						}
					}
				}
			}
			// End Carrier Monitoring
			//====================================================================================
			// if(nPort == 3) printf("-----> CM%d : Status Update Started\n", nPort);
			CLAMP_S = READ_DIGITAL( CM1_CLAMP_STATUS + i , &CommStatus );
			if ( CLAMP_S != CLAMP_S_OLD ) {
				if		( CLAMP_S == CLAMP		) {
					sprintf( Buffer , EVENT_STRING_FOUP_CLAMP , i+1 );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				}
				else if ( CLAMP_S == UNCLAMP		) {
					sprintf( Buffer , EVENT_STRING_FOUP_UNCLAMP , i+1 );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				}
				CLAMP_S_OLD = CLAMP_S;
			}
			DOCK_S  = READ_DIGITAL( CM1_DOCK_STATUS  + i , &CommStatus );
			if ( DOCK_S != DOCK_S_OLD ) {
				if		( DOCK_S == IN1		) {
					sprintf( Buffer , EVENT_STRING_FOUP_DOCK , i+1 );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				}
				else if ( DOCK_S == OUT1		) {
					sprintf( Buffer , EVENT_STRING_FOUP_UNDOCK , i+1 );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				}
				DOCK_S_OLD = DOCK_S;
			}
			OPEN_S  = READ_DIGITAL( CM1_OPEN_STATUS  + i , &CommStatus );
			if ( OPEN_S != OPEN_S_OLD ) {
				if		( OPEN_S == DOWNED		) {
					sprintf( Buffer , EVENT_STRING_DOOR_OPEN , i+1 );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				}
				else if ( OPEN_S == UPPED		) {
					sprintf( Buffer , EVENT_STRING_DOOR_CLOSE , i+1 );
					WRITE_STRING( SCHEDULER , Buffer , &CommStatus );
				}
				OPEN_S_OLD = OPEN_S;
			}
			Status = READ_DIGITAL( CM1_AARM_Status + i , &CommStatus ); // 0 = absent, 1 = present
			if( READ_DIGITAL(CM1_ALARM_LAMP + i, &CommStatus) != Status)
				WRITE_DIGITAL( CM1_ALARM_LAMP + i , Status , &CommStatus );

			Status = READ_DIGITAL( CM1_AMHS_Status + i , &CommStatus ); // 0 = manual, 1 = auto
			if( READ_DIGITAL(CM1_AUTO_LAMP + i, &CommStatus) != Status)
				WRITE_DIGITAL( CM1_AUTO_LAMP + i , Status , &CommStatus );
	
			Status = READ_DIGITAL( CM1_RESERVE_Status + i , &CommStatus ); // 0 = not reserve, 1 = reserve
			if( READ_DIGITAL(CM1_RESERVE_LAMP + i, &CommStatus) != Status)
				WRITE_DIGITAL( CM1_RESERVE_LAMP + i , Status , &CommStatus );

// 2006.07.04 jelee delete start
/*			Status = READ_DIGITAL( CM1_LOADING_SIGNAL + i , &CommStatus ); // 0 = not reserve, 1 = reserve
			if ( Status > 0 )
			{
				if( READ_DIGITAL(CM1_LOAD_LAMP + i, &CommStatus) != ON)
				WRITE_DIGITAL( CM1_LOAD_LAMP + i , ON , &CommStatus );
			}
			else
			{
			    if( READ_DIGITAL(CM1_LOAD_LAMP + i, &CommStatus) != OFF)
			    WRITE_DIGITAL( CM1_LOAD_LAMP + i , OFF , &CommStatus );
			}

			Status = READ_DIGITAL( CM1_UNLOADING_SIGNAL + i , &CommStatus ); // 0 = not reserve, 1 = reserve
			if ( Status > 0 )
			{
				if( READ_DIGITAL(CM1_UNLOAD_LAMP + i, &CommStatus) != ON)
					WRITE_DIGITAL( CM1_UNLOAD_LAMP + i , ON , &CommStatus );
			}
			else
			{
				if( READ_DIGITAL(CM1_UNLOAD_LAMP + i, &CommStatus) != OFF)
					WRITE_DIGITAL( CM1_UNLOAD_LAMP + i , OFF , &CommStatus );
			}
*/ // 2006.07.04 jelee delete end.

// 2006.07.04 jelee add start.
			Status = READ_DIGITAL( CM1_LOADING_SIGNAL + i , &CommStatus ); // 0 = not reserve, 1 = reserve
			Status2 = READ_DIGITAL( CM1_AMHS_Status + i , &CommStatus ); // 0 = manual, 1 = auto
			if ( Status > 0 )
			{
				if( Status2 < 1 )
				{
					if( READ_DIGITAL(CM1_LOAD_LAMP + i, &CommStatus) != ON)
					WRITE_DIGITAL( CM1_LOAD_LAMP + i , ON , &CommStatus );
				}
				else
				{
					if( READ_DIGITAL(CM1_LOAD_LAMP + i, &CommStatus) != OFF)
					WRITE_DIGITAL( CM1_LOAD_LAMP + i , OFF , &CommStatus );
				}
			}
			else
			{
				if( Status2 < 1 )
				{
					if( READ_DIGITAL(CM1_LOAD_LAMP + i, &CommStatus) != OFF)
					WRITE_DIGITAL( CM1_LOAD_LAMP + i , OFF , &CommStatus );
				}
				else
				{
					if( READ_DIGITAL(CM1_LOAD_LAMP + i, &CommStatus) != OFF)
					WRITE_DIGITAL( CM1_LOAD_LAMP + i , OFF , &CommStatus );
				}
			}

			Status = READ_DIGITAL( CM1_UNLOADING_SIGNAL + i , &CommStatus ); // 0 = not reserve, 1 = reserve
			Status2 = READ_DIGITAL( CM1_AMHS_Status + i , &CommStatus ); // 0 = manual, 1 = auto
			if ( Status > 0 )
			{
				if( Status2 < 1 )
				{
					if( READ_DIGITAL(CM1_UNLOAD_LAMP + i, &CommStatus) != ON)
					WRITE_DIGITAL( CM1_UNLOAD_LAMP + i , ON , &CommStatus );
				}
				else
				{
					if( READ_DIGITAL(CM1_UNLOAD_LAMP + i, &CommStatus) != OFF)
					WRITE_DIGITAL( CM1_UNLOAD_LAMP + i , OFF , &CommStatus );
				}				
			}
			else
			{
				if( Status2 < 1 )
				{
					if( READ_DIGITAL(CM1_UNLOAD_LAMP + i, &CommStatus) != OFF)
					WRITE_DIGITAL( CM1_UNLOAD_LAMP + i , OFF , &CommStatus );
				}
				else
				{
					if( READ_DIGITAL(CM1_UNLOAD_LAMP + i, &CommStatus) != OFF)
					WRITE_DIGITAL( CM1_UNLOAD_LAMP + i , OFF , &CommStatus );
				}				
			}
// 2006.07.04 jelee add end.
			// if(nPort == 3) printf("-----> CM%d : Status Update Finished\n", nPort);
		}
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

int g_nPort1_Thread = THREAD_STOP;
int g_nPort2_Thread = THREAD_STOP;
int g_nPort3_Thread = THREAD_STOP;

void Port1_Thread()
{
	PortMonitor(1, &g_nPort1_Thread);
	g_nPort1_Thread = THREAD_STOP;
	_endthread();
}

void Port2_Thread()
{
	PortMonitor(2, &g_nPort2_Thread);
	g_nPort2_Thread = THREAD_STOP;
	_endthread();
}

void Port3_Thread()
{
	PortMonitor(3, &g_nPort3_Thread);
	g_nPort3_Thread = THREAD_STOP;
	_endthread();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {

	if(g_nPort1_Thread != THREAD_RUN) {
		g_nPort1_Thread = THREAD_RUN;
		_beginthread((void*) Port1_Thread, 0, NULL);
	}
	if(g_nPort2_Thread != THREAD_RUN) {
		g_nPort2_Thread = THREAD_RUN;
		_beginthread((void*) Port2_Thread, 0, NULL);
	}
	if(g_nPort3_Thread != THREAD_RUN) {
		g_nPort3_Thread = THREAD_RUN;
		_beginthread((void*) Port3_Thread, 0, NULL);
	}
	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Event_Message_Received() {
	g_REFRESH_MESSAGE[0] = TRUE;
	g_REFRESH_MESSAGE[1] = TRUE;
	g_REFRESH_MESSAGE[2] = TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_DRIVER_LOAD , &CommStatus ) == SIM ) SIMULATION_MODE = TRUE;	
	else SIMULATION_MODE = FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
