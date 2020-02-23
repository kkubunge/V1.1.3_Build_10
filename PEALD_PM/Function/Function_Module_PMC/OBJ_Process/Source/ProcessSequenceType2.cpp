// ProcessSequenceType2.cpp: implementation of the CProcessSequenceType2 class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <math.h>

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "RecipeStruct.h"
#include "ProcessSequenceType2.h"
#include "DynamicIO.h"

//------------------------------------------------------------------------------------------
// #include "TextLogDll.h"
extern void _LOG_RCP(const char *pFormat, ...);
extern void _LOG_PROC(const char *pFormat, ...);
extern void _LOG_RMAP(const char *pFormat, ...);
extern void _LOG_RCP_T(const char *pFormat,...);
extern void _LOG_ERR(const char *pFormat, ...);
#define _LOG_ERR_P(_szLog)	{_LOG_ERR(_szLog);printf("%s\n",_szLog);}

//------------------------------------------------------------------------------------------
#define PROC_TIME_STEP_DELAY	0 // 0.24
#define PROC_TIME_CYC_DELAY		0 // 0.13

//------------------------------------------------------------------------------------------
// Definition for 112 Channel Valve Recipe
// Valve Recipe Header Packet
#define VR_HDR_BASE			2
#define VRHD_MFC_SP_IDX		10
#define VRHD_MFC_VL_IDX		58
#define VRHD_RF_SP_IDX		64
#define VRHD_RF2_SP_IDX		66
#define VRHD_HTE_SP_IDX		68
#define STEP_TIME_SP_IDX    98

// Valve Recipe Step
#define	BASE_IDX			2
#define LINE_STEP_BASE		18
#define LINE_STEP_SIZE		16
//------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcessSequenceType2::CProcessSequenceType2()
{
	m_nMaxValve = 112;			// Valve Recipe 112 Channel
	m_nMaxMfc = 24;				// MFC 24 Channel

	// 2006.05.17
	m_nVR_HDR_BASE			= VR_HDR_BASE;
	m_nVRHD_MFC_SP_IDX		= VRHD_MFC_SP_IDX;
	m_nVRHD_MFC_VL_IDX		= VRHD_MFC_VL_IDX;
	m_nVRHD_RF_SP_IDX		= VRHD_RF_SP_IDX;
	m_nVRHD_RF2_SP_IDX		= VRHD_RF2_SP_IDX;
	m_nVRHD_HTE_SP_IDX		= VRHD_HTE_SP_IDX;		// 2014.05.22
	m_nBASE_IDX				= BASE_IDX;
	m_nLINE_STEP_BASE		= LINE_STEP_BASE;
	m_nLINE_STEP_SIZE		= LINE_STEP_SIZE;
}

CProcessSequenceType2::~CProcessSequenceType2()
{
}

//////////////////////////////////////////////////////////////////////
// Member Function

