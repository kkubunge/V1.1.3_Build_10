#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <KutlGui.h>

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {

}
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	if( !KWIN_DIALOG_DRIVER_LOADING() ) return FALSE;
	return TRUE;
}