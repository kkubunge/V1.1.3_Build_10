#ifndef LOTTYPE_H
#define LOTTYPE_H

typedef struct {
	char	JOB_NAME[MAX_STRING_SIZE+1];
	char	LOT_RECIPE[MAX_STRING_SIZE+1];
	int		IN_CASS;
	int		OUT_CASS;
	int		START_SLOT;
	int		END_SLOT;
	char	START_TIME[MAX_STRING_SIZE+1];
	char	END_TIME[MAX_STRING_SIZE+1];
	int		METHOD_COUNT;
	int		END_TYPE;
	char	USER[MAX_STRING_SIZE+1];
	long	EDIT_TIME;
	int		EDIT_TYPE;
	int		COUNT;
	int		DIRECTORY_TYPE;
	char	DIRECTORY[MAX_STRING_SIZE+1];
	long	DIRECTORY_DATE;
	int		PROGRESS;
} JOBStepTemplate_Old;

typedef struct {
	char	JOB_NAME[MAX_STRING_SIZE+1];
	char	LOT_RECIPE[MAX_STRING_SIZE+1];
	int		IN_CASS;
	int		OUT_CASS;
	int		START_SLOT;
	int		END_SLOT;
	char	RUN_TIME[MAX_STRING_SIZE+1];
	char	END_TIME[MAX_STRING_SIZE+1];
	int		METHOD_COUNT;
	char	START_TIME[MAX_STRING_SIZE+1];
	int		END_TYPE;
	char	USER[MAX_STRING_SIZE+1];
	long	EDIT_TIME;
	int		EDIT_TYPE;
	int		COUNT;
	int		DIRECTORY_TYPE;
	char	DIRECTORY[MAX_STRING_SIZE+1];
	long	DIRECTORY_DATE;
	int		PROGRESS;
	char	ELAPSED_TIME[MAX_STRING_SIZE+1];
	char	MID_NAME[MAX_STRING_SIZE+1];
	char	STATUS_SAVE; // 2003.06.09
	char	SPARE3[MAX_STRING_SIZE+1-1];
	char	SPARE4[MAX_STRING_SIZE+1];
	char	SPARE5[MAX_STRING_SIZE+1];
} JOBStepTemplate_Old2;

typedef struct {
	char	IN_CASS;
	char	OUT_CASS;
	char	START_SLOT;
	char	END_SLOT;
	int		METHOD_COUNT;
	char	EDIT_TYPE;
	char	PROGRESS;
	char	DIRECTORY_TYPE;
	long	DIRECTORY_DATE;
	char	STATUS_SAVE;
} JOBStepTemplate_Head;

typedef struct {
	char	IN_CASS;
	char	OUT_CASS;
	char	START_SLOT;
	char	END_SLOT;
	int		METHOD_COUNT;
	char	EDIT_TYPE;
	char	PROGRESS;
	char	DIRECTORY_TYPE;
	long	DIRECTORY_DATE;
	char	STATUS_SAVE;
	//
	char	JOB_NAME_size;
//	char	JOB_NAME[MAX_STRING_SIZE_50+1]; // 2009.11.20
	char	JOB_NAME[MAX_STRING_SIZE_64+1]; // 2009.11.20
	char	LOT_RECIPE_size;
	char	LOT_RECIPE[MAX_STRING_SIZE_50+1];
	char	MID_NAME_size;
	char	MID_NAME[MAX_STRING_SIZE_50+1];
	char	RUN_TIME_size;
	char	RUN_TIME[MAX_STRING_SIZE_22+1];
	char	END_TIME_size;
	char	END_TIME[MAX_STRING_SIZE_22+1];
	char	START_TIME_size;
	char	START_TIME[MAX_STRING_SIZE_22+1];
	char	ELAPSED_TIME_size;
	char	ELAPSED_TIME[MAX_STRING_SIZE_22+1];
	char	USER_size;
	char	USER[MAX_STRING_SIZE_22+1];
	char	DIRECTORY_size;
	char	DIRECTORY[MAX_STRING_SIZE_100+1];
} JOBStepTemplate;

typedef struct {
	int		LOG_SAVE_MONTH;
	int		DUAL_LOT_MODE;
} InfoTemplate_Old;

