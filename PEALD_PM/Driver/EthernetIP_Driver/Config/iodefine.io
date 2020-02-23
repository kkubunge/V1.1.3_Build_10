#----------------------------------------------------------------------------------------------------------------------
# ID1 : Port Number															
# ID2 : Baud Rate															
# ID3 : Data Bit															
# ID4 : Stop Bit(1:1bit, 2:2bit, 3:1.5bit)															
# ID5 : Parity Bit(0:NoParity, 1:OddParity, 2:EvenParity)															
# ID6 : DTR/DSR 사용 여부(0:사용않함, 1:사용함)															
# ID7 : RTS/CTS 사용 여부(0:사용안함, 1:사용함)															
# ID8 : XON/XOFF 사용여부(0:사용않함, 1:사용함)															
# ID10 : Timeout Time (ms)															

#----------------------------------------------------------------------------------------------------------------------
$$$ Module   MKS651    uLOAD     driver/Mks651.dll       7 	9600    8    1    0    0    0    0    0     3000
#----------------------------------------------------------------------------------------------------------------------

#----------------------------------------------------------------------------------------------------------------------
# Normal(MLS300) Heater
# ID9 : Modebus Controller Address (Node Number)															
#$$$	Module	WT300	LOAD	driver/Wt300.dll	6	9600	8	1	0	0	0	0	1	3000		#17 ~ 48 Channel

$$$	Module	MINI8_1	uLOAD	driver/Mini8_1.dll	6	9600	8	1	0	0	0	0	1	3000		
$$$	Module	MINI8_2	uLOAD	driver/Mini8_2.dll	10	9600	8	1	0	0	0	0	1	3000		
$$$	Module	MINI8_3	uLOAD	driver/Mini8_3.dll	11	9600	8	1	0	0	0	0	1	3000		

#----------------------------------------------------------------------------------------------------------------------

#----------------------------------------------------------------------------------------------------------------------
#	ID9 : Axis Number for AutoHome (0:NoAutoHome,  1:Axis[1], 2:Axis[2], 12: Axis[1~2],  25: Axis[2~5])
$$$	Module	VIX250	uLOAD	driver/Vix250.dll	5	9600	8	1	0	0	0	0 0 1000
#----------------------------------------------------------------------------------------------------------------------


#----------------------------------------------------------------------------------------------------------------------
#                                                            IPPortID                  SM
$$$	Module	HilSch	LOAD	driver/EnetIP.dll		2	0	0	1
#----------------------------------------------------------------------------------------------------------------------
$$$	Module	MEM	uLOAD	Mem.dll
#######################################################################################################################
#----------------------------------------------------------------------------------------------------------------------
# ID9 :	0 -> CTC Raid
#	1 -> PMC Raid Type1 (BLACK)
#	2 -> PMC Raid Type2 (Old Type GRAY)
#
$$$	Module	RAID	uLOAD	driver/RAID.dll		2	9600	8	1	0	0	0	0	1	3000		
#----------------------------------------------------------------------------------------------------------------------

$$$  F    k:/System/System.io

$$$  F    IO/GUI/GuiControl.con
$$$  F    IO/GUI/GuiMonitor.con

$$$  F    IO/LOOKUP/lookup.io
$$$  F    IO/Enum/PMEnum.cfg

$$$  F    IO/CFGS/Mks651.cfg
$$$  F    IO/CFGS/DeviceNet_TiN.cfg
#$$$  F    IO/CFGS/WT300_Heater.cfg
$$$  F    IO/CFGS/Mini8_Heater_1.cfg    
$$$  F    IO/CFGS/Mini8_Heater_2.cfg
$$$  F    IO/CFGS/Mini8_Heater_3.cfg
$$$  F    IO/CFGS/PFMatcher.cfg
#$$$  F    IO/CFGS/RAID_PMC.cfg

$$$  F    IO/CFGS/VIX250.cfg
$$$  F    IO/CFGS/ERTH-3508_DeviceNet.cfg

$$$  F    IO/CONS/Plotting.con
$$$  F    IO/CONS/PMC_ScreenEditor.con
$$$  F    IO/CONS/PMC_Process.con
$$$  F    IO/CONS/PMC_Parameter.con
$$$  F    IO/CONS/PMC_LeakCheck.con
$$$  F    IO/CONS/PMC_Vacuum.con
$$$  F    IO/CONS/PMC_AbortGas.con
$$$  F    IO/CONS/PMC_PRESSCONTROL_TV.con
$$$  F    IO/CONS/PMC_Datalog.con
$$$  F    IO/CONS/DeviceUseConfig.con
$$$  F    IO/CONS/HeaterUseConfig.con
$$$  F    IO/CONS/StgHtrUseConfig_E5ER.con
$$$  F    IO/CONS/MfcUseConfig.con
$$$  F    IO/CONS/Gas_Calibration.con
