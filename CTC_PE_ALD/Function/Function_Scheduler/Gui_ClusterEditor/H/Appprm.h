#ifndef APPPRM_H
#define APPPRM_H

void Set_Prm_CHAMBER_SIZE( int size );
int  Get_Prm_CHAMBER_SIZE();

void Set_Prm_DRAW_SIZE( int size );
int  Get_Prm_DRAW_SIZE();

void Set_Prm_FILE_ID( char *data );
char *Get_Prm_FILE_ID();
//
void Set_Prm_CLUSTER_RECIPE_ID( char *data );
char *Get_Prm_CLUSTER_RECIPE_ID();
//
void Set_Prm_PROCESS_RECIPE_ID( int i , char *data );
char *Get_Prm_PROCESS_RECIPE_ID( int i );
//
void Set_Prm_MAIN_RECIPE_PATH( char *data );
char *Get_Prm_MAIN_RECIPE_PATH();
//
void Set_Prm_MAIN_RECIPE_PATH( char *data );
char *Get_Prm_MAIN_RECIPE_PATH();
//
void Set_Prm_GROUP_RECIPE_PATH( char *data );
char *Get_Prm_GROUP_RECIPE_PATH();

void Set_Prm_LOT_RECIPE_PATH( char *data );
char *Get_Prm_LOT_RECIPE_PATH();

void Set_Prm_CLUSTER_RECIPE_PATH( char *data );
char *Get_Prm_CLUSTER_RECIPE_PATH();
//
void Set_Prm_PROCESS_RECIPE_PATH( int i , char *data );
char *Get_Prm_PROCESS_RECIPE_PATH( int i );
//
void Set_Prm_PROCESS_RECIPE_HEAD_PATH( int i , char *data );
char *Get_Prm_PROCESS_RECIPE_HEAD_PATH( int i );

void Set_Prm_MODULE_MODE( int i , BOOL data );
BOOL Get_Prm_MODULE_MODE( int i );
//
//
void Set_Prm_CLUSTER_RECIPE_PARALLEL_SUPPORT( BOOL data );
BOOL Get_Prm_CLUSTER_RECIPE_PARALLEL_SUPPORT();
//
//
void Set_Prm_PROCESS_RECIPE( int i , char *data );
char *Get_Prm_PROCESS_RECIPE( int i );
//
//
void Set_Prm_Scheduler_Path( char *data );
char *Get_Prm_Scheduler_Path();
//
void Set_Prm_Scheduler_CloseString( char *data ); // 2003.12.18
char *Get_Prm_Scheduler_CloseString(); // 2003.12.18
//
void Set_CLUSTER_FILE_CHANGE( BOOL data );
BOOL Get_CLUSTER_FILE_CHANGE();
//
void Set_Prm_PROCESS_NAME( int i , char *data );
char *Get_Prm_PROCESS_NAME( int i );

void Set_GROUP_RUN_CHECK( BOOL data );
BOOL Get_GROUP_RUN_CHECK();

int  Get_CLUSTER_USE_TYPE_ITEM();
void Set_CLUSTER_USE_TYPE_ITEM( int data );

void Set_HANDLING_SIDE_CHECK( BOOL data );
BOOL Get_HANDLING_SIDE_CHECK();

void Set_Prm_MINISCREEN_MODE( int data );
BOOL Get_Prm_MINISCREEN_MODE();

void Set_Prm_MAXVIEW_MODE( int data );
BOOL Get_Prm_MAXVIEW_MODE();

void Set_Prm_TITLE_MODE( int data );
BOOL Get_Prm_TITLE_MODE();

void Set_Prm_READONLY_LEVEL( int data );
void Set_Prm_READONLY_LEVEL_IO_NAME( char *data );
BOOL Get_Prm_READONLY_MODE();

void Set_Prm_GROUPBUTTONHIDE_MODE( int data );
BOOL Get_Prm_GROUPBUTTONHIDE_MODE();

void Set_Prm_MONITOR_MODE( int data );
BOOL Get_Prm_MONITOR_MODE();

void Set_Prm_MONITOR_FOLDER( char *data );
char *Get_Prm_MONITOR_FOLDER();

void Set_Prm_NAME_STYLE( int data ); // 2004.06.15
int  Get_Prm_NAME_STYLE(); // 2004.06.15

void Set_Prm_ALLMOVE_MODE( BOOL data ); // 2005.07.19
BOOL Get_Prm_ALLMOVE_MODE(); // 2005.07.19

void Set_Prm_NAMEONLY_MODE( BOOL data ); // 2006.04.27
BOOL Get_Prm_NAMEONLY_MODE(); // 2006.04.27

void Set_Prm_SEARCH_MODE( int data ); // 2007.09.18
BOOL Get_Prm_SEARCH_MODE(); // 2007.09.18

void Set_Prm_VERSION_MODE( int data ); // 2007.09.18
BOOL Get_Prm_VERSION_MODE(); // 2007.09.18

void RECIPE_LOG_HANDLING( char *data , LPSTR list , ... ); // 2004.01.08

#endif