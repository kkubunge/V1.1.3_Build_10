// StgHtrControlType2.cpp: implementation of the CStgHtrControlType2 class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//====================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//====================================================================

#include "TextParser.h"
#include "Definition.h"
#include "StgHtrControlType2.h"

//--------------------------------------------------------------------
// #include "DynamicIO.h"
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST g_pIOList[] =
{
	{ "HTA01_TmpRdAI"		, _K_A_IO	,	0	} ,
	{ "HTA01_TmpSpAIO"		, _K_A_IO	,	0	} ,
	{ "HTA01_TmpHgLmtAIO"	, _K_A_IO	,	0	} ,
	{ "HTA01_TmpLwLmtAIO"	, _K_A_IO	,	0	} ,
	{ "HTA_RampRateAIO"		, _K_A_IO	,	0	} ,
	{ "NotDefined"			, _K_A_IO	,	0	} ,
	{ "HTA01_StbStsDM"		, _K_D_IO	,	0	} ,
	// 2006.01.18
	{ "HTA01_CalOffsAIO"	, _K_A_IO	,	0	} ,
	""
};
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStgHtrControlType2::CStgHtrControlType2()
{

}

CStgHtrControlType2::~CStgHtrControlType2()
{

}

BOOL CStgHtrControlType2::Initialize()
{
	BOOL bRet = TRUE;
	if(! InitIOList(g_pIOList)) printf("-----> InitIOList Failed in %s\n", FUNC_NAME);
	return bRet;
}

BOOL CStgHtrControlType2::Initialize(char *szSHTNum)
{
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Member Function