#---------------------------------------------------------------------------
$COLOR_TABLE_FILE	"Engine/TABLE/PALLETTE.tbl"
#---------------------------------------------------------------------------
$DATALOG_CONTROL	PM2.FC_Plotting_Control
$DATALOG_FILE		$DATE_TIME_BASED
$DATALOG_TIME		1
$DATALOG_DIRECTORY      F:\Plotter\PM2
#---------------------------------------------------------------------------
$INFORMATION_FILE	"PARAMETER/PLOTTING/Service_Plotting_PM2.inf"
#---------------------------------------------------------------------------

#$HEADER_MESSAGE	PM2.Process_Info	680	11	28

# Pressure & Throttle

PM2.ChmPrsAI		"PM2 Chamber Pressure"		PM2.ChmPrsAI
PM2.PumpPrsAI		"PM2 Pump line Pressure"	PM2.PumpPrsAI
PM2.M651_Pressure	"PM2 Throttle Pressure"		PM2.M651_Pressure
PM2.M651_Position	"PM2 Throttle Position"		PM2.M651_Position

# Stage Heater
PM2.SHT1_TmpRdAI	"PM2 Stage Heater 1"			PM2.SHT1_TmpRdAI
PM2.StgHtr1CurrAI	"PM2 Stage Heater Current1"		PM2.StgHtr1CurrAI
PM2.StgHtr1VoltAI	"PM2 Stage heater Voltage1"		PM2.StgHtr1VoltAI

PM2.SHT2_TmpRdAI	"PM2 Stage Heater 2"			PM2.SHT2_TmpRdAI
PM2.StgHtr2CurrAI	"PM2 Stage Heater Current2"		PM2.StgHtr2CurrAI
PM2.StgHtr2VoltAI	"PM2 Stage heater Voltage2"		PM2.StgHtr2VoltAI

# MFC & Valve


# WT300 Heater Channel
PM2.HT03_TmpRdAI        "Heater3"	PM2.HT03_TmpRdAI
PM2.HT04_TmpRdAI        "Heater4"	PM2.HT04_TmpRdAI
PM2.HT05_TmpRdAI        "Heater5"	PM2.HT05_TmpRdAI
PM2.HT06_TmpRdAI        "Heater6"	PM2.HT06_TmpRdAI
PM2.HT12_TmpRdAI        "Heater12"	PM2.HT12_TmpRdAI
PM2.HT13_TmpRdAI        "Heater13"	PM2.HT13_TmpRdAI
PM2.HT19_TmpRdAI        "Heater19"	PM2.HT19_TmpRdAI
PM2.HT20_TmpRdAI        "Heater20"	PM2.HT20_TmpRdAI
PM2.HT21_TmpRdAI        "Heater21"	PM2.HT21_TmpRdAI
PM2.HT22_TmpRdAI        "Heater22"	PM2.HT22_TmpRdAI
PM2.HT28_TmpRdAI        "Heater28"	PM2.HT28_TmpRdAI
PM2.HT29_TmpRdAI        "Heater29"	PM2.HT29_TmpRdAI
# Matcher
PM2.HF_LOAD_POS_A	"Matcher1 Load"	PM2.HF_LOAD_POS_A
PM2.HF_TUNE_POS_A	"Matcher1 Tune"	PM2.HF_TUNE_POS_A
PM2.HF_LOAD_POS_B	"Matcher2 Load"	PM2.HF_LOAD_POS_B
PM2.HF_TUNE_POS_B	"Matcher1 Tune"	PM2.HF_TUNE_POS_B