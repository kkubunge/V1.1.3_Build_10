#---------------------------------------------------------------------------
$COLOR_TABLE_FILE	"Engine/TABLE/PALLETTE.tbl"
#---------------------------------------------------------------------------
$DATALOG_CONTROL	PM3.FC_Plotting_Control
$DATALOG_FILE		$DATE_TIME_BASED
$DATALOG_TIME		1
$DATALOG_DIRECTORY      F:\Plotter\PM3
#---------------------------------------------------------------------------
$INFORMATION_FILE	"PARAMETER/PLOTTING/Service_Plotting_PM3.inf"
#---------------------------------------------------------------------------

#$HEADER_MESSAGE	PM3.Process_Info	680	11	28

# Pressure & Throttle

PM3.ChmPrsAI		"PM3 Chamber Pressure"		PM3.ChmPrsAI
PM3.PumpPrsAI		"PM3 Pump line Pressure"	PM3.PumpPrsAI
PM3.M651_Pressure	"PM3 Throttle Pressure"		PM3.M651_Pressure
PM3.M651_Position	"PM3 Throttle Position"		PM3.M651_Position

# Stage Heater
PM3.SHT1_TmpRdAI	"PM3 Stage Heater 1"			PM3.SHT1_TmpRdAI
PM3.StgHtr1CurrAI	"PM3 Stage Heater Current1"		PM3.StgHtr1CurrAI
PM3.StgHtr1VoltAI	"PM3 Stage heater Voltage1"		PM3.StgHtr1VoltAI

PM3.SHT2_TmpRdAI	"PM3 Stage Heater 2"			PM3.SHT2_TmpRdAI
PM3.StgHtr2CurrAI	"PM3 Stage Heater Current2"		PM3.StgHtr2CurrAI
PM3.StgHtr2VoltAI	"PM3 Stage heater Voltage2"		PM3.StgHtr2VoltAI

# MFC & Valve


# WT300 Heater Channel
PM3.HT03_TmpRdAI        "Heater3"	PM3.HT03_TmpRdAI
PM3.HT04_TmpRdAI        "Heater4"	PM3.HT04_TmpRdAI
PM3.HT05_TmpRdAI        "Heater5"	PM3.HT05_TmpRdAI
PM3.HT06_TmpRdAI        "Heater6"	PM3.HT06_TmpRdAI
PM3.HT12_TmpRdAI        "Heater12"	PM3.HT12_TmpRdAI
PM3.HT13_TmpRdAI        "Heater13"	PM3.HT13_TmpRdAI
PM3.HT19_TmpRdAI        "Heater19"	PM3.HT19_TmpRdAI
PM3.HT20_TmpRdAI        "Heater20"	PM3.HT20_TmpRdAI
PM3.HT21_TmpRdAI        "Heater21"	PM3.HT21_TmpRdAI
PM3.HT22_TmpRdAI        "Heater22"	PM3.HT22_TmpRdAI
PM3.HT28_TmpRdAI        "Heater28"	PM3.HT28_TmpRdAI
PM3.HT29_TmpRdAI        "Heater29"	PM3.HT29_TmpRdAI
# Matcher
PM3.HF_LOAD_POS_A	"Matcher1 Load"	PM3.HF_LOAD_POS_A
PM3.HF_TUNE_POS_A	"Matcher1 Tune"	PM3.HF_TUNE_POS_A
PM3.HF_LOAD_POS_B	"Matcher2 Load"	PM3.HF_LOAD_POS_B
PM3.HF_TUNE_POS_B	"Matcher1 Tune"	PM3.HF_TUNE_POS_B