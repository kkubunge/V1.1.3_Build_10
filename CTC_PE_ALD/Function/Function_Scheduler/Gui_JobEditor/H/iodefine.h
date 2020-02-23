#ifndef IODEFINE_H
#define IODEFINE_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

// Don't Modify
#ifdef	X_200_SIZE
	#define MAX_SIDE				10
	#define	MAX_PM_CHAMBER_DEPTH	100
#else
	#ifdef	X_160_SIZE
		#define MAX_SIDE				10
		#define	MAX_PM_CHAMBER_DEPTH	100 // 2012.02.02
	#else
		#define MAX_SIDE				4
		#define	MAX_PM_CHAMBER_DEPTH	60 // 2012.02.02
	#endif
#endif

//

//#define	MAX_PM_CHAMBER_DEPTH	60 // 2012.02.02
#define	MAX_BM_CHAMBER_DEPTH	15
//#define	MAX_CHAMBER				MAX_PM_CHAMBER_DEPTH + MAX_BM_CHAMBER_DEPTH + 3 // 2015.12.03
#define	MAX_CHAMBER				MAX_SIDE+MAX_PM_CHAMBER_DEPTH+MAX_BM_CHAMBER_DEPTH+3 // 2015.12.03

#define	METHOD_LOG_FILE	"WaferData.log"
#define	STATUS_LOG_FILE	"Status.log"
#define	METHOD_LOG_FAIL	"WaferData.Fail"	// 2015.11.12

enum {
	CM1 = 1 ,
	PM1 = ( CM1 + MAX_SIDE ) ,
	BM1 = ( PM1 + MAX_PM_CHAMBER_DEPTH ) ,
};
//------------------------------------------------------------------------------------------
#define MAX_STEP_COUNT			99		// 2005.02.18
//------------------------------------------------------------------------------------------
//#define	MAX_JOB_COUNT		4096	// 2004.01.14

#ifdef	DATA_EXPAND1
	#define	MAX_JOB_COUNT		81920	// 2012.04.10
#else
	#define	MAX_JOB_COUNT		24576	// 2004.01.14
#endif

//------------------------------------------------------------------------------------------
#define	MAX_STRING_SIZE		255
//------------------------------------------------------------------------------------------
#define	MAX_STRING_SIZE_50		50
#define	MAX_STRING_SIZE_22		22
#define	MAX_STRING_SIZE_100		100
#define	MAX_STRING_SIZE_64		64		// 2009.11.20
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//#define MAX_CURRENT_WAFER_VIEW	256 // 2017.03.16
#define MAX_CURRENT_WAFER_VIEW	1024 // 2017.03.16

#define MAX_CURRENT_WAFER_DEPTH	50
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

enum { JOB_READY , JOB_WAITING , JOB_RUNNING , JOB_ABORTING , JOB_ENDING };

enum { JE_SUCCESS , JE_ABORTED , JE_INTERRUPT , JE_FAIL };

//enum { ET_EDITOR , ET_HOST , ET_MAINT }; // 2010.12.10
//enum { ET_EDITOR , ET_APPEND , ET_MAINT }; // 2010.12.10 // 2015.11.12
enum { ET_EDITOR_N , ET_APPEND_N , ET_MAINT_N , ET_EDITOR_UABORT , ET_APPEND_UABORT , ET_MAINT_UABORT }; // 2010.12.10 // 2015.11.12
enum { ET_EDITOR_UPOST_10_N , ET_EDITOR_UPOST_10_ABORT , ET_EDITOR_UPOST_10_SUCCESS }; // 2016.03.29
enum { ET_EDITOR_UPRE_100_N , ET_EDITOR_UPRE_100_ABORT , ET_EDITOR_UPRE_100_SUCCESS }; // 2016.03.29
//
enum { ET_LOT_ICON_NORMAL , ET_LOT_ICON_PRE_NORMAL , ET_LOT_ICON_POST_NORMAL , ET_LOT_ICON_PREPOST_NORMAL , ET_LOT_ICON_FAIL , ET_LOT_ICON_PRE_FAIL , ET_LOT_ICON_POST_FAIL , ET_LOT_ICON_PREPOST_FAIL }; // 2016.03.29
//
typedef struct {
	int		index;
	char	slot[64];
	char	chstr[64];
	int		mode;
	char 	str[256];
} ViewStepTemplate;


// IO List
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { CTC_IDLE , CTC_RUNNING , CTC_PAUSING , CTC_PAUSED , CTC_ABORTING , CTC_DISABLE , CTC_WAITING };

