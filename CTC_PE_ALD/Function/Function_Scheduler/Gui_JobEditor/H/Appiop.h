
#include <iobusc.h>

typedef enum
{
	CTC_At1_Button ,
	CTC_At1Stop_Button ,
	CTC_At1Cont_Button ,
	CTC_At1Abrt_Button ,
	CTC_At1End_Button ,
	CTC_Lot1_SlotS ,
	CTC_Lot1_SlotE ,
	CTC_Lot1_Recipe ,
	CTC_Lot1_StartTime ,
	CTC_Lot1_EndTime ,
	CTC_Lot1_LastInTime ,
	CTC_Lot1_DIR ,
	
	CTC_Cycle1_TrgCass ,
	CTC_Cycle1_PreWfr ,

	CTC_CM1HO_Button ,
	CTC_CM1HOAbrt_Button ,
	CTC_CM1HOOut_Button ,
	CTC_CM1HOIn_Button ,
	CTC_CM1HOIn_Prgs ,
	CTC_CM1HOOut_Prgs ,

	CTC_CM2HO_Button ,
	CTC_CM2HOAbrt_Button ,
	CTC_CM2HOOut_Button ,
	CTC_CM2HOIn_Button ,
	CTC_CM2HOIn_Prgs ,
	CTC_CM2HOOut_Prgs ,

	CTC_At2_Button ,
	CTC_At2Stop_Button ,
	CTC_At2Cont_Button ,
	CTC_At2Abrt_Button ,
	CTC_At2End_Button ,
	CTC_Lot2_SlotS ,
	CTC_Lot2_SlotE ,
	CTC_Lot2_Recipe ,
	CTC_Lot2_StartTime ,
	CTC_Lot2_EndTime ,
	CTC_Lot2_LastInTime ,
	CTC_Lot2_DIR ,

	CTC_Cycle2_TrgCass ,
	CTC_Cycle2_PreWfr ,
} IOPointIndex;

//----------------------------------------------------------------------------------

#define MALL_IOL( ind )    void W_##ind##( long Value )   { WRITE_IO_Long( ind , Value ); } \
                           long R_##ind##()               { return( READ_IO_Long( ind ) ); }
#define MALL_IOD( ind )    void W_##ind##( double Value ) { WRITE_IO_Double( ind , Value ); } \
                           double R_##ind##()             { return( READ_IO_Double( ind ) ); }
#define MALL_IOS( ind )    void W_##ind##( char *Value )  { WRITE_IO_String( ind , Value ); } \
                           void R_##ind##( char *Value )  { READ_IO_String( ind , Value ); }

#define MWRITE_IOL( ind )  void W_##ind##( long Value )   { WRITE_IO_Long( ind , Value ); }
#define MREAD_IOL( ind )   long R_##ind##()               { return( READ_IO_Long( ind ) ); }
#define MWRITE_IOD( ind )  void W_##ind##( double Value ) { WRITE_IO_Double( ind , Value ); }
#define MREAD_IOD( ind )   double R_##ind##()             { return( READ_IO_Double( ind ) ); }
#define MWRITE_IOS( ind )  void W_##ind##( char *Value )  { WRITE_IO_String( ind , Value ); }
#define MREAD_IOS( ind )   void R_##ind##( char *Value )  { READ_IO_String( ind , Value ); }

#define HALL_IOL( ind )    void W_##ind##( long );   long R_##ind##();
#define HALL_IOD( ind )    void W_##ind##( double ); double R_##ind##();
#define HALL_IOS( ind )    void W_##ind##( char * ); void R_##ind##( char * );

#define HWRITE_IOL( ind )  void W_##ind##( long );
#define HREAD_IOL( ind )   long R_##ind##();
#define HWRITE_IOD( ind )  void W_##ind##( double );
#define HREAD_IOD( ind )   double R_##ind##();
#define HWRITE_IOS( ind )  void W_##ind##( char * );
#define HREAD_IOS( ind )   void R_##ind##( char * );

#define WRITE_IO( ind , Value )  W_##ind##( Value )
#define READ_IO(  ind )          R_##ind##()
#define READ_IOS( ind , Value )  R_##ind##( Value )

//-------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

long READ_IO_Long( int );
void WRITE_IO_Long( int , long );
double READ_IO_Double( int );
void WRITE_IO_Double( int , double );
void WRITE_IO_String( int , char * );
void READ_IO_String( int , char * );

int IOP_Initial(void);
int IOP_Program_Init(void);

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
HALL_IOL( CTC_At1_Button )
HALL_IOL( CTC_At1Stop_Button )
HALL_IOL( CTC_At1Cont_Button )
HALL_IOL( CTC_At1Abrt_Button )
HALL_IOL( CTC_At1End_Button )

HALL_IOL( CTC_Lot1_SlotS )
HALL_IOL( CTC_Lot1_SlotE )
HALL_IOS( CTC_Lot1_Recipe )
HALL_IOS( CTC_Lot1_StartTime )
HALL_IOS( CTC_Lot1_EndTime )
HALL_IOS( CTC_Lot1_LastInTime )
HALL_IOS( CTC_Lot1_DIR )

HALL_IOL( CTC_Cycle1_TrgCass )
HALL_IOL( CTC_Cycle1_PreWfr )

HALL_IOL( CTC_CM1HO_Button )
HALL_IOL( CTC_CM1HOAbrt_Button  )
HALL_IOL( CTC_CM1HOOut_Button   )
HALL_IOL( CTC_CM1HOIn_Button    )
HALL_IOL( CTC_CM1HOIn_Prgs      )
HALL_IOL( CTC_CM1HOOut_Prgs     )

HALL_IOL( CTC_CM2HO_Button )
HALL_IOL( CTC_CM2HOAbrt_Button  )
HALL_IOL( CTC_CM2HOOut_Button   )
HALL_IOL( CTC_CM2HOIn_Button    )
HALL_IOL( CTC_CM2HOIn_Prgs      )
HALL_IOL( CTC_CM2HOOut_Prgs     )

HALL_IOL( CTC_At2_Button )
HALL_IOL( CTC_At2Stop_Button )
HALL_IOL( CTC_At2Cont_Button )
HALL_IOL( CTC_At2Abrt_Button )
HALL_IOL( CTC_At2End_Button )

HALL_IOL( CTC_Lot2_SlotS )
HALL_IOL( CTC_Lot2_SlotE )
HALL_IOS( CTC_Lot2_Recipe )
HALL_IOS( CTC_Lot2_StartTime )
HALL_IOS( CTC_Lot2_EndTime )
HALL_IOS( CTC_Lot2_LastInTime )
HALL_IOS( CTC_Lot2_DIR )

HALL_IOL( CTC_Cycle2_TrgCass )
HALL_IOL( CTC_Cycle2_PreWfr )

