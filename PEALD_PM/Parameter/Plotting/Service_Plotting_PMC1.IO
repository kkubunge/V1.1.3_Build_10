#---------------------------------------------------------------------------
$COLOR_TABLE_FILE		"TABLE/PALLETTE.tbl"
#---------------------------------------------------------------------------
$DATALOG_CONTROL		PlotforProcRunDM
$DATALOG_FILE			$DATE_TIME_BASED
$DATALOG_DIRECTORY		DATALOG/SIDE1
$DATALOG_TIME			1
$DATALOG_AUTO_DELETE_TIME	1

$DATALOG_STRING_IO1		PRO_RcpNameSM

#---------------------------------------------------------------------------
$INFORMATION_FILE		"PARAMETER/Plotting/Service_Plotting_PMC1.inf"
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

ZMT_ReadPosAM		"ChuckPos"		ChuckPos

RFSetPointAO		"RFSetPow"		RFSetPow
RF1LoadPowAI		"RFLoadPow"		RFLoadPow
RF1RefPowAI		"RFRefPow"		RFRefPow
RF1FrequencyAI		"RFFreq"		RFFreq

RPGLoadPowAI		"RPGLoadPow"		RPGLoadPow
RPGRefPowAI		"RPGRefPow"		RPGRefPow

SHT1_TmpRdAI		"SHT1_Temp"		SHT1_Temp
SHT1_OutPwAI		"SHT1_Pow"		SHT1_Pow
StgHtr1CurrAI		"SHT1_Cur"		SHT1_Cur
StgHtr1VoltAI		"SHT1_Volt"		SHT1_Volt

MFC01FlwAI		"MFC01Flw"		MFC01Flw
MFC02FlwAI		"MFC02Flw"		MFC02Flw
MFC03FlwAI		"MFC03Flw"		MFC03Flw
MFC04FlwAI		"MFC04Flw"		MFC04Flw

MFC01PressAI		"MFC01Prs"		MFC01Prs
MFC02PressAI		"MFC02Prs"		MFC02Prs
MFC03PressAI		"MFC03Prs"		MFC03Prs
MFC04PressAI		"MFC04Prs"		MFC04Prs

HT01_TmpRdAI		"HT01_Temp"		HT01_Temp
HT02_TmpRdAI		"HT02_Temp"		HT02_Temp
HT03_TmpRdAI		"HT03_Temp"		HT03_Temp
HT04_TmpRdAI		"HT04_Temp"		HT04_Temp
HT05_TmpRdAI		"HT05_Temp"		HT05_Temp
HT06_TmpRdAI		"HT06_Temp"		HT06_Temp
HT07_TmpRdAI		"HT07_Temp"		HT07_Temp
HT08_TmpRdAI		"HT08_Temp"		HT08_Temp
HT09_TmpRdAI		"HT09_Temp"		HT09_Temp
HT10_TmpRdAI		"HT10_Temp"		HT10_Temp
HT11_TmpRdAI		"HT11_Temp"		HT11_Temp
HT12_TmpRdAI		"HT12_Temp"		HT12_Temp
HT13_TmpRdAI		"HT13_Temp"		HT13_Temp
HT14_TmpRdAI		"HT14_Temp"		HT14_Temp
HT15_TmpRdAI		"HT15_Temp"		HT15_Temp
HT16_TmpRdAI		"HT16_Temp"		HT16_Temp

SH1HeatExFlwAI		"SH1HeatExFlw"		SH1HeatExFlw
HtExcSupTempAI		"HtExcSupTemp"		HtExcSupTemp
HtExcRetTempAI		"HtExcRetTemp"		HtExcRetTemp

RF1ForwardAI            "RF1ForwardAI"		RF1ForwardAI        
HF_LOAD_POS_A	        "HF_LOAD_POS_A"	        HF_LOAD_POS_A	
HF_TUNE_POS_A	        "HF_TUNE_POS_A"	        HF_TUNE_POS_A	
HF_R_Real_A	        "HF_R_Real_A"	        HF_R_Real_A	
HF_X_Image_A	        "HF_X_Image_A"	        HF_X_Image_A	
HF_Z_Phase_A	        "HF_Z_Phase_A"	        HF_Z_Phase_A	
HF_Z_Power_A	        "HF_Z_Power_A"	        HF_Z_Power_A	
HF_Z_R_A	        "HF_Z_R_A"	        HF_Z_R_A	
HF_Z_X_A	        "HF_Z_X_A"	        HF_Z_X_A	
HF_Z_Vrms_A	        "HF_Z_Vrms_A"	        HF_Z_Vrms_A	
HF_Z_Irms_A	        "HF_Z_Irms_A"	        HF_Z_Irms_A	