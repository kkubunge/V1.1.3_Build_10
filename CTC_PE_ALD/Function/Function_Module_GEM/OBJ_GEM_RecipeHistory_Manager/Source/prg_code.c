#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <KutlAnal.h>
#include <kutlgui.h>
#include <kutlStr.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
#include "..\resource.h"
#include "IPS_Log.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "CurrentUserName"			, _K_S_IO	,	CurrentUserName	,	0	} , 
	{ "GEM.Recipe_Edit_User"	, _K_S_IO	,	Recipe_Edit_User,	0	} ,
	{ "GEM.Control_State"		, _K_D_IO	,	Online_State	,	0	} ,
	
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	char *pszEd;
	char szBuffer1[256] , szBuffer2[256], szBuffer3[256], szBuffer4[256], szEvent[256];
	char szModuleName[256], szRecipeName[256], szRecipeGroupID[256], szTotalStepNum[256], szEditStatus[256];
	char szCurrentUserName[256];
	char pszFtnName[256] = "";
	int nCommStatus, nStrlength = 0;

	memset(szEvent,			0,  sizeof(szEvent));
	memset(szBuffer1,		0,  sizeof(szBuffer1));
	memset(szBuffer2,		0,	sizeof(szBuffer2));
	memset(szBuffer3,		0,	sizeof(szBuffer3));
	memset(szBuffer4,		0,	sizeof(szBuffer4));
	memset(szModuleName,	0,	sizeof(szModuleName));
	memset(szRecipeName,	0,	sizeof(szRecipeName));
	memset(szRecipeGroupID,	0,	sizeof(szRecipeGroupID));
	memset(szTotalStepNum,	0,	sizeof(szTotalStepNum));
	memset(szEditStatus,	0,	sizeof(szEditStatus));
	memset(szCurrentUserName,	0,	sizeof(szCurrentUserName));

	pszEd = PROGRAM_PARAMETER_READ();

	STR_SEPERATE( pszEd , szEvent , szBuffer1	, 255 );
	STR_SEPERATE_CHAR( szBuffer1, '|', szModuleName, szBuffer2, 255 );	
	STR_SEPERATE_CHAR( szBuffer2, '|', szBuffer3, szBuffer4, 255 );	
	STR_SEPERATE_CHAR( szBuffer3, ':', szRecipeName, szRecipeGroupID, 255);
	STR_SEPERATE_CHAR( szBuffer4, '|', szTotalStepNum, szEditStatus, 255 );	
	
	if( STRCMP_L( szEditStatus, "0" ))
	{
		strcpy( szEditStatus, "Create" );
	}
	else if( STRCMP_L( szEditStatus, "1" ))
	{
		strcpy( szEditStatus, "Edit" );
	}
	else if( STRCMP_L( szEditStatus, "2" ))
	{
		strcpy( szEditStatus, "Delete" );
	}
	
	if( STRCMP_L( szModuleName, "CLT"))
	{
		strcpy( szModuleName, "CLUSTER");
	}
	
	nStrlength = strlen(szRecipeGroupID);

	strncpy(szRecipeGroupID, " ", 1);
	strncpy(szRecipeGroupID+nStrlength-1, " ", 1);
	
	if( READ_DIGITAL( Online_State, &nCommStatus) == 4 )
	{
		READ_STRING( Recipe_Edit_User , szCurrentUserName , &nCommStatus );
	}
	else
	{
		READ_STRING( CurrentUserName , szCurrentUserName , &nCommStatus );
	}
	
	LOG(pszFtnName, ELT_File, "%s	%s	%s	%s	%s", szModuleName, szRecipeGroupID, szRecipeName, szEditStatus, szCurrentUserName);
	
	READ_STRING( CurrentUserName , szCurrentUserName , &nCommStatus );
	WRITE_STRING( Recipe_Edit_User, szCurrentUserName, &nCommStatus);

	return SYS_SUCCESS;
}
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//---------------------------------------------------------------------------------------
void Event_Message_Received() {
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
