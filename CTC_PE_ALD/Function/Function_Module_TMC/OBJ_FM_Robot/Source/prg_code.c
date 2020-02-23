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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "TMC_FMRBLOW_CONTROL"		, _K_F_IO	,	RBLOW_CONTROL		,	0	} ,
	{ "TMC_FMWFRSNS_CONTROL"	, _K_F_IO	,	WFRSNS_CONTROL		,	0	} ,
	{ "FM.RB_SYNCH"				, _K_D_IO	,	ROBOT_SYNCH			,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PickRobot( char *St_Str , BOOL TR, BOOL DB ) {
	char Str_Buffer[255];
	int Result , ResultWFRSNS;
	BOOL bDBCheck = FALSE; //2007.08.01

	while (1) {
		//2007.08.01
		if (TR == FALSE && DB == TRUE) bDBCheck = TRUE; 	
		
		if (TRUE == DB)
			sprintf( Str_Buffer , "ABSENT_PICK %s 0 DB" , St_Str );
		else
			sprintf( Str_Buffer , "ABSENT_PICK %s 0" , St_Str );

		printf( "===> ATM Robot(WFRSNS_CONTROL) : Start Checking %s\n" , Str_Buffer );
		while (1) {
			ResultWFRSNS = SYS_ABORTED;
			ResultWFRSNS = RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer );
			if (ResultWFRSNS == SYS_ABORTED) return SYS_ABORTED;
			else if (ResultWFRSNS == SYS_AVAILABLE) continue;
			else break;
		}
		printf( "===> ATM Robot(WFRSNS_CONTROL) : End Checking %s\n" , Str_Buffer );

		printf("Pick Robot transfer %d, Double %d \n",TR, DB);
		if (TR == TRUE && DB == FALSE)
			sprintf(Str_Buffer , "PICK %s TRANSFER2" , St_Str);
		else if (TR == FALSE && DB == FALSE)
			sprintf(Str_Buffer , "PICK %s" , St_Str);
		else if (TR == FALSE && DB == TRUE) 
			sprintf(Str_Buffer , "PICK %s DB" , St_Str);
		else if(TR == TRUE && DB == TRUE)
			sprintf(Str_Buffer , "PICK %s TRANSFER2" , St_Str);

		printf("===> ATM Robot : Start %s\n"  , Str_Buffer);
		Result = RUN_FUNCTION(RBLOW_CONTROL  , Str_Buffer);
		if (Result == SYS_ABORTED) {
			//2007.08.01
			if (TRUE == bDBCheck)
				sprintf(Str_Buffer , "PRESENT_PICK %s 0 DB TRANSFER2" , St_Str );
			else
				sprintf(Str_Buffer , "PRESENT_PICK %s 0 TRANSFER2" , St_Str );

			printf ("===> ATM Robot(WFRSNS_CONTROL) : Start Checking %s after pick abort\n" , Str_Buffer );
			while (1) {
				ResultWFRSNS = SYS_ABORTED;
				ResultWFRSNS = RUN_FUNCTION(WFRSNS_CONTROL , Str_Buffer);
				if (ResultWFRSNS == SYS_ABORTED) {
					printf("===> ATM Robot(WFRSNS_CONTROL) : End Checking %s after pick abort + WFRSNS_CONTROL abort, \n" , Str_Buffer );
					return SYS_ABORTED;
				}
				else if (ResultWFRSNS == SYS_AVAILABLE) continue;
				else break;
			}
			printf( "===> ATM Robot(WFRSNS_CONTROL) : End Checking %s after pick abort\n" , Str_Buffer );

			return SYS_ABORTED;
		} else {
			if (TR)
				sprintf(Str_Buffer , "PRESENT_PICK %s 0 TRANSFER2" , St_Str);
			else {
				//2007.08.01
				if (TRUE == bDBCheck)
					sprintf(Str_Buffer , "PRESENT_PICK %s 0 DB" , St_Str);
				else
					sprintf(Str_Buffer , "PRESENT_PICK %s 0" , St_Str);
			}

			printf( "===> ATM Robot(WFRSNS_CONTROL) : Start Checking %s after pick\n" , Str_Buffer );
			ResultWFRSNS = SYS_ABORTED;
			ResultWFRSNS = RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer );
			if (ResultWFRSNS == SYS_ABORTED) {
				printf( "===> ATM Robot(WFRSNS_CONTROL) : End Checking %s after pick + WFRSNS_CONTROL abort\n" , Str_Buffer );
				//WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
				_sleep(250);
				return SYS_ABORTED;
			} 
			else if (ResultWFRSNS == SYS_AVAILABLE) continue;
			else break;
		}
	}
	printf( "===> ATM Robot(WFRSNS_CONTROL) : End Checking %s after pick\n" , Str_Buffer );
	printf( "===> ATM Robot : End %s\n" , Str_Buffer );
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status PlaceRobot( char *St_Str , BOOL TR, BOOL DB ) {
	char Str_Buffer[255];
	int  Result , ResultWFRSNS;
	BOOL bDBCheck = FALSE; //2007.08.01

	while(1) {
		//2007.08.01
		if (TR == FALSE && DB == TRUE) bDBCheck = TRUE; 	

		if (TRUE == bDBCheck)
			sprintf( Str_Buffer , "PRESENT_PLACE %s 0 DB" , St_Str );
		else
			sprintf( Str_Buffer , "PRESENT_PLACE %s 0" , St_Str );

		printf( "===> ATM Robot(WFRSNS_CONTROL) : Start Checking %s\n" , Str_Buffer );
		while (1) {
			ResultWFRSNS = SYS_ABORTED;
			ResultWFRSNS = RUN_FUNCTION(WFRSNS_CONTROL , Str_Buffer);
			if (ResultWFRSNS == SYS_ABORTED) return SYS_ABORTED;
			else if (ResultWFRSNS == SYS_AVAILABLE) continue;
			else break;
		}
		printf( "===> ATM Robot(WFRSNS_CONTROL) : End Checking %s\n" , Str_Buffer );
		printf("Pick Robot transfer %d, Double %d \n",TR, DB);

		if (TR == TRUE && DB == FALSE )
			sprintf(Str_Buffer , "PLACE %s TRANSFER2" , St_Str);
		else if (TR == FALSE && DB == FALSE)
			sprintf(Str_Buffer , "PLACE %s" , St_Str);
		else if (TR == FALSE && DB == TRUE)
			sprintf(Str_Buffer , "PLACE %s DB" , St_Str);
		else if (TR == TRUE && DB == TRUE)
			sprintf(Str_Buffer , "PLACE %s TRANSFER2" , St_Str);


		printf("===> ATM Robot : Start %s\n" , Str_Buffer);
		Result = RUN_FUNCTION(RBLOW_CONTROL  , Str_Buffer);
		if (Result == SYS_ABORTED) {
			//2007.08.01
			if (TRUE == bDBCheck)
				sprintf(Str_Buffer , "ABSENT_PLACE %s 0 DB TRANSFER2" , St_Str);
			else
				sprintf(Str_Buffer , "ABSENT_PLACE %s 0 TRANSFER2" , St_Str);

			printf("===> ATM Robot(WFRSNS_CONTROL) : Start Checking %s after place abort\n" , Str_Buffer);
			while (1) {
				ResultWFRSNS = SYS_ABORTED;
				ResultWFRSNS = RUN_FUNCTION(WFRSNS_CONTROL , Str_Buffer );
				if (ResultWFRSNS == SYS_ABORTED) {
					printf( "===> ATM Robot(WFRSNS_CONTROL) : End Checking %s after place abort + WFRSNS_CONTROL abort, \n" , Str_Buffer );
					return SYS_ABORTED;
				}
				else if (ResultWFRSNS == SYS_AVAILABLE) continue;
				else break;
			}
			printf( "===> ATM Robot(WFRSNS_CONTROL) : End Checking %s after place abort\n" , Str_Buffer );

			return SYS_ABORTED;
		}
		else {
			if (TR)
				sprintf(Str_Buffer , "ABSENT_PLACE %s 0 TRANSFER2" , St_Str);
			else {
				//2007.08.01
				if (TRUE == bDBCheck)
					sprintf(Str_Buffer , "ABSENT_PLACE %s 0 DB" , St_Str);
				else 
					sprintf(Str_Buffer , "ABSENT_PLACE %s 0" , St_Str);
			}

			printf( "===> ATM Robot(WFRSNS_CONTROL) : Start Checking %s after place\n" , Str_Buffer );
			ResultWFRSNS = SYS_ABORTED;
			ResultWFRSNS = RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer );
			if (ResultWFRSNS == SYS_ABORTED)  {
				printf( "===> ATM Robot(WFRSNS_CONTROL) : End Checking %s after place + WFRSNS_CONTROL abort\n" , Str_Buffer );
				//WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
				_sleep(250);
				return SYS_ABORTED;
			}
			else if (ResultWFRSNS == SYS_AVAILABLE) continue;
			else break;
		}
	}
	printf( "===> ATM Robot(WFRSNS_CONTROL) : End Checking %s after place\n" , Str_Buffer );
	printf( "===> ATM Robot : End %s\n" , Str_Buffer );
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status ExtPicRobot( char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "ABSENT_PICK %s" , St_Str );
	if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	sprintf( Str_Buffer , "EXTEND %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RetPicRobot( char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "RETRACT %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	sprintf( Str_Buffer , "PRESENT_PICK %s" , St_Str );
	if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status ExtPlcRobot( char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "ROTATE %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	sprintf( Str_Buffer , "PRESENT_PLACE %s" , St_Str );
	if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	sprintf( Str_Buffer , "EXTEND %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RetPlcRobot( char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "RETRACT %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	sprintf( Str_Buffer , "ABSENT_PLACE %s" , St_Str );
	if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//
//------------------------------------------------------------------------------------------
Module_Status RotPicRobot( char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "ROTPIC %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	sprintf( Str_Buffer , "ABSENT_PICK %s" , St_Str );
	if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RotPlcRobot( char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "ROTPLC %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	sprintf( Str_Buffer , "PRESENT_PLACE %s" , St_Str );
	if ( RUN_FUNCTION( WFRSNS_CONTROL , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RetractRobot( char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "RETRACT %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
Module_Status ExtendRobot( char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "EXTEND %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
Module_Status RotateRobot( char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "ROTATE %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
//
//
Module_Status MappingRobot( char *St_Str ) {
	char Str_Buffer[255];
	sprintf( Str_Buffer , "MAPPING %s" , St_Str );
	if ( RUN_FUNCTION( RBLOW_CONTROL  , Str_Buffer ) == SYS_ABORTED ) return SYS_ABORTED;
	return SYS_SUCCESS;
}
//
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
//
//
//


/*
	This function operate three kind of atm robot handling msg.

	1. User msg from 'pick' or 'place' button click in GUI(exactly, Service_FEM screen).

		ex) PICK A_LOWERBM 1.00
			; PICK LOWERBM SLOT 1 WITH ARM A


	2. Special scheduler msg when wafer pick or place to 'aligner'

		ex) PICK AL 1 0 1
			; PICK ALIGNER WAFER WITH ARM A
			PICK AL 0 2 1
			; PICK ALIGNER WAFER WITH ARM B


	3. General scheduler msg from 'pick' or 'place' button click in GUI(Service_Transfer screen).
	   or scheduler msg from lot handling.
	   Caution : This msg need robot handling "twice times".

		ex) PICK CM1 1 2 TRANSFER or
		    PICK CM1 1 2
			; PICK CM1 SLOT 1 WITH ARM A & PICK CM1 SLOT 2 WITH ARM B
		ex) PICK CM1 1 0
			; PICK CM1 SLOT 1 WITH ARM A ONLY

*/


Module_Status Program_Main() {
	BOOL TR = FALSE, DB = FALSE;
	char StationStr[64] , StationStr2[64], Imsi[31] , Imsi2[31] , RunStr[31] , WaferID[31] , WaferID2[31] , MapStr[31];
	
	char szStation[64];
	char szUserMsg[64];
	char szSchdMsg[64];
	char szSpecialSchdMsg[64];
	char szHand[64];

	int nWaferID       = 0;
	int nWaferID2      = 0;
	int nResultWaferID = 0;

	char* pParam         = NULL;
	char* pszUserMsgFind = NULL;
	char* pszSchdMsgFind = NULL;
	char* pszSpecialSchdMsgFind = NULL;

	Module_Status msRet = SYS_SUCCESS;

	memset(StationStr , 0, sizeof(StationStr) );
	memset(StationStr2, 0, sizeof(StationStr2));
	memset(Imsi       , 0, sizeof(Imsi)       );
	memset(Imsi2      , 0, sizeof(Imsi2)      );
	memset(RunStr     , 0, sizeof(RunStr)     );
	memset(WaferID    , 0, sizeof(WaferID)    );
	memset(WaferID2   , 0, sizeof(WaferID2)   );
	memset(MapStr     , 0, sizeof(MapStr)     );
	memset(szStation  , 0, sizeof(szStation)  );
	memset(szUserMsg  , 0, sizeof(szUserMsg)  );
	memset(szSchdMsg  , 0, sizeof(szSchdMsg)  );
	memset(szSpecialSchdMsg, 0, sizeof(szSpecialSchdMsg));
	memset(szHand, 0, sizeof(szHand));

	
	pParam = PROGRAM_PARAMETER_READ();

	STR_SEPERATE( PROGRAM_PARAMETER_READ() , RunStr , Imsi , 30 );
	strcpy( MapStr , Imsi );
	STR_SEPERATE( Imsi , StationStr , Imsi2 , 30 );
	STR_SEPERATE( Imsi2 , WaferID , Imsi , 30 );


	strcpy( szStation, StationStr);		// get station
	strcat( StationStr , " " );

	strcat( StationStr , WaferID );		// get wafer id (if case 2 or case 3, it is 1st wafer id.)
	nWaferID = atoi(WaferID);

	//
	STR_SEPERATE( Imsi  , WaferID , Imsi2 , 30 );
	if ( STRCMP_L( WaferID , "TRANSFER2" ) )
	{
		TR = TRUE;
	}
	else
	{
		nWaferID2 = atoi(WaferID);		// get 2nd wafer id (if case 2 or case 3).
	}

	STR_SEPERATE( Imsi2 , WaferID , Imsi , 30 );
	if ( STRCMP_L( WaferID , "TRANSFER2" ) ) TR = TRUE;
	STR_SEPERATE( Imsi  , WaferID , Imsi2 , 30 );
	if ( STRCMP_L( WaferID , "TRANSFER2" ) ) TR = TRUE;


	// check msg type

	do
	{
		// 1. User msg from GUI
		// ex) PICK A_LOWERBM 1.00
		pszUserMsgFind = strstr(pParam, "_");


		// 2. Special scheduler msg
		// ex) PICK AL 1 0 1
		pszSpecialSchdMsgFind = strstr(pParam, " AL ");
		if(NULL != pszSpecialSchdMsgFind)
		{
			if( (0 != nWaferID) || (0 != nWaferID2) )
			{
				nResultWaferID = (0 != nWaferID) ? nWaferID : nWaferID2;
				sprintf(szHand, "%s", (0 != nWaferID) ? "A" : "B");
			}
		}
			
		// 3. General scheduler msg
		// ex) PICK CM1 1 2 TRANSFER

		// Left msg is general scheduler msg.

	}while(0);

	_LOG("-----> %s Started in FM Robot Func", pParam);

    // operate atm robot handling with msg type

	if (NULL != pszUserMsgFind) 
	{
		// 1. User msg from GUI
		DB = FALSE;
		strcpy(szUserMsg, StationStr);

		if      ( STRCMP_L( RunStr , "PICK"      ) ) msRet = ( PickRobot  ( szUserMsg , TR, DB ) );
		else if ( STRCMP_L( RunStr , "PLACE"     ) ) msRet = ( PlaceRobot ( szUserMsg , TR, DB ) );
		else if ( STRCMP_L( RunStr , "EXTPIC"    ) ) msRet = ( ExtPicRobot( szUserMsg ) );
		else if ( STRCMP_L( RunStr , "RETPIC"    ) ) msRet = ( RetPicRobot( szUserMsg ) );
		else if ( STRCMP_L( RunStr , "EXTPLC"    ) ) msRet = ( ExtPlcRobot( szUserMsg ) );
		else if ( STRCMP_L( RunStr , "RETPLC"    ) ) msRet = ( RetPlcRobot( szUserMsg ) );
		else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotPicRobot( szUserMsg ) );
		else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = ( RotPlcRobot( szUserMsg ) );
		else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = ( RetractRobot( szUserMsg ) );
		else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = ( ExtendRobot( szUserMsg ) );
		else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotateRobot( szUserMsg ) );
		else 
		{
			printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
			msRet =  SYS_ABORTED;
		}
	}
	else if(NULL != pszSpecialSchdMsgFind)
	{
		// 2. Special scheduler msg

		DB = FALSE;
		sprintf(szSpecialSchdMsg, "%s_%s %d", szHand, szStation, nResultWaferID);

		if      ( STRCMP_L( RunStr , "PICK"      ) ) msRet = ( PickRobot  ( szSpecialSchdMsg , TR, DB ) );
		else if ( STRCMP_L( RunStr , "PLACE"     ) ) msRet = ( PlaceRobot ( szSpecialSchdMsg , TR, DB ) );
		else if ( STRCMP_L( RunStr , "EXTPIC"    ) ) msRet = ( ExtPicRobot( szSpecialSchdMsg ) );
		else if ( STRCMP_L( RunStr , "RETPIC"    ) ) msRet = ( RetPicRobot( szSpecialSchdMsg ) );
		else if ( STRCMP_L( RunStr , "EXTPLC"    ) ) msRet = ( ExtPlcRobot( szSpecialSchdMsg ) );
		else if ( STRCMP_L( RunStr , "RETPLC"    ) ) msRet = ( RetPlcRobot( szSpecialSchdMsg ) );
		else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotPicRobot( szSpecialSchdMsg ) );
		else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = ( RotPlcRobot( szSpecialSchdMsg ) );
		else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = ( RetractRobot( szSpecialSchdMsg ) );
		else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = ( ExtendRobot( szSpecialSchdMsg ) );
		else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotateRobot( szSpecialSchdMsg ) );
		else 
		{
			printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
			msRet =  SYS_ABORTED;
		}
	}
	else
	{
		// 3. General scheduler msg
		//	  Caution : This msg need robot handling "twice times".

		do {
			// 3.1 double pick/place from LPM.
			if (0 != nWaferID) 
			{
				if (   (0 != nWaferID2              //Transfer Wafer Should be two
					&& 1 == (nWaferID2 - nWaferID)) //Slot of two wafer must be neighborhood & Wafer ID2 must be greater than Wafer ID1
					&& (STRCMP_L(szStation , "CM1") || STRCMP_L(szStation , "CM2") || STRCMP_L(szStation , "CM3"))) //Only permitted when Target is CM1/CM2/CM3
				{
					DB = TRUE;
					printf("^^ Check Double Transfer W1(%d) , W2(%d) , Station(%s) DB Flag (%d) \n" , nWaferID , nWaferID2 , szStation , DB);
					
					memset (szSchdMsg, 0, sizeof(szSchdMsg));
					sprintf(szSchdMsg, "A_%s %d", szStation, nWaferID);

					if      ( STRCMP_L( RunStr , "PICK"      ) ) msRet = ( PickRobot  ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "PLACE"     ) ) msRet = ( PlaceRobot ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "EXTPIC"    ) ) msRet = ( ExtPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPIC"    ) ) msRet = ( RetPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTPLC"    ) ) msRet = ( ExtPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPLC"    ) ) msRet = ( RetPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = ( RotPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = ( RetractRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = ( ExtendRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotateRobot( szSchdMsg ) );
					else 
					{
						printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
						msRet =  SYS_ABORTED;
					}
				}

				if (SYS_SUCCESS != msRet) break;
			}

			// 3.2 twice msg handling

			// 3.2.1 place case
			//       1st A arm, 2nd B arm

			if(STRCMP_L( RunStr , "PLACE"))
			{
				if (0 != nWaferID && DB == FALSE) {
					memset (szSchdMsg, 0, sizeof(szSchdMsg));
					sprintf(szSchdMsg, "A_%s %d", szStation, nWaferID);
					
					if      ( STRCMP_L( RunStr , "PICK"      ) ) msRet = ( PickRobot  ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "PLACE"     ) ) msRet = ( PlaceRobot ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "EXTPIC"    ) ) msRet = ( ExtPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPIC"    ) ) msRet = ( RetPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTPLC"    ) ) msRet = ( ExtPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPLC"    ) ) msRet = ( RetPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = ( RotPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = ( RetractRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = ( ExtendRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotateRobot( szSchdMsg ) );
					else 
					{
						printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
						msRet =  SYS_ABORTED;
					}
					
					if (SYS_SUCCESS != msRet) break;
				}

				if (0 != nWaferID2 && DB == FALSE) 
				{
					memset (szSchdMsg, 0, sizeof(szSchdMsg));
					sprintf(szSchdMsg, "B_%s %d", szStation, nWaferID2);
					
					if      ( STRCMP_L( RunStr , "PICK"      ) ) msRet = ( PickRobot  ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "PLACE"     ) ) msRet = ( PlaceRobot ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "EXTPIC"    ) ) msRet = ( ExtPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPIC"    ) ) msRet = ( RetPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTPLC"    ) ) msRet = ( ExtPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPLC"    ) ) msRet = ( RetPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = ( RotPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = ( RetractRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = ( ExtendRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotateRobot( szSchdMsg ) );
					else 
					{
						printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
						msRet =  SYS_ABORTED;
					}
				}			
			}

			// 3.2.2 pick case
			//       1st B arm, 2nd A arm


			else if(STRCMP_L( RunStr , "PICK"))
			{
				if (0 != nWaferID2 && DB == FALSE) 
				{
					memset (szSchdMsg, 0, sizeof(szSchdMsg));
					sprintf(szSchdMsg, "B_%s %d", szStation, nWaferID2);
					
					if      ( STRCMP_L( RunStr , "PICK"      ) ) msRet = ( PickRobot  ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "PLACE"     ) ) msRet = ( PlaceRobot ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "EXTPIC"    ) ) msRet = ( ExtPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPIC"    ) ) msRet = ( RetPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTPLC"    ) ) msRet = ( ExtPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPLC"    ) ) msRet = ( RetPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = ( RotPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = ( RetractRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = ( ExtendRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotateRobot( szSchdMsg ) );
					else 
					{
						printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
						msRet =  SYS_ABORTED;
					}

					if (SYS_SUCCESS != msRet) break;
				}
				
				if (0 != nWaferID && DB == FALSE) {
					memset (szSchdMsg, 0, sizeof(szSchdMsg));
					sprintf(szSchdMsg, "A_%s %d", szStation, nWaferID);
					
					if      ( STRCMP_L( RunStr , "PICK"      ) ) msRet = ( PickRobot  ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "PLACE"     ) ) msRet = ( PlaceRobot ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "EXTPIC"    ) ) msRet = ( ExtPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPIC"    ) ) msRet = ( RetPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTPLC"    ) ) msRet = ( ExtPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPLC"    ) ) msRet = ( RetPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = ( RotPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = ( RetractRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = ( ExtendRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotateRobot( szSchdMsg ) );
					else 
					{
						printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
						msRet =  SYS_ABORTED;
					}
					
					if (SYS_SUCCESS != msRet) break;
				}
			}

			// 3.2.3 other case
			//       1st A arm, 2nd B arm

			else
			{
				if (0 != nWaferID && DB == FALSE) {
					memset (szSchdMsg, 0, sizeof(szSchdMsg));
					sprintf(szSchdMsg, "A_%s %d", szStation, nWaferID);
					
					if      ( STRCMP_L( RunStr , "PICK"      ) ) msRet = ( PickRobot  ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "PLACE"     ) ) msRet = ( PlaceRobot ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "EXTPIC"    ) ) msRet = ( ExtPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPIC"    ) ) msRet = ( RetPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTPLC"    ) ) msRet = ( ExtPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPLC"    ) ) msRet = ( RetPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = ( RotPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = ( RetractRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = ( ExtendRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotateRobot( szSchdMsg ) );
					else 
					{
						printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
						msRet =  SYS_ABORTED;
					}
					
					if (SYS_SUCCESS != msRet) break;
				}

				if (0 != nWaferID2 && DB == FALSE) 
				{
					memset (szSchdMsg, 0, sizeof(szSchdMsg));
					sprintf(szSchdMsg, "B_%s %d", szStation, nWaferID2);
					
					if      ( STRCMP_L( RunStr , "PICK"      ) ) msRet = ( PickRobot  ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "PLACE"     ) ) msRet = ( PlaceRobot ( szSchdMsg , TR, DB ) );
					else if ( STRCMP_L( RunStr , "EXTPIC"    ) ) msRet = ( ExtPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPIC"    ) ) msRet = ( RetPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTPLC"    ) ) msRet = ( ExtPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETPLC"    ) ) msRet = ( RetPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPIC"    ) ) msRet = ( RotPicRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTPLC"    ) ) msRet = ( RotPlcRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "RETRACT"   ) ) msRet = ( RetractRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "EXTEND"    ) ) msRet = ( ExtendRobot( szSchdMsg ) );
					else if ( STRCMP_L( RunStr , "ROTATE"    ) ) msRet = ( RotateRobot( szSchdMsg ) );
					else 
					{
						printf( "---> Check Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ() );
						msRet =  SYS_ABORTED;
					}
				}			
			}

		}while(0);

	}


	_LOG("-----> %s Finished in FM Robot Func", pParam);
	return msRet;
}
//
//
//
