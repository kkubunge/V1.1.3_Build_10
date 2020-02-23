#ifndef LOTTYPE_H
#define LOTTYPE_H

typedef struct {
	char	F_RECIPE_SWVERSION[65]; // 2007.10.19
	char	F_RECIPE_ETCDATA[256]; // 2007.10.19
	//
	char	F_RECIPE_CREATE_USER[65]; // 2007.10.19
	char	F_RECIPE_CREATE_LEVL[65]; // 2007.10.19
	char	F_RECIPE_CREATE_DATE[65]; // 2007.10.19
	//
	char	F_RECIPE_LAST_UPDATE_USER[65]; // 2007.09.18
	char	F_RECIPE_LAST_UPDATE_LEVL[65]; // 2007.09.18
	char	F_RECIPE_LAST_UPDATE_DATE[65]; // 2007.09.18
	char	F_RECIPE_LAST2_UPDATE_USER[65]; // 2007.09.18
	char	F_RECIPE_LAST2_UPDATE_LEVL[65]; // 2007.09.18
	char	F_RECIPE_LAST2_UPDATE_DATE[65]; // 2007.09.18
	//--------------
	int		HANDLING_SIDE;
	int		MODULE[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH];
	char	MODULE_PR_RECIPE[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH][MAX_STRING_SIZE+1];
	char	MODULE_IN_RECIPE[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH][MAX_STRING_SIZE+1];
	char	MODULE_OUT_RECIPE[MAX_CLUSTER_DEPTH][MAX_PM_CHAMBER_DEPTH][MAX_STRING_SIZE+1];
	int		EXTRA_STATION;
	int		EXTRA_TIME;
} CLUSTERStepTemplate;

#endif