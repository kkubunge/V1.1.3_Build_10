#include "stdafx.h"
#include "TextParser.h"
#include "TransferManager.h"

#define TRANSFER_INFO_FILE	".\\Function\\Function_Maint_Run\\Info\\TransferConfig.ini"
CTransferManager*  g_pTransferManager = NULL;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "e:SCHEDULER"					, _K_S_IO	,	SCHEDULER				,	0	} ,

	{ "CTC.INFO_SAVE"				, _K_D_IO	,	INFO_SAVE				,	0	} ,
	{ "CTC.TR_CONTROL"				, _K_D_IO	,	TR_CONTROL				,	0	} ,
	//2007.07.30
	{ "CTC.MANUAL_TR_CONTROL"		, _K_D_IO	,	MANUAL_TR_CONTROL		,	0	} ,

	{ "CTC.TR_STATUS"				, _K_D_IO	,	Transfer_Status			,	0	} ,
	//Src Slot
	{ "CTC.TR_SRC_SLOT"				, _K_D_IO	,	TR_SRC_SLOT 			,	0	} ,
	{ "CTC.TR_SRC_SLOT2"			, _K_D_IO	,	TR_SRC_SLOT2 			,	0	} ,
    //Trg Slot
	{ "CTC.TR_TRG_SLOT"				, _K_D_IO	,	TR_TRG_SLOT 			,	0	} ,
	{ "CTC.TR_TRG_SLOT2"			, _K_D_IO	,	TR_TRG_SLOT2 			,	0	} ,
	{ "CTC.Exchange_CM"			    , _K_D_IO	,	Dummy_Sel_CM 			,	0	} ,	

	//Station
	{ "CTC.TR_SRC_STATION"			, _K_D_IO	,	TR_SRC_STATION 			,	0	} ,
	{ "CTC.TR_TRG_STATION"			, _K_D_IO	,	TR_TRG_STATION 			,	0	} ,
	{ "CTC.TR_FINGER"				, _K_D_IO	,	TR_FINGER 				,	0	} ,


	//{ "PM1_MOVE_CONTROL"			, _K_F_IO	,	PM1_MOVE_CONTROL		,	0	} ,
	//{ "PM2_MOVE_CONTROL"			, _K_F_IO	,	PM2_MOVE_CONTROL		,	0	} ,

	//-------------------------------------------------------------------------------------
    //ATM Area
	{ "CTC.FA_Wafer_Status"			, _K_D_IO	,	FA_Wafer_Status		,	0	} ,
	{ "CTC.FA_Wafer_Source"			, _K_D_IO	,	FA_Wafer_Source		,	0	} ,
	{ "CTC.FB_Wafer_Status"			, _K_D_IO	,	FB_Wafer_Status		,	0	} ,
	{ "CTC.FB_Wafer_Source"			, _K_D_IO	,	FB_Wafer_Source		,	0	} ,
	
	//VTM Area
	{ "CTC.TA_Wafer_Status"			, _K_D_IO	,	TA_Wafer_Status		,	0	} ,
	{ "CTC.TA_Wafer_Source"			, _K_D_IO	,	TA_Wafer_Source		,	0	} ,
	{ "CTC.TA_Wafer_Status2"		, _K_D_IO	,	TA_Wafer_Status2		,	0	} ,
	{ "CTC.TA_Wafer_Source2"		, _K_D_IO	,	TA_Wafer_Source2		,	0	} ,
	{ "CTC.TB_Wafer_Status"			, _K_D_IO	,	TB_Wafer_Status		,	0	} ,
	{ "CTC.TB_Wafer_Source"			, _K_D_IO	,	TB_Wafer_Source		,	0	} ,
	{ "CTC.TB_Wafer_Status2"		, _K_D_IO	,	TB_Wafer_Status2	,	0	} ,
	{ "CTC.TB_Wafer_Source2"		, _K_D_IO	,	TB_Wafer_Source2	,	0	} ,
 
  //{ "CTC.TA_Wafer_Status2"		, _K_D_IO	,	TA_Wafer_Status2	,	0	} ,
  //{ "CTC.TA_Wafer_Source2"		, _K_D_IO	,	TA_Wafer_Source2	,	0	} ,
  //{ "CTC.TB_Wafer_Status"		, _K_D_IO	,	TB_Wafer_Status		,	0	} ,
  //{ "CTC.TA2_Wafer_Status"		, _K_D_IO	,	TA2_Wafer_Status	,	0	} ,
  //{ "CTC.TB2_Wafer_Status"		, _K_D_IO	,	TB2_Wafer_Status	,	0	} ,

	//PM1 Area
	{ "CTC.PM1_Wafer_Status"		, _K_D_IO	,	PM1_Wafer_Status	,	0	} ,
	{ "CTC.PM1_Wafer_Source"		, _K_D_IO	,	PM1_Wafer_Source	,	0	} ,
	{ "CTC.PM1_Wafer_Status2"		, _K_D_IO	,	PM1_Wafer_Status2	,	0	} ,
	{ "CTC.PM1_Wafer_Source2"		, _K_D_IO	,	PM1_Wafer_Source2	,	0	} ,
  //{ "CTC.PM1_Wafer_Status3"		, _K_D_IO	,	PM1_Wafer_Status3	,	0	} ,
  //{ "CTC.PM1_Wafer_Source3"		, _K_D_IO	,	PM1_Wafer_Source3	,	0	} ,
  //{ "CTC.PM1_Wafer_Status4"		, _K_D_IO	,	PM1_Wafer_Status4	,	0	} ,
  //{ "CTC.PM1_Wafer_Source4"		, _K_D_IO	,	PM1_Wafer_Source4	,	0	} ,
  //{ "CTC.PM1_Wafer_Status5"		, _K_D_IO	,	PM1_Wafer_Status5	,	0	} ,
  //{ "CTC.PM1_Wafer_Source5"		, _K_D_IO	,	PM1_Wafer_Source5	,	0	} ,

	//PM2 Area
	{ "CTC.PM2_Wafer_Status"		, _K_D_IO	,	PM2_Wafer_Status	,	0	} ,
	{ "CTC.PM2_Wafer_Source"		, _K_D_IO	,	PM2_Wafer_Source	,	0	} ,
	{ "CTC.PM2_Wafer_Status2"		, _K_D_IO	,	PM2_Wafer_Status2	,	0	} ,
	{ "CTC.PM2_Wafer_Source2"		, _K_D_IO	,	PM2_Wafer_Source2	,	0	} ,
  //{ "CTC.PM2_Wafer_Status3"		, _K_D_IO	,	PM2_Wafer_Status3	,	0	} ,
  //{ "CTC.PM2_Wafer_Source3"		, _K_D_IO	,	PM2_Wafer_Source3	,	0	} ,
  //{ "CTC.PM2_Wafer_Status4"		, _K_D_IO	,	PM2_Wafer_Status4	,	0	} ,
  //{ "CTC.PM2_Wafer_Source4"		, _K_D_IO	,	PM2_Wafer_Source4	,	0	} ,
  //{ "CTC.PM2_Wafer_Status5"		, _K_D_IO	,	PM2_Wafer_Status5	,	0	} ,
  //{ "CTC.PM2_Wafer_Source5"		, _K_D_IO	,	PM2_Wafer_Source5	,	0	} ,

	//PM3 Area
	{ "CTC.PM3_Wafer_Status"		, _K_D_IO	,	PM3_Wafer_Status	,	0	} ,
	{ "CTC.PM3_Wafer_Source"		, _K_D_IO	,	PM3_Wafer_Source	,	0	} ,
	{ "CTC.PM3_Wafer_Status2"		, _K_D_IO	,	PM3_Wafer_Status2	,	0	} ,
	{ "CTC.PM3_Wafer_Source2"		, _K_D_IO	,	PM3_Wafer_Source2	,	0	} ,
  //{ "CTC.PM3_Wafer_Status3"		, _K_D_IO	,	PM3_Wafer_Status3	,	0	} ,
  //{ "CTC.PM3_Wafer_Source3"		, _K_D_IO	,	PM3_Wafer_Source3	,	0	} ,
  //{ "CTC.PM3_Wafer_Status4"		, _K_D_IO	,	PM3_Wafer_Status4	,	0	} ,
  //{ "CTC.PM3_Wafer_Source4"		, _K_D_IO	,	PM3_Wafer_Source4	,	0	} ,
  //{ "CTC.PM3_Wafer_Status5"		, _K_D_IO	,	PM3_Wafer_Status5	,	0	} ,
  //{ "CTC.PM3_Wafer_Source5"		, _K_D_IO	,	PM3_Wafer_Source5	,	0	} ,
	

    //BM1 Area
	{ "CTC.BM1_Wafer_Status"		, _K_D_IO	,	BM1_Wafer_Status	,	0	} ,
	{ "CTC.BM1_Wafer_Source"		, _K_D_IO	,	BM1_Wafer_Source	,	0	} ,
	{ "CTC.BM1_Wafer_Status2"		, _K_D_IO	,	BM1_Wafer_Status2	,	0	} ,
	{ "CTC.BM1_Wafer_Source2"		, _K_D_IO	,	BM1_Wafer_Source2	,	0	} ,
	{ "CTC.BM1_Wafer_Status3"		, _K_D_IO	,	BM1_Wafer_Status3	,	0	} ,
	{ "CTC.BM1_Wafer_Source3"		, _K_D_IO	,	BM1_Wafer_Source3	,	0	} ,
	{ "CTC.BM1_Wafer_Status4"		, _K_D_IO	,	BM1_Wafer_Status4	,	0	} ,
	{ "CTC.BM1_Wafer_Source4"		, _K_D_IO	,	BM1_Wafer_Source4	,	0	} ,
  //{ "CTC.BM1_Wafer_Status5"		, _K_D_IO	,	BM1_Wafer_Status5	,	0	} ,
  //{ "CTC.BM1_Wafer_Source5"		, _K_D_IO	,	BM1_Wafer_Source5	,	0	} ,
  //{ "CTC.BM1_Wafer_Status6"		, _K_D_IO	,	BM1_Wafer_Status6	,	0	} ,
  //{ "CTC.BM1_Wafer_Source6"		, _K_D_IO	,	BM1_Wafer_Source6	,	0	} ,
  //{ "CTC.BM1_Wafer_Status7"		, _K_D_IO	,	BM1_Wafer_Status7	,	0	} ,
  //{ "CTC.BM1_Wafer_Source7"		, _K_D_IO	,	BM1_Wafer_Source7	,	0	} ,
  //{ "CTC.BM1_Wafer_Status8"		, _K_D_IO	,	BM1_Wafer_Status8	,	0	} ,
  //{ "CTC.BM1_Wafer_Source8"		, _K_D_IO	,	BM1_Wafer_Source8	,	0	} ,

    //BM2 Area
	{ "CTC.BM2_Wafer_Status"		, _K_D_IO	,	BM2_Wafer_Status	,	0	} ,
	{ "CTC.BM2_Wafer_Source"		, _K_D_IO	,	BM2_Wafer_Source	,	0	} ,
	{ "CTC.BM2_Wafer_Status2"		, _K_D_IO	,	BM2_Wafer_Status2	,	0	} ,
	{ "CTC.BM2_Wafer_Source2"		, _K_D_IO	,	BM2_Wafer_Source2	,	0	} ,
	{ "CTC.BM2_Wafer_Status3"		, _K_D_IO	,	BM2_Wafer_Status3	,	0	} ,
	{ "CTC.BM2_Wafer_Source3"		, _K_D_IO	,	BM2_Wafer_Source3	,	0	} ,
	{ "CTC.BM2_Wafer_Status4"		, _K_D_IO	,	BM2_Wafer_Status4	,	0	} ,
	{ "CTC.BM2_Wafer_Source4"		, _K_D_IO	,	BM2_Wafer_Source4	,	0	} ,

  //{ "CTC.BM2_Wafer_Status5"		, _K_D_IO	,	BM2_Wafer_Status5	,	0	} ,
  //{ "CTC.BM2_Wafer_Source5"		, _K_D_IO	,	BM2_Wafer_Source5	,	0	} ,
  //{ "CTC.BM2_Wafer_Status6"		, _K_D_IO	,	BM2_Wafer_Status6	,	0	} ,
  //{ "CTC.BM2_Wafer_Source6"		, _K_D_IO	,	BM2_Wafer_Source6	,	0	} ,
  //{ "CTC.BM2_Wafer_Status7"		, _K_D_IO	,	BM2_Wafer_Status7	,	0	} ,
  //{ "CTC.BM2_Wafer_Source7"		, _K_D_IO	,	BM2_Wafer_Source7	,	0	} ,
  //{ "CTC.BM2_Wafer_Status8"		, _K_D_IO	,	BM2_Wafer_Status8	,	0	} ,
  //{ "CTC.BM2_Wafer_Source8"		, _K_D_IO	,	BM2_Wafer_Source8	,	0	} ,

	//BM3(DUMMY) Area
	{ "CTC.BM3_Wafer_Status"		, _K_D_IO	,	BM3_Wafer_Status	,	0	} ,
	{ "CTC.BM3_Wafer_Status2"		, _K_D_IO	,	BM3_Wafer_Status2	,	0	} ,
	{ "CTC.BM3_Wafer_Status3"		, _K_D_IO	,	BM3_Wafer_Status3	,	0	} ,
	{ "CTC.BM3_Wafer_Status4"		, _K_D_IO	,	BM3_Wafer_Status4	,	0	} ,
	{ "CTC.BM3_Wafer_Status5"		, _K_D_IO	,	BM3_Wafer_Status5	,	0	} ,
	{ "CTC.BM3_Wafer_Status6"		, _K_D_IO	,	BM3_Wafer_Status6	,	0	} ,
	{ "CTC.BM3_Wafer_Status7"		, _K_D_IO	,	BM3_Wafer_Status7	,	0	} ,
	{ "CTC.BM3_Wafer_Status8"		, _K_D_IO	,	BM3_Wafer_Status8	,	0	} ,
	{ "CTC.BM3_Wafer_Status9"		, _K_D_IO	,	BM3_Wafer_Status9	,	0	} ,
	{ "CTC.BM3_Wafer_Status10"		, _K_D_IO	,	BM3_Wafer_Status10	,	0	} ,
	{ "CTC.BM3_Wafer_Status11"		, _K_D_IO	,	BM3_Wafer_Status11	,	0	} ,
	{ "CTC.BM3_Wafer_Status12"		, _K_D_IO	,	BM3_Wafer_Status12	,	0	} ,
	{ "CTC.BM3_Wafer_Status13"		, _K_D_IO	,	BM3_Wafer_Status13	,	0	} ,
	{ "CTC.BM3_Wafer_Status14"		, _K_D_IO	,	BM3_Wafer_Status14	,	0	} ,
	{ "CTC.BM3_Wafer_Status15"		, _K_D_IO	,	BM3_Wafer_Status15	,	0	} ,
	{ "CTC.BM3_Wafer_Status16"		, _K_D_IO	,	BM3_Wafer_Status16	,	0	} ,
	{ "CTC.BM3_Wafer_Status17"		, _K_D_IO	,	BM3_Wafer_Status17	,	0	} ,
	{ "CTC.BM3_Wafer_Status18"		, _K_D_IO	,	BM3_Wafer_Status18	,	0	} ,
	{ "CTC.BM3_Wafer_Status19"		, _K_D_IO	,	BM3_Wafer_Status19	,	0	} ,
	{ "CTC.BM3_Wafer_Status20"		, _K_D_IO	,	BM3_Wafer_Status20	,	0	} ,
	{ "CTC.BM3_Wafer_Status21"		, _K_D_IO	,	BM3_Wafer_Status21	,	0	} ,
	{ "CTC.BM3_Wafer_Status22"		, _K_D_IO	,	BM3_Wafer_Status22	,	0	} ,
	{ "CTC.BM3_Wafer_Status23"		, _K_D_IO	,	BM3_Wafer_Status23	,	0	} ,
	{ "CTC.BM3_Wafer_Status24"		, _K_D_IO	,	BM3_Wafer_Status24	,	0	} ,
	{ "CTC.BM3_Wafer_Status25"		, _K_D_IO	,	BM3_Wafer_Status25	,	0	} ,
	{ "CTC.BM3_Wafer_Status26"		, _K_D_IO	,	BM3_Wafer_Status26	,	0	} ,

	{ "CTC.BM3_Wafer_Source"		, _K_D_IO	,	BM3_Wafer_Source	,	0	} ,
	{ "CTC.BM3_Wafer_Source2"		, _K_D_IO	,	BM3_Wafer_Source2	,	0	} ,
	{ "CTC.BM3_Wafer_Source3"		, _K_D_IO	,	BM3_Wafer_Source3	,	0	} ,
	{ "CTC.BM3_Wafer_Source4"		, _K_D_IO	,	BM3_Wafer_Source4	,	0	} ,
	{ "CTC.BM3_Wafer_Source5"		, _K_D_IO	,	BM3_Wafer_Source5	,	0	} ,
	{ "CTC.BM3_Wafer_Source6"		, _K_D_IO	,	BM3_Wafer_Source6	,	0	} ,
	{ "CTC.BM3_Wafer_Source7"		, _K_D_IO	,	BM3_Wafer_Source7	,	0	} ,
	{ "CTC.BM3_Wafer_Source8"		, _K_D_IO	,	BM3_Wafer_Source8	,	0	} ,
	{ "CTC.BM3_Wafer_Source9"		, _K_D_IO	,	BM3_Wafer_Source9	,	0	} ,
	{ "CTC.BM3_Wafer_Source10"		, _K_D_IO	,	BM3_Wafer_Source10	,	0	} ,
	{ "CTC.BM3_Wafer_Source11"		, _K_D_IO	,	BM3_Wafer_Source11	,	0	} ,
	{ "CTC.BM3_Wafer_Source12"		, _K_D_IO	,	BM3_Wafer_Source12	,	0	} ,
	{ "CTC.BM3_Wafer_Source13"		, _K_D_IO	,	BM3_Wafer_Source13	,	0	} ,
	{ "CTC.BM3_Wafer_Source14"		, _K_D_IO	,	BM3_Wafer_Source14	,	0	} ,
	{ "CTC.BM3_Wafer_Source15"		, _K_D_IO	,	BM3_Wafer_Source15	,	0	} ,
	{ "CTC.BM3_Wafer_Source16"		, _K_D_IO	,	BM3_Wafer_Source16	,	0	} ,
	{ "CTC.BM3_Wafer_Source17"		, _K_D_IO	,	BM3_Wafer_Source17	,	0	} ,
	{ "CTC.BM3_Wafer_Source18"		, _K_D_IO	,	BM3_Wafer_Source18	,	0	} ,
	{ "CTC.BM3_Wafer_Source19"		, _K_D_IO	,	BM3_Wafer_Source19	,	0	} ,
	{ "CTC.BM3_Wafer_Source20"		, _K_D_IO	,	BM3_Wafer_Source20	,	0	} ,
	{ "CTC.BM3_Wafer_Source21"		, _K_D_IO	,	BM3_Wafer_Source21	,	0	} ,
	{ "CTC.BM3_Wafer_Source22"		, _K_D_IO	,	BM3_Wafer_Source22	,	0	} ,
	{ "CTC.BM3_Wafer_Source23"		, _K_D_IO	,	BM3_Wafer_Source23	,	0	} ,
	{ "CTC.BM3_Wafer_Source24"		, _K_D_IO	,	BM3_Wafer_Source24	,	0	} ,
	{ "CTC.BM3_Wafer_Source25"		, _K_D_IO	,	BM3_Wafer_Source25	,	0	} ,
	{ "CTC.BM3_Wafer_Source26"		, _K_D_IO	,	BM3_Wafer_Source26	,	0	} ,

	{ "CTC.FM_AL_Wafer_Status"		, _K_D_IO	,	FM_AL_Wafer_Status	,	0	} ,
	{ "CTC.FM_AL_Wafer_Source"		, _K_D_IO	,	FM_AL_Wafer_Source	,	0	} ,

	{ "CM1.C01_WAFER"				, _K_D_IO	,	CM1_C01_Wafer		,	0	} ,
	{ "CM1.C02_WAFER"				, _K_D_IO	,	CM1_C02_Wafer		,	0	} ,
	{ "CM1.C03_WAFER"				, _K_D_IO	,	CM1_C03_Wafer		,	0	} ,
	{ "CM1.C04_WAFER"				, _K_D_IO	,	CM1_C04_Wafer		,	0	} ,
	{ "CM1.C05_WAFER"				, _K_D_IO	,	CM1_C05_Wafer		,	0	} ,
	{ "CM1.C06_WAFER"				, _K_D_IO	,	CM1_C06_Wafer		,	0	} ,
	{ "CM1.C07_WAFER"				, _K_D_IO	,	CM1_C07_Wafer		,	0	} ,
	{ "CM1.C08_WAFER"				, _K_D_IO	,	CM1_C08_Wafer		,	0	} ,
	{ "CM1.C09_WAFER"				, _K_D_IO	,	CM1_C09_Wafer		,	0	} ,
	{ "CM1.C10_WAFER"				, _K_D_IO	,	CM1_C10_Wafer		,	0	} ,
	{ "CM1.C11_WAFER"				, _K_D_IO	,	CM1_C11_Wafer		,	0	} ,
	{ "CM1.C12_WAFER"				, _K_D_IO	,	CM1_C12_Wafer		,	0	} ,
	{ "CM1.C13_WAFER"				, _K_D_IO	,	CM1_C13_Wafer		,	0	} ,
	{ "CM1.C14_WAFER"				, _K_D_IO	,	CM1_C14_Wafer		,	0	} ,
	{ "CM1.C15_WAFER"				, _K_D_IO	,	CM1_C15_Wafer		,	0	} ,
	{ "CM1.C16_WAFER"				, _K_D_IO	,	CM1_C16_Wafer		,	0	} ,
	{ "CM1.C17_WAFER"				, _K_D_IO	,	CM1_C17_Wafer		,	0	} ,
	{ "CM1.C18_WAFER"				, _K_D_IO	,	CM1_C18_Wafer		,	0	} ,
	{ "CM1.C19_WAFER"				, _K_D_IO	,	CM1_C19_Wafer		,	0	} ,
	{ "CM1.C20_WAFER"				, _K_D_IO	,	CM1_C20_Wafer		,	0	} ,
	{ "CM1.C21_WAFER"				, _K_D_IO	,	CM1_C21_Wafer		,	0	} ,
	{ "CM1.C22_WAFER"				, _K_D_IO	,	CM1_C22_Wafer		,	0	} ,
	{ "CM1.C23_WAFER"				, _K_D_IO	,	CM1_C23_Wafer		,	0	} ,
	{ "CM1.C24_WAFER"				, _K_D_IO	,	CM1_C24_Wafer		,	0	} ,
	{ "CM1.C25_WAFER"				, _K_D_IO	,	CM1_C25_Wafer		,	0	} ,
	{ "CM1.C26_WAFER"				, _K_D_IO	,	CM1_C26_Wafer		,	0	} ,
	{ "CM1.C27_WAFER"				, _K_D_IO	,	CM1_C27_Wafer		,	0	} ,
	{ "CM1.C28_WAFER"				, _K_D_IO	,	CM1_C28_Wafer		,	0	} ,
	{ "CM1.C29_WAFER"				, _K_D_IO	,	CM1_C29_Wafer		,	0	} ,
	{ "CM1.C30_WAFER"				, _K_D_IO	,	CM1_C30_Wafer		,	0	} ,
	
	{ "CM2.C01_WAFER"				, _K_D_IO	,	CM2_C01_Wafer		,	0	} ,
	{ "CM2.C02_WAFER"				, _K_D_IO	,	CM2_C02_Wafer		,	0	} ,
	{ "CM2.C03_WAFER"				, _K_D_IO	,	CM2_C03_Wafer		,	0	} ,
	{ "CM2.C04_WAFER"				, _K_D_IO	,	CM2_C04_Wafer		,	0	} ,
	{ "CM2.C05_WAFER"				, _K_D_IO	,	CM2_C05_Wafer		,	0	} ,
	{ "CM2.C06_WAFER"				, _K_D_IO	,	CM2_C06_Wafer		,	0	} ,
	{ "CM2.C07_WAFER"				, _K_D_IO	,	CM2_C07_Wafer		,	0	} ,
	{ "CM2.C08_WAFER"				, _K_D_IO	,	CM2_C08_Wafer		,	0	} ,
	{ "CM2.C09_WAFER"				, _K_D_IO	,	CM2_C09_Wafer		,	0	} ,
	{ "CM2.C10_WAFER"				, _K_D_IO	,	CM2_C10_Wafer		,	0	} ,
	{ "CM2.C11_WAFER"				, _K_D_IO	,	CM2_C11_Wafer		,	0	} ,
	{ "CM2.C12_WAFER"				, _K_D_IO	,	CM2_C12_Wafer		,	0	} ,
	{ "CM2.C13_WAFER"				, _K_D_IO	,	CM2_C13_Wafer		,	0	} ,
	{ "CM2.C14_WAFER"				, _K_D_IO	,	CM2_C14_Wafer		,	0	} ,
	{ "CM2.C15_WAFER"				, _K_D_IO	,	CM2_C15_Wafer		,	0	} ,
	{ "CM2.C16_WAFER"				, _K_D_IO	,	CM2_C16_Wafer		,	0	} ,
	{ "CM2.C17_WAFER"				, _K_D_IO	,	CM2_C17_Wafer		,	0	} ,
	{ "CM2.C18_WAFER"				, _K_D_IO	,	CM2_C18_Wafer		,	0	} ,
	{ "CM2.C19_WAFER"				, _K_D_IO	,	CM2_C19_Wafer		,	0	} ,
	{ "CM2.C20_WAFER"				, _K_D_IO	,	CM2_C20_Wafer		,	0	} ,
	{ "CM2.C21_WAFER"				, _K_D_IO	,	CM2_C21_Wafer		,	0	} ,
	{ "CM2.C22_WAFER"				, _K_D_IO	,	CM2_C22_Wafer		,	0	} ,
	{ "CM2.C23_WAFER"				, _K_D_IO	,	CM2_C23_Wafer		,	0	} ,
	{ "CM2.C24_WAFER"				, _K_D_IO	,	CM2_C24_Wafer		,	0	} ,
	{ "CM2.C25_WAFER"				, _K_D_IO	,	CM2_C25_Wafer		,	0	} ,
	{ "CM2.C26_WAFER"				, _K_D_IO	,	CM2_C26_Wafer		,	0	} ,
	{ "CM2.C27_WAFER"				, _K_D_IO	,	CM2_C27_Wafer		,	0	} ,
	{ "CM2.C28_WAFER"				, _K_D_IO	,	CM2_C28_Wafer		,	0	} ,
	{ "CM2.C29_WAFER"				, _K_D_IO	,	CM2_C29_Wafer		,	0	} ,
	{ "CM2.C30_WAFER"				, _K_D_IO	,	CM2_C30_Wafer		,	0	} ,

	{ "CM3.C01_WAFER"				, _K_D_IO	,	CM3_C01_Wafer		,	0	} ,
	{ "CM3.C02_WAFER"				, _K_D_IO	,	CM3_C02_Wafer		,	0	} ,
	{ "CM3.C03_WAFER"				, _K_D_IO	,	CM3_C03_Wafer		,	0	} ,
	{ "CM3.C04_WAFER"				, _K_D_IO	,	CM3_C04_Wafer		,	0	} ,
	{ "CM3.C05_WAFER"				, _K_D_IO	,	CM3_C05_Wafer		,	0	} ,
	{ "CM3.C06_WAFER"				, _K_D_IO	,	CM3_C06_Wafer		,	0	} ,
	{ "CM3.C07_WAFER"				, _K_D_IO	,	CM3_C07_Wafer		,	0	} ,
	{ "CM3.C08_WAFER"				, _K_D_IO	,	CM3_C08_Wafer		,	0	} ,
	{ "CM3.C09_WAFER"				, _K_D_IO	,	CM3_C09_Wafer		,	0	} ,
	{ "CM3.C10_WAFER"				, _K_D_IO	,	CM3_C10_Wafer		,	0	} ,
	{ "CM3.C11_WAFER"				, _K_D_IO	,	CM3_C11_Wafer		,	0	} ,
	{ "CM3.C12_WAFER"				, _K_D_IO	,	CM3_C12_Wafer		,	0	} ,
	{ "CM3.C13_WAFER"				, _K_D_IO	,	CM3_C13_Wafer		,	0	} ,
	{ "CM3.C14_WAFER"				, _K_D_IO	,	CM3_C14_Wafer		,	0	} ,
	{ "CM3.C15_WAFER"				, _K_D_IO	,	CM3_C15_Wafer		,	0	} ,
	{ "CM3.C16_WAFER"				, _K_D_IO	,	CM3_C16_Wafer		,	0	} ,
	{ "CM3.C17_WAFER"				, _K_D_IO	,	CM3_C17_Wafer		,	0	} ,
	{ "CM3.C18_WAFER"				, _K_D_IO	,	CM3_C18_Wafer		,	0	} ,
	{ "CM3.C19_WAFER"				, _K_D_IO	,	CM3_C19_Wafer		,	0	} ,
	{ "CM3.C20_WAFER"				, _K_D_IO	,	CM3_C20_Wafer		,	0	} ,
	{ "CM3.C21_WAFER"				, _K_D_IO	,	CM3_C21_Wafer		,	0	} ,
	{ "CM3.C22_WAFER"				, _K_D_IO	,	CM3_C22_Wafer		,	0	} ,
	{ "CM3.C23_WAFER"				, _K_D_IO	,	CM3_C23_Wafer		,	0	} ,
	{ "CM3.C24_WAFER"				, _K_D_IO	,	CM3_C24_Wafer		,	0	} ,
	{ "CM3.C25_WAFER"				, _K_D_IO	,	CM3_C25_Wafer		,	0	} ,
	{ "CM3.C26_WAFER"				, _K_D_IO	,	CM3_C26_Wafer		,	0	} ,
	{ "CM3.C27_WAFER"				, _K_D_IO	,	CM3_C27_Wafer		,	0	} ,
	{ "CM3.C28_WAFER"				, _K_D_IO	,	CM3_C28_Wafer		,	0	} ,
	{ "CM3.C29_WAFER"				, _K_D_IO	,	CM3_C29_Wafer		,	0	} ,
	{ "CM3.C30_WAFER"				, _K_D_IO	,	CM3_C30_Wafer		,	0	} ,

	//2007.12.06 for dummy exchange
	{ "CTC.Exchange_CMSlot1"		, _K_D_IO	,	CM_Select_Slot1		,	0	} ,
	{ "CTC.Exchange_CMSlot2"		, _K_D_IO	,	CM_Select_Slot2		,	0	} ,
	{ "CTC.Exchange_CMSlot3"		, _K_D_IO	,	CM_Select_Slot3		,	0	} ,
	{ "CTC.Exchange_CMSlot4"		, _K_D_IO	,	CM_Select_Slot4		,	0	} ,
	{ "CTC.Exchange_CMSlot5"		, _K_D_IO	,	CM_Select_Slot5		,	0	} ,
	{ "CTC.Exchange_CMSlot6"		, _K_D_IO	,	CM_Select_Slot6		,	0	} ,
	{ "CTC.Exchange_CMSlot7"		, _K_D_IO	,	CM_Select_Slot7		,	0	} ,
	{ "CTC.Exchange_CMSlot8"		, _K_D_IO	,	CM_Select_Slot8		,	0	} ,
	{ "CTC.Exchange_CMSlot9"		, _K_D_IO	,	CM_Select_Slot9		,	0	} ,
	{ "CTC.Exchange_CMSlot10"		, _K_D_IO	,	CM_Select_Slot10	,	0	} ,
	{ "CTC.Exchange_CMSlot11"		, _K_D_IO	,	CM_Select_Slot11	,	0	} ,
	{ "CTC.Exchange_CMSlot12"		, _K_D_IO	,	CM_Select_Slot12	,	0	} ,
	{ "CTC.Exchange_CMSlot13"		, _K_D_IO	,	CM_Select_Slot13	,	0	} ,
	{ "CTC.Exchange_CMSlot14"		, _K_D_IO	,	CM_Select_Slot14	,	0	} ,
	{ "CTC.Exchange_CMSlot15"		, _K_D_IO	,	CM_Select_Slot15	,	0	} ,
	{ "CTC.Exchange_CMSlot16"		, _K_D_IO	,	CM_Select_Slot16	,	0	} ,
	{ "CTC.Exchange_CMSlot17"		, _K_D_IO	,	CM_Select_Slot17	,	0	} ,
	{ "CTC.Exchange_CMSlot18"		, _K_D_IO	,	CM_Select_Slot18	,	0	} ,
	{ "CTC.Exchange_CMSlot19"		, _K_D_IO	,	CM_Select_Slot19	,	0	} ,
	{ "CTC.Exchange_CMSlot20"		, _K_D_IO	,	CM_Select_Slot20	,	0	} ,
	{ "CTC.Exchange_CMSlot21"		, _K_D_IO	,	CM_Select_Slot21	,	0	} ,
	{ "CTC.Exchange_CMSlot22"		, _K_D_IO	,	CM_Select_Slot22	,	0	} ,
	{ "CTC.Exchange_CMSlot23"		, _K_D_IO	,	CM_Select_Slot23	,	0	} ,
	{ "CTC.Exchange_CMSlot24"		, _K_D_IO	,	CM_Select_Slot24	,	0	} ,
	{ "CTC.Exchange_CMSlot25"		, _K_D_IO	,	CM_Select_Slot25	,	0	} ,

	{ "CTC.Exchange_DummySlot1"		, _K_D_IO	,	Dummy_Select_Slot1	,	0	} ,
	{ "CTC.Exchange_DummySlot2"		, _K_D_IO	,	Dummy_Select_Slot2	,	0	} ,
	{ "CTC.Exchange_DummySlot3"		, _K_D_IO	,	Dummy_Select_Slot3	,	0	} ,
	{ "CTC.Exchange_DummySlot4"		, _K_D_IO	,	Dummy_Select_Slot4	,	0	} ,
	{ "CTC.Exchange_DummySlot5"		, _K_D_IO	,	Dummy_Select_Slot5	,	0	} ,
	{ "CTC.Exchange_DummySlot6"		, _K_D_IO	,	Dummy_Select_Slot6	,	0	} ,
	{ "CTC.Exchange_DummySlot7"		, _K_D_IO	,	Dummy_Select_Slot7	,	0	} ,
	{ "CTC.Exchange_DummySlot8"		, _K_D_IO	,	Dummy_Select_Slot8	,	0	} ,
	{ "CTC.Exchange_DummySlot9"		, _K_D_IO	,	Dummy_Select_Slot9	,	0	} ,
	{ "CTC.Exchange_DummySlot10"	, _K_D_IO	,	Dummy_Select_Slot10	,	0	} ,
	{ "CTC.Exchange_DummySlot11"	, _K_D_IO	,	Dummy_Select_Slot11	,	0	} ,
	{ "CTC.Exchange_DummySlot12"	, _K_D_IO	,	Dummy_Select_Slot12	,	0	} ,
	{ "CTC.Exchange_DummySlot13"	, _K_D_IO	,	Dummy_Select_Slot13	,	0	} ,
	{ "CTC.Exchange_DummySlot14"	, _K_D_IO	,	Dummy_Select_Slot14	,	0	} ,
	{ "CTC.Exchange_DummySlot15"	, _K_D_IO	,	Dummy_Select_Slot15	,	0	} ,
	{ "CTC.Exchange_DummySlot16"	, _K_D_IO	,	Dummy_Select_Slot16	,	0	} ,
	{ "CTC.Exchange_DummySlot17"	, _K_D_IO	,	Dummy_Select_Slot17	,	0	} ,
	{ "CTC.Exchange_DummySlot18"	, _K_D_IO	,	Dummy_Select_Slot18	,	0	} ,
	{ "CTC.Exchange_DummySlot19"	, _K_D_IO	,	Dummy_Select_Slot19	,	0	} ,
	{ "CTC.Exchange_DummySlot20"	, _K_D_IO	,	Dummy_Select_Slot20	,	0	} ,
	{ "CTC.Exchange_DummySlot21"	, _K_D_IO	,	Dummy_Select_Slot21	,	0	} ,
	{ "CTC.Exchange_DummySlot22"	, _K_D_IO	,	Dummy_Select_Slot22	,	0	} ,
	{ "CTC.Exchange_DummySlot23"	, _K_D_IO	,	Dummy_Select_Slot23	,	0	} ,
	{ "CTC.Exchange_DummySlot24"	, _K_D_IO	,	Dummy_Select_Slot24	,	0	} ,
	{ "CTC.Exchange_DummySlot25"	, _K_D_IO	,	Dummy_Select_Slot25	,	0	} ,
	//2008.10.06 Only BM Slot Range IO Added
	{ "CTC.TR_SRC_BMSLOT"			, _K_D_IO	,	TR_SRC_BMSLOT		,	0	} ,
	{ "CTC.TR_SRC_BMSLOT2"			, _K_D_IO	,	TR_SRC_BMSLOT2		,	0	} ,
	{ "CTC.TR_TRG_BMSLOT"			, _K_D_IO	,	TR_TRG_BMSLOT		,	0	} ,
	{ "CTC.TR_TRG_BMSLOT2"			, _K_D_IO	,	TR_TRG_BMSLOT2		,	0	} ,

	//2010.10.10 Bluetain PM can transfer with single wafer 
	{ "CTC.TR_SRC_PMSLOT"			, _K_D_IO	,	TR_SRC_PMSLOT		,	0	} ,
	{ "CTC.TR_SRC_PMSLOT2"			, _K_D_IO	,	TR_SRC_PMSLOT2		,	0	} ,
	{ "CTC.TR_TRG_PMSLOT"			, _K_D_IO	,	TR_TRG_PMSLOT		,	0	} ,
	{ "CTC.TR_TRG_PMSLOT2"			, _K_D_IO	,	TR_TRG_PMSLOT2		,	0	} ,

	//2008.11.30 VTM Arm Forced Selection IO Added
	{ "VTMARM_USING_OPTION"			, _K_D_IO	,	VTMARM_USING_OPTION	,	0	} ,
	//2009.02.25
	//Dummy Exchange Align Option
	{ "FC_FM_DUMMY_ALIGN"       	, _K_D_IO	,	FC_FM_DUMMY_ALIGN	,	0	} ,

	{ "DUMMY_MANAGEMENT"			, _K_F_IO	,	DUMMY_MANAGEMENT	,	0	} ,
	{ "SCHEDULER_MAINT_PM1"			, _K_F_IO	,	FUNC_CALL_PM1		,	0	} ,
	{ "SCHEDULER_MAINT_PM2"			, _K_F_IO	,	FUNC_CALL_PM2		,	0	} ,
	{ "SCHEDULER_MAINT_PM3"			, _K_F_IO	,	FUNC_CALL_PM3		,	0	} ,
	//...2015.04.17
	{ "PC_CheckJob1DO"				, _K_D_IO,		PC_CheckJob1DO		,	0	},  		
	{ "PC_CheckJob2DO"              , _K_D_IO,		PC_CheckJob2DO      ,	0	},
	{ "PC_CheckJob3DO"              , _K_D_IO,		PC_CheckJob3DO      ,	0	},
	{ "PC_CheckJob4DO"              , _K_D_IO,		PC_CheckJob4DO      ,	0	},
	{ "PC_CheckJob5DO"              , _K_D_IO,		PC_CheckJob5DO      ,	0	},
	  
	{ "Sel_LPMDO"                   , _K_D_IO,		Sel_LPMDO	        ,	0	},
	{ "Sel1LPMSlt1DO"               , _K_D_IO,		Sel1LPMSlt1DO       ,	0	},
	{ "Sel2LPMSlt1DO"               , _K_D_IO,		Sel2LPMSlt1DO       ,	0	},
	{ "Sel3LPMSlt1DO"               , _K_D_IO,		Sel3LPMSlt1DO       ,	0	},
	{ "Sel4LPMSlt1DO"               , _K_D_IO,		Sel4LPMSlt1DO       ,	0	},
	{ "Sel5LPMSlt1DO"               , _K_D_IO,		Sel5LPMSlt1DO       ,	0	},	
	{ "Sel1LPMSlt2DO"               , _K_D_IO,		Sel1LPMSlt2DO       ,	0	},
	{ "Sel2LPMSlt2DO"               , _K_D_IO,		Sel2LPMSlt2DO       ,	0	},
	{ "Sel3LPMSlt2DO"               , _K_D_IO,		Sel3LPMSlt2DO       ,	0	},
	{ "Sel4LPMSlt2DO"               , _K_D_IO,		Sel4LPMSlt2DO       ,	0	},
	{ "Sel5LPMSlt2DO"               , _K_D_IO,		Sel5LPMSlt2DO       ,	0	},
	  
	{ "Sel1LLDO"                    , _K_D_IO,		Sel1LLDO	        ,	0	},
	{ "Sel2LLDO"                    , _K_D_IO,		Sel2LLDO	        ,	0	},
	{ "Sel3LLDO"                    , _K_D_IO,		Sel3LLDO	        ,	0	},
	{ "Sel4LLDO"                    , _K_D_IO,		Sel4LLDO	        ,	0	},
	{ "Sel5LLDO"                    , _K_D_IO,		Sel5LLDO	        ,	0	},
	{ "Sel1LLSlt1DO"                , _K_D_IO,		Sel1LLSlt1DO	    ,	0	},
	{ "Sel2LLSlt1DO"                , _K_D_IO,		Sel2LLSlt1DO	    ,	0	},
	{ "Sel3LLSlt1DO"                , _K_D_IO,		Sel3LLSlt1DO	    ,	0	},
	{ "Sel4LLSlt1DO"                , _K_D_IO,		Sel4LLSlt1DO	    ,	0	},
	{ "Sel5LLSlt1DO"                , _K_D_IO,		Sel5LLSlt1DO	    ,	0	},
	{ "Sel1LLSlt2DO"                , _K_D_IO,		Sel1LLSlt2DO	    ,	0	},
	{ "Sel2LLSlt2DO"                , _K_D_IO,		Sel2LLSlt2DO	    ,	0	},
	{ "Sel3LLSlt2DO"                , _K_D_IO,		Sel3LLSlt2DO	    ,	0	},
	{ "Sel4LLSlt2DO"                , _K_D_IO,		Sel4LLSlt2DO	    ,	0	},
	{ "Sel5LLSlt2DO"                , _K_D_IO,		Sel5LLSlt2DO	    ,	0	},
	  
	{ "LLDoorCyc1DO"                , _K_D_IO,		LLDoorCyc1DO        ,	0	},
	{ "LLDoorCyc2DO"                , _K_D_IO,		LLDoorCyc2DO        ,	0	},
	{ "LLDoorCyc3DO"                , _K_D_IO,		LLDoorCyc3DO        ,	0	},
	{ "LLDoorCyc4DO"                , _K_D_IO,		LLDoorCyc4DO        ,	0	},
	{ "LLDoorCyc5DO"                , _K_D_IO,		LLDoorCyc5DO        ,	0	},
	{ "LLDoorCyc1DI"                , _K_D_IO,		LLDoorCyc1DI        ,	0	},
	{ "LLDoorCyc2DI"                , _K_D_IO,		LLDoorCyc2DI        ,	0	},
	{ "LLDoorCyc3DI"                , _K_D_IO,		LLDoorCyc3DI        ,	0	},
	{ "LLDoorCyc4DI"                , _K_D_IO,		LLDoorCyc4DI        ,	0	},
	{ "LLDoorCyc5DI"                , _K_D_IO,		LLDoorCyc5DI        ,	0	},


	{ "LLSlitCyc1DO"                , _K_D_IO,		LLSlitCyc1DO        ,	0	},
	{ "LLSlitCyc2DO"                , _K_D_IO,		LLSlitCyc2DO        ,	0	},
	{ "LLSlitCyc3DO"                , _K_D_IO,		LLSlitCyc3DO        ,	0	},
	{ "LLSlitCyc4DO"                , _K_D_IO,		LLSlitCyc4DO        ,	0	},
	{ "LLSlitCyc5DO"                , _K_D_IO,		LLSlitCyc5DO        ,	0	},
	{ "LLSlitCyc1DI"                , _K_D_IO,		LLSlitCyc1DI        ,	0	},
	{ "LLSlitCyc2DI"                , _K_D_IO,		LLSlitCyc2DI        ,	0	},
	{ "LLSlitCyc3DI"                , _K_D_IO,		LLSlitCyc3DI        ,	0	},
	{ "LLSlitCyc4DI"                , _K_D_IO,		LLSlitCyc4DI        ,	0	},
	{ "LLSlitCyc5DI"                , _K_D_IO,		LLSlitCyc5DI        ,	0	},

	{ "LLPmVnCyc1DO"                , _K_D_IO,		LLPmVnCyc1DO        ,	0	},
	{ "LLPmVnCyc2DO"                , _K_D_IO,		LLPmVnCyc2DO        ,	0	},
	{ "LLPmVnCyc3DO"                , _K_D_IO,		LLPmVnCyc3DO        ,	0	},
	{ "LLPmVnCyc4DO"                , _K_D_IO,		LLPmVnCyc4DO        ,	0	},
	{ "LLPmVnCyc5DO"                , _K_D_IO,		LLPmVnCyc5DO        ,	0	},
	{ "LLPmVnCyc1DI"                , _K_D_IO,		LLPmVnCyc1DI        ,	0	},
	{ "LLPmVnCyc2DI"                , _K_D_IO,		LLPmVnCyc2DI        ,	0	},
	{ "LLPmVnCyc3DI"                , _K_D_IO,		LLPmVnCyc3DI        ,	0	},
	{ "LLPmVnCyc4DI"                , _K_D_IO,		LLPmVnCyc4DI        ,	0	},
	{ "LLPmVnCyc5DI"                , _K_D_IO,		LLPmVnCyc5DI        ,	0	},

	{ "TM_ArmSel1DO"                , _K_D_IO,		TM_ArmSel1DO	    ,	0	},
	{ "TM_ArmSel2DO"                , _K_D_IO,		TM_ArmSel2DO	    ,	0	},
	{ "TM_ArmSel3DO"                , _K_D_IO,		TM_ArmSel3DO	    ,	0	},
	{ "TM_ArmSel4DO"                , _K_D_IO,		TM_ArmSel4DO	    ,	0	},
	{ "TM_ArmSel5DO"                , _K_D_IO,		TM_ArmSel5DO	    ,	0	},
	  
	{ "TM_Rb1SelDO"                 , _K_D_IO,		TM_Rb1SelDO	        ,	0	},
	{ "TM_Rb2SelDO"                 , _K_D_IO,		TM_Rb2SelDO	        ,	0	},
	{ "TM_Rb3SelDO"                 , _K_D_IO,		TM_Rb3SelDO	        ,	0	},
	{ "TM_Rb4SelDO"                 , _K_D_IO,		TM_Rb4SelDO	        ,	0	},
	{ "TM_Rb5SelDO"                 , _K_D_IO,		TM_Rb5SelDO         ,	0	},
	  
	{ "TMSlitCyc1DO"                , _K_D_IO,		TMSlitCyc1DO        ,	0	},
	{ "TMSlitCyc2DO"                , _K_D_IO,		TMSlitCyc2DO        ,	0	},
	{ "TMSlitCyc3DO"                , _K_D_IO,		TMSlitCyc3DO        ,	0	},
	{ "TMSlitCyc4DO"                , _K_D_IO,		TMSlitCyc4DO        ,	0	},
	{ "TMSlitCyc5DO"                , _K_D_IO,		TMSlitCyc5DO        ,	0	},
	{ "TMSlitCyc1DI"                , _K_D_IO,		TMSlitCyc1DI        ,	0	},
	{ "TMSlitCyc2DI"                , _K_D_IO,		TMSlitCyc2DI        ,	0	},
	{ "TMSlitCyc3DI"                , _K_D_IO,		TMSlitCyc3DI        ,	0	},
	{ "TMSlitCyc4DI"                , _K_D_IO,		TMSlitCyc4DI        ,	0	},
	{ "TMSlitCyc5DI"                , _K_D_IO,		TMSlitCyc5DI        ,	0	},

	{ "PM_Sel1DO"                  , _K_D_IO,		PM_Sel1DO	        ,	0	},
	{ "PM_Sel2DO"                  , _K_D_IO,		PM_Sel2DO	        ,	0	},
	{ "PM_Sel3DO"                  , _K_D_IO,		PM_Sel3DO	        ,	0	},
	{ "PM_Sel4DO"                  , _K_D_IO,		PM_Sel4DO	        ,	0	},
	{ "PM_Sel5DO"                  , _K_D_IO,		PM_Sel5DO	        ,	0	},
	{ "PM_Slt1CycDO"               , _K_D_IO,		PM_Slt1CycDO	    ,	0	},
	{ "PM_Slt2CycDO"               , _K_D_IO,		PM_Slt2CycDO	    ,	0	},
	{ "PM_Slt3CycDO"               , _K_D_IO,		PM_Slt3CycDO	    ,	0	},
	{ "PM_Slt4CycDO"               , _K_D_IO,		PM_Slt4CycDO	    ,	0	},
	{ "PM_Slt5CycDO"               , _K_D_IO,		PM_Slt5CycDO	    ,	0	},
	{ "PM_Slt1CycDI"               , _K_D_IO,		PM_Slt1CycDI	    ,	0	},
	{ "PM_Slt2CycDI"               , _K_D_IO,		PM_Slt2CycDI	    ,	0	},
	{ "PM_Slt3CycDI"               , _K_D_IO,		PM_Slt3CycDI	    ,	0	},
	{ "PM_Slt4CycDI"               , _K_D_IO,		PM_Slt4CycDI	    ,	0	},
	{ "PM_Slt5CycDI"               , _K_D_IO,		PM_Slt5CycDI	    ,	0	},
	{ "Chuck1CycDO"                , _K_D_IO,		Chuck1CycDO			,	0	},
	{ "Chuck2CycDO"                , _K_D_IO,		Chuck2CycDO			,	0	},
	{ "Chuck3CycDO"                , _K_D_IO,		Chuck3CycDO			,	0	},
	{ "Chuck4CycDO"                , _K_D_IO,		Chuck4CycDO			,	0	},
	{ "Chuck5CycDO"                , _K_D_IO,		Chuck5CycDO			,	0	},
	{ "Chuck1CycDI"                , _K_D_IO,		Chuck1CycDI			,	0	},
	{ "Chuck2CycDI"                , _K_D_IO,		Chuck2CycDI			,	0	},
	{ "Chuck3CycDI"                , _K_D_IO,		Chuck3CycDI			,	0	},
	{ "Chuck4CycDI"                , _K_D_IO,		Chuck4CycDI			,	0	},
	{ "Chuck5CycDI"                , _K_D_IO,		Chuck5CycDI			,	0	},

	{ "Loop1CycDO"                , _K_D_IO,		Loop1CycDO			,	0	},
	{ "Loop2CycDO"                , _K_D_IO,		Loop2CycDO			,	0	},
	{ "Loop3CycDO"                , _K_D_IO,		Loop3CycDO			,	0	},
	{ "Loop4CycDO"                , _K_D_IO,		Loop4CycDO			,	0	},
	{ "Loop5CycDO"                , _K_D_IO,		Loop5CycDO			,	0	},

	{ "Loop1CycDI"                , _K_D_IO,		Loop1CycDI			,	0	},
	{ "Loop2CycDI"                , _K_D_IO,		Loop2CycDI			,	0	},
	{ "Loop3CycDI"                , _K_D_IO,		Loop3CycDI			,	0	},
	{ "Loop4CycDI"                , _K_D_IO,		Loop4CycDI			,	0	},
	{ "Loop5CycDI"                , _K_D_IO,		Loop5CycDI			,	0	},	

	{ "ChuckSel1DO"				  , _K_D_IO,		ChuckSel1DO			,	0	},	
	{ "ChuckSel2DO"		          , _K_D_IO,		ChuckSel2DO			,	0	},	
	{ "ChuckSel3DO"               , _K_D_IO,		ChuckSel3DO			,	0	},
	{ "ChuckSel4DO"		          , _K_D_IO,		ChuckSel4DO			,	0	},
	{ "ChuckSel5DO"		          , _K_D_IO,		ChuckSel5DO			,	0	},
	           
	{ "ChuckUpPos1"     	      , _K_A_IO,		ChuckUpPos1			,	0	},
	{ "ChuckUpPos2"     	      , _K_A_IO,		ChuckUpPos2			,	0	},
	{ "ChuckUpPos3"               , _K_A_IO,		ChuckUpPos3			,	0	},
	{ "ChuckUpPos4"     	      , _K_A_IO,		ChuckUpPos4			,	0	},
	{ "ChuckUpPos5"     	      , _K_A_IO,		ChuckUpPos5			,	0	}, 
	{ "ChuckDnPos1"     	      , _K_A_IO,		ChuckDnPos1			,	0	},
	{ "ChuckDnPos2"     	      , _K_A_IO,		ChuckDnPos2			,	0	},
	{ "ChuckDnPos3"               , _K_A_IO,		ChuckDnPos3			,	0	},
	{ "ChuckDnPos4"               , _K_A_IO,		ChuckDnPos4			,	0	},
	{ "ChuckDnPos5"     	      , _K_A_IO,		ChuckDnPos5			,	0	},

	{ "SCHEDULER_MAINT_BM1"       , _K_F_IO,		SCHEDULER_MAINT_BM1,	0	},
	{ "SCHEDULER_MAINT_BM2"       , _K_F_IO,		SCHEDULER_MAINT_BM2,	0	},
	
	{ "SCHEDULER_MAINT_TM"		  , _K_F_IO,		SCHEDULER_MAINT_TM  ,	0	},

	{ "TM.MAINT_FINGER"		      , _K_D_IO,		TM_MAINT_FINGER     ,	0	},
	{ "TM.MAINT_STATION3"		  , _K_D_IO,		TM_MAINT_STATION3	,	0	},
	{ "TM.MAINT_STATION4"		  , _K_D_IO,		TM_MAINT_STATION4	,	0	},

	{ "PM1.ZMT_StageSelectDM"	  , _K_D_IO,		PM1_ZMT_StageSelectDM	,	0	},	
	{ "PM2.ZMT_StageSelectDM"	  , _K_D_IO,		PM2_ZMT_StageSelectDM	,	0	},
	{ "PM3.ZMT_StageSelectDM"	  , _K_D_IO,		PM3_ZMT_StageSelectDM	,	0	},

	{ "PM1.ZMT_MaintSetPosAM"	  , _K_A_IO,		PM1_ZMT_MaintSetPosAM	,	0	},
	{ "PM2.ZMT_MaintSetPosAM"	  , _K_A_IO,		PM2_ZMT_MaintSetPosAM	,	0	},
	{ "PM3.ZMT_MaintSetPosAM"	  , _K_A_IO,		PM3_ZMT_MaintSetPosAM	,	0	},
	
	{ "CTC.PM1_CommStatus"		  , _K_D_IO,		PM1_CommStatus	,	0	},	
	{ "CTC.PM2_CommStatus"		  , _K_D_IO,		PM2_CommStatus	,	0	},
	{ "CTC.PM3_CommStatus"		  , _K_D_IO,		PM3_CommStatus	,	0	},
	
	{ "PM1.PRO_RunStsDM"		  , _K_D_IO,		PM1_PRO_RunStsDM	,	0	},
	{ "PM2.PRO_RunStsDM"		  , _K_D_IO,		PM2_PRO_RunStsDM	,	0	},
	{ "PM3.PRO_RunStsDM"		  , _K_D_IO,		PM3_PRO_RunStsDM	,	0	},
	{ "PCJobSts"				  , _K_D_IO,		PCJobSts			,	0	},
	{ "CTC.MAIN_CONTROL"		  , _K_D_IO,		CTC_MAIN_CONTROL	,	0	},
	{ "CTC.MAIN_CONTROL2"		  , _K_D_IO,		CTC_MAIN_CONTROL2	,	0	},
	{ "CTC.MAIN_CONTROL3"		  , _K_D_IO,		CTC_MAIN_CONTROL3	,	0	},
	{ "AutoPC_ParmSaveDM"		  , _K_D_IO,		AutoPC_ParmSaveDM	,	0	},
	{ "UPC_CHECK1DO"			  , _K_D_IO,		UPC_CHECK1DO		,	0	},
	{ "UPC_CHECK2DO"		      , _K_D_IO,		UPC_CHECK2DO		,	0	},
	{ "UPC_CHECK3DO"		      , _K_D_IO,		UPC_CHECK3DO		,	0	},
	{ "UPC_CHECK4DO"		      , _K_D_IO,		UPC_CHECK4DO		,	0	},
	{ "UPC_CHECK5DO"		      , _K_D_IO,		UPC_CHECK5DO		,	0	},
	{ "TMC_TM_OVERPRESSURE"		  , _K_F_IO,		TMC_TM_OVERPRESSURE	,	0	},
	//...end 2015.04.17
	//... 2016.03.03
	{ "LPMA.ShuttleInOutXI"		  , _K_D_IO,		LPMA_ShuttleInOutXI	,	0	},
	{ "LPMB.ShuttleInOutXI"		  , _K_D_IO,		LPMB_ShuttleInOutXI	,	0	},
	{ "LPMC.ShuttleInOutXI"		  , _K_D_IO,		LPMC_ShuttleInOutXI	,	0	},
	{ "LPMA.DoorUpDownXI"		  , _K_D_IO,		LPMA_DoorUpDownXI	,	0	},
	{ "LPMB.DoorUpDownXI"		  , _K_D_IO,		LPMB_DoorUpDownXI	,	0	},
	{ "LPMC.DoorUpDownXI"		  , _K_D_IO,		LPMC_DoorUpDownXI	,	0	},
	//...
	""
	//=============================================================================
};


