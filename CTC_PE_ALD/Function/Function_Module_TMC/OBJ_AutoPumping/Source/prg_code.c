#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
//------------------------------------------------------------------------------------------
#include <Kutlstr.h>
#include <Kutltime.h>
//------------------------------------------------------------------------------------------
#include "cimseqnc.h"
#include "iodefine.h"
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE	= FALSE;
//------------------------------------------------------------------------------------------
enum { Off , On };
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"			, _K_D_IO	,	IO_Driver_Loding		,	0	} ,

	{ "TMC_TM_VAC_CONTROL"			, _K_F_IO	,	TM_VACUUM_CONTROL		,	0	} ,
	{ "TMC_RBLOW_CONTROL"			, _K_F_IO	,	RBLOW_CONTROL			,	0	} ,
	{ "TMC_FMRBLOW_CONTROL"			, _K_F_IO	,	FMRBLOW_CONTROL			,	0	} ,
	{ "TMC_BM1_VAC_CONTROL"			, _K_F_IO	,	BM1_VAC_CONTROL			,	0	} ,
	{ "TMC_BM2_VAC_CONTROL"			, _K_F_IO	,	BM2_VAC_CONTROL			,	0	} ,
//	{ "TMC_MAINBM_VAC_CONTROL"		, _K_F_IO	,	MAINBM_VAC_CONTROL		,	0	} ,
	{ "TMC_FMCM1_CONTROL"			, _K_F_IO	,	CM1_CONTROLL			,	0	} ,
	{ "TMC_FMCM2_CONTROL"			, _K_F_IO	,	CM2_CONTROLL			,	0	} ,
	{ "TMC_FMCM3_CONTROL"			, _K_F_IO	,	CM3_CONTROLL			,	0	} ,

	{ "FC_TM_INIT_CONTROL"			, _K_D_IO	,	INIT_CONTROL			,	0	} ,
	{ "FC_CONFIG_LOADING"			, _K_D_IO	,	CONFIG_LOADING			,	0	} ,
	{ "CTC.TM_M_CONTROL"			, _K_D_IO	,	TM_M_CONTROL			,	0	} ,
	{ "CTC.FM_M_CONTROL"			, _K_D_IO	,	FM_M_CONTROL			,	0	} ,
	{ "CTC.BM1M_CONTROL"			, _K_D_IO	,	BM1M_CONTROL			,	0	} ,
	{ "CTC.BM2M_CONTROL"			, _K_D_IO	,	BM2M_CONTROL			,	0	} ,
