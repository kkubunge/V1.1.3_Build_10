//-----------------------------------------------------------------------------------
// Common OBJ_GEM_OperatorStartInterface_Manager
//
// File Name : AppRecipe.h
//
// (C) 2001-2007 IPS Ltd.
//-----------------------------------------------------------------------------------
#ifndef APPRECIPE_H
#define APPRECIPE_H
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
#define MAX_CLUSTERDATA_USE_CHAMBER		12
#define MAX_CASTSLOT_COUNT				30
#define MAX_CLUSTERDATA_ITEM_COUNT		50
#define PPARMTX_LENGTH					64

typedef struct {
	char	RECIPE_CREATE_USER[65]; // 2007.10.16
	char	RECIPE_CREATE_LEVL[65]; // 2007.10.16
	char	RECIPE_CREATE_DATE[65]; // 2007.10.16
	//
	char	RECIPE_LAST_UPDATE_USER[65]; // 2007.09.18
	char	RECIPE_LAST_UPDATE_LEVL[65]; // 2007.09.18
	char	RECIPE_LAST_UPDATE_DATE[65]; // 2007.09.18
	char	RECIPE_LAST2_UPDATE_USER[65]; // 2007.09.18
	char	RECIPE_LAST2_UPDATE_LEVL[65]; // 2007.09.18
	char	RECIPE_LAST2_UPDATE_DATE[65]; // 2007.09.18
	//
	char	RECIPE_SWVERSION[65]; // 2007.10.16
	char	RECIPE_ETC[256]; // 2007.10.16
	//--------------
	char	LOT_ID[PPARMTX_LENGTH];
	char	CLUSTER_RECIPE[MAX_CASTSLOT_COUNT][PPARMTX_LENGTH];
	char	CLUSTER_RECIPE2[MAX_CASTSLOT_COUNT][PPARMTX_LENGTH];
	int		CLUSTER_USING[MAX_CASTSLOT_COUNT];
	int		CLUSTER_DUMMY_USING;
	char	CLUSTER_DUMMY_RECIPE[PPARMTX_LENGTH+1];
} LOTStepTemplate;
//----------------------------------------------------------------------------------------
typedef struct {
	int		HANDLING_SIDE;
	int		MODULE[MAX_CLUSTERDATA_ITEM_COUNT][MAX_CLUSTERDATA_USE_CHAMBER];
	char	MODULE_PR_RECIPE[MAX_CLUSTERDATA_ITEM_COUNT][MAX_CLUSTERDATA_USE_CHAMBER][PPARMTX_LENGTH+1];
	char	MODULE_IN_RECIPE[MAX_CLUSTERDATA_ITEM_COUNT][MAX_CLUSTERDATA_USE_CHAMBER][PPARMTX_LENGTH+1];
	char	MODULE_OUT_RECIPE[MAX_CLUSTERDATA_ITEM_COUNT][MAX_CLUSTERDATA_USE_CHAMBER][PPARMTX_LENGTH+1];
	int		EXTRA_STATION;
	int		EXTRA_TIME;
} CLUSTERStepTemplate;
//----------------------------------------------------------------------------------------
typedef struct {
	int		PJ_COUNT;
	char	PJ_NAME[MAX_CASTSLOT_COUNT][PPARMTX_LENGTH];
	char	PJ_CLUSTER_RECIPE[MAX_CASTSLOT_COUNT][PPARMTX_LENGTH];
	char	PJ_ENDSLOT[MAX_CASTSLOT_COUNT][PPARMTX_LENGTH];
} PJCreateInfo;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif