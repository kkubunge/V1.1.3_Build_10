#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutlanal.h>
#include <KutlFile.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlgui2.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
#include "AppRecipe.h"
#include "schjobintf.h"

#include "IPS_Log.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		MAX_SLOT	25
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		Process_Recipe_Tunning_Data_Check_Error							3
int		ALARM_START_POINT	= 280;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "e:SCHEDULER"					, _K_S_IO	,	S_SCHEDULER			,	0	} ,
//	{ "CTC.RECIPE_FILE$1"			, _K_S_IO	,	RECIPE_FILE			,	0	} ,
//	{ "CTC.MAIN_CONTROL$1"			, _K_D_IO	,	MAIN_CONTROL		,	0	} ,
//	{ "CTC.MAIN_STATUS$1"			, _K_D_IO	,	MAIN_STATUS			,	0	} ,
	{ "CTC.MID_NAME"				, _K_S_IO	,	MID_NAME			,	0	} ,
	{ "CTC.MID_NAME2"				, _K_S_IO	,	MID_NAME2			,	0	} ,
	{ "CTC.MID_NAME3"				, _K_S_IO	,	MID_NAME3			,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	char	pszFtnName[1024] = "";
	char	szEventID[1024], szPJID[1024], szCJID[1024], szType[1024], szBuffer1[1024], szBuffer2[1024], szBuffer3[1024], szTemp[1024], szTemp2[1024];
	char	szPort1CarrierID[128], szPort2CarrierID[128], szPort3CarrierID[128], szCarrierID[128];
	int		i, j, k, nBufferSize, nBufferSize2;
	int		nCommStatus;

	memset(szEventID,	0,		sizeof(szEventID));
	memset(szPJID,		0,		sizeof(szPJID));
	memset(szCJID,		0,		sizeof(szCJID));
	memset(szType,		0,		sizeof(szType));
	memset(szBuffer1,	0,		sizeof(szBuffer1));
	memset(szBuffer2,	0,		sizeof(szBuffer2));
	memset(szBuffer3,	0,		sizeof(szBuffer3));
	memset(szTemp,		0,		sizeof(szTemp));
	memset(szTemp2,		0,		sizeof(szTemp2));
	memset(szPort1CarrierID,	0,	sizeof(szPort1CarrierID));
	memset(szPort2CarrierID,	0,	sizeof(szPort2CarrierID));
	memset(szPort3CarrierID,	0,	sizeof(szPort3CarrierID));
	memset(szCarrierID,			0,	sizeof(szCarrierID));

//	printf("==========> FA Start Option is [%s]\n", PROGRAM_PARAMETER_READ());

	STR_SEPERATE_CHAR( PROGRAM_PARAMETER_READ() , ' ' , szEventID , szBuffer1 , 1024 );
	STR_SEPERATE_CHAR( szBuffer1 , '|' , szType , szBuffer1 , 1024 );

	if ( STRCMP_L("PROCESSJOB", szEventID)) {
		nBufferSize = strlen(szBuffer1);
		//	printf(" nBufferSize = [%d]\n", nBufferSize);
		if( nBufferSize > 0 ) {
			STR_SEPERATE_CHAR( szBuffer1, '|', szPJID, szBuffer2, 1024);
			//		printf(" szType = [%s]\n", szType);
			if( STRCMP_L("INSERT", szType) ) {
				for(j=0; j<MAX_SLOT; j++) {
					nBufferSize = strlen(Sch_Multi_ProcessJob[j]->JobID);
					if ( nBufferSize > 0 ) {
						if ( STRCMP_L( szPJID, Sch_Multi_ProcessJob[j]->JobID ) ) {
							//						printf( "=============> Sch_Multi_ProcessJob_Buffer->MtlCarrName[%d] : [%s]\n", j, Sch_Multi_ProcessJob[j]->MtlCarrName);
							//						LOG(pszFtnName, ELT_File, "CarrierID : %s", Sch_Multi_ProcessJob[j]->MtlCarrName);
							sprintf(szTemp, "CarrierID:%s,  ", Sch_Multi_ProcessJob[j]->MtlCarrName);
							sprintf(szCarrierID, "%s", Sch_Multi_ProcessJob[j]->MtlCarrName);
							strcat(szTemp2, szTemp);
							//						printf( "=============> Sch_Multi_ProcessJob_Buffer->JobID[%d] : [%s]\n", j, Sch_Multi_ProcessJob[j]->JobID);
							//						LOG(pszFtnName, ELT_File, "Process_JobID : %s", Sch_Multi_ProcessJob[j]->JobID);
							sprintf(szTemp, "Process_JobID:%s,  ", Sch_Multi_ProcessJob[j]->JobID);
							strcat(szTemp2, szTemp);
							//						printf( "=============> Sch_Multi_ProcessJob_Buffer->MtlRecipeName[%d] : [%s]\n", j, Sch_Multi_ProcessJob[j]->MtlRecipeName);
							//						LOG(pszFtnName, ELT_File, "RecipeName : %s", Sch_Multi_ProcessJob[j]->MtlRecipeName);
							sprintf(szTemp, "RecipeName:%s,  ", Sch_Multi_ProcessJob[j]->MtlRecipeName);
							strcat(szTemp2, szTemp);
							//						printf( "=============> Sch_Multi_ProcessJob_Buffer->MtlRecipeMode [%d] : [%d]\n", j, Sch_Multi_ProcessJob[j]->MtlRecipeMode);
							//						LOG(pszFtnName, ELT_File, "RecipeMode : %s", itoa(Sch_Multi_ProcessJob[j]->MtlRecipeMode, szBuffer3, 10));
							sprintf(szTemp, "RecipeMode:%s,  ", itoa(Sch_Multi_ProcessJob[j]->MtlRecipeMode, szBuffer3, 10));
							strcat(szTemp2, szTemp);
							for(i=0; i<4; i++) {
								if ( Sch_Multi_ProcessJob[j]->MtlRcpTuneModule[i] > 0 ) {
									//								printf( "=============> Sch_Multi_ProcessJob_Buffer->MtlRcpTuneModule[%d][%d] : [PM%d]\n", j, i, Sch_Multi_ProcessJob[j]->MtlRcpTuneModule[i]);
									//								LOG(pszFtnName, ELT_File, "RecipeTuneModule : PM%s", itoa(Sch_Multi_ProcessJob[j]->MtlRcpTuneModule[i], szBuffer3, 10));
									sprintf(szTemp, "RecipeTuneModule:PM%s,  ", itoa(Sch_Multi_ProcessJob[j]->MtlRcpTuneModule[i], szBuffer3, 10));
									strcat(szTemp2, szTemp);
									//								printf( "=============> Sch_Multi_ProcessJob_Buffer->MtlRcpTuneStep [%d][%d] : [%d]\n", j, i, Sch_Multi_ProcessJob[j]->MtlRcpTuneStep[i]);
									//								LOG(pszFtnName, ELT_File, "RecipeTuneStep : %s", itoa(Sch_Multi_ProcessJob[j]->MtlRcpTuneStep[i], szBuffer3, 10));
									sprintf(szTemp, "RecipeTuneStep:%s,  ", itoa(Sch_Multi_ProcessJob[j]->MtlRcpTuneStep[i], szBuffer3, 10));
									strcat(szTemp2, szTemp);
									//								printf( "=============> Sch_Multi_ProcessJob_Buffer->MtlRcpTuneName [%d][%d] : [%s]\n", j, i, Sch_Multi_ProcessJob[j]->MtlRcpTuneName[i]);
									//								LOG(pszFtnName, ELT_File, "RecipeTuneName : %s", Sch_Multi_ProcessJob[j]->MtlRcpTuneName[i]);
									sprintf(szTemp, "RecipeTuneName:%s,  ", Sch_Multi_ProcessJob[j]->MtlRcpTuneName[i]);
									strcat(szTemp2, szTemp);
									//								printf( "=============> Sch_Multi_ProcessJob_Buffer->MtlRcpTuneData [%d][%d] : [%s]\n", j, i, Sch_Multi_ProcessJob[j]->MtlRcpTuneData[i]);
									//								LOG(pszFtnName, ELT_File, "RecipeTuneData : %s", Sch_Multi_ProcessJob[j]->MtlRcpTuneData[i]);
									sprintf(szTemp, "RecipeTuneData:%s,  ", Sch_Multi_ProcessJob[j]->MtlRcpTuneData[i]);
									strcat(szTemp2, szTemp);
//									LOG(pszFtnName, ELT_File, "RecipeTuneData :D: %d\n", atoi(Sch_Multi_ProcessJob[j]->MtlRcpTuneData[i]));
//									LOG(pszFtnName, ELT_File, "RecipeTuneData :F: %f\n", atoi(Sch_Multi_ProcessJob[j]->MtlRcpTuneData[i]));
									if( STRCMP_L(Sch_Multi_ProcessJob[j]->MtlRcpTuneName[i], "VAL_TIME_00_00")) {
										if( (1 > atoi(Sch_Multi_ProcessJob[j]->MtlRcpTuneData[i])) || (100 < atoi(Sch_Multi_ProcessJob[j]->MtlRcpTuneData[i])) ) {
											LOG(pszFtnName, ELT_File, "Error RecipeTuneData : [%s]\n", Sch_Multi_ProcessJob[j]->MtlRcpTuneData[i]);
											READ_STRING(MID_NAME, szPort1CarrierID, &nCommStatus);
											READ_STRING(MID_NAME2, szPort2CarrierID, &nCommStatus);
											READ_STRING(MID_NAME3, szPort3CarrierID, &nCommStatus);
											
											if( STRCMP_L(szPort1CarrierID, szCarrierID) ) {
												WRITE_STRING( S_SCHEDULER , "CANCEL" , &nCommStatus );
												ALARM_POST(ALARM_START_POINT + Process_Recipe_Tunning_Data_Check_Error);
											}
											else if( STRCMP_L(szPort2CarrierID, szCarrierID) ) {
												WRITE_STRING( S_SCHEDULER , "CANCEL2" , &nCommStatus );
												ALARM_POST(ALARM_START_POINT + 300 + Process_Recipe_Tunning_Data_Check_Error);
											}
											else if( STRCMP_L(szPort3CarrierID, szCarrierID) ) {
												WRITE_STRING( S_SCHEDULER , "CANCEL3" , &nCommStatus );
												ALARM_POST(ALARM_START_POINT + 600 + Process_Recipe_Tunning_Data_Check_Error);
											}
											else {
												LOG(pszFtnName, ELT_File, "CarrierID Check Error of RecipeTuneData : %s\n", Sch_Multi_ProcessJob[j]->MtlRcpTuneData[i]);
											}
										}
									}
								}
							}
							strcat(szTemp2, "WaferSlotID:");
							for(k=0; k<MAX_SLOT; k++) {
								if ( Sch_Multi_ProcessJob[j]->MtlSlot[0][k] > 0 ) {
									//								printf( "=============> Sch_Multi_ProcessJob_Buffer->MtlSlot [%d][%d] : [%d]\n", j, k, Sch_Multi_ProcessJob[j]->MtlSlot[0][k]);
									//								LOG(pszFtnName, ELT_File, "WaferSlotID : %s", itoa(Sch_Multi_ProcessJob[j]->MtlSlot[0][k], szBuffer3, 10));
									sprintf(szTemp, "%s,  ", itoa(Sch_Multi_ProcessJob[j]->MtlSlot[0][k], szBuffer3, 10));
									strcat(szTemp2, szTemp);
								}
							}
							//						strcat(szTemp2, szTemp);
							//						printf( "=============> Sch_Multi_ProcessJob_Buffer->StartMode[%d] : [%d]\n", j, Sch_Multi_ProcessJob[j]->StartMode);
							//						LOG(pszFtnName, ELT_File, "JobStartMode : %s", itoa(Sch_Multi_ProcessJob[j]->StartMode, szBuffer3, 10));
							sprintf(szTemp, "JobStartMode:%s,  ", itoa(Sch_Multi_ProcessJob[j]->StartMode, szBuffer3, 10));
							strcat(szTemp2, szTemp);
							//						printf( "=============> Sch_Multi_ProcessJob_Buffer->ResultStatus[%d] : [%d]\n", j, Sch_Multi_ProcessJob[j]->ResultStatus); 
							//						LOG(pszFtnName, ELT_File, "JobResultStatus : %s", itoa(Sch_Multi_ProcessJob[j]->ResultStatus, szBuffer3, 10));
							sprintf(szTemp, "JobResultStatus:%s.", itoa(Sch_Multi_ProcessJob[j]->ResultStatus, szBuffer3, 10));
							strcat(szTemp2, szTemp);
						}
					}
				}
				LOG(pszFtnName, ELT_File, "Process_Job_Insert : %s\n", szTemp2);

				
				//			printf( "=============> Sch_Multi_ProcessJob_Buffer->ControlStatus : [%d]\n", Sch_Multi_ProcessJob[0]->ControlStatus); 
				//			printf( "=============> Sch_Multi_ProcessJob_Buffer->EventInfo : [%d]\n", Sch_Multi_ProcessJob[0]->EventInfo[0]); 
				//			printf( "=============> Sch_Multi_ProcessJob_Buffer->MtlUse : [%d]\n", Sch_Multi_ProcessJob[0]->MtlUse[0]); 
			}
			else {
				//
			}
		}
		else {
			//
		}
	}
	else if (STRCMP_L("CONTROLJOB", szEventID)) {
		nBufferSize = strlen(szBuffer1);
		//		printf(" nBufferSize = [%d]\n", nBufferSize);
		if( nBufferSize > 0 ) {
			STR_SEPERATE_CHAR( szBuffer1, '|', szCJID, szBuffer2, 1024);
			printf(" szType = [%s]\n", szType);
			if( STRCMP_L("INSERT", szType) ) {
				for(j=0; j<MAX_SLOT; j++) {
					nBufferSize = strlen(Sch_Multi_ControlJob[j]->JobID);
					if ( nBufferSize > 0 ) {
						if ( STRCMP_L( szCJID, Sch_Multi_ControlJob[j]->JobID ) ) {
							sprintf(szTemp, "CarrierID:%s,  ", Sch_Multi_ControlJob[j]->CassInID);
							strcat(szTemp2, szTemp);
							//
							sprintf(szTemp, "Control_JobID:%s,  ", Sch_Multi_ControlJob[j]->JobID);
							strcat(szTemp2, szTemp);
							//
							strcat(szTemp2, "Linked_PJID:");
							for(k=0; k<MAX_SLOT; k++) {
								nBufferSize2 = strlen(Sch_Multi_ControlJob[j]->CtrlSpec_ProcessJobID[k]);
								if ( nBufferSize2 > 0 ) {
									sprintf(szTemp, "%s,  ", Sch_Multi_ControlJob[j]->CtrlSpec_ProcessJobID[k]);
									strcat(szTemp2, szTemp);
								}
							}
						}
					}
				}
				LOG(pszFtnName, ELT_File, "Control_Job_Insert : %s\n", szTemp2);
			}
			else {
				//
			}
		}
		else {
			//
		}
	}
	else {
		//
	}

	
/*	sprintf( FileName , "RECIPE/PROC/LOT/%s" , Temp ) ;
	
	WRITE_DIGITAL( Start_Option_Err , HOST , &CommStatus );

	for ( i = 0 ; i < MAX_SLOT ; i++ ) {
		Recipe_Use[i] = LRNoUse;
	}

	i = atoi( CarrierID );

  for ( j = 0 ; j < MAX_MULTI_CTJOBSELECT_SIZE ; j++ ) {
		printf("===###===>1 [%d] : [MtlCJName : %s], [MtlPJName : %s]\n", j, Sch_Multi_CassRunJob_Data[i]->MtlCJName, Sch_Multi_CassRunJob_Data[i]->MtlPJName);
		if ( STRCMP_L( Sch_Multi_CassRunJob_Data[i]->MtlCJName , Sch_Multi_ControlJob_Select_Data[j]->JobID ) ) {
			for ( k = 0 ; k < MAX_MULTI_CTJOB_CASSETTE ; k++ ) {
				printf("###===###>2 [%d] : [CtrlSpec_ProcessJobID : %s]\n", k, Sch_Multi_ControlJob_Select_Data[j]->CtrlSpec_ProcessJobID);
				printf("###===###>2 [%d] : [CtrlSpec_RuleName : %s]\n", k, Sch_Multi_ControlJob_Select_Data[j]->CtrlSpec_RuleName);
				printf("###===###>2 [%d] : [EventInfo : %s]\n", k, Sch_Multi_ControlJob_Select_Data[j]->EventInfo);
				printf("###===###>2 [%d] : [JobID : %s]\n", k, Sch_Multi_ControlJob_Select_Data[j]->JobID);
				if ( Sch_Multi_ControlJob_Select_Data[j]->CtrlSpec_Use[k] ) {
					for ( l = 0 ; l < MAX_SLOT ; l++ ) {
						if ( STRCMP_L( Sch_Multi_CassRunJob_Data[i]->MtlPJName[l] , Sch_Multi_ControlJob_Select_Data[j]->CtrlSpec_ProcessJobID[k] ) ) {
							Recipe_Use[l] = LRUse;
						}
					}
				}
			}
			break;
		}
	}
*/
	return SYS_SUCCESS ;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Program_Enter_Code() {
//	printf("===###===> FA Start Option [Program Enter Code]\n");
	_SCH_MULTIJOB_INTERFACE_INITIALIZING();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
