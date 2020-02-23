#---------------------------------------------------------------------------
$COLOR_TABLE_FILE	"Engine/TABLE/PALLETTE.tbl"
#---------------------------------------------------------------------------
$DATALOG_CONTROL	PM1.FC_Plotting_Control
$DATALOG_FILE		$DATE_TIME_BASED
$DATALOG_TIME		1
$DATALOG_DIRECTORY      F:\Plotter\PM1
#---------------------------------------------------------------------------
$INFORMATION_FILE	"PARAMETER/PLOTTING/Service_Plotting_PM1.inf"
#---------------------------------------------------------------------------

#$HEADER_MESSAGE	PM1.Process_Info	680	11	28

# Pressure & Throttle

PM1.ChmPrsAI		"PM1 Chamber Pressure"		PM1.ChmPrsAI
PM1.PumpPrsAI		"PM1 Pump line Pressure"	PM1.PumpPrsAI
PM1.M651_Pressure	"PM1 Throttle Pressure"		PM1.M651_Pressure
PM1.M651_Position	"PM1 Throttle Position"		PM1.M651_Position

# Stage Heater
PM1.SHT1_TmpRdAI	"PM1 Stage Heater 1"			PM1.SHT1_TmpRdAI
PM1.StgHtr1CurrAI	"PM1 Stage Heater Current1"		PM1.StgHtr1CurrAI
PM1.StgHtr1VoltAI	"PM1 Stage heater Voltage1"		PM1.StgHtr1VoltAI

PM1.SHT2_TmpRdAI	"PM1 Stage Heater 2"			PM1.SHT2_TmpRdAI
PM1.StgHtr2CurrAI	"PM1 Stage Heater Current2"		PM1.StgHtr2CurrAI
PM1.StgHtr2VoltAI	"PM1 Stage heater Voltage2"		PM1.StgHtr2VoltAI

# MFC & Valve


# WT300 Heater Channel
PM1.HT03_TmpRdAI        "Heater3"	PM1.HT03_TmpRdAI
PM1.HT04_TmpRdAI        "Heater4"	PM1.HT04_TmpRdAI
PM1.HT05_TmpRdAI        "Heater5"	PM1.HT05_TmpRdAI
PM1.HT06_TmpRdAI        "Heater6"	PM1.HT06_TmpRdAI
PM1.HT12_TmpRdAI        "Heater12"	PM1.HT12_TmpRdAI
PM1.HT13_TmpRdAI        "Heater13"	PM1.HT13_TmpRdAI
PM1.HT19_TmpRdAI        "Heater19"	PM1.HT19_TmpRdAI
PM1.HT20_TmpRdAI        "Heater20"	PM1.HT20_TmpRdAI
PM1.HT21_TmpRdAI        "Heater21"	PM1.HT21_TmpRdAI
PM1.HT22_TmpRdAI        "Heater22"	PM1.HT22_TmpRdAI
PM1.HT28_TmpRdAI        "Heater28"	PM1.HT28_TmpRdAI
PM1.HT29_TmpRdAI        "Heater29"	PM1.HT29_TmpRdAI
# Matcher
PM1.HF_LOAD_POS_A	"Matcher1 Load"	PM1.HF_LOAD_POS_A
PM1.HF_TUNE_POS_A	"Matcher1 Tune"	PM1.HF_TUNE_POS_A
PM1.HF_LOAD_POS_B	"Matcher2 Load"	PM1.HF_LOAD_POS_B
PM1.HF_TUNE_POS_B	"Matcher1 Tune"	PM1.HF_TUNE_POS_B