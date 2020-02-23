#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
		TMIsoVlvClosedDI,

		FC_VACUUM,
		ChmPrsAI,
		ChmPrssDI1,
		M651_Valve,
		VacuumStateDM,
		FastRoughVlvDO,
		FastRoughVlvDI,
		DPRunDI,
		DPDO,
		PumpPrsAI,
		MBStsDI,
		MBCmdDO,
		PumpTime,
		M651_Pressure,
		M651_Position,

		V1ValveDO, 		// V1
		V2ValveDO, 		// V2
		V3ValveDO, 		// V3
		V4ValveDO,		// V4
		V5ValveDO,		// V5
		V6ValveDO,		// V6
		V7ValveDO,		// V7
		V8ValveDO,		// V8
		V9ValveDO,			// V9
		V10ValveDO,		// V10
		V11ValveDO,		// V11
		V12ValveDO,		// V12
		V13ValveDO,		// V13
		V14ValveDO,		// V14
		V15ValveDO,		// V15
		V16ValveDO,		// V16
		V17ValveDO,		// V17
		V18ValveDO,		// V18
		V19ValveDO,		// V19
		V20ValveDO,		// V20
		V21ValveDO,		// V21

		V22ValveDO,		// V22
		V23ValveDO,		// V23
		V24ValveDO,		// V24
		V25ValveDO,		// V25
		V26ValveDO,		// V26
		V27ValveDO,		// V27
		V28ValveDO,		// V28
		V29ValveDO,		// V29
		V30ValveDO,		// V30

		V31ValveDO,
		V32ValveDO,
		V33ValveDO,
		V34ValveDO,
		V35ValveDO,
		V36ValveDO,
		V37ValveDO,
		V38ValveDO,
		V39ValveDO,
		V40ValveDO,
		V41ValveDO,
		V42ValveDO,
		V43ValveDO,
		V44ValveDO,
		// 2014.04.09
		V45ValveDO,
		V46ValveDO,
		V47ValveDO,
		V48ValveDO,
		V49ValveDO,
		V50ValveDO,
		V51ValveDO,
		V52ValveDO,
		V53ValveDO,
		V54ValveDO,
		V55ValveDO,
		V56ValveDO,
		V57ValveDO,
		V58ValveDO,
		V59ValveDO,
		V60ValveDO,
		V61ValveDO,
		V62ValveDO,
		V63ValveDO,
		V64ValveDO,
		V65ValveDO,
		V66ValveDO,
		V67ValveDO,
		V68ValveDO,
		V69ValveDO,
		V70ValveDO,
		V71ValveDO,
		V72ValveDO,
		V73ValveDO,
		V74ValveDO,
		V75ValveDO,
		V76ValveDO,
		V77ValveDO,
		V78ValveDO,
		V79ValveDO,
		V80ValveDO,
		V81ValveDO,
		V82ValveDO,
		V83ValveDO,
		V84ValveDO,
		V85ValveDO,
		V86ValveDO,
		V87ValveDO,
		V88ValveDO,
		V89ValveDO,
		V90ValveDO,
		V91ValveDO,
		V92ValveDO,
		V93ValveDO,
		V94ValveDO,
		V95ValveDO,
		V96ValveDO,
		V97ValveDO,
		V98ValveDO,
		V99ValveDO,
		V100ValveDO,
		V101ValveDO,
		V102ValveDO,
		V103ValveDO,
		V104ValveDO,
		V105ValveDO,
		V106ValveDO,
		V107ValveDO,
		V108ValveDO,
		V109ValveDO,
		V110ValveDO,
		V111ValveDO,
		V112ValveDO,

		LkChkStartPress1AM,
		LkChkStartPress2AM,
		LkChkStartPress3AM,
		LkChkStartPress4AM,
		LkChkStartPress5AM,
		LkChkStartPress6AM,
		LkChkStartPress7AM,
		LkChkStartPress8AM,
		LkChkStartPress9AM,
		LkChkStartPress10AM,

		LkChkEndPress1AM,
		LkChkEndPress2AM,
		LkChkEndPress3AM,
		LkChkEndPress4AM,
		LkChkEndPress5AM,
		LkChkEndPress6AM,
		LkChkEndPress7AM,
		LkChkEndPress8AM,
		LkChkEndPress9AM,
		LkChkEndPress10AM,

		LeakChkRate1AM,
		LeakChkRate2AM,
		LeakChkRate3AM,
		LeakChkRate4AM,
		LeakChkRate5AM,
		LeakChkRate6AM,
		LeakChkRate7AM,
		LeakChkRate8AM,
		LeakChkRate9AM,
		LeakChkRate10AM,
		LeakChkAvgAM,

		LeakChkCountAM,
		LeakChkProgrssTimeSM,
		LeakChkProgrssStepSM,
		LeakChkStartTimeSM,
		LeakChkEndTimeSM,

		BaseDelayTimeAM,  //... 2016.01.19
		LeakChkBasePressAM,
		LeakChkLimitPressAM,
		LeakChkPumpTimeOutAM,
		LeakChkDelayTimeAM,
		LeakChkCountSPAM,
		LeakChkStsDM,
		LeakChkStopDM,
		//Leak Check Associated 2014.12.01
		Log_Rate1,
		Log_Rate2,
		Log_Rate3,
		Log_Rate4,
		Log_Rate5,
		Log_Avg1,
		Log_Avg2,
		Log_Avg3,
		Log_Avg4,
		Log_Avg5,
		Log_Time1,
		Log_Time2,
		Log_Time3,
		Log_Time4,
		Log_Time5,

		PRMA_VS_PrsBaRough,
		PRMD_AI_DPCheck,
		PRMA_TO_DryPumpOn,
		PRMA_VS_PrsBaPmping,
		PRMA_TO_PmpLnPmping,
		PRMA_TO_SlowRghPrs,
		PRMA_TL_ThrtlPos,
		PRMA_TO_ThrtlPos,
		PRMA_TO_BaseRghPrs,
		PRMD_HW_DPType,
		PRMA_WT_AfterFstRgh,
		PRMA_VS_PrsBaVent,
		PRMA_VS_VCSnVlvOpPs,
		PRMA_TO_PressVent,
		PRMA_AL_VentOKTemp,
		PRMA_WT_AfterVent,
		PRMA_VS_TrPrs,

		//
		PRESS_CONTROL,
		ABORT_GAS,
		MFC_CONTROL,
		STGHTR_CONTROL,

		//... 2015.03.23 Add Check Teach Mode for Safety PLC
		PLCSysRunModeDI,
		PLCTeachModeDI,

		//... 2015.03.24 Add Process, Vac Switch, Foreline Gauge Isolation Valve Auto OPEN
		ProcBGIsoVlvDO,
		VacSwIsoVlvDO,
		PumplnCVGIsoVlvDO,
		//...2015.09.11
		//2014.10.27
		ALDBaseDelayTimeAM,
		ALDLkChkBasePressAM,
		ALDLkChkLimitPressAM,
		ALDLkChkDelayTimeAM,
		ALDLkChkPumpTimeOutAM,
		ALDLkChkCountSPAM,
		LEAK_ModeDM,
		//...2016.01.19
		PurgeLineLK_DM,
		CleanLineLK_DM,
		SrcLineLK_DM,
		//... 2016.04.04
		DyALECHK_MTRDM,
		DyALECHK_VLVDM,
		CHUCK_CONTROL,
		ALD_VLV_CTRL,
		//... Gas Line Leak check Parameter
		LnBaseDelayTimeAM,
		LnLkChkBasePressAM,
		LnLkChkLimitPressAM,
		LnLkChkDelayTimeAM,
		LnLkChkPumpTimeOutAM,
		LnLkChkCountSPAM,
		//...User Mode Leak Check Parameter
		UrBaseDelayTimeAM,
		UrLkChkBasePressAM,
		UrLkChkLimitPressAM,
		UrLkChkDelayTimeAM,
		UrLkChkPumpTimeOutAM,
		UrLkChkCountSPAM,
		//...
		MFC01FlwAI,
		MFC02FlwAI,
		MFC03FlwAI,
		MFC04FlwAI,
		MFC05FlwAI,
		MFC06FlwAI,
		MFC07FlwAI,
		MFC08FlwAI,
		MFC09FlwAI,

		LeakChk_ParmSaveDM,
		StgHtr1RelayDI,
		StgHtr2RelayDI,

		SysLeakChkRateAM,
		SysPurLkChkRateAM,
		SysClnLkChkRateAM,

		PRMA_TM_ToxicGasPurge,
		PRMA_TO_ArCharging,
		//... 2016.05.12
		LKMacroType1DM,
		LKMacroType2DM,
		ALECHK_NormDM,
		ALECHK_GASLnDM,
		MFC_PurgeTmAM,

		MFC01PressAI,
		MFC02PressAI,
		MFC03PressAI,
		MFC04PressAI,
		MFC05PressAI,
		MFC06PressAI,
		MFC07PressAI,
		MFC08PressAI,
		MFC09PressAI,
		//... 2016.06.14
		PRMA_TO_BeforeVent,
		RRMD_VENT_SEQ,
		//... 2016.08.16
		SW_GbIntlckDM,
		RoRLimitAM,
		LnRoRLimitAM,
		ALDRoRLimitAM,
		UrRoRLimitAM,
		// 2020. 01. 22
		PRMD_GATE_VALVE_OPT,
		RPGGateVlvPosDI,
		RPGGateVlv2PosDI,
		//...
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
