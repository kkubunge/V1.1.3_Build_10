#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//--2005.12.13 by ycchoi -->>
#include "CimSeqnc.h"
#include "Kutlstr.h"
#include "Kutlgui2.h"
//--2005.12.13 by ycchoi --<<


//------------2005.11.04 by ycchoi ---------------------->>
extern BOOL gbUseGroupAuthority;
extern BOOL UseGroupAuthority();
//------------2005.11.04 by ycchoi ----------------------<<

//------------2005.12.13 by ycchoi ---------------------->>
extern HWND gMainHWnd;
//------------2005.12.13 by ycchoi ----------------------<<

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
HWND KGUI_STANDARD_Get_Screen_Handle( void );

//---> DEL 2012/08/24 Encryption by KSM
/*
void Event_Message_Received() 
{
	//------------2005.12.13 by ycchoi --------------------------->>
	char *Event;
	Event = PROGRAM_EVENT_READ( );
		
#ifdef _DEBUG
	_IO_CIM_PRINTF("RecipeEditPM1 :: EVENT RECIEVED[%s]\n", Event);
#endif

	// KGUI_STANDARD_Get_Screen_Handle() API는 최상위의 Dialog Window의 Handle을 가져온다.
	// WM_USER message는 반드시 Recipe Editor Main Dialog Window에게 전달 되어야 한다.
	//////////////////////////////////////////////////////////////////////////
//	SendMessage( (HWND) KGUI_STANDARD_Get_Screen_Handle() , WM_USER , WM_USER , 0 );
	
	SendMessage(gMainHWnd, WM_USER , WM_USER , 0);


	//------------2005.12.13 by ycchoi ---------------------------<<
}
*/

//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	return TRUE;
}

//-------------------------------------------------------------------------
void Program_Enter_Code() {
	//----- 2005.11.08 by ycchoi -------------->>
	gbUseGroupAuthority = UseGroupAuthority();
	//----- 2005.11.08 by ycchoi --------------<<
}