extern "C" void Screen_Nav_Info( int no )
{
	//Class destruct here
	if (no == -1) {
		delete g_pTransferManager;
		g_pTransferManager = NULL;	
	}
}

extern "C" BOOL Program_Init_Code()
{
	//Class Construct here
	if (NULL == g_pTransferManager) {
		g_pTransferManager = new CTransferManager();
		if (FALSE == g_pTransferManager->InitConfigFile(TRANSFER_INFO_FILE)) {
			printf("Error - TransferManager Initialize file %s \n" ,TRANSFER_INFO_FILE );
			return FALSE;
		}
		if (FALSE == g_pTransferManager->InitManager()) {
			printf("Error - TransferManager System Create Fail \n");
			return FALSE;
		}
	} 	
	return TRUE;
}

extern "C" void Event_Message_Received()
{
	
}

extern "C" void Program_Enter_Code()
{
	//When Function Loaded by engine
	
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main()
{
	int Result , CommStatus;
	//-------------------------------------------------------------------------------------------
	//2007.07.30
	WRITE_DIGITAL( MANUAL_TR_CONTROL , 1 , &CommStatus );
	//2007.07.30

	if (STRCMP_L( "SYS_AL_HOME" , PROGRAM_PARAMETER_READ())) {
	} else if (STRCMP_L("SYS_TRS_PICK" , PROGRAM_PARAMETER_READ()))      {
		Result = g_pTransferManager->GetWafer();		
	} else if (STRCMP_L("SYS_TRS_PLACE" , PROGRAM_PARAMETER_READ()))     {
		Result = g_pTransferManager->PutWafer();	
	} else if (STRCMP_L("SYS_TRS_MOVE" , PROGRAM_PARAMETER_READ()))      {
		Result = g_pTransferManager->MoveWafer();
	} else if (STRCMP_L("SYS_TRS_CLEAR" , PROGRAM_PARAMETER_READ()))     {
		Result = g_pTransferManager->ClearWafer();
	} else if (STRCMP_L("SYS_DUMMY_AUTOEXEC", PROGRAM_PARAMETER_READ())) {
		Result = g_pTransferManager->DummyAutoExchange();
	} else if (STRCMP_L("SYS_DUMMY_SELEXEC" , PROGRAM_PARAMETER_READ())) {
		Result = g_pTransferManager->DummySelectExchange();
	} else if (STRCMP_L("DUMMY_RESET" , PROGRAM_PARAMETER_READ())) {
		Result = g_pTransferManager->DummySelectSlotReset();
	} else if (STRCMP_L("CM_RESET" , PROGRAM_PARAMETER_READ())) {
		Result = g_pTransferManager->DummyCMSelectSlotReset();
	} else if (STRCMP_L("PARTICLE_CHECK" , PROGRAM_PARAMETER_READ())) {
		Result = g_pTransferManager->Auto_Particle_Check();
	} else {
		Result = SYS_ABORTED;
	}

	//2007.07.30
	WRITE_DIGITAL(MANUAL_TR_CONTROL , 0 , &CommStatus);
	//2007.07.30
	WRITE_DIGITAL(INFO_SAVE , 1 , &CommStatus);

	return (Module_Status)Result;
	
}

