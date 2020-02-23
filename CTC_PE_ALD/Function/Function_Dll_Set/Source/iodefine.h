#ifndef IODEFINE_H
#define IODEFINE_H
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

enum { On_0 , Off_1 };
enum { RS_No_0, RS_Align_1, ENT_No_2, ENT_Align_3 };

typedef enum {
	SCHEDULER_S			,
	SCHEDULER_D			,
	PM1_PreRecipeUseA	,
	PM2_PreRecipeUseA	,
	PM3_PreRecipeUseA	,
	PM4_PreRecipeUseA	,
	PM5_PreRecipeUseA	,
	PM1_PreRecipeA		,
	PM2_PreRecipeA		,
	PM3_PreRecipeA		,
	PM4_PreRecipeA		,
	PM5_PreRecipeA		,
	PM1_PreRecipeUseB	,
	PM2_PreRecipeUseB	,
	PM3_PreRecipeUseB	,
	PM4_PreRecipeUseB	,
	PM5_PreRecipeUseB	,
	PM1_PreRecipeB		,
	PM2_PreRecipeB		,
	PM3_PreRecipeB		,
	PM4_PreRecipeB		,
	PM5_PreRecipeB		,
	PM1_PreRecipeUseC	,
	PM2_PreRecipeUseC	,
	PM3_PreRecipeUseC	,
	PM4_PreRecipeUseC	,
	PM5_PreRecipeUseC	,
	PM1_PreRecipeC		,
	PM2_PreRecipeC		,
	PM3_PreRecipeC		,
	PM4_PreRecipeC		,
	PM5_PreRecipeC		,
	PM1_PreRecipeUseD	,
	PM2_PreRecipeUseD	,
	PM3_PreRecipeUseD	,
	PM4_PreRecipeUseD	,
	PM5_PreRecipeUseD	,
	PM1_PreRecipeD		,
	PM2_PreRecipeD		,
	PM3_PreRecipeD		,
	PM4_PreRecipeD		,
	PM5_PreRecipeD		,
	PM1_PreRecipeUseE	,
	PM2_PreRecipeUseE	,
	PM3_PreRecipeUseE	,
	PM4_PreRecipeUseE	,
	PM5_PreRecipeUseE	,
	PM1_PreRecipeE		,
	PM2_PreRecipeE		,
	PM3_PreRecipeE		,
	PM4_PreRecipeE		,
	PM5_PreRecipeE		,
	LotPreOption		,
	CM1_Control			,
	CM2_Control			,
	CM3_Control			,
	N2Purge_Control		,
	Robot_Control		,

} IOPointIndex;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif
