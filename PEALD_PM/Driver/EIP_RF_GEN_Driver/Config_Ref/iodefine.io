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
#----------------------------------------------------------------------------------------------------------------------

#----------------------------------------------------------------------------------------------------------------------
# Matcher [AE]
$$$	Module	MATCHER1	uLOAD	driver/AE_MatcherZ1.dll	2	115200 	8    	1    	1	0   	0   	1    	2  	500
$$$	Module	MATCHER2	uLOAD	driver/AE_MatcherZ2.dll	4	115200 	8    	1    	1	0   	0   	1    	2  	500

$$$ 	Module	PFMat		uLOAD	driver/PFMatcher.dll	10	19200	8   	1    	0    	0    	0    	0    	0     	1000
$$$ 	Module	PFMat2		uLOAD	driver/PFMatcher.dll	10	19200	8    	1    	0    	0    	0    	0    	0     	1000
#----------------------------------------------------------------------------------------------------------------------
# RF Generator [AE]
$$$	Module	LFGS1		uLOAD	driver/LFGS1.dll	15	19200 	8   	1    	1    	0    	0    	0    	0 	2000
$$$	Module	LFGS2		uLOAD	driver/LFGS2.dll	16	19200 	8    	1    	1    	0    	0    	0    	0  	2000

$$$	Module	EIP_RF1		uLOAD	driver/EIP_RF.dll	0	0 	0   	0    	0   	0    	0    	0    	0 	1000
#----------------------------------------------------------------------------------------------------------------------

# Pressure Controller [MKS]
$$$	Module	MKS651		uLOAD	driver/Mks651.dll	7	9600	8	1	0	0	0	0	0	2000

#----------------------------------------------------------------------------------------------------------------------
# Motor Controller
$$$	Module	RORZE		uLOAD	driver/Rorze_Route.dll	8	9600	8	1	0	0	0	0	0	500

#----------------------------------------------------------------------------------------------------------------------
# Normal Heater [MLS332]
# ID9 : Modebus Controller Address (Node Number)
$$$	Module	WT300		uLOAD	driver/Wt300.dll	11	9600	8	1	0	0	0	0	1	3000	#  1 ~ 32 Channel
$$$	Module	WT332		uLOAD	driver/Wt332.dll	12	9600	8	1	0	0	0	0	1	3000	# 33 ~ 64 Channel

#----------------------------------------------------------------------------------------------------------------------
#	ID9 : Axis Number for AutoHome (0:NoAutoHome,  1:Axis[1], 2:Axis[2], 12: Axis[1~2],  25: Axis[2~5])
# NotUse PEALD
$$$	Module	VIX250		uLOAD	driver/VIX250/Vix250.dll	5	9600	8	1	0	0	0	0	0	1000
$$$	Module	VIX250B		uLOAD	driver/VIX250b/Vix250.dll	6	9600	8	1	0	0	0	0	0	1000
#----------------------------------------------------------------------------------------------------------------------
#                                                            NetworkID                SM Use
$$$	Module	HilSch	LOAD	driver/EnetIP.dll		2	0	0	1
#----------------------------------------------------------------------------------------------------------------------

#######################################################################################################################
#$$$	Module	HilSch		uLOAD	driver/hs_dnet.dll		1000	0	0	1
$$$	Module	MEM		uLOAD	Mem.dll

$$$	F	k:/System/System.io

$$$	F	IO/GUI/GuiControl.con
$$$	F	IO/GUI/GuiMonitor.con

$$$	F	IO/LOOKUP/lookup.io
$$$	F	IO/Enum/PMEnum.cfg

$$$	F	IO/CFGS/Mks651.cfg
$$$	F	IO/CFGS/DeviceNet_PEALD.cfg
$$$	F	IO/CFGS/DeviceNet_MFC.cfg
$$$	F	IO/CFGS/WT300_Heater.cfg
$$$	F	IO/CFGS/WT332_Heater.cfg
$$$	F	IO/CFGS/VIX250.cfg
$$$	F	IO/CFGS/ERTH-3508_DeviceNet.cfg
$$$	F	IO/CFGS/PFMatcher.cfg
$$$	F	IO/CFGS/RorzeMtr.cfg
$$$	F	IO/CFGS/AE_RFMatcherZ1.cfg
$$$	F	IO/CFGS/AE_RFMatcherZ2.cfg
$$$	F	IO/CFGS/LFGS1.cfg
$$$	F	IO/CFGS/LFGS2.cfg
$$$	F	IO/CFGS/EIP_RF1.cfg

$$$	F	IO/CONS/Plotting.con
$$$	F	IO/CONS/PMC_ScreenEditor.con
$$$	F	IO/CONS/PMC_Process.con
$$$	F	IO/CONS/PMC_Parameter.con
$$$	F	IO/CONS/PMC_LeakCheck.con
$$$	F	IO/CONS/PMC_Vacuum.con
$$$	F	IO/CONS/PMC_AbortGas.con
$$$	F	IO/CONS/PMC_PRESSCONTROL_TV.con
$$$	F	IO/CONS/PMC_Datalog.con
$$$	F	IO/CONS/DeviceUseConfig.con
$$$	F	IO/CONS/HeaterUseConfig.con
$$$	F	IO/CONS/StgHtrUseConfig_E5ER.con
$$$	F	IO/CONS/MfcUseConfig.con
$$$	F	IO/CONS/PMC_TrapControl.con
$$$	F	IO/CONS/PMC_MotorControl.con	# for MP Motor Control
$$$ 	F    IO/CONS/Gas_Calibration.con
