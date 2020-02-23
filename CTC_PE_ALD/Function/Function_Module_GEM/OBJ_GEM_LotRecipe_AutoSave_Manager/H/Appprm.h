#ifndef APPPRM_H
#define APPPRM_H

void Set_MAIN_PATH( char *data );

void Set_Prm_FILE_ID( char *data );
char *Get_Prm_FILE_ID();

void Set_Prm_LOT_RECIPE_ID( char *data );
char *Get_Prm_LOT_RECIPE_ID();

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Set_Prm_DUMMY_PROCESS_CHAMBER( int data );
int  Get_Prm_DUMMY_PROCESS_CHAMBER();
//
void Set_Prm_DUMMY_PROCESS_SLOT( int data );
int  Get_Prm_DUMMY_PROCESS_SLOT();
//
void Set_Prm_DUMMY_PROCESS_MODE( int data );
int  Get_Prm_DUMMY_PROCESS_MODE();
//----------------------------------------------------------------------------
void Set_Prm_MAIN_RECIPE_PATH( char *data );
char *Get_Prm_MAIN_RECIPE_PATH();

void Set_Prm_GROUP_RECIPE_PATH( char *data );
char *Get_Prm_GROUP_RECIPE_PATH();

void Set_Prm_LOT_RECIPE_PATH( char *data );
char *Get_Prm_LOT_RECIPE_PATH();

void Set_Prm_CLUSTER_RECIPE_PATH( char *data );
char *Get_Prm_CLUSTER_RECIPE_PATH();


void Set_Prm_PROCESS_RECIPE_PATH( int i , char *data );
char *Get_Prm_PROCESS_RECIPE_PATH( int i );

void Set_Prm_PROCESS_RECIPE_HEAD_PATH( int i , char *data );
char *Get_Prm_PROCESS_RECIPE_HEAD_PATH( int i );

//
void Set_Prm_MAX_CASSETTE_SLOT( int data );
int  Get_Prm_MAX_CASSETTE_SLOT();
//

void Set_LOT_FILE_CHANGE( BOOL data );
BOOL Get_LOT_FILE_CHANGE();

void Set_BLANK_SKIP_CHECK( BOOL data );
BOOL Get_BLANK_SKIP_CHECK();

void Set_GROUP_RUN_CHECK( BOOL data );
BOOL Get_GROUP_RUN_CHECK();

void Set_Prm_USE_PROCESS( BOOL data );
BOOL Get_Prm_USE_PROCESS();

void Set_Prm_MINISCREEN_MODE( int data );
BOOL Get_Prm_MINISCREEN_MODE();

void Set_Prm_TITLE_MODE( int data );
BOOL Get_Prm_TITLE_MODE();

void Set_Prm_MONITOR_MODE( int data );
BOOL Get_Prm_MONITOR_MODE();

void Set_Prm_MONITOR_FOLDER( char *data );
char *Get_Prm_MONITOR_FOLDER();

void Set_Prm_READONLY_LEVEL( int data );
void Set_Prm_READONLY_LEVEL_IO_NAME( char *data );
BOOL Get_Prm_READONLY_MODE();

void Set_Prm_GROUPBUTTONHIDE_MODE( int data );
BOOL Get_Prm_GROUPBUTTONHIDE_MODE();

void Set_Prm_REVERSE_MODE( int data ); // 2003.06.11
BOOL Get_Prm_REVERSE_MODE(); // 2003.06.11

void Set_Prm_NAME_STYLE( int data ); // 2004.06.15
int  Get_Prm_NAME_STYLE(); // 2004.06.15

//----------------------------------------------------------------------------

void RECIPE_LOG_HANDLING( char *data , LPSTR list , ... ); // 2004.01.08

#endif