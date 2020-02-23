//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#endif