#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include <Kutlstr.h>
#include <Kutltime.h>

#include "cimseqnc.h"
#include "iodefine.h"
#include "IPS_Log.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "LPMA.AMHS_VALID_DI"	                , _K_D_IO	,	LPMA_AMHS_VALID                 ,	0	} ,                                                          
	{ "LPMA.AMHS_CS_0_DI"	                , _K_D_IO	,	LPMA_AMHS_CS_0                  ,	0	} ,                                                                 
	{ "LPMA.AMHS_CS_1_DI"	                , _K_D_IO	,	LPMA_AMHS_CS_1                  ,	0	} ,                                                                 
	{ "LPMA.AMHS_TR_REQ_DI"	                , _K_D_IO	,	LPMA_AMHS_TR_REQ                ,	0	} ,                                                                   
	{ "LPMA.AMHS_BUSY_DI"	                , _K_D_IO	,	LPMA_AMHS_BUSY                  ,	0	} ,                                                                 
	{ "LPMA.AMHS_COMPT_DI"	                , _K_D_IO	,	LPMA_AMHS_COMPT                 ,	0	} ,                                                                  
	{ "LPMA.AMHS_CONT_DI"	                , _K_D_IO	,	LPMA_AMHS_CONT                  ,	0	} ,                                                                 
	{ "LPMA.AMHS_L_REQ_DO"	                , _K_D_IO	,	LPMA_AMHS_L_REQ                 ,	0	} ,                                                                  
	{ "LPMA.AMHS_U_REQ_DO"	                , _K_D_IO	,	LPMA_AMHS_U_REQ                 ,	0	} ,                                                                  
	{ "LPMA.AMHS_READY_DO"	                , _K_D_IO	,	LPMA_AMHS_READY                 ,	0	} ,                                                                  
	{ "LPMA.AMHS_HO_AVBL_DO"				, _K_D_IO	,	LPMA_AMHS_HO_AVBL               ,	0	} ,                                                                                
	{ "LPMA.AMHS_ES_DO"						, _K_D_IO	,	LPMA_AMHS_ES                    ,	0	} ,

	{ "LPMB.AMHS_VALID_DI"	                , _K_D_IO	,	LPMB_AMHS_VALID                 ,	0	} ,                                                          
	{ "LPMB.AMHS_CS_0_DI"	                , _K_D_IO	,	LPMB_AMHS_CS_0                  ,	0	} ,                                                                 
	{ "LPMB.AMHS_CS_1_DI"	                , _K_D_IO	,	LPMB_AMHS_CS_1                  ,	0	} ,                                                                 
	{ "LPMB.AMHS_TR_REQ_DI"	                , _K_D_IO	,	LPMB_AMHS_TR_REQ                ,	0	} ,                                                                   
	{ "LPMB.AMHS_BUSY_DI"	                , _K_D_IO	,	LPMB_AMHS_BUSY                  ,	0	} ,                                                                 
	{ "LPMB.AMHS_COMPT_DI"	                , _K_D_IO	,	LPMB_AMHS_COMPT                 ,	0	} ,                                                                  
	{ "LPMB.AMHS_CONT_DI"	                , _K_D_IO	,	LPMB_AMHS_CONT                  ,	0	} ,                                                                 
	{ "LPMB.AMHS_L_REQ_DO"	                , _K_D_IO	,	LPMB_AMHS_L_REQ                 ,	0	} ,                                                                  
	{ "LPMB.AMHS_U_REQ_DO"	                , _K_D_IO	,	LPMB_AMHS_U_REQ                 ,	0	} ,                                                                  
	{ "LPMB.AMHS_READY_DO"	                , _K_D_IO	,	LPMB_AMHS_READY                 ,	0	} ,                                                                  
	{ "LPMB.AMHS_HO_AVBL_DO"				, _K_D_IO	,	LPMB_AMHS_HO_AVBL               ,	0	} ,                                                                                
	{ "LPMB.AMHS_ES_DO"						, _K_D_IO	,	LPMB_AMHS_ES                    ,	0	} ,

	{ "LPMC.AMHS_VALID_DI"	                , _K_D_IO	,	LPMC_AMHS_VALID                 ,	0	} ,                                                          
	{ "LPMC.AMHS_CS_0_DI"	                , _K_D_IO	,	LPMC_AMHS_CS_0                  ,	0	} ,                                                                 
	{ "LPMC.AMHS_CS_1_DI"	                , _K_D_IO	,	LPMC_AMHS_CS_1                  ,	0	} ,                                                                 
	{ "LPMC.AMHS_TR_REQ_DI"	                , _K_D_IO	,	LPMC_AMHS_TR_REQ                ,	0	} ,                                                                   
	{ "LPMC.AMHS_BUSY_DI"	                , _K_D_IO	,	LPMC_AMHS_BUSY                  ,	0	} ,                                                                 
	{ "LPMC.AMHS_COMPT_DI"	                , _K_D_IO	,	LPMC_AMHS_COMPT                 ,	0	} ,                                                                  
	{ "LPMC.AMHS_CONT_DI"	                , _K_D_IO	,	LPMC_AMHS_CONT                  ,	0	} ,                                                                 
	{ "LPMC.AMHS_L_REQ_DO"	                , _K_D_IO	,	LPMC_AMHS_L_REQ                 ,	0	} ,                                                                  
	{ "LPMC.AMHS_U_REQ_DO"	                , _K_D_IO	,	LPMC_AMHS_U_REQ                 ,	0	} ,                                                                  
	{ "LPMC.AMHS_READY_DO"	                , _K_D_IO	,	LPMC_AMHS_READY                 ,	0	} ,                                                                  
	{ "LPMC.AMHS_HO_AVBL_DO"			    , _K_D_IO	,	LPMC_AMHS_HO_AVBL               ,	0	} ,                                                                                
	{ "LPMC.AMHS_ES_DO"						, _K_D_IO	,	LPMC_AMHS_ES                    ,	0	} ,	

	""
};
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void CM1_PIO_Check_Thread()
{
	int nCommStatus;

	int nPre_LPMA_VALID, nPre_LPMA_CS_0, nPre_LPMA_CS_1, nPre_LPMA_TR_REQ, nPre_LPMA_BUSY, nPre_LPMA_COMPT, nPre_LPMA_CONT, nPre_LPMA_L_REQ, nPre_LPMA_U_REQ, nPre_LPMA_READY, nPre_LPMA_HO_AVBL, nPre_LPMA_ES;  
	int nCur_LPMA_VALID, nCur_LPMA_CS_0, nCur_LPMA_CS_1, nCur_LPMA_TR_REQ, nCur_LPMA_BUSY, nCur_LPMA_COMPT, nCur_LPMA_CONT, nCur_LPMA_L_REQ, nCur_LPMA_U_REQ, nCur_LPMA_READY, nCur_LPMA_HO_AVBL, nCur_LPMA_ES ;    

	char szPortID[128];
	char pszFtnName[256] = "";

	memset( szPortID	  ,	0,	sizeof(szPortID));

	strcpy( szPortID, "LPMA" );
	
	while(1)
	{
//		LOG(pszFtnName, ELT_File, "========================> %s First Cycle", szPortID );
		nPre_LPMA_VALID	  = READ_DIGITAL( LPMA_AMHS_VALID   , &nCommStatus );
		nPre_LPMA_CS_0	  = READ_DIGITAL( LPMA_AMHS_CS_0    , &nCommStatus );
		nPre_LPMA_CS_1	  = READ_DIGITAL( LPMA_AMHS_CS_1    , &nCommStatus );
		nPre_LPMA_TR_REQ  = READ_DIGITAL( LPMA_AMHS_TR_REQ  , &nCommStatus );
		nPre_LPMA_BUSY	  = READ_DIGITAL( LPMA_AMHS_BUSY    , &nCommStatus );
		nPre_LPMA_COMPT	  = READ_DIGITAL( LPMA_AMHS_COMPT   , &nCommStatus );
		nPre_LPMA_CONT	  = READ_DIGITAL( LPMA_AMHS_CONT    , &nCommStatus );

		nPre_LPMA_L_REQ	  = READ_DIGITAL( LPMA_AMHS_L_REQ   , &nCommStatus );
		nPre_LPMA_U_REQ   = READ_DIGITAL( LPMA_AMHS_U_REQ   , &nCommStatus );
		nPre_LPMA_READY   = READ_DIGITAL( LPMA_AMHS_READY   , &nCommStatus );
		nPre_LPMA_HO_AVBL = READ_DIGITAL( LPMA_AMHS_HO_AVBL , &nCommStatus );
		nPre_LPMA_ES	  = READ_DIGITAL( LPMA_AMHS_ES      , &nCommStatus );

		//WAIT_SECONDS(0.1);
		//... 2017.03.07
		Sleep(100);

		nCur_LPMA_VALID	  = READ_DIGITAL( LPMA_AMHS_VALID   , &nCommStatus );
		nCur_LPMA_CS_0	  = READ_DIGITAL( LPMA_AMHS_CS_0    , &nCommStatus );
		nCur_LPMA_CS_1	  = READ_DIGITAL( LPMA_AMHS_CS_1    , &nCommStatus );
		nCur_LPMA_TR_REQ  = READ_DIGITAL( LPMA_AMHS_TR_REQ  , &nCommStatus );
		nCur_LPMA_BUSY	  = READ_DIGITAL( LPMA_AMHS_BUSY    , &nCommStatus );
		nCur_LPMA_COMPT	  = READ_DIGITAL( LPMA_AMHS_COMPT   , &nCommStatus );
		nCur_LPMA_CONT	  = READ_DIGITAL( LPMA_AMHS_CONT    , &nCommStatus );

		nCur_LPMA_L_REQ	  = READ_DIGITAL( LPMA_AMHS_L_REQ   , &nCommStatus );
		nCur_LPMA_U_REQ   = READ_DIGITAL( LPMA_AMHS_U_REQ   , &nCommStatus );
		nCur_LPMA_READY   = READ_DIGITAL( LPMA_AMHS_READY   , &nCommStatus );
		nCur_LPMA_HO_AVBL = READ_DIGITAL( LPMA_AMHS_HO_AVBL , &nCommStatus );
		nCur_LPMA_ES	  = READ_DIGITAL( LPMA_AMHS_ES      , &nCommStatus );
		
		if( nPre_LPMA_VALID != nCur_LPMA_VALID )
		{
			LOG(pszFtnName, ELT_File, "%s	VALID	%d", szPortID, nCur_LPMA_VALID );
		}
		if( nPre_LPMA_CS_0 != nCur_LPMA_CS_0 )
		{
			LOG(pszFtnName, ELT_File, "%s	CS_0	%d", szPortID, nCur_LPMA_CS_0 );
		}
		if( nPre_LPMA_CS_1 != nCur_LPMA_CS_1 )
		{
			LOG(pszFtnName, ELT_File, "%s	CS_1	%d", szPortID, nCur_LPMA_CS_1 );
		}
		if( nPre_LPMA_TR_REQ != nCur_LPMA_TR_REQ )
		{
			LOG(pszFtnName, ELT_File, "%s	TR_REQ	%d", szPortID, nCur_LPMA_TR_REQ );
		}
		if( nPre_LPMA_BUSY != nCur_LPMA_BUSY )
		{
			LOG(pszFtnName, ELT_File, "%s	BUSY	%d", szPortID, nCur_LPMA_BUSY );
		}
		if( nPre_LPMA_COMPT != nCur_LPMA_COMPT )
		{
			LOG(pszFtnName, ELT_File, "%s	COMPT	%d", szPortID, nCur_LPMA_COMPT );
		}
		if( nPre_LPMA_CONT != nCur_LPMA_CONT )
		{
			LOG(pszFtnName, ELT_File, "%s	CONT	%d", szPortID, nCur_LPMA_CONT );
		}
		if( nPre_LPMA_L_REQ != nCur_LPMA_L_REQ )
		{
			LOG(pszFtnName, ELT_File, "%s	L_REQ	%d", szPortID, nCur_LPMA_L_REQ );
		}
		if( nPre_LPMA_U_REQ != nCur_LPMA_U_REQ )
		{
			LOG(pszFtnName, ELT_File, "%s	U_REQ	%d", szPortID, nCur_LPMA_U_REQ );
		}
		if( nPre_LPMA_READY != nCur_LPMA_READY )
		{
			LOG(pszFtnName, ELT_File, "%s	READY	%d", szPortID, nCur_LPMA_READY );
		}
		if( nPre_LPMA_HO_AVBL != nCur_LPMA_HO_AVBL )
		{
			LOG(pszFtnName, ELT_File, "%s	HO_AVBL	%d", szPortID, nCur_LPMA_HO_AVBL );
		}
		if( nPre_LPMA_ES != nCur_LPMA_ES )
		{
			LOG(pszFtnName, ELT_File, "%s	ES	%d", szPortID, nCur_LPMA_ES );
		}
	}
	_endthread();
}

