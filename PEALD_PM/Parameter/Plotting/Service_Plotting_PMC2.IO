#---------------------------------------------------------------------------
$COLOR_TABLE_FILE		"TABLE/PALLETTE.tbl"
#---------------------------------------------------------------------------
$DATALOG_CONTROL		PlotforProcRunDM
$DATALOG_FILE			$DATE_TIME_BASED
$DATALOG_DIRECTORY		DATALOG/SIDE2
$DATALOG_TIME			1
$DATALOG_AUTO_DELETE_TIME	1

$DATALOG_STRING_IO1		PRO_RcpNameSM

#---------------------------------------------------------------------------
$INFORMATION_FILE		"PARAMETER/Plotting/Service_Plotting_PMC2.inf"
#---------------------------------------------------------------------------
# MAX ITEM : 95

PRO_CurStepAM		"PrcsStep"		PrcsStep
PRO_StepTimeAM		"PrcsTime"		PrcsTime

PLC_CurCycAI		"PrcsStepCyc"		PrcsStepCyc
PRO_StpGrpCCycAM	"StepGrpCyc"		StepGrpCyc

M651_Pressure		"ThrtPrs"		ThrtPrs
M651_Position		"ThrtPos"		ThrtPos
ChmPrsAI		"ChmPrs"		ChmPrs
PumpPrsAI		"PumpPrs"		PumpPrs

ZMT_ReadPos2AM		"Chuck2Pos"		Chuck2Pos

RFSetPoint2AO		"RF2SetPow"		RF2SetPow
RFLoadPow2AI		"RF2LoadPow"		RF2LoadPow
RFRefPow2AI		"RF2RefPow"		RF2RefPow
RF2FrequencyAI		"RF2Freq"		RF2Freq

RPGLoadPowAI		"RPGLoadPow"		RPGLoadPow
RPGRefPowAI		"RPGRefPow"		RPGRefPow

SHT2_TmpRdAI		"SHT2_Temp"		SHT2_Temp
SHT2_OutPwAI		"SHT2_Pow"		SHT2_Pow
StgHtr2CurrAI		"SHT2_Cur"		SHT2_Cur
StgHtr2VoltAI		"SHT2_Volt"		SHT2_Volt

MFC09FlwAI		"MFC09Flw"		MFC09Flw
MFC08FlwAI		"MFC08Flw"		MFC08Flw
MFC07FlwAI		"MFC07Flw"		MFC07Flw
MFC04FlwAI		"MFC04Flw"		MFC04Flw
MFC05FlwAI		"MFC05Flw"		MFC05Flw
MFC06FlwAI		"MFC06Flw"		MFC06Flw

MFC09PressAI		"MFC09Prs"		MFC09Prs
MFC08PressAI		"MFC08Prs"		MFC08Prs
MFC07PressAI		"MFC07Prs"		MFC07Prs
MFC04PressAI		"MFC04Prs"		MFC04Prs
MFC05PressAI		"MFC05Prs"		MFC05Prs
MFC06PressAI		"MFC06Prs"		MFC06Prs

HT17_TmpRdAI		"HT01_Temp"		HT17_Temp
HT18_TmpRdAI		"HT02_Temp"		HT18_Temp
HT19_TmpRdAI		"HT03_Temp"		HT19_Temp
HT20_TmpRdAI		"HT04_Temp"		HT20_Temp
HT21_TmpRdAI		"HT05_Temp"		HT21_Temp
HT22_TmpRdAI		"HT06_Temp"		HT22_Temp
HT23_TmpRdAI		"HT07_Temp"		HT23_Temp
HT24_TmpRdAI		"HT08_Temp"		HT24_Temp
HT25_TmpRdAI		"HT09_Temp"		HT25_Temp
HT26_TmpRdAI		"HT10_Temp"		HT26_Temp
HT27_TmpRdAI		"HT11_Temp"		HT27_Temp
HT28_TmpRdAI		"HT12_Temp"		HT28_Temp
HT29_TmpRdAI		"HT13_Temp"		HT29_Temp
HT30_TmpRdAI		"HT14_Temp"		HT30_Temp
HT31_TmpRdAI		"HT15_Temp"		HT31_Temp
HT32_TmpRdAI		"HT16_Temp"		HT32_Temp

SH2HeatExFlwAI		"SH2HeatExFlw"		SH2HeatExFlw
HtExcSupTempAI		"HtExcSupTemp"		HtExcSupTemp
HtExcRetTempAI		"HtExcRetTemp"		HtExcRetTemp

RF2ForwardAI            "RF2ForwardAI"		RF2ForwardAI        
HF_LOAD_POS_B	        "HF_LOAD_POS_B"	        HF_LOAD_POS_B	
HF_TUNE_POS_B	        "HF_TUNE_POS_B"	        HF_TUNE_POS_B	
HF_R_Real_B	        "HF_R_Real_B"	        HF_R_Real_B	
HF_X_Image_B	        "HF_X_Image_B"	        HF_X_Image_B	
HF_Z_Phase_B	        "HF_Z_Phase_B"	        HF_Z_Phase_B	
HF_Z_Power_B	        "HF_Z_Power_B"	        HF_Z_Power_B	
HF_Z_R_B	        "HF_Z_R_B"	        HF_Z_R_B	
HF_Z_X_B	        "HF_Z_X_B"	        HF_Z_X_B	
HF_Z_Vrms_B	        "HF_Z_Vrms_B"	        HF_Z_Vrms_B	
HF_Z_Irms_B	        "HF_Z_Irms_B"	        HF_Z_Irms_B	