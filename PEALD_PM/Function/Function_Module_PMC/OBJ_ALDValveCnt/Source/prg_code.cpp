#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "SeqInterface.h"
#include "ALDValveCnt.h"

#define FUNC_NAME		"ALDValveMon"
// CSeqInterface g_SeqInterface("SeqInterf");
CALDVlvMon g_SeqInterface;

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO		,		IO_Driver_Loding,		0		} ,

	//////////////////////////////////////////////////////////////////////////
	//... 2015.04.28 Fixed ALD Valve 13 EA (Top 10 EA, Canister 3 EA)
	{ "ALD97OpenCnAI"  	        , _K_A_IO       ,		ALD97OpenCnAI,  		0       } ,	
	{ "ALD98OpenCnAI"	        , _K_A_IO       ,       ALD98OpenCnAI,          0       } ,
	{ "ALD100OpenCnAI"	        , _K_A_IO       ,       ALD100OpenCnAI,	        0       } ,
	{ "ALD101OpenCnAI"	        , _K_A_IO       ,       ALD101OpenCnAI,	        0       } ,
	{ "ALD102OpenCnAI"	        , _K_A_IO       ,       ALD102OpenCnAI,	        0       } ,
	{ "ALD105OpenCnAI"	        , _K_A_IO       ,       ALD105OpenCnAI,	        0       } ,
	{ "ALD106OpenCnAI"	        , _K_A_IO       ,       ALD106OpenCnAI,	        0       } ,
	{ "ALD108OpenCnAI"	        , _K_A_IO       ,       ALD108OpenCnAI,	        0       } ,
	{ "ALD109OpenCnAI"	        , _K_A_IO       ,       ALD109OpenCnAI,	        0       } ,
	{ "ALD110OpenCnAI"	        , _K_A_IO       ,       ALD110OpenCnAI,	        0       } ,
	{ "ALD11OpenCnAI"  	        , _K_A_IO       ,		ALD11OpenCnAI,  		0       } ,	
	{ "ALD12OpenCnAI"  	        , _K_A_IO       ,		ALD12OpenCnAI,  		0       } ,	
	{ "ALD17OpenCnAI"  	        , _K_A_IO       ,		ALD17OpenCnAI,  		0       } ,	
	//
	{ "ALD97CloseCnAI"	        , _K_A_IO       ,       ALD97CloseCnAI,	        0       } ,
	{ "ALD98CloseCnAI"	        , _K_A_IO       ,       ALD98CloseCnAI,	        0       } ,
	{ "ALD100CloseCnAI"	        , _K_A_IO       ,       ALD100CloseCnAI,		0       } ,
	{ "ALD101CloseCnAI"	        , _K_A_IO       ,       ALD101CloseCnAI,		0       } ,
	{ "ALD102CloseCnAI"	        , _K_A_IO       ,       ALD102CloseCnAI,		0       } ,
	{ "ALD105CloseCnAI"	        , _K_A_IO       ,       ALD105CloseCnAI,		0       } ,
	{ "ALD106CloseCnAI"	        , _K_A_IO       ,       ALD106CloseCnAI,		0       } ,
	{ "ALD108CloseCnAI"	        , _K_A_IO       ,       ALD108CloseCnAI,		0       } ,
	{ "ALD109CloseCnAI"	        , _K_A_IO       ,       ALD109CloseCnAI,		0       } ,
	{ "ALD110CloseCnAI"	        , _K_A_IO       ,       ALD110CloseCnAI,		0       } ,
	{ "ALD11CloseCnAI"	        , _K_A_IO       ,       ALD11CloseCnAI,	        0       } ,
	{ "ALD12CloseCnAI"	        , _K_A_IO       ,       ALD12CloseCnAI,	        0       } ,
	{ "ALD17CloseCnAI"	        , _K_A_IO       ,       ALD17CloseCnAI,	        0       } ,
	//
	{ "ALD97OpenCnAM"  	        , _K_A_IO       ,		ALD97OpenCnAM,			0       } ,	
	{ "ALD98OpenCnAM"	        , _K_A_IO       ,       ALD98OpenCnAM,          0       } ,
	{ "ALD100OpenCnAM"	        , _K_A_IO       ,       ALD100OpenCnAM,	        0       } ,
	{ "ALD101OpenCnAM"	        , _K_A_IO       ,       ALD101OpenCnAM,	        0       } ,
	{ "ALD102OpenCnAM"	        , _K_A_IO       ,       ALD102OpenCnAM,	        0       } ,
	{ "ALD105OpenCnAM"	        , _K_A_IO       ,       ALD105OpenCnAM,	        0       } ,
	{ "ALD106OpenCnAM"	        , _K_A_IO       ,       ALD106OpenCnAM,	        0       } ,
	{ "ALD108OpenCnAM"	        , _K_A_IO       ,       ALD108OpenCnAM,	        0       } ,
	{ "ALD109OpenCnAM"	        , _K_A_IO       ,       ALD109OpenCnAM,	        0       } ,
	{ "ALD110OpenCnAM"	        , _K_A_IO       ,       ALD110OpenCnAM,	        0       } ,
	{ "ALD11OpenCnAM"  	        , _K_A_IO       ,		ALD11OpenCnAM,			0       } ,	
	{ "ALD12OpenCnAM"  	        , _K_A_IO       ,		ALD12OpenCnAM,			0       } ,	
	{ "ALD17OpenCnAM"  	        , _K_A_IO       ,		ALD17OpenCnAM,			0       } ,	
	//
	{ "ALD97CloseCnAM"	        , _K_A_IO       ,       ALD97CloseCnAM,	        0       } ,
	{ "ALD98CloseCnAM"	        , _K_A_IO       ,       ALD98CloseCnAM,	        0       } ,
	{ "ALD100CloseCnAM"	        , _K_A_IO       ,       ALD100CloseCnAM,		0       } ,
	{ "ALD101CloseCnAM"	        , _K_A_IO       ,       ALD101CloseCnAM,		0       } ,
	{ "ALD102CloseCnAM"	        , _K_A_IO       ,       ALD102CloseCnAM,		0       } ,
	{ "ALD105CloseCnAM"	        , _K_A_IO       ,       ALD105CloseCnAM,		0       } ,
	{ "ALD106CloseCnAM"	        , _K_A_IO       ,       ALD106CloseCnAM,		0       } ,
	{ "ALD108CloseCnAM"	        , _K_A_IO       ,       ALD108CloseCnAM,		0       } ,
	{ "ALD109CloseCnAM"	        , _K_A_IO       ,       ALD109CloseCnAM,		0       } ,
	{ "ALD110CloseCnAM"	        , _K_A_IO       ,       ALD110CloseCnAM,		0       } ,
	{ "ALD11CloseCnAM"	        , _K_A_IO       ,       ALD11CloseCnAM,	        0       } ,
	{ "ALD12CloseCnAM"	        , _K_A_IO       ,       ALD12CloseCnAM,	        0       } ,
	{ "ALD17CloseCnAM"	        , _K_A_IO       ,       ALD17CloseCnAM,	        0       } ,
	//
	{ "ALD97VlvRstTime"			, _K_S_IO		,		ALD97VlvRstTime,		0		} ,
	{ "ALD98VlvRstTime"			, _K_S_IO		,		ALD98VlvRstTime,		0		} ,
	{ "ALD100VlvRstTime"		, _K_S_IO		,		ALD100VlvRstTime,		0		} ,
	{ "ALD101VlvRstTime"		, _K_S_IO		,		ALD101VlvRstTime,		0		} ,
	{ "ALD102VlvRstTime"		, _K_S_IO		,		ALD102VlvRstTime,		0		} ,
	{ "ALD105VlvRstTime"		, _K_S_IO		,		ALD105VlvRstTime,		0		} ,
	{ "ALD106VlvRstTime"		, _K_S_IO		,		ALD106VlvRstTime,		0		} ,
	{ "ALD108VlvRstTime"		, _K_S_IO		,		ALD108VlvRstTime,		0		} ,
	{ "ALD109VlvRstTime"		, _K_S_IO		,		ALD109VlvRstTime,		0		} ,
	{ "ALD110VlvRstTime"		, _K_S_IO		,		ALD110VlvRstTime,		0		} ,
	{ "ALD11VlvRstTime"			, _K_S_IO		,		ALD11VlvRstTime,		0		} ,
	{ "ALD12VlvRstTime"			, _K_S_IO		,		ALD12VlvRstTime,		0		} ,
	{ "ALD17VlvRstTime"			, _K_S_IO		,		ALD17VlvRstTime,		0		} ,
	
	//////////////////////////////////////////////////////////////////////////
	//... Reset History List 10 Fixed
	{ "ALDVlvRstTime01"			, _K_S_IO		,		ALDResetTime01,			0		} ,
	{ "ALDVlvRstTime02"			, _K_S_IO		,		ALDResetTime02,			0		} ,
	{ "ALDVlvRstTime03"			, _K_S_IO		,		ALDResetTime03,			0		} ,
	{ "ALDVlvRstTime04"			, _K_S_IO		,		ALDResetTime04,			0		} ,
	{ "ALDVlvRstTime05"			, _K_S_IO		,		ALDResetTime05,			0		} ,
	{ "ALDVlvRstTime06"			, _K_S_IO		,		ALDResetTime06,			0		} ,
	{ "ALDVlvRstTime07"			, _K_S_IO		,		ALDResetTime07,			0		} ,
	{ "ALDVlvRstTime08"			, _K_S_IO		,		ALDResetTime08,			0		} ,
	{ "ALDVlvRstTime09"			, _K_S_IO		,		ALDResetTime09,			0		} ,
	{ "ALDVlvRstTime10"			, _K_S_IO		,		ALDResetTime10,			0		} ,
	//
	{ "ALDVlvRstCnt01"			, _K_D_IO		,		ALDVlvRstCnt01,   		0		} ,
	{ "ALDVlvRstCnt02"			, _K_D_IO		,		ALDVlvRstCnt02,   		0		} ,
	{ "ALDVlvRstCnt03"			, _K_D_IO		,		ALDVlvRstCnt03,   		0		} ,
	{ "ALDVlvRstCnt04"			, _K_D_IO		,		ALDVlvRstCnt04,   		0		} ,
	{ "ALDVlvRstCnt05"			, _K_D_IO		,		ALDVlvRstCnt05,   		0		} ,
	{ "ALDVlvRstCnt06"			, _K_D_IO		,		ALDVlvRstCnt06,   		0		} ,
	{ "ALDVlvRstCnt07"			, _K_D_IO		,		ALDVlvRstCnt07,   		0		} ,
	{ "ALDVlvRstCnt08"			, _K_D_IO		,		ALDVlvRstCnt08,   		0		} ,
	{ "ALDVlvRstCnt09"			, _K_D_IO		,		ALDVlvRstCnt09,   		0		} ,
	{ "ALDVlvRstCnt10"			, _K_D_IO		,		ALDVlvRstCnt10,   		0		} ,

	//////////////////////////////////////////////////////////////////////////
	{ "ALDVlvRstCnt"			, _K_D_IO		,		ALDResetCount,   		0		} ,

	{ "PRO_RunStsDM"	        , _K_D_IO       ,       PRO_RunStsDM,			0       } ,	
	
	""
};

//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	if(g_SeqInterface.m_bSimulMode)			return g_SeqInterface.Main();
	else									return g_SeqInterface.Main();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	g_SeqInterface.Enter_Code();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;

	if(READ_DIGITAL(IO_Driver_Loding, &CommStatus) == 0)	g_SeqInterface.m_bSimulMode = TRUE;
	else													g_SeqInterface.m_bSimulMode = FALSE;

	return g_SeqInterface.CSeqInterface::Initialize(FUNC_NAME);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		g_SeqInterface.Deinitialize();
	}
}

} // End of [extern "C"]
//==========================================================================================
