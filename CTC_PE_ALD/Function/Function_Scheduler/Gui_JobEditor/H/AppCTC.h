//
//
//
//
//
enum { Success , Not_Enable_Status , Event_Spawn_Error , Parameter_Fail , Abort_Status };
//
//
void IO_READ_PART();
//
int RUN_FUNCTION_For_Recipe_Select( int CHECKING_SIDE , char * RcpFile );
int RUN_FUNCTION_For_PATH_Select( int CHECKING_SIDE , long Path , long Path2 );
int RUN_FUNCTION_For_START_SLOT_Select( int CHECKING_SIDE , long StartSlot );
int RUN_FUNCTION_For_END_SLOT_Select( int CHECKING_SIDE , long EndSlot );
int RUN_FUNCTION_For_LOOP_COUNT_Select( int CHECKING_SIDE , long LoopCount );
//
//
//
//
BOOL RUN_FUNCTION_For_ENDING_CHECK( int CHECKING_SIDE );
int  RUN_FUNCTION_For_RUNWAIT_CHECK( int CHECKING_SIDE );
BOOL RUN_FUNCTION_For_READY_CHECK( int CHECKING_SIDE );
//
int RUN_FUNCTION_Event_START( int CHECKING_SIDE , int MODE , char *data );
int RUN_FUNCTION_Event_STOP( int CHECKING_SIDE );
int RUN_FUNCTION_Event_CONTINUE( int CHECKING_SIDE );
int RUN_FUNCTION_Event_ABORT( int CHECKING_SIDE );
int RUN_FUNCTION_Event_END( int CHECKING_SIDE );
//
//
//
//
BOOL HANDOFF_For_RUNNING_CHECK( int ID );
//
int HANDOFF_Event_OUT( int ID );
int HANDOFF_Event_IN( int ID );
int HANDOFF_Event_ABORT( int ID );

void Get_Job_Name( int CHECKING_SIDE , char *Data );
void Get_MID_Name( int CHECKING_SIDE , char *Data );
void Get_Start_Time( int CHECKING_SIDE , BOOL , char *Data , int * , int * );
void Get_End_Time( int CHECKING_SIDE , BOOL , char *Data , int * , int * );
void Get_Elapsed_Time( int CHECKING_SIDE , char *Data );
void Get_Directory( int CHECKING_SIDE , char *Data );
int  Get_Wfr_Count( int CHECKING_SIDE );
//
void Get_Lot_Recipe( int CHECKING_SIDE , int cmin , char *Data );
int  Get_In_Cassette( int CHECKING_SIDE );
int  Get_Out_Cassette( int CHECKING_SIDE );
int	 Get_Start_Slot( int CHECKING_SIDE );
int	 Get_End_Slot( int CHECKING_SIDE );
//
BOOL Get_FirstIn_Supply( int CHECKING_SIDE , char *Data );
BOOL Get_LastIn_Supply( int CHECKING_SIDE );

int Get_Enable_Side_Lot( int in , int out );


//BOOL Check_File_is_Open_Possible_Check( char *file );
