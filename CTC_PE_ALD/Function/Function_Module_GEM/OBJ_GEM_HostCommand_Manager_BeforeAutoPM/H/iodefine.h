#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum {
	PM1_DryClean_EDAbleOption		,
	PM1_DryLeak_EDAbleOption		,
	PM1_DryPost_EDAbleOption		,
	PM1_WetClean_EDAbleOption		,
	PM1_Idle_EDAbleOption			,	
	PM2_DryClean_EDAbleOption		,
	PM2_DryLeak_EDAbleOption		,
	PM2_DryPost_EDAbleOption		,
	PM2_WetClean_EDAbleOption		,
	PM2_Idle_EDAbleOption			,	
	PM3_DryClean_EDAbleOption		,
	PM3_DryLeak_EDAbleOption		,
	PM3_DryPost_EDAbleOption		,
	PM3_WetClean_EDAbleOption		,
	PM3_Idle_EDAbleOption			,	
	PM4_DryClean_EDAbleOption		,
	PM4_DryLeak_EDAbleOption		,
	PM4_DryPost_EDAbleOption		,
	PM4_WetClean_EDAbleOption		,
	PM4_Idle_EDAbleOption			,	
	PM5_DryClean_EDAbleOption		,
	PM5_DryLeak_EDAbleOption		,
	PM5_DryPost_EDAbleOption		,
	PM5_WetClean_EDAbleOption		,
	PM5_Idle_EDAbleOption			,
	
	PM1_DryClean_RunState		,
	PM1_DryLeak_RunState		,
	PM1_DryPost_RunState		,
	PM1_WetClean_RunState		,
	PM1_Idle_RunState			,
	PM2_DryClean_RunState		,
	PM2_DryLeak_RunState		,
	PM2_DryPost_RunState		,
	PM2_WetClean_RunState		,
	PM2_Idle_RunState			,
	PM3_DryClean_RunState		,
	PM3_DryLeak_RunState		,
	PM3_DryPost_RunState		,
	PM3_WetClean_RunState		,
	PM3_Idle_RunState			,	
	PM4_DryClean_RunState		,
	PM4_DryLeak_RunState		,
	PM4_DryPost_RunState		,
	PM4_WetClean_RunState		,
	PM4_Idle_RunState			,	
	PM5_DryClean_RunState		,
	PM5_DryLeak_RunState		,
	PM5_DryPost_RunState		,
	PM5_WetClean_RunState		,
	PM5_Idle_RunState			,

	PM1_DryCleaning_Recipe		,
	PM1_PostCleaning_Recipe		,
	PM1_WetCleaning_Recipe		,
	PM1_Idle_Recipe				,
	PM2_DryCleaning_Recipe		,
	PM2_PostCleaning_Recipe		,
	PM2_WetCleaning_Recipe		,
	PM2_Idle_Recipe				,
	PM3_DryCleaning_Recipe		,
	PM3_PostCleaning_Recipe		,
	PM3_WetCleaning_Recipe		,
	PM3_Idle_Recipe				,
	PM4_DryCleaning_Recipe		,
	PM4_PostCleaning_Recipe		,
	PM4_WetCleaning_Recipe		,
	PM4_Idle_Recipe				,
	PM5_DryCleaning_Recipe		,
	PM5_PostCleaning_Recipe		,
	PM5_WetCleaning_Recipe		,
	PM5_Idle_Recipe				,

	CTC_PM1_CommStatus			,
	CTC_PM2_CommStatus			,
	CTC_PM3_CommStatus			,
	CTC_PM4_CommStatus			,
	CTC_PM5_CommStatus			,

	PORT1_CONTROL				,
	PORT2_CONTROL   			,
	PORT3_CONTROL   			,

	PORT1_STATUS    			,
	PORT2_STATUS    			,
	PORT3_STATUS    			,

	PORT1_AMHS_MODE 			,
	PORT2_AMHS_MODE 			,
	PORT3_AMHS_MODE 			,

	CONTROL_STATE   			,

	PORT1_RESERVE   			,
	PORT2_RESERVE   			,
	PORT3_RESERVE   			,

	FOUP1_STATUS    			,
	FOUP2_STATUS    			,
	FOUP3_STATUS    			,

	PORT1_AMHS_HO   			,
	PORT2_AMHS_HO   			,
	PORT3_AMHS_HO   			,

