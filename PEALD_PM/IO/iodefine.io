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

#######################################################################################################################
# MOXA 4 PORT
#----------------------------------------------------------------------------------------------------------------------
# Motor Controller
$$$	Module	RORZE		LOAD	driver/Rorze_Route.dll		2	9600	8	1	0	0	0	0	0	500

#----------------------------------------------------------------------------------------------------------------------
# Normal Heater [MLS332]
# ID9 : Modebus Controller Address (Node Number)
$$$	Module	WT332		LOAD	driver/Wt332.dll		4	9600	8	1	0	0	0	0	1	3000	#  1 ~ 32 Channel

#----------------------------------------------------------------------------------------------------------------------
# Pressure Controller [MKS]
$$$	Module	MKS651		LOAD	driver/Mks651.dll		5	9600	8	1	0	0	0	0	0	2000

#######################################################################################################################
# ETHERNET HUB 4 PORT
#----------------------------------------------------------------------------------------------------------------------
# Matcher [AE]
# ID9 : Matcher Number (1 or 2) for EIP_Match Config IP Address Define
$$$	Module	EIP_MAT1	LOAD	driver/EIP_MATCH1.dll		0	0	0	0	0	0	0	0	1	500
$$$	Module	EIP_MAT2	LOAD	driver/EIP_MATCH2.dll		0	0	0	0	0	0	0	0	2	500

#----------------------------------------------------------------------------------------------------------------------
# RF Generator [AE]
# ID9 : RF Number (1 or 2) for EIP_RFGen Config IP Address Define
$$$	Module	EIP_RF1		LOAD	driver/EIP_RF1.dll		0	0	0	0	0	0	0	0	1	500
$$$	Module	EIP_RF2		LOAD	driver/EIP_RF2.dll		0	0	0	0	0	0	0	0	2	500
#----------------------------------------------------------------------------------------------------------------------

#######################################################################################################################
# Ethernet IP Type
#								NetworkID		SM Use
$$$	Module	ENetIP		LOAD	driver/EnetIP.dll	2	0	0	1

$$$	Module	MEM		uLOAD	Mem.dll

$$$	F	Engine/System/System.io

$$$	F	IO/GUI/GuiControl.con
$$$	F	IO/GUI/GuiMonitor.con

$$$	F	IO/LOOKUP/lookup.io
$$$	F	IO/Enum/PMEnum.cfg

$$$	F	IO/CFGS/Mks651.cfg
$$$	F	IO/CFGS/EthernetIP_PEALD.cfg
$$$	F	IO/CFGS/EthernetIP_MFC.cfg
$$$	F	IO/CFGS/WT332_Heater.cfg
$$$	F	IO/CFGS/EthernetIP_EU3508.cfg
$$$	F	IO/CFGS/RorzeMtr.cfg
$$$	F	IO/CFGS/EIP_MATCH1.cfg		# Ethernet Type
$$$	F	IO/CFGS/EIP_MATCH2.cfg		# Ethernet Type
$$$	F	IO/CFGS/EIP_RF1.cfg		# Ethernet Type
$$$	F	IO/CFGS/EIP_RF2.cfg		# Ethernet Type

$$$	F	IO/CONS/Plotting.con
$$$	F	IO/CONS/PMC_ScreenEditor.con
$$$	F	IO/CONS/PMC_Process.con
$$$	F	IO/CONS/PMC_Parameter.con     # Parameter 1~7
$$$	F	IO/CONS/PMC_Parameter2.con     # Parameter 8
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
$$$ F	IO/CONS/Gas_Calibration.con
$$$	F	IO/CONS/MFC_FlowTest.con
$$$	F	IO/CONS/ALDValveControl.con	# for ALD Valve
$$$	F	System/IO/PMABC_Difference.con