typedef struct {
	int		LOG_SAVE_MONTH;
	int		DUAL_LOT_MODE;
	int		DISPLAY_MODE;
	int		AUTO_DELETE_MODE;
	int		END_TIME_SHOW_MODE;
	//
//	int		USER_MODE_03; // 2006.02.22
	int		PRE_POST_SHOW_MODE; // 2006.02.22
	//
//	int		USER_MODE_04; // 2014.10.15
	int		DELETE_COUNT; // 2014.10.15
	//
	int		USER_MODE_05;
	int		USER_MODE_06;
	int		USER_MODE_07;
	int		USER_MODE_08;
	int		USER_MODE_09;
	int		USER_MODE_10;
} InfoTemplate;

//
// 2016.10.11
//
typedef struct {
	//
	//================================
	//
	char	SV_Opt_Button_Char1[64];
	char	SV_Opt_Button_Char2[64];
	char	SV_Opt_Ex_Char3[64];
	char	SV_Opt_Ex_Char4[64];
	char	SV_Opt_Ex_Char5[64];
	//
	//================================
	//
	int		SV_Opt_Summary_CharSize;
	//
	int		SV_Opt_Summary_BackColor;
	int		SV_Opt_Summary_LineColor;
	int		SV_Opt_Summary_BackColor_Job;
	int		SV_Opt_Summary_BackColor_TimeJob;
	int		SV_Opt_Summary_BackColor_Prcs;
	int		SV_Opt_Summary_BackColor_TimePrcs;
	int		SV_Opt_Summary_BackColor_ex0;
	int		SV_Opt_Summary_BackColor_ex1;
	int		SV_Opt_Summary_BackColor_ex2;
	int		SV_Opt_Summary_BackColor_ex3;
	int		SV_Opt_Summary_BackColor_ex4;
	int		SV_Opt_Summary_BackColor_ex5;
	int		SV_Opt_Summary_BackColor_ex6;
	int		SV_Opt_Summary_BackColor_ex7;
	int		SV_Opt_Summary_BackColor_ex8;
	int		SV_Opt_Summary_BackColor_ex9;
	//
	int		SV_Opt_Summary_ColumnSize_NameJob;
	int		SV_Opt_Summary_ColumnSize_DataJob;
	int		SV_Opt_Summary_ColumnSize_NamePrcs;
	int		SV_Opt_Summary_ColumnSize_DataPrcs;
	int		SV_Opt_Summary_ColumnSize_ex0;
	int		SV_Opt_Summary_ColumnSize_ex1;
	int		SV_Opt_Summary_ColumnSize_ex2;
	int		SV_Opt_Summary_ColumnSize_ex3;
	int		SV_Opt_Summary_ColumnSize_ex4;
	int		SV_Opt_Summary_ColumnSize_ex5;
	int		SV_Opt_Summary_ColumnSize_ex6;
	int		SV_Opt_Summary_ColumnSize_ex7;
	int		SV_Opt_Summary_ColumnSize_ex8;
	int		SV_Opt_Summary_ColumnSize_ex9;
	//
	int		SV_Opt_Summary_Align_NameJob;
	int		SV_Opt_Summary_Align_DataJob;
	int		SV_Opt_Summary_Align_NamePrcs;
	int		SV_Opt_Summary_Align_DataPrcs;
	int		SV_Opt_Summary_Align_ex0;
	int		SV_Opt_Summary_Align_ex1;
	int		SV_Opt_Summary_Align_ex2;
	int		SV_Opt_Summary_Align_ex3;
	int		SV_Opt_Summary_Align_ex4;
	int		SV_Opt_Summary_Align_ex5;
	int		SV_Opt_Summary_Align_ex6;
	int		SV_Opt_Summary_Align_ex7;
	int		SV_Opt_Summary_Align_ex8;
	int		SV_Opt_Summary_Align_ex9;
	//
	int SV_Opt_Summary_ex0;
	int SV_Opt_Summary_ex1;
	int SV_Opt_Summary_ex2;
	int SV_Opt_Summary_ex3;
	int SV_Opt_Summary_ex4;
	int SV_Opt_Summary_ex5;
	int SV_Opt_Summary_ex6;
	int SV_Opt_Summary_ex7;
	int SV_Opt_Summary_ex8;
	int SV_Opt_Summary_ex9;
	//
	//================================
	//
	int SV_Opt_StepItem_CharSize;
	//
	int SV_Opt_Step_BackColor;
	int SV_Opt_Step_LineColor;
	int SV_Opt_Step_BackColor_TimeTitle;
	int SV_Opt_Step_BackColor_TimeData;
	int SV_Opt_Step_BackColor_StepTitle;
	int SV_Opt_Step_BackColor_StepData;
	int SV_Opt_Step_BackColor_StepPrePost;
	int SV_Opt_Step_BackColor_ex0;
	int SV_Opt_Step_BackColor_ex1;
	int SV_Opt_Step_BackColor_ex2;
	int SV_Opt_Step_BackColor_ex3;
	int SV_Opt_Step_BackColor_ex4;
	int SV_Opt_Step_BackColor_ex5;
	int SV_Opt_Step_BackColor_ex6;
	int SV_Opt_Step_BackColor_ex7;
	int SV_Opt_Step_BackColor_ex8;
	int SV_Opt_Step_BackColor_ex9;
	//
	int SV_Opt_Step_ColumnSize_Step;
	int SV_Opt_Step_ColumnSize_StartTime;
	int SV_Opt_Step_ColumnSize_EndTime;
	int SV_Opt_Step_ColumnSize_ActualTime;
	int SV_Opt_Step_ColumnSize_StableTime;
	int SV_Opt_Step_ColumnSize_StepTime;
	int SV_Opt_Step_ColumnSize_SetTime;
	int SV_Opt_Step_ColumnSize_Result;
	int SV_Opt_Step_ColumnSize_ex0;
	int SV_Opt_Step_ColumnSize_ex1;
	int SV_Opt_Step_ColumnSize_ex2;
	int SV_Opt_Step_ColumnSize_ex3;
	int SV_Opt_Step_ColumnSize_ex4;
	int SV_Opt_Step_ColumnSize_ex5;
	int SV_Opt_Step_ColumnSize_ex6;
	int SV_Opt_Step_ColumnSize_ex7;
	int SV_Opt_Step_ColumnSize_ex8;
	int SV_Opt_Step_ColumnSize_ex9;

	int SV_Opt_Step_Align_TimeTitle;
	int SV_Opt_Step_Align_TimeData;
	int SV_Opt_Step_Align_Step;
	int SV_Opt_Step_Align_StartTime;
	int SV_Opt_Step_Align_EndTime;
	int SV_Opt_Step_Align_ActualTime;
	int SV_Opt_Step_Align_StableTime;
	int SV_Opt_Step_Align_StepTime;
	int SV_Opt_Step_Align_SetTime;
	int SV_Opt_Step_Align_Result;
	int SV_Opt_Step_Align_ex0;
	int SV_Opt_Step_Align_ex1;
	int SV_Opt_Step_Align_ex2;
	int SV_Opt_Step_Align_ex3;
	int SV_Opt_Step_Align_ex4;
	int SV_Opt_Step_Align_ex5;
	int SV_Opt_Step_Align_ex6;
	int SV_Opt_Step_Align_ex7;
	int SV_Opt_Step_Align_ex8;
	int SV_Opt_Step_Align_ex9;
	//
	int SV_Opt_Step_View_HidePrePost;
	int SV_Opt_Step_View_HideStartTime;
	int SV_Opt_Step_View_HideEndTime;
	int SV_Opt_Step_View_HideActualTime;
	int SV_Opt_Step_View_HideStableTime;
	int SV_Opt_Step_View_HideStepTime;
	int SV_Opt_Step_View_HideSetTime;
	int SV_Opt_Step_View_HideResult;
	int SV_Opt_Step_View_ex0;
	int SV_Opt_Step_View_ex1;
	int SV_Opt_Step_View_ex2;
	int SV_Opt_Step_View_ex3;
	int SV_Opt_Step_View_ex4;
	int SV_Opt_Step_View_ex5;
	int SV_Opt_Step_View_ex6;
	int SV_Opt_Step_View_ex7;
	int SV_Opt_Step_View_ex8;
	int SV_Opt_Step_View_ex9;
	//
	int SV_Opt_Step_ex0;
	int SV_Opt_Step_ex1;
	int SV_Opt_Step_ex2;
	int SV_Opt_Step_ex3;
	int SV_Opt_Step_ex4;
	int SV_Opt_Step_ex5;
	int SV_Opt_Step_ex6;
	int SV_Opt_Step_ex7;
	int SV_Opt_Step_ex8;
	int SV_Opt_Step_ex9;
	//
	//================================
	//
	int SV_Opt_Item_BackColor;
	int SV_Opt_Item_LineColor;
	//
	int SV_Opt_Item_ColorMode; // Column/Row
	//
	int SV_Opt_Item_BackColor_Title;
	int SV_Opt_Item_BackColor_No;
	int SV_Opt_Item_BackColor_Name;
	int SV_Opt_Item_BackColor_Count;
	int SV_Opt_Item_BackColor_Target;
	int SV_Opt_Item_BackColor_Start;
	int SV_Opt_Item_BackColor_End;
	int SV_Opt_Item_BackColor_Min;
	int SV_Opt_Item_BackColor_Max;
	int SV_Opt_Item_BackColor_Avr;
	int SV_Opt_Item_BackColor_StdDev;
	int SV_Opt_Item_BackColor_StableTime;
	int SV_Opt_Item_BackColor_StepTime;
	int SV_Opt_Item_BackColor_Result;
	int SV_Opt_Item_BackColor_Row1;
	int SV_Opt_Item_BackColor_Row2;
	int SV_Opt_Item_BackColor_ex0;
	int SV_Opt_Item_BackColor_ex1;
	int SV_Opt_Item_BackColor_ex2;
	int SV_Opt_Item_BackColor_ex3;
	int SV_Opt_Item_BackColor_ex4;
	int SV_Opt_Item_BackColor_ex5;
	int SV_Opt_Item_BackColor_ex6;
	int SV_Opt_Item_BackColor_ex7;
	int SV_Opt_Item_BackColor_ex8;
	int SV_Opt_Item_BackColor_ex9;
	//
	int SV_Opt_Item_ColumnSize_No;
	int SV_Opt_Item_ColumnSize_Name;
	int SV_Opt_Item_ColumnSize_Count;
	int SV_Opt_Item_ColumnSize_Target;
	int SV_Opt_Item_ColumnSize_Start;
	int SV_Opt_Item_ColumnSize_End;
	int SV_Opt_Item_ColumnSize_Min;
	int SV_Opt_Item_ColumnSize_Max;
	int SV_Opt_Item_ColumnSize_Avr;
	int SV_Opt_Item_ColumnSize_StdDev;
	int SV_Opt_Item_ColumnSize_StblTime;
	int SV_Opt_Item_ColumnSize_StepTime;
	int SV_Opt_Item_ColumnSize_Result;
	int SV_Opt_Item_RowSize_Row1;
	int SV_Opt_Item_RowSize_Row2;
	int SV_Opt_Item_ColumnSize_ex0;
	int SV_Opt_Item_ColumnSize_ex1;
	int SV_Opt_Item_ColumnSize_ex2;
	int SV_Opt_Item_ColumnSize_ex3;
	int SV_Opt_Item_ColumnSize_ex4;
	int SV_Opt_Item_ColumnSize_ex5;
	int SV_Opt_Item_ColumnSize_ex6;
	int SV_Opt_Item_ColumnSize_ex7;
	int SV_Opt_Item_ColumnSize_ex8;
	int SV_Opt_Item_ColumnSize_ex9;
	//
	int SV_Opt_Item_Align_No;
	int SV_Opt_Item_Align_Name;
	int SV_Opt_Item_Align_Count;
	int SV_Opt_Item_Align_Target;
	int SV_Opt_Item_Align_Start;
	int SV_Opt_Item_Align_End;
	int SV_Opt_Item_Align_Min;
	int SV_Opt_Item_Align_Max;
	int SV_Opt_Item_Align_Avr;
	int SV_Opt_Item_Align_StdDev;
	int SV_Opt_Item_Align_StblTime;
	int SV_Opt_Item_Align_StepTime;
	int SV_Opt_Item_Align_Result;
	int SV_Opt_Item_Align_ex0;
	int SV_Opt_Item_Align_ex1;
	int SV_Opt_Item_Align_ex2;
	int SV_Opt_Item_Align_ex3;
	int SV_Opt_Item_Align_ex4;
	int SV_Opt_Item_Align_ex5;
	int SV_Opt_Item_Align_ex6;
	int SV_Opt_Item_Align_ex7;
	int SV_Opt_Item_Align_ex8;
	int SV_Opt_Item_Align_ex9;
	//
	int SV_Opt_Item_View_HideCount;
	int SV_Opt_Item_View_HideTarget;
	int SV_Opt_Item_View_HideStart;
	int SV_Opt_Item_View_HideEnd;
	int SV_Opt_Item_View_HideMin;
	int SV_Opt_Item_View_HideMax;
	int SV_Opt_Item_View_HideAvr;
	int SV_Opt_Item_View_HideStdDev;
	int SV_Opt_Item_View_HideStblTime;
	int SV_Opt_Item_View_HideStepTime;
	int SV_Opt_Item_View_HideResult;
	int SV_Opt_Item_View_ex0;
	int SV_Opt_Item_View_ex1;
	int SV_Opt_Item_View_ex2;
	int SV_Opt_Item_View_ex3;
	int SV_Opt_Item_View_ex4;
	int SV_Opt_Item_View_ex5;
	int SV_Opt_Item_View_ex6;
	int SV_Opt_Item_View_ex7;
	int SV_Opt_Item_View_ex8;
	int SV_Opt_Item_View_ex9;

	int SV_Opt_Item_Cal_ExceptStable;
	//
	int SV_Opt_Item_Time_Calculation;	// LOG/Cal1,LOG/Cal2,LOG,Cal1,Cal2,X
	//
	int SV_Opt_Item_ex0;
	int SV_Opt_Item_ex1;
	int SV_Opt_Item_ex2;
	int SV_Opt_Item_ex3;
	int SV_Opt_Item_ex4;
	int SV_Opt_Item_ex5;
	int SV_Opt_Item_ex6;
	int SV_Opt_Item_ex7;
	//
	//================================
	//
	int SV_Opt_TimeDisplay_Run;		// def h:m:s.ms h:m:s
	int SV_Opt_TimeDisplay_Cal;		// def xx:xx:xx xx:xx:xx.x xx:xx:xx.xx xx:xx:xx.xxx xx xx.x xx.xx xx.xxx
	int SV_Opt_TimeDisplay_Unknown;	// Default 0
	int SV_Opt_DecimalPoint;		// Default 0 , 1 , 2 , 3 , 4
	//
	//================================
	//
	int SV_Opt_ex0;
	int SV_Opt_ex1;
	int SV_Opt_ex2;
	int SV_Opt_ex3;
	int SV_Opt_ex4;
	int SV_Opt_ex5;
	int SV_Opt_ex6;
	int SV_Opt_ex7;
	int SV_Opt_ex8;
	int SV_Opt_ex9;
	//
	int SV_Opt_ex10;
	int SV_Opt_ex11;
	int SV_Opt_ex12;
	int SV_Opt_ex13;
	int SV_Opt_ex14;
	int SV_Opt_ex15;
	int SV_Opt_ex16;
	int SV_Opt_ex17;
	int SV_Opt_ex18;
	int SV_Opt_ex19;
	//
	int SV_Opt_ex20;
	int SV_Opt_ex21;
	int SV_Opt_ex22;
	int SV_Opt_ex23;
	int SV_Opt_ex24;
	int SV_Opt_ex25;
	int SV_Opt_ex26;
	int SV_Opt_ex27;
	int SV_Opt_ex28;
	int SV_Opt_ex29;
	//
	int SV_Opt_ex30;
	int SV_Opt_ex31;
	int SV_Opt_ex32;
	int SV_Opt_ex33;
	int SV_Opt_ex34;
	int SV_Opt_ex35;
	int SV_Opt_ex36;
	int SV_Opt_ex37;
	int SV_Opt_ex38;
	int SV_Opt_ex39;
	//
	int SV_Opt_ex40;
	int SV_Opt_ex41;
	int SV_Opt_ex42;
	int SV_Opt_ex43;
	int SV_Opt_ex44;
	int SV_Opt_ex45;
	int SV_Opt_ex46;
	int SV_Opt_ex47;
	int SV_Opt_ex48;
	int SV_Opt_ex49;
	//
	int SV_Opt_ex50;
	int SV_Opt_ex51;
	int SV_Opt_ex52;
	int SV_Opt_ex53;
	int SV_Opt_ex54;
	int SV_Opt_ex55;
	int SV_Opt_ex56;
	int SV_Opt_ex57;
	int SV_Opt_ex58;
	int SV_Opt_ex59;
	//
} StepViewTemplate;


extern StepViewTemplate			STEPVIEW_DATA; // 2016.10.11
extern BOOL						STEPVIEW_SAVE; // 2016.10.11


#endif
