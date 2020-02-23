#ifndef IODEFINE_H
#define IODEFINE_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//
// IO List
//
typedef enum {
	//PM1
	PM1_RcpTypeDM			,
	PM1_RUN_STATUS			,
	PM1_CommStatus			,
                                 
	PM1_DayIDLE_TimeD		,
	PM1_DayIDLE_TimeH		,
	PM1_DayIDLE_TimeM		,
	PM1_DayIDLE_TimeS		,
	PM1_DayIDLE_Time		,
                                 
	PM1_DayClean_TimeD		,
	PM1_DayClean_TimeH		,
	PM1_DayClean_TimeM		,
	PM1_DayClean_TimeS		,
	PM1_DayClean_Time		,
                                 
	PM1_DayRun_TimeD		,
	PM1_DayRun_TimeH		,
	PM1_DayRun_TimeM		,
	PM1_DayRun_TimeS		,
	PM1_DayRun_Time			,
                                 
	PM1_Idle_Log1			,
	PM1_Idle_Log2			,
	PM1_Idle_Log3			,
	PM1_Idle_Log4			,
	PM1_Idle_Log5			,
                                 
	PM1_Clean_Log1			,
	PM1_Clean_Log2			,
	PM1_Clean_Log3			,
	PM1_Clean_Log4			,
	PM1_Clean_Log5			,
                                 
	PM1_Run_Log1			,
	PM1_Run_Log2			,
	PM1_Run_Log3			,
	PM1_Run_Log4			,
	PM1_Run_Log5			,

	PM1_OperRate_Log1		,
	PM1_OperRate_Log2		,
	PM1_OperRate_Log3		,
	PM1_OperRate_Log4		,
	PM1_OperRate_Log5		,

	//PM2
	PM2_RcpTypeDM			,
	PM2_RUN_STATUS			,
	PM2_CommStatus			,
                                 
	PM2_DayIDLE_TimeD		,
	PM2_DayIDLE_TimeH		,
	PM2_DayIDLE_TimeM		,
	PM2_DayIDLE_TimeS		,
	PM2_DayIDLE_Time		,
                                 
	PM2_DayClean_TimeD		,
	PM2_DayClean_TimeH		,
	PM2_DayClean_TimeM		,
	PM2_DayClean_TimeS		,
	PM2_DayClean_Time		,
                                 
	PM2_DayRun_TimeD		,
	PM2_DayRun_TimeH		,
	PM2_DayRun_TimeM		,
	PM2_DayRun_TimeS		,
	PM2_DayRun_Time			,
                                 
	PM2_Idle_Log1			,
	PM2_Idle_Log2			,
	PM2_Idle_Log3			,
	PM2_Idle_Log4			,
	PM2_Idle_Log5			,
                                 
	PM2_Clean_Log1			,
	PM2_Clean_Log2			,
	PM2_Clean_Log3			,
	PM2_Clean_Log4			,
	PM2_Clean_Log5			,
                                 
	PM2_Run_Log1			,
	PM2_Run_Log2			,
	PM2_Run_Log3			,
	PM2_Run_Log4			,
	PM2_Run_Log5			,

	PM2_OperRate_Log1		,
	PM2_OperRate_Log2		,
	PM2_OperRate_Log3		,
	PM2_OperRate_Log4		,
	PM2_OperRate_Log5		,

	//PM3
	PM3_RcpTypeDM			,
	PM3_RUN_STATUS			,
	PM3_CommStatus			,
                                 
	PM3_DayIDLE_TimeD		,
	PM3_DayIDLE_TimeH		,
	PM3_DayIDLE_TimeM		,
	PM3_DayIDLE_TimeS		,
	PM3_DayIDLE_Time		,
                                 
	PM3_DayClean_TimeD		,
	PM3_DayClean_TimeH		,
	PM3_DayClean_TimeM		,
	PM3_DayClean_TimeS		,
	PM3_DayClean_Time		,
                                 
	PM3_DayRun_TimeD		,
	PM3_DayRun_TimeH		,
	PM3_DayRun_TimeM		,
	PM3_DayRun_TimeS		,
	PM3_DayRun_Time			,
                                 
	PM3_Idle_Log1			,
	PM3_Idle_Log2			,
	PM3_Idle_Log3			,
	PM3_Idle_Log4			,
	PM3_Idle_Log5			,
                                 
	PM3_Clean_Log1			,
	PM3_Clean_Log2			,
	PM3_Clean_Log3			,
	PM3_Clean_Log4			,
	PM3_Clean_Log5			,
                                 
	PM3_Run_Log1			,
	PM3_Run_Log2			,
	PM3_Run_Log3			,
	PM3_Run_Log4			,
	PM3_Run_Log5			,

	PM3_OperRate_Log1		,
	PM3_OperRate_Log2		,
	PM3_OperRate_Log3		,
	PM3_OperRate_Log4		,
	PM3_OperRate_Log5		,

	Total_OperRate_Log1		,
	Total_OperRate_Log2		,
	Total_OperRate_Log3		,
	Total_OperRate_Log4		,
	Total_OperRate_Log5		,

	Log_Save,

} IOPointIndex;

#endif


