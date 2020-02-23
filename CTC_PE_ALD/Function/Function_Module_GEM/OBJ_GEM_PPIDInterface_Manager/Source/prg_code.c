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
	{ "e:SCHEDULER"					, _K_S_IO	,	SCHEDULER					,	0	} , 
	{ "GEM.RECIPE_HISTORY_MANAGER"	, _K_F_IO	,	Recipe_Histroy_Manager		,	0	} ,
	{ "GEM.Recipe_Edit_User"		, _K_S_IO	,	Recipe_Edit_User			,	0	} ,
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// This Program is simply to check the PPID items for interlock
Module_Status PPID_Analysis( char *RecipeType , char *Data ) { 
	char UpDownType[256] , FullRecipeName[256];
	char pszFtnName[256] = "";
	char szHost[256] = "HOST";
	int nCommStatus;

	HFILE hFile;
	STR_SEPERATE_CHAR( Data , '|' , UpDownType , FullRecipeName	, 255 );

	LOG(pszFtnName, ELT_File, "===> 1 %s	%s", UpDownType, FullRecipeName);

	if		( STRCMP_L( RecipeType , "LOT" ) ) {
		if		( STRCMP_L( UpDownType , "DOWN" ) ) { // S7F23 => Total Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "DOWN2" ) ) { // S7F63 => Partial Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP" ) ) { // S7F25 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP2" ) ) { // S7F65 => Partial Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP3" ) ) { // S7F65 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
	}
	else if ( STRCMP_L( RecipeType , "CLT" ) ) {
		if		( STRCMP_L( UpDownType , "DOWN" ) ) { // S7F23 => Total Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "DOWN2" ) ) { // S7F63 => Partial Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP" ) ) { // S7F25 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP2" ) ) { // S7F65 => Partial Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP3" ) ) { // S7F65 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
	}
	else if ( STRCMP_L( RecipeType , "PM1" ) ) { // you can add any other pm
		if		( STRCMP_L( UpDownType , "DOWN" ) ) { // S7F23 => Total Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "DOWN2" ) ) { // S7F63 => Partial Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP" ) ) { // S7F25 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP2" ) ) { // S7F65 => Partial Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP3" ) ) { // S7F65 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
	}
	else if ( STRCMP_L( RecipeType , "PM2" ) ) { // you can add any other pm
		if		( STRCMP_L( UpDownType , "DOWN" ) ) { // S7F23 => Total Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "DOWN2" ) ) { // S7F63 => Partial Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP" ) ) { // S7F25 => Total Upload
			LOG(pszFtnName, ELT_File, "===> 2 %s	%s", UpDownType, FullRecipeName);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 )
			{
				LOG(pszFtnName, ELT_File, "===> 3 %s	%s", UpDownType, FullRecipeName);
				return SYS_SUCCESS; 
			}
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP2" ) ) { // S7F65 => Partial Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP3" ) ) { // S7F65 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
	}
	else if ( STRCMP_L( RecipeType , "PM3" ) ) { // you can add any other pm
		if		( STRCMP_L( UpDownType , "DOWN" ) ) { // S7F23 => Total Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "DOWN2" ) ) { // S7F63 => Partial Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP" ) ) { // S7F25 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP2" ) ) { // S7F65 => Partial Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP3" ) ) { // S7F65 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
	}
	else if ( STRCMP_L( RecipeType , "PM4" ) ) { // you can add any other pm
		if		( STRCMP_L( UpDownType , "DOWN" ) ) { // S7F23 => Total Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "DOWN2" ) ) { // S7F63 => Partial Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP" ) ) { // S7F25 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP2" ) ) { // S7F65 => Partial Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP3" ) ) { // S7F65 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
	}
	else if ( STRCMP_L( RecipeType , "PM5" ) ) { // you can add any other pm
		if		( STRCMP_L( UpDownType , "DOWN" ) ) { // S7F23 => Total Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "DOWN2" ) ) { // S7F63 => Partial Down
			WRITE_STRING( Recipe_Edit_User, szHost, &nCommStatus);
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP" ) ) { // S7F25 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP2" ) ) { // S7F65 => Partial Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
		else if	( STRCMP_L( UpDownType , "UP3" ) ) { // S7F65 => Total Upload
			hFile = _lopen( FullRecipeName , OF_READ );
			if ( hFile == -1 ) return SYS_SUCCESS; 
			_lclose( hFile );
		}
	}

	return SYS_SUCCESS;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	int	Dl;
	unsigned char *Ed;
	char Buffer[256] , Buffer2[256];
	char pszFtnName[256] = "";

	Ed = PROGRAM_PARAMETER_READ();
	Dl = strlen( Ed );
	Ed[ Dl ] = 0x00;

	if ( Dl < 1 ) return SYS_SUCCESS;

	STR_SEPERATE( Ed , Buffer , Buffer2	, 255 );

	return PPID_Analysis( Buffer , Buffer2 );
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Event_Message_Received() {
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