//	{ "CTC.MAINBMM_CONTROL"			, _K_D_IO	,	MAINBMM_CONTROL			,	0	} ,
	{ "CTC.CM1M_CONTROL"			, _K_D_IO	,	CM1M_CONTROL			,	0	} ,
	{ "CTC.CM2M_CONTROL"			, _K_D_IO	,	CM2M_CONTROL			,	0	} ,
	{ "CTC.CM3M_CONTROL"			, _K_D_IO	,	CM3M_CONTROL			,	0	} ,
	
	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
	Module_Status TM_Vacuum_Status , RBLow_Status , FM_RBLow_Status , LLA_Vacuum_Status , LLB_Vacuum_Status , CM1_Status , CM2_Status , CM3_Status;
	int	CommStatus;
	
	printf( "Configuration file Loading start...\n" );
	S_TIMER_READY();
	while ( TRUE ) {
		WAIT_SECONDS(0.1);
		if ( READ_DIGITAL( CONFIG_LOADING , &CommStatus ) == On ) break;
	}
	printf( "Configuration file Loading end(%d)...\n" , (int)S_TIMER_ELAPSED() );

	if ( READ_DIGITAL( INIT_CONTROL , &CommStatus ) == Off )	return SYS_SUCCESS;
	else {        
		S_TIMER_READY();
		WRITE_DIGITAL( TM_M_CONTROL , 1 , &CommStatus );
		WRITE_DIGITAL( FM_M_CONTROL , 1 , &CommStatus );
		WRITE_DIGITAL( BM1M_CONTROL , 1 , &CommStatus );
		WRITE_DIGITAL( BM2M_CONTROL , 1 , &CommStatus );
//		WRITE_DIGITAL( MAINBMM_CONTROL , 1 , &CommStatus ); //added 20070418
		WRITE_DIGITAL( CM1M_CONTROL , 1 , &CommStatus );
		WRITE_DIGITAL( CM2M_CONTROL , 1 , &CommStatus );
		WRITE_DIGITAL( CM3M_CONTROL , 1 , &CommStatus );
		printf( "Initialize Function Starting!!!.............\n" );

        RUN_SET_FUNCTION( RBLOW_CONTROL			, "HOME" );
		RUN_SET_FUNCTION( FMRBLOW_CONTROL		, "HOME" );
	}
	while( TRUE ) {
		if ( !WAIT_SECONDS( 1 ) ) {
		WRITE_DIGITAL( TM_M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( FM_M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( BM1M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( BM2M_CONTROL , 0 , &CommStatus );
//		WRITE_DIGITAL( MAINBMM_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( CM1M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( CM2M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( CM3M_CONTROL , 0 , &CommStatus );
			return SYS_ABORTED;
		}
		TM_Vacuum_Status	= READ_FUNCTION( TM_VACUUM_CONTROL	);
		RBLow_Status		= READ_FUNCTION( RBLOW_CONTROL		);
		FM_RBLow_Status		= READ_FUNCTION( FMRBLOW_CONTROL	);
		LLA_Vacuum_Status	= READ_FUNCTION( BM1_VAC_CONTROL	);
		LLB_Vacuum_Status	= READ_FUNCTION( BM2_VAC_CONTROL	);
//		MAINLL_Vacuum_Status= READ_FUNCTION( MAINBM_VAC_CONTROL	);
		CM1_Status			= READ_FUNCTION( CM1_CONTROLL		);
		CM2_Status			= READ_FUNCTION( CM2_CONTROLL		);
		CM3_Status			= READ_FUNCTION( CM3_CONTROLL		);

		if ( ( TM_Vacuum_Status != SYS_RUNNING ) && ( RBLow_Status != SYS_RUNNING ) && ( FM_RBLow_Status != SYS_RUNNING ) && ( LLA_Vacuum_Status != SYS_RUNNING ) && ( LLB_Vacuum_Status != SYS_RUNNING ) && ( CM1_Status != SYS_RUNNING ) && ( CM2_Status != SYS_RUNNING ) && ( CM3_Status != SYS_RUNNING ) ) break;
	}
	
	if ( ( TM_Vacuum_Status == SYS_ABORTED ) || ( RBLow_Status == SYS_ABORTED ) || ( FM_RBLow_Status == SYS_ABORTED ) || ( LLA_Vacuum_Status == SYS_ABORTED ) || ( LLB_Vacuum_Status == SYS_ABORTED ) || ( CM1_Status == SYS_ABORTED ) || ( CM2_Status == SYS_ABORTED ) || ( CM3_Status == SYS_ABORTED ) ) {
		printf( "Initialize Function End(%d) - Failed!!!.............\n" , (int)S_TIMER_ELAPSED() );
		WRITE_DIGITAL( TM_M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( FM_M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( BM1M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( BM2M_CONTROL , 0 , &CommStatus );
//		WRITE_DIGITAL( MAINBMM_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( CM1M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( CM2M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( CM3M_CONTROL , 0 , &CommStatus );
		return SYS_ABORTED;
	}
	else {
		printf( "Initialize Function End(%d) - Success!!!.............\n" , (int)S_TIMER_ELAPSED() );
		WRITE_DIGITAL( TM_M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( FM_M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( BM1M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( BM2M_CONTROL , 0 , &CommStatus );
//		WRITE_DIGITAL( MAINBMM_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( CM1M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( CM2M_CONTROL , 0 , &CommStatus );
		WRITE_DIGITAL( CM3M_CONTROL , 0 , &CommStatus );
		return SYS_SUCCESS;
	}
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Program_Enter_Code() {
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	return TRUE;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
