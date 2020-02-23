#-----------------------------------------------------------------------------------------------------------------------
$$$	Enum
#-----------------------------------------------------------------------------------------------------------------------
AbsPres		Absent	Present
ALGNRecan	No	Yes	Error
ATMVACSts	Unknown	ATM	VAC	Error
ATR8Carrier	Absent	Present	None
ATR8Chuck	Down	Up	None
ATR8Clear	None	Clear
ATR8ComMode	Bkg	Seq
ATR8CommSts	OffLine	ONLINE
ATR8Door	Lock	UnLock	None
ATR8HomeSts	NotHome	Homed
ATR8Intlks	RUN
ATR8Align	RUN
ATR8Load	Load	UnLoad
ATR8LoadSts	Unknown	Absent	Present
ATR8OffOn	Off	On
ATR8Placed	NotPlaced	Placed	None
ATR8Present	NotPresent	Present	None
ATR8Reset	None	Reset
ATR8RunStat	Ready	Run	Fault
ATR8Save	None	Save
ATR8Size	200MM	300MM	None
ATR8Slide	NoSlide	Slide	None
ATR8SlotNum	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25
ATR8Slots	13	25	None
ATR8Station	None	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16
#ATR8Station	None	BM2	BM1	LPMA	LPMB	LPMC	6	AL	8	9	10	11	12	13	14	15	16
ATR8Type	FOUP	CASS	None
ATR8WafStat	Unknown	Absent	Present	double	Cross
#ALEAKCHECKMODE	DAILY	WEEKLY	MONTHLY
ALEAKCHECKMODE	DAILY	WEEKLY	MONTHLY	DAYPERIOD	LOTCOUNT	WAFERCOUNT	IDLETIME
ALEAKCHECKWEEK  MON	TUES	WEDNES	THURS	FRI	SATUR	SUN
BLC2LaserOff	LaserOff	LAOffEscape	Quit
BLC2OKNGCtrl	OKOn	NGOn	ALLOff
BLC2OnlTstMode	TestON	TestOFF	TestCheck
BLC2ReadStatus	NotReady	Ready
BLC2RunIntlk	Run
BLC2SetMode	Normal	SettingMode
BLC2TrCtrl	Off	On
BLC2TstMdeCtrl	Test1	Test2	Quit
BLC3LaserOff	LaserOff	LAOffEscape	Quit
BLC3OKNGCtrl	OKOn	NGOn	ALLOff
BLC3OnlTstMode	TestON	TestOFF	TestCheck
BLC3ReadStatus	NotReady	Ready
BLC3RunIntlk	Run
BLC3SetMode	Normal	SettingMode
BLC3TrCtrl	Off	On
BLC3TstMdeCtrl	Test1	Test2	Quit
BLCLaserOff	LaserOff	LAOffEscape	Quit
BLCOKNGCtrl	OKOn	NGOn	ALLOff
BLCOnlTstMode	TestON	TestOFF	TestCheck
BLCReadStatus	NotReady	Ready
BLCRunIntlk	Run
BLCSetMode	Normal	SettingMode
BLCTrCtrl	Off	On
BLCTstMdeCtrl	Test1	Test2	Quit
BusyReady	Busy	Ready
CassMode	CM1	CM2	CM3
Clmp_Mode	Clamp	Unclamp
CloseOpen	Close	Open
ClampUnclampCmd	Unknown	Clamp	Unclamp
ClampUnclampSts	Unknown	Clamp	Unclamp	Error
CommStsEnum	OffLine	OnLine	CtrlerFailed
CleanEnableType	WAFER	END_WAIT	IDLE_WAIT
CycClnRunEnum	Idle	Running	Failed	Reserv
CycClnStpEnum	Idle	Ready	Stop
DeadAlive	DEAD	Alive
DisableEnable	Disable	Enable
DockCMD		FullDock	ShuttleIn	Clamp	FullDockMap
DownUp		Down	Up
DummyUseMode	CM3	DUMMY
DryControlType	MANUAL	AUTO
EnableDisable	Enable	Disable
ErrorOK		ERROR	OK
ExtRetCmd	Unknown	Extend	Retract
ExtRetSts	Unknown	Extend	Retract	Error
FMRbRet		Ret	NotRet
FOUPDoor	Unknown	Close	Open
GaugeGasName	Air	Ar	N2	H2
HRF2CommStatus	Offline	Online
HRF2ErrStatus	None	AutoFail	ExFail	WriteFail	NoTag	Invalid	UnKnown	UnConfig	Check	VoidAckn	Locked	MsgLen	Invalid2	NoAckn
HRF2PageCtrl	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17
HRF2PageRead	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17
HRF2RunIntlk	Run
HRF3CommStatus	Offline	Online
HRF3ErrStatus	None	AutoFail	ExFail	WriteFail	NoTag	Invalid	UnKnown	UnConfig	Check	VoidAckn	Locked	MsgLen	Invalid2	NoAckn
HRF3PageCtrl	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17
HRF3PageRead	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17
HRF3RunIntlk	Run
HRFCommStatus	Offline	Online
HRFErrStatus	None	AutoFail	ExFail	WriteFail	NoTag	Invalid	UnKnown	UnConfig	Check	VoidAckn	Locked	MsgLen	Invalid2	NoAckn
HRFPageCtrl	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17
HRFPageRead	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17
HRFRunIntlk	Run
HtrStbStsEnum	Stable	Unstable	Moving
InOutCmd	Unknown	In	Out
InOutSts	Unknown	In	Out	Error
IsoGateCtl	None	Open	Close
IsoGateCtlE	OO.CC	OO.CN	ON.CC	ON.CN	E
IsoGateSns	None	Opened	Closed	Error
IsoGateType	NOC.NOC	NCO.NCO	NOC.NCO	NCO.NOC	E
IsoUseNoUse	Using	NotUsing
IsoControlType	Sync	ASync
LEYASts	None	Alarm
LEYCommSts	Offline	Online
LEYMCtl	None	Ready	Halt	StopDirect	StopDown	Start	Flush	Standby	Wait	Operating
LEYMSts	None	Ready	Start	Unknown1	Halt	Wait	Operating	Standby	Flush	Stop
LEYSSts	None	B_LOLO	B_LO	A_HI	A_HIHI	O_RANGE	D_WARN	D_HARZ	S_DOWN
LEYW2Sts	None	Low	High
LEYWSts	None	Warning
LPInitCMD	Init		InitX
#LogCMD		NoLog		File		Console		FileConsole
LogCMD		Log_Off		Log_On
MapCMD		None		Map
MapSts		None		Mapping		MapComplete	MapFail
MID_RD_Mode	0	1	2	3	4	5	6	7	8	9
MTR7ArmSts	Extended	Retracted	NotAtStn
MTR7CalStation	PM1	PM2	PM3
MTR7DrvLock	Unlock	Lock
MTR7CalStat	Idle	Run
MTR7ArmChk	None	CheckStart
MTR7Clear	None	Clear
MTR7CommSts	OffLine	ONLINE
MTR7Intlks	RUN
MTR7OffOn	Off	On
MTR7LoadSts	Off	On	Unknown
MTR7InSts	None	OK	Unknown
MTR7RunStat	Ready	Run	FatalErr	NotHomed	MtnErr	OtherErr
MTR7Station	Empty	BM1	Empty2	BM2	Empty3	PM1	Empty4	PM2	Empty5	PM3
MTR7AWCStat	Empty	BM1_1	BM1_2	BM2_1	BM2_2	PM1_1	PM1_2	PM2_1	PM2_2	PM3_1	PM3_2
MTR7OPMode	CDM	HOST

MTR7CmdError	OPError	MotionErr	MotorErr
mTransFinger2	A
mTransFinger3	A	B
mTransPath	BM1	BM2	PM1	PM2	PM3
mTransPath2	BM1	BM2	CM1	CM2	CM3	AL	BM3
mTransPath3	PMA	PMB	PMC	BM1	BM2
mTransPath4	PMA	PMB	PMC	BM1_1	BM1_2	BM2_1	BM2_2

NotATMATM	NotATM	ATM
NotOkOK	NotOK	OK
NotVACVAC	NotVAC	VAC
NoYes		No	Yes
NormalCycle	Normal	Cycle
NmIntEnum	Normal	Interlock
NmlResetEnum	Normal	Reset
OffOn		Off	On
OffOnBlink	Off	On	Blink
OKError		OK	ERROR
OnOff		On	Off
OpenClose	Open	Close
OpenCloseCmd	Unknown	Open	Close
OpenCloseSts	Unknown	Open	Close	Error
OpenCloseWait	OpenWait	CloseWait
SlowFastEnum	Close	OpenSlow OpenFast Unknown
SlowFastEnum2	Close	OpenSlow OpenFast OpenSFast Unknown
PageSelect	Off	On
PlotControl	Idle	Saving
PlotDisplay	Stop	Start	Resume
PMVacState	NotVAC	VAC
PumpLockSts	PumpUnlock	PumpLock
PurNotpur	Purge	Notpurge
RBTState	None	Retracted
RMapCMD		None		ReadMap
ReadyBusy	Ready	Busy
SLVState	None	Closed
StopRun	Stop	Run
StpRunng	Stopped	Running
TMLEYASts	None	Alarm
TMLEYCommSts	Offline	Online
TMLEYMCtl	None	Ready	Halt	StopDirect	StopDown	Start	Flush	Standby	Wait	Operating
TMLEYMSts	None	Ready	Start	Unknown1	Halt	Wait	Operating	Standby	Flush	Stop
TMLEYSSts	None	B_LOLO	B_LO	A_HI	A_HIHI	O_RANGE	D_WARN	D_HARZ	S_DOWN
TMLEYW2Sts	None	Low	High
TMLEYWSts	None	Warning
TMRbRet	Ret	NotRet
TPBaud	600	1200	2400	4800	9600
TPCommStatus	Offline	Online
TPErrStatus	NoErr	Error
TPIntlk	Impulse	Continuous
TPMode	Remote	Local
TPOffOn	Off	On
TPOpClose	Open	Close
TPOutSig	High	Low
TPPumpSts	Stop	WaitIntlk	Starting	Regulation	Braking	Normal	Fail
TPReset		None	Reset
TPSetType	Frequency	Current	Time
TPVlvMode	Automatic	Command
UndockCMD	FullUndock	CloseDoor	ShuttleOut
UnlockLock	Unlock	Lock
UpDownCmd	Unknown	Up	Down
UpDownSts	Unknown	Up	Down	Error
UseNotuse	Use	Notuse
NoUse		Use	No
PurgeMode	Idle	Foup
NotuseUse	Notuse	Use
VacSta		Vacuum	Atm	IntVA	IntAV
VacStatus	ATM	Vacuum	GoATM	GoVacuum	Abort
VGCommState	OFFLINE	ONLINE
VGOffOn	OFF	ON
VtmArm		AB_ALL	A_Only	B_Only

