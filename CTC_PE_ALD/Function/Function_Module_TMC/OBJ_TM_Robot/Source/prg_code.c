#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"
#include "TextLogDll.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { ROBOT_SYNCH_READY , ROBOT_SYNCH_ROTATE_START , ROBOT_SYNCH_EXTEND_START , ROBOT_SYNCH_UPDATE , ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RESET_REVERSE , ROBOT_SYNCH_RESET_ONLY , ROBOT_SYNCH_SUCCESS };

//[ 2009/06/01 AWS Datalogging시 어느 Chamber에서 Pick한 Wafer인지 Logging추가 ( BY LJH )
enum	{ None , PMA , PMB	, PMC , BM1	, BM2 } ;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "TMC_RBLOW_CONTROL"		, _K_F_IO	,	RBLOW_CONTROL		,	0	} ,
	{ "TMC_WFRSNS_CONTROL"		, _K_F_IO	,	WFRSNS_CONTROL		,	0	} ,
	{ "TM.RB_SYNCH"				, _K_D_IO	,	ROBOT_SYNCH			,	0	} ,
	{ "TM.Wafer_Result"			, _K_D_IO	,	TM_Wafer_Result		,	0	} ,

	//[ 2009/06/01 AWS Datalogging시 어느 Chamber에서 Pick한 Wafer인지 Logging추가 ( BY LJH )
	{ "TM_Pick_U_Arm_Station"	, _K_D_IO	,	TM_Pick_U_Arm_Station	,	0	} ,
	{ "TM_Pick_L_Arm_Station"	, _K_D_IO	,	TM_Pick_L_Arm_Station	,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PickRobot( int ALStation , char *StationStr, char *St_Str , BOOL TR , BOOL SINGLE , int nSide) {
	char Str_Buffer[255];
	int Result, CommStatus ;
	if ( ALStation == 0 ) {
		if( !TR ) {
			//mgsong remark
			/*
			sprintf( Str_Buffer , "ROTATE %s" , St_Str );
			if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) {
				_sleep(250);
				return SYS_ABORTED;
			}*/

			sprintf( Str_Buffer , "ROTPIC %s" , St_Str );			
			if (RUN_FUNCTION (RBLOW_CONTROL  , Str_Buffer) == SYS_ABORTED ) {
				_sleep(250);
				return SYS_ABORTED;
			}

			//2010.10.03
			if (TRUE == SINGLE) {
				//2014.05.19
				//Single Mode가 추가 됨
				if (0 == nSide) {
					sprintf( Str_Buffer , "ABSENT_PICK SINGLE %s" , St_Str);
				}
				else {
					sprintf( Str_Buffer , "ABSENT_PICK SINGLE2 %s" , St_Str);
				}
				//2014.05.19

			}
			else
				sprintf( Str_Buffer , "ABSENT_PICK DOUBLE %s" , St_Str);
			//2010.10.03

			if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer) == SYS_ABORTED ) {
				_sleep(250);
				return SYS_ABORTED;
			}
		}
	}
	//
	//[ 2009/06/01 AWS Datalogging시 어느 Chamber에서 Pick한 Wafer인지 Logging추가 ( BY LJH )
	if ( STRCMP_L( StationStr , "A_PM1" ) || STRCMP_L( StationStr , "B_PM1" ) ) {
		if ( STRCMP_L( StationStr , "A_PM1" )) {
			WRITE_DIGITAL ( TM_Pick_U_Arm_Station , PMA , &CommStatus ) ;
		}else {
			WRITE_DIGITAL ( TM_Pick_L_Arm_Station , PMA , &CommStatus ) ;
		}
	}
	else if ( STRCMP_L( StationStr , "A_PM2" ) || STRCMP_L( StationStr , "B_PM2" ) ) {
		if ( STRCMP_L( StationStr , "A_PM2" )) {
			WRITE_DIGITAL ( TM_Pick_U_Arm_Station , PMB , &CommStatus ) ;
		}else {
			WRITE_DIGITAL ( TM_Pick_L_Arm_Station , PMB , &CommStatus ) ;
		}
	}
	else if ( STRCMP_L( StationStr , "A_PM3" ) || STRCMP_L( StationStr , "B_PM3" ) ) {
		if ( STRCMP_L( StationStr , "A_PM3" )) {
			WRITE_DIGITAL ( TM_Pick_U_Arm_Station , PMC , &CommStatus ) ;
		}else {
			WRITE_DIGITAL ( TM_Pick_L_Arm_Station , PMC , &CommStatus ) ;
		}
	}
	else if ( STRCMP_L( StationStr , "A_BM1" ) || STRCMP_L( StationStr , "B_BM1") ) {
		if ( STRCMP_L( StationStr , "A_BM1" )) {
			WRITE_DIGITAL ( TM_Pick_U_Arm_Station , BM1 , &CommStatus ) ;
		}else {
			WRITE_DIGITAL ( TM_Pick_L_Arm_Station , BM1 , &CommStatus ) ;
		}
	}
	else if ( STRCMP_L( StationStr , "A_BM2" ) || STRCMP_L( StationStr , "B_BM2") ) {
		if ( STRCMP_L( StationStr , "A_BM2" )) {
			WRITE_DIGITAL ( TM_Pick_U_Arm_Station , BM2 , &CommStatus ) ;
		}else {
			WRITE_DIGITAL ( TM_Pick_L_Arm_Station , BM2 , &CommStatus ) ;
		}
	}
	//]

	if (TR)
		sprintf( Str_Buffer , "PICK %s TRANSFER2" , St_Str );
	else {
		//2010.10.03
		if (TRUE == SINGLE) {
			//2014.05.19
			//Single Mode가 추가 됨
			if (0 == nSide) {
				sprintf( Str_Buffer , "PICK %s SINGLE" , St_Str );
				printf("^&*^%$# Pick Handling Single Wafer \n");
			}
			else {
				sprintf( Str_Buffer , "PICK %s SINGLE2" , St_Str );
				printf("^&*^%$# Pick Handling Single2 Wafer \n");
			}
			//2014.05.19
		}
		else {
			sprintf( Str_Buffer , "PICK %s" , St_Str );
		}
		//2010.10.03		
	}

	Result = RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer );

	if ( Result == SYS_ABORTED ) {
		_sleep(250);
		return SYS_ABORTED;
	}
	else {
		if ( TR ) {
			sprintf( Str_Buffer , "PRESENT_PICK %s 0 TRANSFER2" , St_Str );
			if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) {
				_sleep(250);
				return SYS_ABORTED;
			}
		}
		else {
			//2010.10.03
			if (TRUE == SINGLE) {
				//2014.05.19
				//Single Mode가 추가 됨
				if (0 == nSide) {
					sprintf( Str_Buffer , "PRESENT_PICK SINGLE %s 0 TRANSFER" , St_Str );
					printf("^&*^%$# Pick Transfer Handling Single Wafer \n");
				}
				else {
					sprintf( Str_Buffer , "PRESENT_PICK SINGLE2 %s 0 TRANSFER" , St_Str );
					printf("^&*^%$# Pick Transfer Handling Single2 Wafer \n");
				}
				//2014.05.19
			}
			else
				sprintf( Str_Buffer , "PRESENT_PICK DOUBLE %s 0 TRANSFER" , St_Str );
			//2010.10.03

			if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) {
				_sleep(250);
				return SYS_ABORTED;
			}
		}
	}
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PlaceRobot( int ALStation , char *St_Str , BOOL TR , BOOL SINGLE , int nSide) {
	char Str_Buffer[255];
	int Result /*, CommStatus*/;
	if ( ALStation == 0 ) {
		if( !TR ) {
			//mgsong remark
			/*
			sprintf( Str_Buffer , "ROTATE %s" , St_Str );
			if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) {
				_sleep(250);
				return SYS_ABORTED;
			}*/

			sprintf( Str_Buffer , "ROTPLC %s" , St_Str );
			if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) {
				_sleep(250);
				return SYS_ABORTED;
			}

			//2010.10.03
			if (TRUE == SINGLE) {
				//2014.05.19
				//Single Mode가 추가 됨
				if (0 == nSide) {
					sprintf( Str_Buffer , "PRESENT_PLACE SINGLE %s" , St_Str );
				}
				else {
					sprintf( Str_Buffer , "PRESENT_PLACE SINGLE2 %s" , St_Str );
				}
				//2014.05.19
			}
			else
				sprintf( Str_Buffer , "PRESENT_PLACE DOUBLE %s" , St_Str );
			//2010.10.03

			
			if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) {
				_sleep(250);
				return SYS_ABORTED;
			}
		}
	}
	if ( TR )
		sprintf( Str_Buffer , "PLACE %s TRANSFER2" , St_Str );
	else {
		//2010.10.03
		if (TRUE == SINGLE) {
			//2014.05.19
			//Single Mode가 추가 됨
			if (0 == nSide) {
				sprintf( Str_Buffer , "PLACE %s SINGLE" , St_Str );
				printf("^&*^%$# Place Handling Single Wafer \n");
			}
			else {
				sprintf( Str_Buffer , "PLACE %s SINGLE2" , St_Str );
				printf("^&*^%$# Place Handling Single Wafer \n");
			}
			//2014.05.19
		}
		else
			sprintf( Str_Buffer , "PLACE %s" , St_Str );
		//2010.10.03
	}
	Result = RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer );
	if ( Result == SYS_ABORTED ) {
		_sleep(250);
		return SYS_ABORTED;
	}
	else {
		if ( TR ) {
			sprintf( Str_Buffer , "ABSENT_PLACE %s 0 TRANSFER2" , St_Str );
			if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) {
				_sleep(250);
				return SYS_ABORTED;
			}
		}
		else {	
			//2010.10.03
			if (TRUE == SINGLE) {
				//2014.05.19
				//Single Mode가 추가 됨
				if (0 == nSide) {
					sprintf( Str_Buffer , "ABSENT_PLACE SINGLE %s 0 TRANSFER" , St_Str );
					printf("^&*^%$# Place Transfer Handling Single Wafer \n");
				}
				else {
					sprintf( Str_Buffer , "ABSENT_PLACE SINGLE2 %s 0 TRANSFER" , St_Str );
					printf("^&*^%$# Place Transfer Handling Single2 Wafer \n");
				}
			}
			else
				sprintf( Str_Buffer , "ABSENT_PLACE DOUBLE %s 0 TRANSFER" , St_Str );
			//2010.10.03

			if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) {
				_sleep(250);
				return SYS_ABORTED;
			}
		}
	}
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status ExtPicRobot( int ALStation , char *St_Str ) {
	char Str_Buffer[255];
	if ( ALStation == 0 ) {
		sprintf( Str_Buffer , "ROTATE %s" , St_Str );
		if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
		sprintf( Str_Buffer , "ABSENT_PICK %s" , St_Str );
		if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	}
	sprintf( Str_Buffer , "EXTEND %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RetPicRobot( int ALStation , char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "RETRACT %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( ALStation == 0 ) {
		sprintf( Str_Buffer , "PRESENT_PICK %s" , St_Str );
		if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	}
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status ExtPlcRobot( int ALStation , char *St_Str ) {
	char Str_Buffer[255];
	if ( ALStation == 0 ) {
		sprintf( Str_Buffer , "ROTATE %s" , St_Str );
		if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
		sprintf( Str_Buffer , "PRESENT_PLACE %s" , St_Str );
		if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	}
	sprintf( Str_Buffer , "EXTEND %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RetPlcRobot( int ALStation , char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "RETRACT %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	if ( ALStation == 0 ) {
		sprintf( Str_Buffer , "ABSENT_PLACE %s" , St_Str );
		if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	}
	return SYS_SUCCESS;
}
//
//
//
//------------------------------------------------------------------------------------------
Module_Status RotPicRobot( int ALStation , char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "ROTPIC %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	//mgsong remark
	/*if ( ALStation == 0 ) {
		sprintf( Str_Buffer , "ABSENT_PICK %s" , St_Str );
		if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	}*/
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RotPlcRobot( int ALStation , char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "ROTPLC %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	//mgsong remark
	/*if ( ALStation == 0 ) {
		sprintf( Str_Buffer , "PRESENT_PLACE %s" , St_Str );
		if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	}*/
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RetractRobot( int ALStation , char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "RETRACT %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
Module_Status ExtendRobot( int ALStation , char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "EXTEND %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
Module_Status RotateRobot( int ALStation , char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "ROTATE %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status CheckLoadRobot( int ALStation , char *St_Str ) {
	char Str_Buffer[255];
	if ( ALStation == 0 ) {
		sprintf( Str_Buffer , "CHECKLOAD %s" , St_Str );
		if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
		sprintf( Str_Buffer , "PRESENT_PICK %s 1 TRANSFER2" , St_Str );
		if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	}
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
//
//
Module_Status Program_Main() {
	BOOL TR       = FALSE;
	BOOL bSingle  = FALSE;
	int nWaferID  = 0;
	int nSingleSide = 0;
	int ALStation = 0;
	char StationStr[31] , Imsi[31] , Imsi2[31] , Imsi3[31] , RunStr[31] , WaferID[31] , Else[31];
	Module_Status msRet = SYS_SUCCESS;

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , Imsi , 30 ); //PICK,PLACE
	STR_SEPERATE( Imsi , StationStr , Imsi2 , 30 );                //A_BM1
	//
	STR_SEPERATE( Imsi2 , WaferID , Imsi3 , 30 );                  //Slot
	if ( STRCMP_L( WaferID , "TRANSFER2" ) ) TR = TRUE;
	STR_SEPERATE( Imsi3  , Else , Imsi2 , 30 );                    //
	if ( STRCMP_L( Else , "TRANSFER2" ) ) TR = TRUE;
	if ( STRCMP_L( Imsi2 , "TRANSFER2" ) ) TR = TRUE;

	if      ( STRCMP_L( StationStr , "S_AL"  ) ) ALStation = 1;
	else if ( STRCMP_L( StationStr , "S_IC"  ) ) ALStation = 2;
	else if ( STRCMP_L( StationStr , "T_AL"  ) ) ALStation = 1;
	else if ( STRCMP_L( StationStr , "T_IC"  ) ) ALStation = 2;
	else if ( STRCMP_L( StationStr , "A_AL"  ) ) ALStation = 1;
	else if ( STRCMP_L( StationStr , "A_IC"  ) ) ALStation = 2;
	else if ( STRCMP_L( StationStr , "B_AL"  ) ) ALStation = 1;
	else if ( STRCMP_L( StationStr , "B_IC"  ) ) ALStation = 2;
	else if ( STRCMP_L( StationStr , "AL"  ) ) ALStation = 1;
	else if ( STRCMP_L( StationStr , "IC"  ) ) ALStation = 2;
	else if ( STRCMP_L( StationStr , "AL" ) ) ALStation = 1;
	else if ( STRCMP_L( StationStr , "IC" ) ) ALStation = 2;

	_LOG("-----> %s %s Started in TM Robot Func", StationStr, RunStr);

	//2010.10.03
	nWaferID = atoi(WaferID);
	//VTM Slot Comes as BM or PM Slot
	//Double Wafer Slot coming as like 201, 403, 605 so single slot number will smaller than 4
	if (nWaferID >= 1 && nWaferID <= 4) bSingle = TRUE;
	else {
		//2014.05.19 Slot 2번으로만 Single Case 추가
		if (200 == nWaferID || 400 == nWaferID) {
			bSingle = TRUE;
			nSingleSide = 1;
		}
		//2014.05.19
	}
	//2010.10.03
	

	if      ( STRCMP_L( RunStr , "PICK"      ) ) msRet = ( PickRobot ( ALStation  , StationStr, Imsi , TR , bSingle , nSingleSide) );
	else if ( STRCMP_L( RunStr , "PLACE"     ) ) msRet = ( PlaceRobot( ALStation  , Imsi , TR , bSingle , nSingleSide) );
	//else if ( STRCMP_L( RunStr , "EXTPIC"    ) ) msRet = ( ExtPicRobot( ALStation , Imsi ) );
	//else if ( STRCMP_L( RunStr , "RETPIC"    ) ) msRet = ( RetPicRobot( ALStation , Imsi ) );
	//else if ( STRCMP_L( RunStr , "EXTPLC"    ) ) msRet = ( ExtPlcRobot( ALStation , Imsi ) );
	//else if ( STRCMP_L( RunStr , "RETPLC"    ) ) msRet = ( RetPlcRobot( ALStation , Imsi ) );
	//else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotateRobot( ALStation , Imsi ) );
	else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotPicRobot( ALStation , Imsi ) );
	else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = ( RotPlcRobot( ALStation , Imsi ) );
	else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotPicRobot( ALStation , Imsi ) );
	//
	//else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = ( RetractRobot( ALStation , Imsi ) );
	//else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = ( ExtendRobot( ALStation , Imsi ) );
	//else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotateRobot( ALStation , Imsi ) );
	//else if ( STRCMP_L( RunStr , "CHECKLOAD" ) ) msRet = ( CheckLoadRobot( ALStation , Imsi ) );
	else {
		printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
		return SYS_ABORTED;
	}

	_LOG("-----> %s %s Finished in TM Robot Func", StationStr, RunStr);
	return msRet;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		// Deinitialize
		_gCLOSE_LOG();
	}
}
//
//
//