/*	PORT1_SERVICE   			,
	PORT2_SERVICE   			,
	PORT3_SERVICE   			,

	PORT1_WAFERID_01			,
	PORT1_WAFERID_02			,
	PORT1_WAFERID_03			,
	PORT1_WAFERID_04			,
	PORT1_WAFERID_05			,
	PORT1_WAFERID_06			,
	PORT1_WAFERID_07			,
	PORT1_WAFERID_08			,
	PORT1_WAFERID_09			,
	PORT1_WAFERID_10			,
	PORT1_WAFERID_11			,
	PORT1_WAFERID_12			,
	PORT1_WAFERID_13			,
	PORT1_WAFERID_14			,
	PORT1_WAFERID_15			,
	PORT1_WAFERID_16			,
	PORT1_WAFERID_17			,
	PORT1_WAFERID_18			,
	PORT1_WAFERID_19			,
	PORT1_WAFERID_20			,
	PORT1_WAFERID_21			,
	PORT1_WAFERID_22			,
	PORT1_WAFERID_23			,
	PORT1_WAFERID_24			,
	PORT1_WAFERID_25			,

	PORT2_WAFERID_01			,
	PORT2_WAFERID_02			,
	PORT2_WAFERID_03			,
	PORT2_WAFERID_04			,
	PORT2_WAFERID_05			,
	PORT2_WAFERID_06			,
	PORT2_WAFERID_07			,
	PORT2_WAFERID_08			,
	PORT2_WAFERID_09			,
	PORT2_WAFERID_10			,
	PORT2_WAFERID_11			,
	PORT2_WAFERID_12			,
	PORT2_WAFERID_13			,
	PORT2_WAFERID_14			,
	PORT2_WAFERID_15			,
	PORT2_WAFERID_16			,
	PORT2_WAFERID_17			,
	PORT2_WAFERID_18			,
	PORT2_WAFERID_19			,
	PORT2_WAFERID_20			,
	PORT2_WAFERID_21			,
	PORT2_WAFERID_22			,
	PORT2_WAFERID_23			,
	PORT2_WAFERID_24			,
	PORT2_WAFERID_25			,

	PORT3_WAFERID_01			,
	PORT3_WAFERID_02			,
	PORT3_WAFERID_03			,
	PORT3_WAFERID_04			,
	PORT3_WAFERID_05			,
	PORT3_WAFERID_06			,
	PORT3_WAFERID_07			,
	PORT3_WAFERID_08			,
	PORT3_WAFERID_09			,
	PORT3_WAFERID_10			,
	PORT3_WAFERID_11			,
	PORT3_WAFERID_12			,
	PORT3_WAFERID_13			,
	PORT3_WAFERID_14			,
	PORT3_WAFERID_15			,
	PORT3_WAFERID_16			,
	PORT3_WAFERID_17			,
	PORT3_WAFERID_18			,
	PORT3_WAFERID_19			,
	PORT3_WAFERID_20			,
	PORT3_WAFERID_21			,
	PORT3_WAFERID_22			,
	PORT3_WAFERID_23			,
	PORT3_WAFERID_24			,
	PORT3_WAFERID_25			,

	PORT1_WAFER_01  			,
	PORT1_WAFER_02  			,
	PORT1_WAFER_03  			,
	PORT1_WAFER_04  			,
	PORT1_WAFER_05  			,
	PORT1_WAFER_06  			,
	PORT1_WAFER_07  			,
	PORT1_WAFER_08  			,
	PORT1_WAFER_09  			,
	PORT1_WAFER_10  			,
	PORT1_WAFER_11  			,
	PORT1_WAFER_12  			,
	PORT1_WAFER_13  			,
	PORT1_WAFER_14  			,
	PORT1_WAFER_15  			,
	PORT1_WAFER_16  			,
	PORT1_WAFER_17  			,
	PORT1_WAFER_18  			,
	PORT1_WAFER_19  			,
	PORT1_WAFER_20  			,
	PORT1_WAFER_21  			,
	PORT1_WAFER_22  			,
	PORT1_WAFER_23  			,
	PORT1_WAFER_24  			,
	PORT1_WAFER_25  			,

	PORT2_WAFER_01  			,
	PORT2_WAFER_02  			,
	PORT2_WAFER_03  			,
	PORT2_WAFER_04  			,
	PORT2_WAFER_05  			,
	PORT2_WAFER_06  			,
	PORT2_WAFER_07  			,
	PORT2_WAFER_08  			,
	PORT2_WAFER_09  			,
	PORT2_WAFER_10  			,
	PORT2_WAFER_11  			,
	PORT2_WAFER_12  			,
	PORT2_WAFER_13  			,
	PORT2_WAFER_14  			,
	PORT2_WAFER_15  			,
	PORT2_WAFER_16  			,
	PORT2_WAFER_17  			,
	PORT2_WAFER_18  			,
	PORT2_WAFER_19  			,
	PORT2_WAFER_20  			,
	PORT2_WAFER_21  			,
	PORT2_WAFER_22  			,
	PORT2_WAFER_23  			,
	PORT2_WAFER_24  			,
	PORT2_WAFER_25  			,

	PORT3_WAFER_01  			,
	PORT3_WAFER_02  			,
	PORT3_WAFER_03  			,
	PORT3_WAFER_04  			,
	PORT3_WAFER_05  			,
	PORT3_WAFER_06  			,
	PORT3_WAFER_07  			,
	PORT3_WAFER_08  			,
	PORT3_WAFER_09  			,
	PORT3_WAFER_10  			,
	PORT3_WAFER_11  			,
	PORT3_WAFER_12  			,
	PORT3_WAFER_13  			,
	PORT3_WAFER_14  			,
	PORT3_WAFER_15  			,
	PORT3_WAFER_16  			,
	PORT3_WAFER_17  			,
	PORT3_WAFER_18  			,
	PORT3_WAFER_19  			,
	PORT3_WAFER_20  			,
	PORT3_WAFER_21  			,
	PORT3_WAFER_22  			,
	PORT3_WAFER_23  			,
	PORT3_WAFER_24  			,
	PORT3_WAFER_25  			,
	*/
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