WaferStatus	Empty	Occupied
WarnOK	WARN	OK
WfrSts	Unknown	Absent	Present
XferState	NotXfer	AtXfer
################## OHT PLC IO CHANNEL
AGVClr		Run
AGVMode		Manual	Auto
AGVSts		Ready	Busy	Error

WIDReadySts	NotReady	Ready
WIDInitCmd	Run
WIDLiveImageCmd	Run
WIDResetCmd	Run
WIDRcpType	DDM	BC	OCR
WIDRcpVal	1	2	3	4	5

DryStopRun1	None	Run	Stop
DryStopRun2	Stop	Run
DPStartStop	Start	Stop
DPLocalRemote	Local	Remote
DPLocalRemote1	None	Remote	Local
NormalWarn	Normal	Warn
NormalAlarm	Normal	Alarm
AlarmNormal	Alarm	Normal
WarnNormal	Warn	Normal
NormWarnAlarm	Normal	Warning	Alarm
ManualAuto	Manual	Auto
ManualRemote	Manual	Remote

PumpVentStep	UnKnown	VentStart VentDelay SlowVent FastVent OverVent Vented VentAborted PumpStart PumpLinePump SlowPump FastPump Pumped PumpAborted


IntNmEnum	Interlock	Normal
NoneChkEnum	None		Check
AbsIncEunm	Absolute	Incremental
ForRevEnum	Forward		Reverse
InPosEnum	NotInPos	InPosition
NmCancel	Normal		Cancel
StopStart	Stop		Start
StopNormal	Stop		Normal
CloseOpenCmd	Unknown	Close	Open
AlarmOptionEnum	Ignore	Warning	Abort
# Add for TSMC 2008.01.10------------------------------------------------------------------------------
### Start Tr Status ###
ChamberStatus      Unknown        Atmosphere      PumpingDown    Vacuum        Standby         ReqMaterial       RecMaterial        HaveMaterial
Mode_Status        Maintenance    Engineering     Production
Pressure_Status    Unknown        Atmosphere      Vacuum
CIGCh_Status       Unknown        Normal          Error
CGPump_Status      Unknown        SetPoint1_On    SetPoint2_On   Error
VS_Status          Atmosphere     Vacuum
ChamFS_Status      Unknown        Flow_OK
Bake_Status        Unknown        BakeOn          BakeOff
TMP_Status         Unknown        PowerFail       Normal         Acceleration  SlowDown        Stop              Error
CP_Status          Unknown        Running         NotReady       RegeneReq     Regeneration    Warming           PumpingDown        CoolDown        CryoError    RegenStopReq
Valve1_Status      Unknown        Open            Close          Moving
Valve3_Status      Unknown        Open            Close
### End Tr Status ###