//------------------------------------------------------------------------------------------
//
enum { TCH_PROCESS , TCH_PROCESS_X , TCH_CM_IN , TCH_PM_PLACE , TCH_PM_PICK , TCH_BM_PLACE , TCH_BM_PICK , TCH_CM_OUT , TCH_TM_SWAP , TCH_TM_MOVE , TCH_FM_MOVE , TCH_FM_PICK , TCH_FM_PLACE , TCH_BM_FMSIDE , TCH_BM_TMSIDE , TCH_ALIGN , TCH_COOL , TCH_REJECT };
//
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void READ_FUNCTION_For_RESET();
BOOL READ_FUNCTION_For_STOP_CHECK( int Side , int *Res );
void READ_FUNCTION_For_STOP_CHECK_DATA( int Res , char *END_TIME , char *ELAPSED_TIME , int *METHOD_COUNT );
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//enum { BUTTON_TREE , BUTTON_DATA , BUTTON_DATA_S , BUTTON_DATA_Sstr , BUTTON_DATA_S2 , BUTTON_DATA_S2str , BUTTON_ADD , BUTTON_DELETE , BUTTON_START , BUTTON_ABORT , BUTTON_END , BUTTON_CANCEL , BUTTON_LOG , BUTTON_FIND , BUTTON_BACK , BUTTON_UP , BUTTON_SUP , BUTTON_SDOWN , BUTTON_DOWN , BUTTON_RIGHT , BUTTON_LEFT }; // 2005.04.08
//enum { BUTTON_TREE , BUTTON_DATA , BUTTON_DATA_S , BUTTON_DATA_Sstr , BUTTON_DATA_S2 , BUTTON_DATA_S2str , BUTTON_ADD , BUTTON_DELETE , BUTTON_START , BUTTON_ABORT , BUTTON_END , BUTTON_CANCEL , BUTTON_LOG , BUTTON_FIND , BUTTON_BACK , BUTTON_UP , BUTTON_SUP , BUTTON_SDOWN , BUTTON_DOWN , BUTTON_RIGHT , BUTTON_LEFT, BUTTON_CTRL1, BUTTON_CTRL2, BUTTON_CTRL3, BUTTON_CTRL4, BUTTON_CTRL5, BUTTON_CTRL6, BUTTON_CTRL7 ,  BUTTON_CTRL8 }; // mok 2012.05.14 // 2012.06.16
//enum { BUTTON_TREE , BUTTON_DATA , BUTTON_DATA_S , BUTTON_DATA_Sstr , BUTTON_DATA_S2 , BUTTON_DATA_S2str , BUTTON_ADD , BUTTON_DELETE , BUTTON_START , BUTTON_ABORT , BUTTON_END , BUTTON_CANCEL , BUTTON_LOG , BUTTON_FIND , BUTTON_BACK , BUTTON_UP , BUTTON_SUP , BUTTON_SDOWN , BUTTON_DOWN , BUTTON_RIGHT , BUTTON_LEFT, BUTTON_CTRL1, BUTTON_CTRL2, BUTTON_CTRL3, BUTTON_CTRL4, BUTTON_CTRL5, BUTTON_CTRL6, BUTTON_CTRL7 ,  BUTTON_CTRL8 , BUTTON_CTRL_SAVE }; // mok 2012.05.14 // 2012.06.16 // 2014.02.10
//enum { BUTTON_TREE , BUTTON_DATA , BUTTON_DATA_S , BUTTON_DATA_Sstr , BUTTON_DATA_S2 , BUTTON_DATA_S2str , BUTTON_DATA_SPstr , BUTTON_ADD , BUTTON_DELETE , BUTTON_START , BUTTON_ABORT , BUTTON_END , BUTTON_CANCEL , BUTTON_LOG , BUTTON_FIND , BUTTON_BACK , BUTTON_UP , BUTTON_SUP , BUTTON_SDOWN , BUTTON_DOWN , BUTTON_RIGHT , BUTTON_LEFT, BUTTON_CTRL1, BUTTON_CTRL2, BUTTON_CTRL3, BUTTON_CTRL4, BUTTON_CTRL5, BUTTON_CTRL6, BUTTON_CTRL7 ,  BUTTON_CTRL8 , BUTTON_CTRL_SAVE }; // mok 2012.05.14 // 2012.06.16 // 2014.02.10 // 2016.08.17
enum { BUTTON_TREE , BUTTON_DATA , BUTTON_DATA_S , BUTTON_DATA_Sstr , BUTTON_DATA_S2 , BUTTON_DATA_S2str , BUTTON_DATA_SPstr , BUTTON_ADD , BUTTON_DELETE , BUTTON_START , BUTTON_ABORT , BUTTON_END , BUTTON_CANCEL , BUTTON_LOG , BUTTON_FIND , BUTTON_BACK , BUTTON_UP , BUTTON_SUP , BUTTON_SDOWN , BUTTON_DOWN , BUTTON_RIGHT , BUTTON_LEFT, BUTTON_CTRL1, BUTTON_CTRL2, BUTTON_CTRL3, BUTTON_CTRL4, BUTTON_CTRL5, BUTTON_CTRL6, BUTTON_CTRL7 ,  BUTTON_CTRL8 , BUTTON_CTRL_SAVE , BUTTON_SPM , BUTTON_DATA_SPM , BUTTON_DATA_SPMS }; // mok 2012.05.14 // 2012.06.16 // 2014.02.10 // 2016.08.17

//#define MAX_BUTTON_COUNT	30 // 2012.06.26
//#define MAX_BUTTON_COUNT	31 // 2012.06.26 // 2014.02.10
//#define MAX_BUTTON_COUNT	32 // 2012.06.26 // 2014.02.10 // 2016.08.17
#define MAX_BUTTON_COUNT	35 // 2012.06.26 // 2014.02.10 // 2016.08.17

#define MAX_BUTTON_Y_SPM_MORE	130 // 2016.08.17

#define MAX_MULTI_CHECK_JOB		32 // 2017.03.17

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
enum { 
	CTR_PROCESS_DATA_1_WAFER ,
	CTR_PROCESS_PLOT_1_WAFER ,
	CTR_RECIPE_DATA_1_WAFER ,
	CTR_LOT_SUMMARY_DATA ,
	CTR_LOT_SUMMARY_MODULE ,
	CTR_PROCESS_DATA_MODULE ,
	CTR_FLOW_HISTORY 
};

extern int LISTVIEW_HEIGHT_UPDATE;
extern int LISTVIEW_HEIGHT;
extern double LISTVIEW_HEIGHT_RATIO;



#endif