void CM2_PIO_Check_Thread()
{
	int nCommStatus;

	int nPre_LPMB_VALID, nPre_LPMB_CS_0, nPre_LPMB_CS_1, nPre_LPMB_TR_REQ, nPre_LPMB_BUSY, nPre_LPMB_COMPT, nPre_LPMB_CONT, nPre_LPMB_L_REQ, nPre_LPMB_U_REQ, nPre_LPMB_READY, nPre_LPMB_HO_AVBL, nPre_LPMB_ES;  
	int nCur_LPMB_VALID, nCur_LPMB_CS_0, nCur_LPMB_CS_1, nCur_LPMB_TR_REQ, nCur_LPMB_BUSY, nCur_LPMB_COMPT, nCur_LPMB_CONT, nCur_LPMB_L_REQ, nCur_LPMB_U_REQ, nCur_LPMB_READY, nCur_LPMB_HO_AVBL, nCur_LPMB_ES ;    

	char szPortID[128];
	char pszFtnName[256] = "";

	memset( szPortID	  ,	0,	sizeof(szPortID));

	strcpy( szPortID, "LPMB" );
	
	while(1)
	{
		nPre_LPMB_VALID	  = READ_DIGITAL( LPMB_AMHS_VALID   , &nCommStatus );
		nPre_LPMB_CS_0	  = READ_DIGITAL( LPMB_AMHS_CS_0    , &nCommStatus );
		nPre_LPMB_CS_1	  = READ_DIGITAL( LPMB_AMHS_CS_1    , &nCommStatus );
		nPre_LPMB_TR_REQ  = READ_DIGITAL( LPMB_AMHS_TR_REQ  , &nCommStatus );
		nPre_LPMB_BUSY	  = READ_DIGITAL( LPMB_AMHS_BUSY    , &nCommStatus );
		nPre_LPMB_COMPT	  = READ_DIGITAL( LPMB_AMHS_COMPT   , &nCommStatus );
		nPre_LPMB_CONT	  = READ_DIGITAL( LPMB_AMHS_CONT    , &nCommStatus );

		nPre_LPMB_L_REQ	  = READ_DIGITAL( LPMB_AMHS_L_REQ   , &nCommStatus );
		nPre_LPMB_U_REQ   = READ_DIGITAL( LPMB_AMHS_U_REQ   , &nCommStatus );
		nPre_LPMB_READY   = READ_DIGITAL( LPMB_AMHS_READY   , &nCommStatus );
		nPre_LPMB_HO_AVBL = READ_DIGITAL( LPMB_AMHS_HO_AVBL , &nCommStatus );
		nPre_LPMB_ES	  = READ_DIGITAL( LPMB_AMHS_ES      , &nCommStatus );

		//WAIT_SECONDS(0.1);
		//... 2017.03.07
		Sleep(100);

		nCur_LPMB_VALID	  = READ_DIGITAL( LPMB_AMHS_VALID   , &nCommStatus );
		nCur_LPMB_CS_0	  = READ_DIGITAL( LPMB_AMHS_CS_0    , &nCommStatus );
		nCur_LPMB_CS_1	  = READ_DIGITAL( LPMB_AMHS_CS_1    , &nCommStatus );
		nCur_LPMB_TR_REQ  = READ_DIGITAL( LPMB_AMHS_TR_REQ  , &nCommStatus );
		nCur_LPMB_BUSY	  = READ_DIGITAL( LPMB_AMHS_BUSY    , &nCommStatus );
		nCur_LPMB_COMPT	  = READ_DIGITAL( LPMB_AMHS_COMPT   , &nCommStatus );
		nCur_LPMB_CONT	  = READ_DIGITAL( LPMB_AMHS_CONT    , &nCommStatus );

		nCur_LPMB_L_REQ	  = READ_DIGITAL( LPMB_AMHS_L_REQ   , &nCommStatus );
		nCur_LPMB_U_REQ   = READ_DIGITAL( LPMB_AMHS_U_REQ   , &nCommStatus );
		nCur_LPMB_READY   = READ_DIGITAL( LPMB_AMHS_READY   , &nCommStatus );
		nCur_LPMB_HO_AVBL = READ_DIGITAL( LPMB_AMHS_HO_AVBL , &nCommStatus );
		nCur_LPMB_ES	  = READ_DIGITAL( LPMB_AMHS_ES      , &nCommStatus );
		
		if( nPre_LPMB_VALID != nCur_LPMB_VALID )
		{
			LOG(pszFtnName, ELT_File, "%s	VALID	%d", szPortID, nCur_LPMB_VALID );
		}
		if( nPre_LPMB_CS_0 != nCur_LPMB_CS_0 )
		{
			LOG(pszFtnName, ELT_File, "%s	CS_0	%d", szPortID, nCur_LPMB_CS_0 );
		}
		if( nPre_LPMB_CS_1 != nCur_LPMB_CS_1 )
		{
			LOG(pszFtnName, ELT_File, "%s	CS_1	%d", szPortID, nCur_LPMB_CS_1 );
		}
		if( nPre_LPMB_TR_REQ != nCur_LPMB_TR_REQ )
		{
			LOG(pszFtnName, ELT_File, "%s	TR_REQ	%d", szPortID, nCur_LPMB_TR_REQ );
		}
		if( nPre_LPMB_BUSY != nCur_LPMB_BUSY )
		{
			LOG(pszFtnName, ELT_File, "%s	BUSY	%d", szPortID, nCur_LPMB_BUSY );
		}
		if( nPre_LPMB_COMPT != nCur_LPMB_COMPT )
		{
			LOG(pszFtnName, ELT_File, "%s	COMPT	%d", szPortID, nCur_LPMB_COMPT );
		}
		if( nPre_LPMB_CONT != nCur_LPMB_CONT )
		{
			LOG(pszFtnName, ELT_File, "%s	CONT	%d", szPortID, nCur_LPMB_CONT );
		}
		if( nPre_LPMB_L_REQ != nCur_LPMB_L_REQ )
		{
			LOG(pszFtnName, ELT_File, "%s	L_REQ	%d", szPortID, nCur_LPMB_L_REQ );
		}
		if( nPre_LPMB_U_REQ != nCur_LPMB_U_REQ )
		{
			LOG(pszFtnName, ELT_File, "%s	U_REQ	%d", szPortID, nCur_LPMB_U_REQ );
		}
		if( nPre_LPMB_READY != nCur_LPMB_READY )
		{
			LOG(pszFtnName, ELT_File, "%s	READY	%d", szPortID, nCur_LPMB_READY );
		}
		if( nPre_LPMB_HO_AVBL != nCur_LPMB_HO_AVBL )
		{
			LOG(pszFtnName, ELT_File, "%s	HO_AVBL	%d", szPortID, nCur_LPMB_HO_AVBL );
		}
		if( nPre_LPMB_ES != nCur_LPMB_ES )
		{
			LOG(pszFtnName, ELT_File, "%s	ES	%d", szPortID, nCur_LPMB_ES );
		}
	}
	_endthread();
}

