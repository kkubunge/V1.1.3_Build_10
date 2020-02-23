#ifndef Data_Type__h
#define Data_Type__h

// Enumeration Define ==============================================================================================
typedef enum { RCP_NONE, RCP_PRC, RCP_PRE, RCP_PST, RCP_IDL, RCP_CLN, RCP_PUG, RCP_LPR, RCP_LPS } RCP_TYPE;
typedef enum { NO, YES } NOYES;
typedef enum { POSITION, PRESSURE } POS_PRESSURE;
typedef enum { RCP_MODE_UNKNOWN, RCP_MODE_HEATER, RCP_MODE_MFC, RCP_MODE_VALVE, RCP_MODE_EXT } RCP_MODE;
typedef enum { TYPE_FLOAT, TYPE_INT } IOCTYPE;
typedef enum { VAL_NOUSE, VAL_OPEN, VAL_CLOSE, RF_ON, RF_OFF } VAL_STATUS;
typedef enum { INSERT_OFF, REMOVE_OFF, INSERT_ON, REMOVE_ON, REMOVE_PRESS, STEP_MOVE_PREV_OFF, STEP_MOVE_NEXT_OFF, STEP_MOVE_PREV_ON, STEP_MOVE_NEXT_ON
              ,EDIT_ON , EDIT_OFF, EDIT_PRESS, COPY_ON , COPY_OFF, COPY_PRESS } BUTTON_STATUS;
typedef enum { HT_TYPE_FLOAT, HT_TYPE_INT } HEATER_DATA_TYPE;
typedef enum { DESELECT, SELECT } SEL_STATUS;
// =================================================================================================================

typedef struct {
	int		x, y;
	char	szTitle[50];
} Draw_Title;

typedef struct pNode {
	struct pNode* PrevNode;
	int			dx;
	int			dy;
	int			dImgNo;
	int			dValNo;
	char		szValveName[20];
	struct pNode* NextNode;
} IMG_NODE;

/*
typedef struct {
	UINT	uNo;
	char	szDisplayName[50];
	double	dbSetPoint;
	double	dbTolerance;
	double	dbMin;
	double  dbMax;
	double  dbUnit;
	IOCTYPE udIocType;
} HEATER_RCP_DATA;
*/

typedef struct {
	BOOL	bUse;
	char	szMFCRealName[50];
	char	szMFCDisplayName[50];
	double	dbSetPoint;
	double	dbSetPoint_DMY;
	// double	dbDefaultTolerance;
	// double	dbDefaultTimeOut;
	double	dbMin;
	double	dbMax;
	char	szEnum[9][20];
} MFC_RCP_DATA;

typedef struct{
	double		dbTime;
	VAL_STATUS	RCP_DATA[MAX_VAL_NO];
} VAL_DATA;

typedef struct {
	int			dTotalStep;
	int			dValveStepCycleCount[MAX_VAL_STEP_NO];
	VAL_DATA	RCP_DATA[MAX_VAL_STEP_NO][MAX_VAL_TIME_STEP];
} VALVE_RCP_DATA;

typedef struct {
	char				szHtrName[40];
	BOOL				bUse;
} TEMP_RCP_DATA;

typedef struct
{
	char szName[40];
	double dbMin;
	double dbMax;
	char szOptEnum[256];
	int nDecPoint;
} ITEM_STRUCT;

#define MAX_VALVE_SEQ	4
typedef struct
{
	char szTypeName[20];
	int nEnumCount;
	char pszEnum[10][20];
	int pnValve[10][10];
	int pnRcpLock[10];
} VRCP_SEQ_STRUCT;

typedef struct
{
	int nEnumCount;
	char pszEnum[10][20];
	int pnValve[10][10];
	int pnRcpLock[10];
} MFC_VLV_SEQ_STRUCT;

typedef struct _ITEM_BTN_STRUCT
{
	int nID;
	int nX;
	int nY;
	int nXSize;
	int nYSize;
} ITEM_BTN_STRUCT;

#endif