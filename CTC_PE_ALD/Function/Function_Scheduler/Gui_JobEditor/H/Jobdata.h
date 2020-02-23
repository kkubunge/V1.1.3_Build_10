#ifndef JOBDATA_H
#define JOBDATA_H

//
int JOB_RUNNING_CHECK();
int JOB_TIMEOVER_DELETE_CHECK( int );
int JOB_DATA_REMAPPING();
//
BOOL JOB_Delete_Folder_Item( int index , char *optiondata ); // 2003.09.17

BOOL JOB_Make_Delete_Item( int index ); // 2003.09.17
//
BOOL JOB_RunMode_Start_Action( HWND hwnd , int index );
BOOL JOB_RunMode_Abort_Action( HWND hwnd , int index );
BOOL JOB_RunMode_End_Action( HWND hwnd , int index );
BOOL JOB_RunMode_Cancel_Action( HWND hwnd , int index );
BOOL JOB_RunMode_Maint_Insert_Action( HWND hwnd , int index );
//-------------------------------------------------------------------------
int  JOB_Get_Current_Count();
//
int  JOB_FIND_Get_Current_Count();
int  JOB_FIND_Get_Real_Index( int id );
//
BOOL JOB_Question_Count_Empty();
BOOL JOB_Question_Count_Full();
BOOL JOB_Question_Count_Warning( int data );
//
BOOL JOB_Question_Data_Delete_Enable( int index );
//
int  JOB_Delete_Old_Data( int count );
void JOB_Display_Data_Area( int index , int item , char *szString , int param );

int JOB_Get_Run_Status( int index );
//int JOB_Get_Run_Extern_Status( int index ); // 2016.03.29
int JOB_Get_Run_Extern_Status( int index , int *multiicon ); // 2016.03.29
//
//char *JOB_Get_Directory0( int index ); // 2017.03.13
//char *JOB_Get_Directory( int index ); // 2017.03.13
//char *JOB_Get_Directory2( int index , int *loop ); // 2017.03.13

void JOB_Get_Directory_0( int index , char *retstr ); // 2017.03.13
void JOB_Get_Directory( int index , char *retstr ); // 2017.03.13
void JOB_Get_Directory_2( int index , int *loop , char *retstr ); // 2017.03.13

void  JOB_Set_Directory( int index , char *data , int loop );
int   JOB_Get_LoopCount( int index , int * );
void  JOB_Set_Sel_LoopCount( int index , int data );
BOOL  JOB_Has_LoopCount( int index );
//
int  JOB_Get_Directory_Type( int index );
//
void JOB_Get_Display_String( int index , char *data );
void JOB_Get_Display_String2( int index , char *defmsg , char *data );
//
BOOL JOB_Insert_New_Job_Data( char *name , char *recipe , int ic , int oc , int ss , int es , int prg );
BOOL JOB_Insert_New_Job_Data_for_Maint( int side );
//int  JOB_Insert_New_Job_Data_for_Extern( int runmode , char *jname , char *mname , char *recipe , int ic , int ss , int es , char *runtime , char *starttime , char *endtime , char *elapsedtime , char *folder , int cnt ); // 2015.11.13
//int  JOB_Insert_New_Job_Data_for_Extern( int runmode , char *jname , char *mname , char *recipe , int ic , int ss , int es , char *runtime , char *starttime , char *endtime , char *elapsedtime , char *folder , int cnt , int *failindex ); // 2015.11.13 // 2016.03.29
int  JOB_Insert_New_Job_Data_for_Extern( int runmode , char *jname , char *mname , char *recipe , int ic , int ss , int es , char *runtime , char *starttime , char *endtime , char *elapsedtime , char *folder , int cnt , int *failindex , int *failicon ); // 2015.11.13 // 2016.03.29
//
BOOL JOB_Question_Name_Exist( char *name );

int JOB_Find_Search( char *jname , char *lotname , char *midname , BOOL Full );


void JOB_FIND_GO_BACK( HWND hWnd );
void JOB_FIND_SEARCH_START( HWND hWnd );

void JOB_Display_Data_Save_per_Each( BOOL always , int index ); // 2003.06.09
BOOL JOB_Display_Data_Save_per_Each_Check( int * ); // 2003.06.09


#endif