void CM3_PIO_Check_Thread()
{
	int nCommStatus;

	int nPre_LPMC_VALID, nPre_LPMC_CS_0, nPre_LPMC_CS_1, nPre_LPMC_TR_REQ, nPre_LPMC_BUSY, nPre_LPMC_COMPT, nPre_LPMC_CONT, nPre_LPMC_L_REQ, nPre_LPMC_U_REQ, nPre_LPMC_READY, nPre_LPMC_HO_AVBL, nPre_LPMC_ES;  
	int nCur_LPMC_VALID, nCur_LPMC_CS_0, nCur_LPMC_CS_1, nCur_LPMC_TR_REQ, nCur_LPMC_BUSY, nCur_LPMC_COMPT, nCur_LPMC_CONT, nCur_LPMC_L_REQ, nCur_LPMC_U_REQ, nCur_LPMC_READY, nCur_LPMC_HO_AVBL, nCur_LPMC_ES ;    

	char szPortID[128];
	char pszFtnName[256] = "";

	memset( szPortID	  ,	0,	sizeof(szPortID));

	strcpy( szPortID, "LPMC" );
	
	while(1)
	{
		nPre_LPMC_VALID	  = READ_DIGITAL( LPMC_AMHS_VALID   , &nCommStatus );
		nPre_LPMC_CS_0	  = READ_DIGITAL( LPMC_AMHS_CS_0    , &nCommStatus );
		nPre_LPMC_CS_1	  = READ_DIGITAL( LPMC_AMHS_CS_1    , &nCommStatus );
		nPre_LPMC_TR_REQ  = READ_DIGITAL( LPMC_AMHS_TR_REQ  , &nCommStatus );
		nPre_LPMC_BUSY	  = READ_DIGITAL( LPMC_AMHS_BUSY    , &nCommStatus );
		nPre_LPMC_COMPT	  = READ_DIGITAL( LPMC_AMHS_COMPT   , &nCommStatus );
		nPre_LPMC_CONT	  = READ_DIGITAL( LPMC_AMHS_CONT    , &nCommStatus );

		nPre_LPMC_L_REQ	  = READ_DIGITAL( LPMC_AMHS_L_REQ   , &nCommStatus );
		nPre_LPMC_U_REQ   = READ_DIGITAL( LPMC_AMHS_U_REQ   , &nCommStatus );
		nPre_LPMC_READY   = READ_DIGITAL( LPMC_AMHS_READY   , &nCommStatus );
		nPre_LPMC_HO_AVBL = READ_DIGITAL( LPMC_AMHS_HO_AVBL , &nCommStatus );
		nPre_LPMC_ES	  = READ_DIGITAL( LPMC_AMHS_ES      , &nCommStatus );

		//WAIT_SECONDS(0.1);
		//... 2017.03.07
		Sleep(100);

		nCur_LPMC_VALID	  = READ_DIGITAL( LPMC_AMHS_VALID   , &nCommStatus );
		nCur_LPMC_CS_0	  = READ_DIGITAL( LPMC_AMHS_CS_0    , &nCommStatus );
		nCur_LPMC_CS_1	  = READ_DIGITAL( LPMC_AMHS_CS_1    , &nCommStatus );
		nCur_LPMC_TR_REQ  = READ_DIGITAL( LPMC_AMHS_TR_REQ  , &nCommStatus );
		nCur_LPMC_BUSY	  = READ_DIGITAL( LPMC_AMHS_BUSY    , &nCommStatus );
		nCur_LPMC_COMPT	  = READ_DIGITAL( LPMC_AMHS_COMPT   , &nCommStatus );
		nCur_LPMC_CONT	  = READ_DIGITAL( LPMC_AMHS_CONT    , &nCommStatus );

		nCur_LPMC_L_REQ	  = READ_DIGITAL( LPMC_AMHS_L_REQ   , &nCommStatus );
		nCur_LPMC_U_REQ   = READ_DIGITAL( LPMC_AMHS_U_REQ   , &nCommStatus );
		nCur_LPMC_READY   = READ_DIGITAL( LPMC_AMHS_READY   , &nCommStatus );
		nCur_LPMC_HO_AVBL = READ_DIGITAL( LPMC_AMHS_HO_AVBL , &nCommStatus );
		nCur_LPMC_ES	  = READ_DIGITAL( LPMC_AMHS_ES      , &nCommStatus );
		
		if( nPre_LPMC_VALID != nCur_LPMC_VALID )
		{
			LOG(pszFtnName, ELT_File, "%s	VALID	%d", szPortID, nCur_LPMC_VALID );
		}
		if( nPre_LPMC_CS_0 != nCur_LPMC_CS_0 )
		{
			LOG(pszFtnName, ELT_File, "%s	CS_0	%d", szPortID, nCur_LPMC_CS_0 );
		}
		if( nPre_LPMC_CS_1 != nCur_LPMC_CS_1 )
		{
			LOG(pszFtnName, ELT_File, "%s	CS_1	%d", szPortID, nCur_LPMC_CS_1 );
		}
		if( nPre_LPMC_TR_REQ != nCur_LPMC_TR_REQ )
		{
			LOG(pszFtnName, ELT_File, "%s	TR_REQ	%d", szPortID, nCur_LPMC_TR_REQ );
		}
		if( nPre_LPMC_BUSY != nCur_LPMC_BUSY )
		{
			LOG(pszFtnName, ELT_File, "%s	BUSY	%d", szPortID, nCur_LPMC_BUSY );
		}
		if( nPre_LPMC_COMPT != nCur_LPMC_COMPT )
		{
			LOG(pszFtnName, ELT_File, "%s	COMPT	%d", szPortID, nCur_LPMC_COMPT );
		}
		if( nPre_LPMC_CONT != nCur_LPMC_CONT )
		{
			LOG(pszFtnName, ELT_File, "%s	CONT	%d", szPortID, nCur_LPMC_CONT );
		}
		if( nPre_LPMC_L_REQ != nCur_LPMC_L_REQ )
		{
			LOG(pszFtnName, ELT_File, "%s	L_REQ	%d", szPortID, nCur_LPMC_L_REQ );
		}
		if( nPre_LPMC_U_REQ != nCur_LPMC_U_REQ )
		{
			LOG(pszFtnName, ELT_File, "%s	U_REQ	%d", szPortID, nCur_LPMC_U_REQ );
		}
		if( nPre_LPMC_READY != nCur_LPMC_READY )
		{
			LOG(pszFtnName, ELT_File, "%s	READY	%d", szPortID, nCur_LPMC_READY );
		}
		if( nPre_LPMC_HO_AVBL != nCur_LPMC_HO_AVBL )
		{
			LOG(pszFtnName, ELT_File, "%s	HO_AVBL	%d", szPortID, nCur_LPMC_HO_AVBL );
		}
		if( nPre_LPMC_ES != nCur_LPMC_ES )
		{
			LOG(pszFtnName, ELT_File, "%s	ES	%d", szPortID, nCur_LPMC_ES );
		}
	}
	_endthread();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Program_Main() {
/*	int nCommStatus;
	
	printf( "Configuration file Load starting(Monitoring function)!!!.............\n" );
	S_TIMER_READY();
	while ( TRUE ) {
		WAIT_SECONDS(0.1);
		if ( READ_DIGITAL( CONFIG_LOADING , &nCommStatus ) == ON ) break;
	}
	printf( "Configuration file Loading end((Monitoring function - %d sec)!!!.............\n" , (int)S_TIMER_ELAPSED() );
*/

	_beginthread((void*)CM1_PIO_Check_Thread, 0, NULL);
	_beginthread((void*)CM2_PIO_Check_Thread, 0, NULL);
	_beginthread((void*)CM3_PIO_Check_Thread, 0, NULL);

	return SYS_SUCCESS;
}

//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	CreateDirectory("f:\\datalog\\_Log\\PIOViewer" , NULL);
}

