#ifndef APPPRM_H
#define APPPRM_H

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

enum { CONTROL_RECIPE_ALL , CONTROL_RECIPE_LOT_PROCESS , CONTROL_RECIPE_CLUSTER , CONTROL_RECIPE_PROCESS };

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void _SET_CHAMBER_STRING( int ch , char *data );
char *_GET_CHAMBER_STRING( int ch );
BOOL _GET_CHAMBER_STRING_POSSIBLE( int ch );
void CHAMBER_STRING_CHANGE( char *data , char *data2 );
void CHAMBER_STRING_CHANGE_FOR_CM( char *data , char *data2 );
int CHAMBER_STRING_FIND( char *data );
//----------------------------------------------------------------------------
void Set_Prm_CONTROL_RECIPE_TYPE( int data );
int  Get_Prm_CONTROL_RECIPE_TYPE();
void Set_Prm_CONTROL_FIXED_CASSETTE_IN( int data );
int  Get_Prm_CONTROL_FIXED_CASSETTE_IN();
void Set_Prm_CONTROL_FIXED_CASSETTE_OUT( int data );
int  Get_Prm_CONTROL_FIXED_CASSETTE_OUT();
void Set_Prm_CONTROL_FIXED_SLOT_START( int data );
int  Get_Prm_CONTROL_FIXED_SLOT_START();
void Set_Prm_CONTROL_FIXED_SLOT_END( int data );
int  Get_Prm_CONTROL_FIXED_SLOT_END();
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
void Set_Prm_PRE_STRING( char *data );// 2018.09.04
char *Get_Prm_PRE_STRING(); // 2018.09.04
//
void Set_Prm_POST_STRING( char *data ); // 2018.09.04
char *Get_Prm_POST_STRING(); // 2018.09.04
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Set_Prm_MODULE_MODE( int i , BOOL data );
BOOL Get_Prm_MODULE_MODE( int i );
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Set_Prm_SYSTEM_DIRECTORY( char *data );
char *Get_Prm_SYSTEM_DIRECTORY();
//----------------------------------------------------------------------------
void Set_Prm_LOG_DIRECTORY( char *data );
char *Get_Prm_LOG_DIRECTORY();
//----------------------------------------------------------------------------
void Set_Prm_METHOD_STRING( char *data );
char *Get_Prm_METHOD_STRING();
//----------------------------------------------------------------------------
void Set_Prm_JID_STRING( char *data );
char *Get_Prm_JID_STRING();
//----------------------------------------------------------------------------
void Set_Prm_LID_STRING( char *data );
char *Get_Prm_LID_STRING();
//----------------------------------------------------------------------------
void Set_Prm_MID_STRING( char *data );
char *Get_Prm_MID_STRING();
//----------------------------------------------------------------------------
void Set_Prm_WID_STRING( char *data );
char *Get_Prm_WID_STRING();
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Set_Prm_FILE_ID( char *data );
char *Get_Prm_FILE_ID();

void Set_Prm_LOT_RECIPE_ID( char *data );
char *Get_Prm_LOT_RECIPE_ID();
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Set_Prm_MAX_CASSETTE_SLOT( int data );
int  Get_Prm_MAX_CASSETTE_SLOT();
//
//
void Set_Prm_MAX_CASSETTE_COUNT( int data );
int  Get_Prm_MAX_CASSETTE_COUNT();
//
void Set_Prm_SIMPLE_DISPLAY( int data );
int  Get_Prm_SIMPLE_DISPLAY();
//
void Set_Prm_TREE_ITEM_DISPLAY( int data );
int  Get_Prm_TREE_ITEM_DISPLAY();
//
void Set_LOT_FILE_CHANGE( BOOL data );
BOOL Get_LOT_FILE_CHANGE();

int Check_Single_Chamber_Use();

void Set_GROUP_RUN_CHECK( BOOL data );
BOOL Get_GROUP_RUN_CHECK();

void Set_Prm_MINISCREEN_MODE( int data );
BOOL Get_Prm_MINISCREEN_MODE();

void Set_Prm_FIND_MODE( int data );
BOOL Get_Prm_FIND_MODE();

void Set_Prm_NOTANAL_MODE( int data );
BOOL Get_Prm_NOTANAL_MODE();

void Set_Prm_SAVEFILE_MODE( int data );
BOOL Get_Prm_SAVEFILE_MODE();

void Set_Prm_SCROLLBUTTON_MODE( int data );
BOOL Get_Prm_SCROLLBUTTON_MODE();

void Set_Prm_EXCLUSIVE_JOB_CHECK( int side , BOOL data );
BOOL Get_Prm_EXCLUSIVE_JOB_CHECK( int side );

void Set_Prm_TITLE_MODE( int data );
BOOL Get_Prm_TITLE_MODE();

void Set_Prm_WINDOW_RETURN_MODE( int data );
BOOL Get_Prm_WINDOW_RETURN_MODE();

void Set_Prm_NETMONITOR_MODE( int data );
BOOL Get_Prm_NETMONITOR_MODE();

void Set_Prm_NETMONITOR_FOLDER( char *data );
char *Get_Prm_NETMONITOR_FOLDER();

void Set_Prm_READONLY_LEVEL( int data );
void Set_Prm_READONLY_LEVEL_IO_NAME( char *data );
BOOL Get_Prm_READONLY_MODE();


void Set_Prm_EXTENDVIEW_MODE( int data );
int  Get_Prm_EXTENDVIEW_MODE();

void Set_Prm_DELETE_HIDE( int data );
BOOL Get_Prm_DELETE_HIDE();

void Set_Prm_SORT_MODE( int data );
BOOL Get_Prm_SORT_MODE();

void Set_Prm_DISPLAY_MODE( int data ); // 2010.11.30
BOOL Get_Prm_DISPLAY_MODE(); // 2010.11.30

BOOL Process_Log_File_Is_Empty( char *filename ); // 2007.06.25
DWORD Process_Log_File_Get_Size( char *filename ); // 2014.02.06

void Set_Prm_STATUS_VIEW( int data ); // 2014.07.17
BOOL Get_Prm_STATUS_VIEW(); // 2014.07.17


#endif