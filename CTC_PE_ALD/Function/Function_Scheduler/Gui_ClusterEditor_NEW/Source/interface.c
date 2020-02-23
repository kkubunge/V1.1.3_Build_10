#include <stdio.h>

#include <windows.h>
#include <commctrl.h>

#include <Kutlstr.h>
#include <Kutlgui.h>

#include "appinit.h"
#include "appprm.h"

#include "cimseqnc.h"
#include "resource.h"
#include "interface.h"
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
extern int	 MAX_VIEW_CHAMBER;

BOOL Get_Resource_LIST_PMFILE_Index_from_Resource_RELOAD_PM( int data , int *res , int *index ) {
	switch( data ) {
	case IDC_CTRL_RELOAD_PM1 :	*res = IDC_LIST_PM1FILE;	*index = 0;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM2 :	*res = IDC_LIST_PM2FILE;	*index = 1;		return TRUE;	break;
	case IDC_CTRL_RELOAD_PM3 :	*res = IDC_LIST_PM3FILE;	*index = 2;		return TRUE;	break;
	}
	return FALSE;
}

BOOL Get_Index_from_Resource_LIST_PMFILE( int data , int *index ) {

	switch( data ) {
	case IDC_LIST_PM1FILE :		*index = 0;		return TRUE;	break;
	case IDC_LIST_PM2FILE :		*index = 1;		return TRUE;	break;
	case IDC_LIST_PM3FILE :		*index = 2;		return TRUE;	break;
	default:
		printf("[ERROR] Get_Index_from_Resource_LIST_PMFILE Invalid Data[%d]\n", data);
		break;
	}

	return FALSE;
}
