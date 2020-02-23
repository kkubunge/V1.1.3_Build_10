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
#include "KeySensorMon.h"

#define FUNC_NAME		"KeySensorMon"
// CSeqInterface g_SeqInterface("SeqInterf");
CKeySensorMon g_SeqInterface;

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding,		0	},

	{ "CTC.RECIPE_FILE"	        , _K_S_IO   ,   CTC_RECIPE_FILE,		0   },	
	{ "CTC.RECIPE_FILE2"	    , _K_S_IO   ,   CTC_RECIPE_FILE2,		0   },	
	{ "CTC.RECIPE_FILE3"	    , _K_S_IO   ,   CTC_RECIPE_FILE3,		0   },	
	{ "CTC.MID_NAME"	        , _K_S_IO   ,   CTC_MID_NAME,			0   },	
	{ "CTC.MID_NAME2"	        , _K_S_IO   ,   CTC_MID_NAME2,			0   },	
	{ "CTC.MID_NAME3"	        , _K_S_IO   ,   CTC_MID_NAME3,			0   },	

	{ "CTC.FA_Wafer_Status"	    , _K_D_IO	,	CTC_FA_Wafer_Status,    0 	},
	{ "CTC.FB_Wafer_Status"	    , _K_D_IO	,	CTC_FB_Wafer_Status,	0 	},
	{ "CTC.FM_AL_Wafer_Status"	, _K_D_IO	,	CTC_FM_AL_Wafer_Status,	0 	},
	{ "CTC.FA_Wafer_Source"	    , _K_D_IO	,	CTC_FA_Wafer_Source,	0 	},
	{ "CTC.FB_Wafer_Source"	    , _K_D_IO	,	CTC_FB_Wafer_Source,	0 	},
	{ "CTC.FM_AL_Wafer_Source"	, _K_D_IO	,	CTC_FM_AL_Wafer_Source, 0 	},	

	{ "FA_Wafer_StatusDM"	    , _K_D_IO	,	FA_Wafer_StatusDM,		0 	},
	{ "FB_Wafer_StatusDM"	    , _K_D_IO	,	FB_Wafer_StatusDM,		0 	},
	{ "FM_AL_Wafer_StatusDM"	, _K_D_IO	,	FM_AL_Wafer_StatusDM,   0 	},
	{ "FA_Wafer_SourceDM"	    , _K_D_IO	,	FA_Wafer_SourceDM,		0 	},
	{ "FB_Wafer_SourceDM"	    , _K_D_IO	,	FB_Wafer_SourceDM,		0 	},
	{ "FM_AL_Wafer_SourceDM"	, _K_D_IO	,	FM_AL_Wafer_SourceDM,   0 	},	

	{ "FA_Wafer_PPIDSM"			, _K_S_IO	,	FA_Wafer_PPIDSM,		0 	},
	{ "FB_Wafer_PPIDSM"			, _K_S_IO	,	FB_Wafer_PPIDSM,		0 	},
	{ "FM_AL_Wafer_PPIDSM"		, _K_S_IO	,	FM_AL_Wafer_PPIDSM,		0 	},
	{ "FA_Wafer_LOTIDSM"	    , _K_S_IO	,	FA_Wafer_LOTIDSM,		0 	},
	{ "FB_Wafer_LOTIDSM"	    , _K_S_IO	,	FB_Wafer_LOTIDSM,		0 	},
	{ "FM_AL_Wafer_LOTIDSM"		, _K_S_IO	,	FM_AL_Wafer_LOTIDSM,	0 	},	
	

	{ "CTC.TA_Wafer_Status"	    , _K_D_IO	,	CTC_TA_Wafer_Status,	0 	},
	{ "CTC.TA_Wafer_Status2"	, _K_D_IO	,	CTC_TA_Wafer_Status2,	0 	},
	{ "CTC.TB_Wafer_Status"	    , _K_D_IO	,	CTC_TB_Wafer_Status,	0 	},
	{ "CTC.TB_Wafer_Status2"	, _K_D_IO	,	CTC_TB_Wafer_Status2,	0 	},
	{ "CTC.TA_Wafer_Source"	    , _K_D_IO	,	CTC_TA_Wafer_Source,	0 	},
	{ "CTC.TA_Wafer_Source2"	, _K_D_IO	,	CTC_TA_Wafer_Source2,	0 	},
	{ "CTC.TB_Wafer_Source"	    , _K_D_IO	,	CTC_TB_Wafer_Source,	0 	},
	{ "CTC.TB_Wafer_Source2"	, _K_D_IO	,	CTC_TB_Wafer_Source2,	0 	},

	{ "TA_Wafer_StatusDM"	    , _K_D_IO	,	TA_Wafer_StatusDM,		0 	},
	{ "TA_Wafer_Status2DM"		, _K_D_IO	,	TA_Wafer_Status2DM,		0 	},
	{ "TB_Wafer_StatusDM"	    , _K_D_IO	,	TB_Wafer_StatusDM,		0 	},
	{ "TB_Wafer_Status2DM"		, _K_D_IO	,	TB_Wafer_Status2DM,		0 	},
	{ "TA_Wafer_SourceDM"	    , _K_D_IO	,	TA_Wafer_SourceDM,		0 	},
	{ "TA_Wafer_Source2DM"		, _K_D_IO	,	TA_Wafer_Source2DM,		0 	},
	{ "TB_Wafer_SourceDM"	    , _K_D_IO	,	TB_Wafer_SourceDM,		0 	},
	{ "TB_Wafer_Source2DM"		, _K_D_IO	,	TB_Wafer_Source2DM,		0 	},
	{ "TA_Wafer_PPIDSM"			, _K_S_IO	,	TA_Wafer_PPIDSM,		0 	},
	{ "TA_Wafer_PPID2SM"		, _K_S_IO	,	TA_Wafer_PPID2SM,		0 	},
	{ "TB_Wafer_PPIDSM"			, _K_S_IO	,	TB_Wafer_PPIDSM,		0 	},
	{ "TB_Wafer_PPID2SM"		, _K_S_IO	,	TB_Wafer_PPID2SM,		0 	},
	{ "TA_Wafer_LOTIDSM"	    , _K_S_IO	,	TA_Wafer_LOTIDSM,		0 	},
	{ "TA_Wafer_LOTID2SM"		, _K_S_IO	,	TA_Wafer_LOTID2SM,		0 	},
	{ "TB_Wafer_LOTIDSM"	  	, _K_S_IO	,	TB_Wafer_LOTIDSM,		0 	},
	{ "TB_Wafer_LOTID2SM"		, _K_S_IO	,	TB_Wafer_LOTID2SM,		0 	},


	{ "CTC.BM1_Wafer_Status"	, _K_D_IO	,	CTC_BM1_Wafer_Status,	0 	},
	{ "CTC.BM1_Wafer_Status2"	, _K_D_IO	,	CTC_BM1_Wafer_Status2,	0 	},
	{ "CTC.BM1_Wafer_Status3"	, _K_D_IO	,	CTC_BM1_Wafer_Status3,	0 	},
	{ "CTC.BM1_Wafer_Status4"	, _K_D_IO	,	CTC_BM1_Wafer_Status4,	0 	},
	{ "CTC.BM2_Wafer_Status"	, _K_D_IO	,	CTC_BM2_Wafer_Status,	0 	},
	{ "CTC.BM2_Wafer_Status2"	, _K_D_IO	,	CTC_BM2_Wafer_Status2,	0 	},
	{ "CTC.BM2_Wafer_Status3"	, _K_D_IO	,	CTC_BM2_Wafer_Status3,	0 	},
	{ "CTC.BM2_Wafer_Status4"	, _K_D_IO	,	CTC_BM2_Wafer_Status4,	0 	},
	{ "CTC.BM1_Wafer_Source"	, _K_D_IO	,	CTC_BM1_Wafer_Source,	0 	},
	{ "CTC.BM1_Wafer_Source2"	, _K_D_IO	,	CTC_BM1_Wafer_Source2,	0 	},
	{ "CTC.BM1_Wafer_Source3"	, _K_D_IO	,	CTC_BM1_Wafer_Source3,	0 	},
	{ "CTC.BM1_Wafer_Source4"	, _K_D_IO	,	CTC_BM1_Wafer_Source4,	0 	},
	{ "CTC.BM2_Wafer_Source"	, _K_D_IO	,	CTC_BM2_Wafer_Source,	0 	},
	{ "CTC.BM2_Wafer_Source2"	, _K_D_IO	,	CTC_BM2_Wafer_Source2,	0 	},
	{ "CTC.BM2_Wafer_Source3"	, _K_D_IO	,	CTC_BM2_Wafer_Source3,	0 	},
	{ "CTC.BM2_Wafer_Source4"	, _K_D_IO	,	CTC_BM2_Wafer_Source4,	0 	},

	{ "BM1_Wafer_StatusDM"		, _K_D_IO	,	BM1_Wafer_StatusDM,		0 	},
	{ "BM1_Wafer_Status2DM"		, _K_D_IO	,	BM1_Wafer_Status2DM,	0 	},
	{ "BM1_Wafer_Status3DM"		, _K_D_IO	,	BM1_Wafer_Status3DM,	0 	},
	{ "BM1_Wafer_Status4DM"		, _K_D_IO	,	BM1_Wafer_Status4DM,	0 	},
	{ "BM2_Wafer_StatusDM"		, _K_D_IO	,	BM2_Wafer_StatusDM,		0 	},
	{ "BM2_Wafer_Status2DM"		, _K_D_IO	,	BM2_Wafer_Status2DM,	0 	},
	{ "BM2_Wafer_Status3DM"		, _K_D_IO	,	BM2_Wafer_Status3DM,	0 	},
	{ "BM2_Wafer_Status4DM"		, _K_D_IO	,	BM2_Wafer_Status4DM,	0 	},
	{ "BM1_Wafer_SourceDM"		, _K_D_IO	,	BM1_Wafer_SourceDM,		0 	},
	{ "BM1_Wafer_Source2DM"		, _K_D_IO	,	BM1_Wafer_Source2DM,	0 	},
	{ "BM1_Wafer_Source3DM"		, _K_D_IO	,	BM1_Wafer_Source3DM,	0 	},
	{ "BM1_Wafer_Source4DM"		, _K_D_IO	,	BM1_Wafer_Source4DM,	0 	},
	{ "BM2_Wafer_SourceDM"		, _K_D_IO	,	BM2_Wafer_SourceDM,		0 	},
	{ "BM2_Wafer_Source2DM"		, _K_D_IO	,	BM2_Wafer_Source2DM,	0 	},
	{ "BM2_Wafer_Source3DM"		, _K_D_IO	,	BM2_Wafer_Source3DM,	0 	},
	{ "BM2_Wafer_Source4DM"		, _K_D_IO	,	BM2_Wafer_Source4DM,	0 	},

	{ "BM1_Wafer_PPIDSM"		, _K_S_IO	,	BM1_Wafer_PPIDSM,		0 	},
	{ "BM1_Wafer_PPID2SM"		, _K_S_IO	,	BM1_Wafer_PPID2SM,	0 	},
	{ "BM1_Wafer_PPID3SM"		, _K_S_IO	,	BM1_Wafer_PPID3SM,	0 	},
	{ "BM1_Wafer_PPID4SM"		, _K_S_IO	,	BM1_Wafer_PPID4SM,	0 	},
	{ "BM2_Wafer_PPIDSM"		, _K_S_IO	,	BM2_Wafer_PPIDSM,		0 	},
	{ "BM2_Wafer_PPID2SM"		, _K_S_IO	,	BM2_Wafer_PPID2SM,	0 	},
	{ "BM2_Wafer_PPID3SM"		, _K_S_IO	,	BM2_Wafer_PPID3SM,	0 	},
	{ "BM2_Wafer_PPID4SM"		, _K_S_IO	,	BM2_Wafer_PPID4SM,	0 	},
	{ "BM1_Wafer_LOTIDSM"		, _K_S_IO	,	BM1_Wafer_LOTIDSM,		0 	},
	{ "BM1_Wafer_LOTID2SM"		, _K_S_IO	,	BM1_Wafer_LOTID2SM,	0 	},
	{ "BM1_Wafer_LOTID3SM"		, _K_S_IO	,	BM1_Wafer_LOTID3SM,	0 	},
	{ "BM1_Wafer_LOTID4SM"		, _K_S_IO	,	BM1_Wafer_LOTID4SM,	0 	},
	{ "BM2_Wafer_LOTIDSM"		, _K_S_IO	,	BM2_Wafer_LOTIDSM,		0 	},
	{ "BM2_Wafer_LOTID2SM"		, _K_S_IO	,	BM2_Wafer_LOTID2SM,	0 	},
	{ "BM2_Wafer_LOTID3SM"		, _K_S_IO	,	BM2_Wafer_LOTID3SM,	0 	},
	{ "BM2_Wafer_LOTID4SM"		, _K_S_IO	,	BM2_Wafer_LOTID4SM,	0 	},

	{ "TM.VacuumStatusDM"		, _K_D_IO	,	TM_VacuumStatusDM,	0 	},
	{ "BM1.VacuumStatusDM"		, _K_D_IO	,	BM1_VacuumStatusDM,	0 	},
	{ "BM2.VacuumStatusDM"		, _K_D_IO	,	BM2_VacuumStatusDM,	0 	},