### Start Pass Status ###
P_ChamberStatus    Unknown        Atmosphere      PumpingDown    Vacuum        Standby         ReqMaterial       RecMaterial        HaveMaterial  Processing   Idle            SendingMat    MatReleased
Wafer_Status2      WaferNone      WaferOK
WL_Status          Unknown        Up        Contact        Down
Btn_Status         Disable        Enable          Running        Dis_Running
### End Pass Status ###

### Start LL Status ###
Heat_Status        Unknown        STOP            HeatOk
WL_Status2         Unknown        Transfer        Contact        Down
### End LL Status ###

### Start System Status ###
Lamp_Status        LampOff        LampOn          LampOn(Blk1)   LampOn(Blk2)
Buzzer_Status      BuzzerOff      BuzzerOn1       BuzzerOn2      BuzzerOn3     BuzzerOn4       BuzzerOn5         BuzzerOn6          BuzzerOn7      BuzzerOn8    BuzzerOn9    BuzzerOn10    BuzzerOn11    BuzzerOn12    BuzzerOn13    BuzzerOn14    BuzzerOn15
DRP_Status         Stop           Running         DRP_Error
#CanonANELVA End

VacStatus1      Idle  GoATM  GoVacuum  Abort
VacStatus2      Vacuum ATM  GoATM  GoVacuum  Abort

ARFCommStatus	Offline	Online
ARFErrStatus	NO	CE	TE	HE	EE	ETC
ARFPageCtrl	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17
ARFPageRead	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17

