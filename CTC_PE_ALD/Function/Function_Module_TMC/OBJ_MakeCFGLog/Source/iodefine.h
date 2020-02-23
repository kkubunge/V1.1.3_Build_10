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
		CONTROL_STATE		,	

		CM1_CASS_SenSor_STATUS	,	
		CM2_CASS_SenSor_STATUS	,	
		CM3_CASS_SenSor_STATUS	,	
	
		CurrentTime		,	
		IPS_VERSION		,	
		PM1_IPS_VERSION		,	
		PM2_IPS_VERSION		,	
		PM3_IPS_VERSION		,	
		Ver_Build_Date		,
			
		PRMA_BM1_SVENT_PRES	,	
		PRMA_BM1_FVENT_PRES	,	
		PRMA_BM1_VENT_TD	,	
		PRMA_BM1_SPUMP_PRES	,	
		F_PUMP_PRESSURE_BM1	,	
		PRMA_BM1_RPUMP_PRES	,	
		PRMD_BM1_COOL_TIME	,	
		PRMA_BM2_SVENT_PRES	,	
	
		PRMA_BM2_FVENT_PRES	,	
		PRMA_BM2_VENT_TD	,	
		PRMA_BM2_SPUMP_PRES	,	
		F_PUMP_PRESSURE_BM2	,	
		PRMA_BM2_RPUMP_PRES	,	
		PRMD_BM2_COOL_TIME	,	
		EQ_NAME			,
		CTC_EVENT_LOG_SVR,
} IOPointIndex;

#endif