//	{ "Key_TMVacuumStatusDM"	, _K_D_IO	,	Key_TMVacuumStatusDM,	0 	},
//	{ "Key_BM1VacuumStatusDM"	, _K_D_IO	,	Key_BM1VacuumStatusDM,	0 	},
//	{ "Key_BM2VacuumStatusDM"	, _K_D_IO	,	Key_BM2VacuumStatusDM,	0 	},

	{ "Key_TMVacuumStepSM"		, _K_S_IO	,	Key_TMVacuumStepSM,		0 	},
	{ "Key_BM1VacuumStepSM"		, _K_S_IO	,	Key_BM1VacuumStepSM,	0 	},
	{ "Key_BM2VacuumStepSM"		, _K_S_IO	,	Key_BM2VacuumStepSM,	0 	},

	{ "TM.CGChmPrsAI"			, _K_A_IO	,	TM_CGChmPrsAI,			0 	},
	{ "BM1.CGChmPrsAI"			, _K_A_IO	,	BM1_CGChmPrsAI,			0 	},	
	{ "BM2.CGChmPrsAI"			, _K_A_IO	,	BM2_CGChmPrsAI,			0 	},
	
	{ "PRMA_BM1_SVENT_PRES"		, _K_A_IO	,	PRMA_BM1_SVENT_PRES,	0 	},
	{ "PRMA_BM1_FVENT_PRES"		, _K_A_IO	,	PRMA_BM1_FVENT_PRES,	0 	},
	{ "PRMA_BM1_SPUMP_PRES"		, _K_A_IO	,	PRMA_BM1_SPUMP_PRES,	0 	},
	{ "PRMA_BM1_FPUMP_PRES"		, _K_A_IO	,	PRMA_BM1_FPUMP_PRES,	0 	},

	{ "PRMA_BM2_SVENT_PRES"		, _K_A_IO	,	PRMA_BM2_SVENT_PRES,	0 	},
	{ "PRMA_BM2_FVENT_PRES"		, _K_A_IO	,	PRMA_BM2_FVENT_PRES,	0 	},
	{ "PRMA_BM2_SPUMP_PRES"		, _K_A_IO	,	PRMA_BM2_SPUMP_PRES,	0 	},
	{ "PRMA_BM2_FPUMP_PRES"		, _K_A_IO	,	PRMA_BM2_FPUMP_PRES,	0 	},

	{ "PRMA_TM_SVENT_PRES"		, _K_A_IO	,	PRMA_TM_SVENT_PRES,		0 	},
	{ "PRMA_TM_FVENT_PRES"		, _K_A_IO	,	PRMA_TM_FVENT_PRES,		0 	},
	{ "PRMA_TM_SPUMP_PRES"		, _K_A_IO	,	PRMA_TM_SPUMP_PRES,		0 	},
	{ "PRMA_TM_FPUMP_PRES"		, _K_A_IO	,	PRMA_TM_FPUMP_PRES,		0 	},
	//...2015.06.10
	{ "PM1.RF1FreqAvrAI"		, _K_A_IO	,	PM1_RF1FreqAvrAI,		0 	},
	{ "PM1.RF1RefPwAvrAI"		, _K_A_IO	,	PM1_RF1RefPwAvrAI,		0 	},
	{ "PM1.RF1LoadPwAvrAI"		, _K_A_IO	,	PM1_RF1LoadPwAvrAI,		0 	},
	{ "PM1.RF2RefPwAvrAI"		, _K_A_IO	,	PM1_RF2RefPwAvrAI,		0 	},
	{ "PM1.RF2FreqAvrAI"		, _K_A_IO	,	PM1_RF2FreqAvrAI,		0 	},
	{ "PM1.RF2LoadPwAvrAI"		, _K_A_IO	,	PM1_RF2LoadPwAvrAI,		0 	},	

	{ "PM1.HF_LOADPOSAvr_A"		, _K_A_IO	,	PM1_HF1LoadPosAvr,		0	},    	
	{ "PM1.HF_TunePOSAvr_A"		, _K_A_IO	,	PM1_HF1TunePosAvr,		0	},
	{ "PM1.HF_Z_RAvr_A"			, _K_A_IO	,	PM1_HF1Z_ScanRAvr,		0	},
	{ "PM1.HF_Z_XAvr_A"			, _K_A_IO	,	PM1_HF1Z_ScanXAvr,		0	},
	{ "PM1.HF_Z_VrmsAvr_A"		, _K_A_IO	,	PM1_HF1Z_ScanVltAvr,	0	},
	{ "PM1.HF_Z_IrmsAvr_A"		, _K_A_IO	,	PM1_HF1Z_ScanCurAvr,	0	},
	{ "PM1.HF_Z_PhaseAvr_A"		, _K_A_IO	,	PM1_HF1Z_ScanPhsAvr,	0	},
	{ "PM1.HF_Z_PowerAvr_A"		, _K_A_IO	,	PM1_HF1Z_ScanPwrAvr,	0	},

	{ "PM1.HF_LOADPOSAvr_B"		, _K_A_IO	,	PM1_HF2LoadPosAvr,		0	},    	
	{ "PM1.HF_TunePOSAvr_B"		, _K_A_IO	,	PM1_HF2TunePosAvr,		0	},
	{ "PM1.HF_Z_RAvr_B"			, _K_A_IO	,	PM1_HF2Z_ScanRAvr,		0	},
	{ "PM1.HF_Z_XAvr_B"			, _K_A_IO	,	PM1_HF2Z_ScanXAvr,		0	},
	{ "PM1.HF_Z_VrmsAvr_B"		, _K_A_IO	,	PM1_HF2Z_ScanVltAvr,	0	},
	{ "PM1.HF_Z_IrmsAvr_B"		, _K_A_IO	,	PM1_HF2Z_ScanCurAvr,	0	},
	{ "PM1.HF_Z_PhaseAvr_B"		, _K_A_IO	,	PM1_HF2Z_ScanPhsAvr,	0	},
	{ "PM1.HF_Z_PowerAvr_B"		, _K_A_IO	,	PM1_HF2Z_ScanPwrAvr,	0	},

	{ "PM2.RF1FreqAvrAI"		, _K_A_IO	,	PM2_RF1FreqAvrAI,		0 	},
	{ "PM2.RF1RefPwAvrAI"		, _K_A_IO	,	PM2_RF1RefPwAvrAI,		0 	},
	{ "PM2.RF1LoadPwAvrAI"		, _K_A_IO	,	PM2_RF1LoadPwAvrAI,		0 	},	
	{ "PM2.RF2FreqAvrAI"		, _K_A_IO	,	PM2_RF2FreqAvrAI,		0 	},
	{ "PM2.RF2RefPwAvrAI"		, _K_A_IO	,	PM2_RF2RefPwAvrAI,		0 	},
	{ "PM2.RF2LoadPwAvrAI"		, _K_A_IO	,	PM2_RF2LoadPwAvrAI,		0 	},	

	{ "PM2.HF_LOADPOSAvr_A"		, _K_A_IO	,	PM2_HF1LoadPosAvr,		0	},
	{ "PM2.HF_TunePOSAvr_A"		, _K_A_IO	,	PM2_HF1TunePosAvr,		0	},
	{ "PM2.HF_Z_RAvr_A"			, _K_A_IO	,	PM2_HF1Z_ScanRAvr,		0	},
	{ "PM2.HF_Z_XAvr_A"			, _K_A_IO	,	PM2_HF1Z_ScanXAvr,		0	},
	{ "PM2.HF_Z_VrmsAvr_A"		, _K_A_IO	,	PM2_HF1Z_ScanVltAvr,	0	},
	{ "PM2.HF_Z_IrmsAvr_A"		, _K_A_IO	,	PM2_HF1Z_ScanCurAvr,	0	},
	{ "PM2.HF_Z_PhaseAvr_A"		, _K_A_IO	,	PM2_HF1Z_ScanPhsAvr,	0	},
	{ "PM2.HF_Z_PowerAvr_A"		, _K_A_IO	,	PM2_HF1Z_ScanPwrAvr,	0	},

	{ "PM2.HF_LOADPOSAvr_B"		, _K_A_IO	,	PM2_HF2LoadPosAvr,		0	},
	{ "PM2.HF_TunePOSAvr_B"		, _K_A_IO	,	PM2_HF2TunePosAvr,		0	},
	{ "PM2.HF_Z_RAvr_B"			, _K_A_IO	,	PM2_HF2Z_ScanRAvr,		0	},
	{ "PM2.HF_Z_XAvr_B"			, _K_A_IO	,	PM2_HF2Z_ScanXAvr,		0	},
	{ "PM2.HF_Z_VrmsAvr_B"		, _K_A_IO	,	PM2_HF2Z_ScanVltAvr,	0	},
	{ "PM2.HF_Z_IrmsAvr_B"		, _K_A_IO	,	PM2_HF2Z_ScanCurAvr,	0	},
	{ "PM2.HF_Z_PhaseAvr_B"		, _K_A_IO	,	PM2_HF2Z_ScanPhsAvr,	0	},
	{ "PM2.HF_Z_PowerAvr_B"		, _K_A_IO	,	PM2_HF2Z_ScanPwrAvr,	0	},

	{ "PM3.RF1FreqAvrAI"		, _K_A_IO	,	PM3_RF1FreqAvrAI,		0 	},
	{ "PM3.RF1RefPwAvrAI"		, _K_A_IO	,	PM3_RF1RefPwAvrAI,		0 	},
	{ "PM3.RF1LoadPwAvrAI"		, _K_A_IO	,	PM3_RF1LoadPwAvrAI,		0 	},	
	{ "PM3.RF2FreqAvrAI"		, _K_A_IO	,	PM3_RF2FreqAvrAI,		0 	},	
	{ "PM3.RF2RefPwAvrAI"		, _K_A_IO	,	PM3_RF2RefPwAvrAI,		0 	},
	{ "PM3.RF2LoadPwAvrAI"		, _K_A_IO	,	PM3_RF2LoadPwAvrAI,		0 	},

	{ "PM3.HF_LOADPOSAvr_A"		, _K_A_IO	,	PM3_HF1LoadPosAvr,		0	},
	{ "PM3.HF_TunePOSAvr_A"		, _K_A_IO	,	PM3_HF1TunePosAvr,		0	},
	{ "PM3.HF_Z_RAvr_A"			, _K_A_IO	,	PM3_HF1Z_ScanRAvr,		0	},
	{ "PM3.HF_Z_XAvr_A"			, _K_A_IO	,	PM3_HF1Z_ScanXAvr,		0	},
	{ "PM3.HF_Z_VrmsAvr_A"		, _K_A_IO	,	PM3_HF1Z_ScanVltAvr,	0	},
	{ "PM3.HF_Z_IrmsAvr_A"		, _K_A_IO	,	PM3_HF1Z_ScanCurAvr,	0	},
	{ "PM3.HF_Z_PhaseAvr_A"		, _K_A_IO	,	PM3_HF1Z_ScanPhsAvr,	0	},
	{ "PM3.HF_Z_PowerAvr_A"		, _K_A_IO	,	PM3_HF1Z_ScanPwrAvr,	0	},

	{ "PM3.HF_LOADPOSAvr_B"		, _K_A_IO	,	PM3_HF2LoadPosAvr,		0	},
	{ "PM3.HF_TunePOSAvr_B"		, _K_A_IO	,	PM3_HF2TunePosAvr,		0	},
	{ "PM3.HF_Z_RAvr_B"			, _K_A_IO	,	PM3_HF2Z_ScanRAvr,		0	},
	{ "PM3.HF_Z_XAvr_B"			, _K_A_IO	,	PM3_HF2Z_ScanXAvr,		0	},
	{ "PM3.HF_Z_VrmsAvr_B"		, _K_A_IO	,	PM3_HF2Z_ScanVltAvr,	0	},
	{ "PM3.HF_Z_IrmsAvr_B"		, _K_A_IO	,	PM3_HF2Z_ScanCurAvr,	0	},
	{ "PM3.HF_Z_PhaseAvr_B"		, _K_A_IO	,	PM3_HF2Z_ScanPhsAvr,	0	},
	{ "PM3.HF_Z_PowerAvr_B"		, _K_A_IO	,	PM3_HF2Z_ScanPwrAvr,	0	},

	//...PM1
	{ "PM1_RF1FreqLAvrAI"		, _K_A_IO	,	PM1_RF1FreqLAvrAI,		0 	},
	{ "PM1_RF1RefPwLAvrAI"		, _K_A_IO	,	PM1_RF1RefPwLAvrAI,		0 	},
	{ "PM1_RF1LoadPwLAvrAI"	    , _K_A_IO	,	PM1_RF1LoadPwLAvrAI,	0 	},
	{ "PM1_RF2FreqLAvrAI"		, _K_A_IO	,	PM1_RF2FreqLAvrAI,		0 	},
	{ "PM1_RF2RefPwLAvrAI"		, _K_A_IO	,	PM1_RF2RefPwLAvrAI,		0 	},	
	{ "PM1_RF2LoadPwLAvrAI"  	, _K_A_IO	,	PM1_RF2LoadPwLAvrAI,	0 	},

	{ "PM1_HF1LoadPosLAvr"		, _K_A_IO	,	PM1_HF1LoadPosLAvr,		0	},
	{ "PM1_HF1TunePosLAvr"		, _K_A_IO	,	PM1_HF1TunePosLAvr,		0	},
	{ "PM1_HF1Z_ScanRLAvr"		, _K_A_IO	,	PM1_HF1Z_ScanRLAvr,		0	},
	{ "PM1_HF1Z_ScanXLAvr"		, _K_A_IO	,	PM1_HF1Z_ScanXLAvr,		0	},
	{ "PM1_HF1Z_ScanVltLAvr"	, _K_A_IO	,	PM1_HF1Z_ScanVltLAvr,	0	},
	{ "PM1_HF1Z_ScanCurLAvr"	, _K_A_IO	,	PM1_HF1Z_ScanCurLAvr,	0	},
	{ "PM1_HF1Z_ScanPhsLAvr"	, _K_A_IO	,	PM1_HF1Z_ScanPhsLAvr,	0	},
	{ "PM1_HF1Z_ScanPwrLAvr"	, _K_A_IO	,	PM1_HF1Z_ScanPwrLAvr,	0	},

	{ "PM1_HF2LoadPosLAvr"		, _K_A_IO	,	PM1_HF2LoadPosLAvr,		0	},
	{ "PM1_HF2TunePosLAvr"		, _K_A_IO	,	PM1_HF2TunePosLAvr,		0	},
	{ "PM1_HF2Z_ScanRLAvr"		, _K_A_IO	,	PM1_HF2Z_ScanRLAvr,		0	},
	{ "PM1_HF2Z_ScanXLAvr"		, _K_A_IO	,	PM1_HF2Z_ScanXLAvr,		0	},
	{ "PM1_HF2Z_ScanVltLAvr"	, _K_A_IO	,	PM1_HF2Z_ScanVltLAvr,	0	},
	{ "PM1_HF2Z_ScanCurLAvr"	, _K_A_IO	,	PM1_HF2Z_ScanCurLAvr,	0	},
	{ "PM1_HF2Z_ScanPhsLAvr"	, _K_A_IO	,	PM1_HF2Z_ScanPhsLAvr,	0	},
	{ "PM1_HF2Z_ScanPwrLAvr"	, _K_A_IO	,	PM1_HF2Z_ScanPwrLAvr,	0	},
	//...PM2
	{ "PM2_RF1FreqLAvrAI"		, _K_A_IO	,	PM2_RF1FreqLAvrAI,		0 	},
	{ "PM2_RF1RefPwLAvrAI"		, _K_A_IO	,	PM2_RF1RefPwLAvrAI,		0 	},
	{ "PM2_RF1LoadPwLAvrAI"		, _K_A_IO	,	PM2_RF1LoadPwLAvrAI,	0 	},
	{ "PM2_RF2FreqLAvrAI"		, _K_A_IO	,	PM2_RF2FreqLAvrAI,		0 	},
	{ "PM2_RF2RefPwLAvrAI"		, _K_A_IO	,	PM2_RF2RefPwLAvrAI,		0 	},
	{ "PM2_RF2LoadPwLAvrAI"		, _K_A_IO	,	PM2_RF2LoadPwLAvrAI,	0 	},

	{ "PM2_HF1LoadPosLAvr"		, _K_A_IO	,	PM2_HF1LoadPosLAvr,		0	},
	{ "PM2_HF1TunePosLAvr"		, _K_A_IO	,	PM2_HF1TunePosLAvr,		0	},
	{ "PM2_HF1Z_ScanRLAvr"		, _K_A_IO	,	PM2_HF1Z_ScanRLAvr,		0	},
	{ "PM2_HF1Z_ScanXLAvr"		, _K_A_IO	,	PM2_HF1Z_ScanXLAvr,		0	},
	{ "PM2_HF1Z_ScanVltLAvr"	, _K_A_IO	,	PM2_HF1Z_ScanVltLAvr,	0	},
	{ "PM2_HF1Z_ScanCurLAvr"	, _K_A_IO	,	PM2_HF1Z_ScanCurLAvr,	0	},
	{ "PM2_HF1Z_ScanPhsLAvr"	, _K_A_IO	,	PM2_HF1Z_ScanPhsLAvr,	0	},
	{ "PM2_HF1Z_ScanPwrLAvr"	, _K_A_IO	,	PM2_HF1Z_ScanPwrLAvr,	0	},

	{ "PM2_HF2LoadPosLAvr"		, _K_A_IO	,	PM2_HF2LoadPosLAvr,		0	},
	{ "PM2_HF2TunePosLAvr"		, _K_A_IO	,	PM2_HF2TunePosLAvr,		0	},
	{ "PM2_HF2Z_ScanRLAvr"		, _K_A_IO	,	PM2_HF2Z_ScanRLAvr,		0	},
	{ "PM2_HF2Z_ScanXLAvr"		, _K_A_IO	,	PM2_HF2Z_ScanXLAvr,		0	},
	{ "PM2_HF2Z_ScanVltLAvr"	, _K_A_IO	,	PM2_HF2Z_ScanVltLAvr,	0	},
	{ "PM2_HF2Z_ScanCurLAvr"	, _K_A_IO	,	PM2_HF2Z_ScanCurLAvr,	0	},
	{ "PM2_HF2Z_ScanPhsLAvr"	, _K_A_IO	,	PM2_HF2Z_ScanPhsLAvr,	0	},
	{ "PM2_HF2Z_ScanPwrLAvr"	, _K_A_IO	,	PM2_HF2Z_ScanPwrLAvr,	0	},
	//...PM3
	{ "PM3_RF1FreqLAvrAI"		, _K_A_IO	,	PM3_RF1FreqLAvrAI,		0 	},
	{ "PM3_RF1RefPwLAvrAI"		, _K_A_IO	,	PM3_RF1RefPwLAvrAI,		0 	},
	{ "PM3_RF1LoadPwLAvrAI"		, _K_A_IO	,	PM3_RF1LoadPwLAvrAI,	0 	},
	{ "PM3_RF2FreqLAvrAI"		, _K_A_IO	,	PM3_RF2FreqLAvrAI,		0 	},
	{ "PM3_RF2RefPwLAvrAI"		, _K_A_IO	,	PM3_RF2RefPwLAvrAI,		0 	},
	{ "PM3_RF2LoadPwLAvrAI"		, _K_A_IO	,	PM3_RF2LoadPwLAvrAI,	0 	},

	{ "PM3_HF1LoadPosLAvr"		, _K_A_IO	,	PM3_HF1LoadPosLAvr,		0	},
	{ "PM3_HF1TunePosLAvr"		, _K_A_IO	,	PM3_HF1TunePosLAvr,		0	},
	{ "PM3_HF1Z_ScanRLAvr"		, _K_A_IO	,	PM3_HF1Z_ScanRLAvr,		0	},
	{ "PM3_HF1Z_ScanXLAvr"		, _K_A_IO	,	PM3_HF1Z_ScanXLAvr,		0	},
	{ "PM3_HF1Z_ScanVltLAvr"	, _K_A_IO	,	PM3_HF1Z_ScanVltLAvr,	0	},
	{ "PM3_HF1Z_ScanCurLAvr"	, _K_A_IO	,	PM3_HF1Z_ScanCurLAvr,	0	},
	{ "PM3_HF1Z_ScanPhsLAvr"	, _K_A_IO	,	PM3_HF1Z_ScanPhsLAvr,	0	},
	{ "PM3_HF1Z_ScanPwrLAvr"	, _K_A_IO	,	PM3_HF1Z_ScanPwrLAvr,	0	},

	{ "PM3_HF2LoadPosLAvr"		, _K_A_IO	,	PM3_HF2LoadPosLAvr,		0	},
	{ "PM3_HF2TunePosLAvr"		, _K_A_IO	,	PM3_HF2TunePosLAvr,		0	},
	{ "PM3_HF2Z_ScanRLAvr"		, _K_A_IO	,	PM3_HF2Z_ScanRLAvr,		0	},
	{ "PM3_HF2Z_ScanXLAvr"		, _K_A_IO	,	PM3_HF2Z_ScanXLAvr,		0	},
	{ "PM3_HF2Z_ScanVltLAvr"	, _K_A_IO	,	PM3_HF2Z_ScanVltLAvr,	0	},
	{ "PM3_HF2Z_ScanCurLAvr"	, _K_A_IO	,	PM3_HF2Z_ScanCurLAvr,	0	},
	{ "PM3_HF2Z_ScanPhsLAvr"	, _K_A_IO	,	PM3_HF2Z_ScanPhsLAvr,	0	},
	{ "PM3_HF2Z_ScanPwrLAvr"	, _K_A_IO	,	PM3_HF2Z_ScanPwrLAvr,	0	},
	//...
	""
};

//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {
	if ( g_SeqInterface.m_bSimulMode ) return g_SeqInterface.Sim_Main();
	else return g_SeqInterface.Main();
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
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	g_SeqInterface.m_bSimulMode = TRUE;
	else														g_SeqInterface.m_bSimulMode = FALSE;
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