#Unclamp_Ctrl	AMHS_UNCLAMP	CARR_COMSTOP
Unclamp_Ctrl	Unclamp		CarrStop
GUI_Start_Mode	SchStart	JobStart
ServiceMode	InService	OutofService
CheckNotCheck	Check	NotCheck
UnknownOkEnum	Unknown		Ok
#------------------------------------------------------------------------------------------------------
#DP Serial
DryCommStat   Offline Online
DryOffOn	Off	On
DryTPS	Running ADP ADPLP LoadLock Exhaust Roots Free
DryErrStat	Ok	Err
DryRun	Run
DryControl	LOC	REM	KEY
DryOkFault	Ok	Fault
#... 2016.04.03
LkChkModeEnm		NotDefine 	LEAKCHK 	GasLineLK
#------------------------------------------------------------------------------------------------------
#LL Pump Valve
PUMPVlv		SFastVlv	FastVlv
################# N2 Purge ##########################
N2MODE		NotUse	2Nozzle 4Nozzle	Auto
N2STATUS	Pre	Process	Post	TPre	PostPost	MBlow	Idle	MPurge	Error
N2MODE_STATUS	NotUse	Disable	Maint	Standby
N2FOUPSTS	Unknown	Absent	2Nozzle	4Nozzle
MANSSP		DRYCLEAN	WETPM
#------------------------------------------------------------------------------------------------------
#FFU
FFU_MAKER   SINSUNG MARS
#------------------------------------------------------------------------------------------------------
#SIGNALTOWER
SIGNALTOWER     Default Red Yellow  Green   AllOff


	GaugeSel		BG		CG
	ModeSel			M_PRESS		M_TIME
#				0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18
	MFCCALNUM		Ar_M1	O2_M2	Ar_M3	Ar_M4	NF3_M5	Ar_M6	Ar_M7	O2_M8	Ar_M9	NA_M10	NA_M11	NA_M12	NA_M13	NA_M14	NA_M15	NA_M16	NA_M17	NA_M18  NONE

	# 2012/01/30 제조 단계에서 MFC Verification 위한 Normal N2 Mode 추가 by JTM
	MCal_GasType		RealGas		Purge_Ar		Normal_N2	GasFinMax
	MCal_AutoMan		Auto     Manual
	MLFMStatus		OFF		M_REF		M_TEST
	Htr_Model		NOTSelect	ALN	Aluminum
	OFF_ON2			OFF_0	ON_1	ON_2
	ModuleType	TEOS		ARC_Oxi


#	ON_OFF		ON_0	 	OFF_1
#	OFF_ON		OFF_0		ON_1

	OK_WARN		OK_0		WARNING_1
	OK_HAZ		OK_0		HAZARD_1
	OK_FAIL		OK_0		FAILURE_1

	WARN_OK		WARNING_0	OK_1

	HAZ_OK 		HAZARD_0	OK_1
	LOW_OK		LOW_0		OK_1
	FAIL_OK		FAILURE_0	OK_1
	OPEN_OK		OPENED_0	OK_1
	OVER_OK		OVERED_0	OK_1
	LEAK_OK		LEAKED_0        OK_1
	STOP_OK		STOPPED_0	OK_1
	SLIDE_OK	SLIDED_0	OK_1

	NOT_ATM		NOTATM		ATM
	NOT_ACT		NOTACT		ACTIVE
	NOT_HOME	NOTHOME		HOME
	NOT_OPEN	NOTOPEN	 	OPENED_1
	NOT_CLOSE	NOTCLOSE	CLOSED_1
	NOT_READY	NOTREADY	READY
	NOT_LIMIT	NOTLIMIT	LIMIT

	20TOV_UN	20T_OV		20T_UN

	PINDNSTS	DOWNED_0 	NONE_1
	PINUPSTS	ERROR		LOADUP		UNLOADUP	NONE

	VALVESTS	NONE		OPENED		CLOSED		ERROR


	ERROR_OK	ERROR		OK
	NONE_DOWN	NONE	 	DOWN

	OPEN_CLOSE	NOPEN		FCLOSE
	CLOSE_OPEN 	CLOSE		OPEN
	PRE_AUTO	PRESET		AUTO

	PUMPACT		NONE		PON		POFF		ERROR

	PINUPACT	NONE		LOADUP		ERROR		UNLOADUP
	HEATERACT	ERROR		HON		HOFF		NONE
