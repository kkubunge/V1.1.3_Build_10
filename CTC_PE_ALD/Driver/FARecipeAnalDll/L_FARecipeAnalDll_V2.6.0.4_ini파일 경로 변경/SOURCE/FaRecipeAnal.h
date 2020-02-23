#ifndef FARECIPEANAL__H
#define FARECIPEANAL__H

#include <stdio.h>

#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <windows.h>
#include <windowsx.h>

#include <kutlfile.h>
#include <kutletc.h>
#include <kutlanal.h>
#include <kutlstr.h>
#include <kutltime.h>

//=========== Please Modify with Correct Version
#define MAX_CHAMBER_DEPTH		30
#define CASSETTE_MAX  			3
//=========== Don't Modify Begin
#define MAX_CASSETTE_SLOT		50//30
#define MAX_CLUSTER_DEPTH		50

typedef struct {
	char	LOT_ID[255+1];
	char	CLUSTER_RECIPE[MAX_CASSETTE_SLOT][64+1];
	char	CLUSTER_RECIPE2[MAX_CASSETTE_SLOT][64+1];
	int		CLUSTER_USING[MAX_CASSETTE_SLOT];
	int		CLUSTER_EXTRA_STATION;
	int		CLUSTER_EXTRA_TIME;
} LOTStepTemplate;

typedef struct {
	char	CLUSTER_RECIPE[64+1];
	int		CLUSTER_USING;
} LOTStepTemplate2;

typedef struct {
	int		MODULE[MAX_CLUSTER_DEPTH][MAX_CHAMBER_DEPTH];
	char	MODULE_PR_RECIPE[MAX_CLUSTER_DEPTH][MAX_CHAMBER_DEPTH][64+1];
	char	MODULE_IN_RECIPE[MAX_CLUSTER_DEPTH][MAX_CHAMBER_DEPTH][64+1];
	char	MODULE_OUT_RECIPE[MAX_CLUSTER_DEPTH][MAX_CHAMBER_DEPTH][64+1];
	int		EXTRA_STATION;
	int		EXTRA_TIME;
	int		HANDLING_SIDE;
} CLUSTERStepTemplate;

typedef struct {
	int		CASSETTE_IN_ORDER[MAX_CASSETTE_SLOT];
	int		CASSETTE_OUT_PATH[MAX_CASSETTE_SLOT];
	int		CASSETTE_OUT_SLOT[MAX_CASSETTE_SLOT];
} OUTCASSETTETemplate;

typedef struct {
	int		CASSETTE1;
	int		CASSETTE2;
	int		CASSETTE1_STATUS[MAX_CASSETTE_SLOT];
	int		CASSETTE2_STATUS[MAX_CASSETTE_SLOT];
} CASSETTEVerTemplate;

typedef struct {
	char    CM_ROUTE[CASSETTE_MAX][256];
	char	CM_TRAIL_INFO[CASSETTE_MAX][256];
	char	ROUTE_TRAIL_INFO[CASSETTE_MAX][256];
} RouteInfoTemplate;

typedef struct {
	int		nLotPreType; // LotPre A - E
	int     nPMLotPreEnableOpt[8];
	char	szPMLotPreName[8][256];
} LotPreInfo;

typedef struct {
	int     nPMOnlineStatus[8];
	int     nPMLotPreType[8]; // READY7,READY3.....
	int     nSpecialData[8];  // Lot Recipe LotPre Number
} PMOnlineStsInfo;

typedef struct {
	int		nLotPreType; // LotPre A - E
	char	szPMLotPreFullRcpName[8][256];
} LotPreInfoToScheduler;


typedef enum {
	SYS_AVAILABLE ,
	SYS_RUNNING ,
	SYS_ABORTED ,
	SYS_SUCCESS ,
	SYS_ERROR ,
	//
	SYS_WAITING
} Module_Status , SYS_Status;

enum{
	PORT1,
	PORT2,
	PORT3 };
	
enum{ 
	  PREVIOUS_BEFORE,
      PREVIOUS,
	  CURRENT };

enum {	TYPE_ITEM_USE ,
		TYPE_ITEM_NOTUSE };

enum {	TYPE_LOT_FILE_CLUSTER_FILE ,
		TYPE_LOT_FILE_CLUSTER_DLL ,
		TYPE_LOT_FILE_CLUSTER_ALL ,//
		TYPE_LOT_DLL_CLUSTER_FILE ,
		TYPE_LOT_DLL_CLUSTER_DLL ,
		TYPE_LOT_DLL_CLUSTER_ALL ,
		TYPE_LOT_ALL_CLUSTER_FILE ,
		TYPE_LOT_ALL_CLUSTER_DLL ,
		TYPE_LOT_ALL_CLUSTER_ALL };

//=========== Don't Modify End

//=========== For use Cims API Index

typedef void      (*TypeREM_COMMON_PRINTF_API) ( BOOL , char * , char * , LPSTR , ... );
//
typedef int       (*TypeREM_FIND_FROM_STRING_API) ( int , char * );
typedef int       (*TypeREM_dWRITE_FUNCTION_API) ( int , char * );
typedef int       (*TypeREM_dREAD_FUNCTION_API) ( int );
typedef void      (*TypeREM_dRUN_FUNCTION_ABORT_API) ( int );
typedef void      (*TypeREM_dWRITE_FUNCTION_EVENT_API) ( int , char * );
typedef int       (*TypeREM_dREAD_FUNCTION_EVENT_API) ( int );

typedef void      (*TypeREM_dREAD_UPLOAD_MESSAGE_API) ( int , char * );
typedef BOOL      (*TypeREM_dWRITE_UPLOAD_MESSAGE_API) ( char * );

typedef void      (*TypeREM_dWRITE_DIGITAL_API) ( int , int , int * );
typedef int       (*TypeREM_dREAD_DIGITAL_API) ( int , int * );
typedef void      (*TypeREM_dWRITE_ANALOG_API) ( int , double , int * );
typedef double    (*TypeREM_dREAD_ANALOG_API) ( int , int * );
typedef void      (*TypeREM_dWRITE_STRING_API) ( int , char * , int * );
typedef void      (*TypeREM_dREAD_STRING_API) ( int , char * , int * );

#endif
