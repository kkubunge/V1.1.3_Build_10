#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef enum {
	IO_DRIVER_LOAD		,	
	ModuleNo			,
	SW_GbIntlckDM		,
	MFCInfoComDO		,
	MFCDataScenDI		,
	PRMA_TO_MFCScan		,
	PRMA_TO_MFCScanRty	,
	DN_MFCInfoRead_DO	,
	PRMD_MFC_Diagnosis	,
	                                
	MFC01_SerialNumSI 	,	
	MFC02_SerialNumSI	,	
	MFC03_SerialNumSI	,	
	MFC04_SerialNumSI	,	
	MFC05_SerialNumSI	,	
	MFC06_SerialNumSI	,	
	MFC07_SerialNumSI	,	
	MFC08_SerialNumSI	,	
	MFC09_SerialNumSI	,	
	                                
	MFC01_ManuFacSI 	,	
	MFC02_ManuFacSI		,	
	MFC03_ManuFacSI		,	
	MFC04_ManuFacSI		,	
	MFC05_ManuFacSI		,	
	MFC06_ManuFacSI		,	
	MFC07_ManuFacSI		,	
	MFC08_ManuFacSI		,	
	MFC09_ManuFacSI		,	
	                                
	MFC01_ModelNumSI	,	
	MFC02_ModelNumSI	,	
	MFC03_ModelNumSI	,	
	MFC04_ModelNumSI	,	
	MFC05_ModelNumSI	,	
	MFC06_ModelNumSI	,	
	MFC07_ModelNumSI	,	
	MFC08_ModelNumSI	,	
	MFC09_ModelNumSI	,	
	                                
	MFC01_GasSymbolSI	,	
	MFC02_GasSymbolSI	,	
	MFC03_GasSymbolSI	,	
	MFC04_GasSymbolSI	,	
	MFC05_GasSymbolSI	,	
	MFC06_GasSymbolSI	,	
	MFC07_GasSymbolSI	,	
	MFC08_GasSymbolSI	,	
	MFC09_GasSymbolSI	,	
	                                
	MFC01_FullScaleSI	,	
	MFC02_FullScaleSI	,	
	MFC03_FullScaleSI	,	
	MFC04_FullScaleSI	,	
	MFC05_FullScaleSI	,	
	MFC06_FullScaleSI	,	
	MFC07_FullScaleSI	,	
	MFC08_FullScaleSI	,	
	MFC09_FullScaleSI	,	
	                                
	MFC01_SWVersionSI	,	
	MFC02_SWVersionSI	,	
	MFC03_SWVersionSI	,	
	MFC04_SWVersionSI	,	
	MFC05_SWVersionSI	,	
	MFC06_SWVersionSI	,	
	MFC07_SWVersionSI	,	
	MFC08_SWVersionSI	,	
	MFC09_SWVersionSI	,	
	                                
	MFC01_HWVersionSI	,	
	MFC02_HWVersionSI	,	
	MFC03_HWVersionSI	,	
	MFC04_HWVersionSI	,	
	MFC05_HWVersionSI	,	
	MFC06_HWVersionSI	,	
	MFC07_HWVersionSI	,	
	MFC08_HWVersionSI	,	
	MFC09_HWVersionSI	,	
	                                
	MFC01_GasNumSI	 	,	
	MFC02_GasNumSI	 	,	
	MFC03_GasNumSI	 	,	
	MFC04_GasNumSI	 	,	
	MFC05_GasNumSI	 	,	
	MFC06_GasNumSI	 	,	
	MFC07_GasNumSI	 	,	
	MFC08_GasNumSI	 	,	
	MFC09_GasNumSI	 	,	
	                                
	MFC01_OffsetASI	 	,	
	MFC02_OffsetASI	 	,	
	MFC03_OffsetASI	 	,	
	MFC04_OffsetASI	 	,	
	MFC05_OffsetASI	 	,	
	MFC06_OffsetASI	 	,	
	MFC07_OffsetASI	 	,	
	MFC08_OffsetASI	 	,	
	MFC09_OffsetASI	 	,	
	                                
	MFC01_OffsetBSI	 	,	
	MFC02_OffsetBSI	 	,	
	MFC03_OffsetBSI	 	,	
	MFC04_OffsetBSI	 	,	
	MFC05_OffsetBSI	 	,	
	MFC06_OffsetBSI	 	,	
	MFC07_OffsetBSI	 	,	
	MFC08_OffsetBSI	 	,	
	MFC09_OffsetBSI	 	,	
	                                
	MFC01_MKP_OffsetBSI	,	
	MFC02_MKP_OffsetBSI	,	
	MFC03_MKP_OffsetBSI	,	
	MFC04_MKP_OffsetBSI	,	
	MFC05_MKP_OffsetBSI	,	
	MFC06_MKP_OffsetBSI	,	
	MFC07_MKP_OffsetBSI	,	
	MFC08_MKP_OffsetBSI	,	
	MFC09_MKP_OffsetBSI	,	
	                                
	MFC01_HITACHI_OffsetBSI	,	
	MFC02_HITACHI_OffsetBSI	,	
	MFC03_HITACHI_OffsetBSI	,	
	MFC04_HITACHI_OffsetBSI	,	
	MFC05_HITACHI_OffsetBSI	,	
	MFC06_HITACHI_OffsetBSI	,	
	MFC07_HITACHI_OffsetBSI	,	
	MFC08_HITACHI_OffsetBSI	,	
	MFC09_HITACHI_OffsetBSI	,	
	                                
	MFC01_GainSI	 	,	
	MFC02_GainSI	 	,	
	MFC03_GainSI	 	,	
	MFC04_GainSI	 	,	
	MFC05_GainSI	 	,	
	MFC06_GainSI	 	,	
	MFC07_GainSI	 	,	
	MFC08_GainSI	 	,	
	MFC09_GainSI	 	,	
	                                
	MFC01_MacIDSI	 	,
	MFC02_MacIDSI	 	,
	MFC03_MacIDSI	 	,
	MFC04_MacIDSI	 	,
	MFC05_MacIDSI	 	,
	MFC06_MacIDSI	 	,
	MFC07_MacIDSI	 	,
	MFC08_MacIDSI	 	,
	MFC09_MacIDSI	 	,
	                                
	PM_COMMSTATUS		,	
} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
typedef struct {
    DWORD   dwUnknown1;
    ULONG   uKeMaximumIncrement;
    ULONG   uPageSize;
    ULONG   uMmNumberOfPhysicalPages;
    ULONG   uMmLowestPhysicalPage;
    ULONG   uMmHighestPhysicalPage;
    ULONG   uAllocationGranularity;
    PVOID   pLowestUserAddress;
    PVOID   pMmHighestUserAddress;
    ULONG   uKeActiveProcessors;
    BYTE    bKeNumberProcessors;
    BYTE    bUnknown2;
    WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct {
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct {
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif

