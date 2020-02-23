#ifndef IODEFINE_H
#define IODEFINE_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
#define		ID_PLOT_SCROLL_1	901
#define		ID_PLOT_SCROLL_2	902

#define		ID_PLOT_AUTO_1		903
#define		ID_PLOT_AUTO_2		904
//------------------------------------------------------------------------------------------

enum { _KG_D_IO , _KG_A_IO , _KG_S_IO , _KG_F_IO , _KG_P_IO };
enum { DRAW_NONE , DRAW_1_LEFT , DRAW_1_RIGHT , DRAW_2_LEFT , DRAW_2_RIGHT };
enum { SCR_HIDE_DRAW , SCR_SMALL_DRAW , SCR_BIG_DRAW };
enum { SCREEN_1 , SCREEN_2 };

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
#define MAX_PLOTTING_SIZE			7200

#define MAX_CHANNEL_NAME_SIZE		30
//
#define MAX_ANALOG_TABLE			99
//
#define MAX_EVENT_TABLE				99
//
#define MAX_UNIT_NAME_SIZE			8
#define MAX_STRING_SIZE				99
#define MAX_PLTTIME_SIZE			10
#define MAX_VALUE_SIZE				99

//
//
//
//

#define		DRAW_SIZE_X_SMALL			300
#define		DRAW_SIZE_Y_SMALL			240

#define		DRAW_SIZE_X_BIG				720
#define		DRAW_SIZE_Y_BIG				240
//

#define		PLT0_X_START			104
#define		PLT0_Y_START			65

#define		PLT1_X_START			87
#define		PLT1_Y_START			65

#define		PLT2_X_START			542
#define		PLT2_Y_START			65

#define		PLT0_X1_START_TIME			102
#define		PLT0_X2_START_TIME			PLT0_X1_START_TIME + DRAW_SIZE_X_BIG - 60
//
#define		PLT1_X1_START_TIME			85
#define		PLT1_X2_START_TIME			PLT1_X1_START_TIME + DRAW_SIZE_X_SMALL- 60
#define		PLT2_X1_START_TIME			540
#define		PLT2_X2_START_TIME			PLT2_X1_START_TIME + DRAW_SIZE_X_SMALL - 60
//
#define		PLT_Y_START_TIME			310
#define		PLT_X_SIZE_TIME				8
//========================================================================================
//========================================================================================
#define	DISPLAY_START_X_FOR_PLOTTER_0L		112
#define	DISPLAY_START_X_FOR_PLOTTER_0R		704

#define	DISPLAY_START_X_FOR_PLOTTER_1L		95
#define	DISPLAY_START_X_FOR_PLOTTER_1R		264
#define	DISPLAY_START_X_FOR_PLOTTER_2L		550
#define	DISPLAY_START_X_FOR_PLOTTER_2R		719
#define	DISPLAY_START_Y_FOR_PLOTTER			340
#define	DISPLAY_SIZE_Y_FOR_PLOTTER			17
#define	DISPLAY_SIZE_X_FOR_PLOTTER			17
//========================================================================================
#define	DISPLAY_START_X_FOR_MINMAX_0L		36
#define	DISPLAY_START_X_FOR_MINMAX_0R		DISPLAY_START_X_FOR_MINMAX_0L+DRAW_SIZE_X_BIG+72

#define	DISPLAY_START_X_FOR_MINMAX_1L		19
#define	DISPLAY_START_X_FOR_MINMAX_1R		DISPLAY_START_X_FOR_MINMAX_1L+DRAW_SIZE_X_SMALL+72
#define	DISPLAY_START_X_FOR_MINMAX_2L		474
#define	DISPLAY_START_X_FOR_MINMAX_2R		DISPLAY_START_X_FOR_MINMAX_2L+DRAW_SIZE_X_SMALL+72

#define	DISPLAY_START_Y_FOR_MINMAX_UP		48
#define	DISPLAY_START_Y_FOR_MINMAX_CE		DISPLAY_START_Y_FOR_MINMAX_UP+8
#define	DISPLAY_START_Y_FOR_MINMAX_DN		DISPLAY_START_Y_FOR_MINMAX_UP+17

#define	DISPLAY_SIZE_Y_FOR_MINMAX			60
#define	DISPLAY_SIZE_X_FOR_MINMAX			8
//
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//========================================================================================
//
//
//
#define COUNTER_CHAR		sizeof(char)	//1
#define COUNTER_INT			sizeof(int)		//4
#define COUNTER_LONG		sizeof(long)	//4
#define COUNTER_FLOAT		sizeof(float)	//4
#define COUNTER_DOUBLE		sizeof(double)	//8
#define COUNTER_CLOCK		sizeof(long)
#define COUNTER_BOOL		sizeof(char)

//
// Analog IO Table
#define		AIO_NAME_POINTER				0
#define		AIO_SHOW_POINTER				AIO_NAME_POINTER+MAX_CHANNEL_NAME_SIZE+1
#define		AIO_SAVE_POINTER				AIO_SHOW_POINTER+MAX_CHANNEL_NAME_SIZE+1
#define		AIO_SAVE_OLD_POINTER			AIO_SAVE_POINTER+COUNTER_INT
#define		AIO_TYPE_POINTER				AIO_SAVE_OLD_POINTER+COUNTER_INT
#define		AIO_POINT_POINTER				AIO_TYPE_POINTER+COUNTER_INT
#define		AIO_UNIT_POINTER				AIO_POINT_POINTER+COUNTER_INT
#define		AIO_IN_DEVIATION_POINTER		AIO_UNIT_POINTER+MAX_UNIT_NAME_SIZE+1
#define		AIO_VALUE_POINTER				AIO_IN_DEVIATION_POINTER+COUNTER_INT
#define		AIO_VALUE_OLD_POINTER			AIO_VALUE_POINTER+COUNTER_DOUBLE
#define		AIO_VALUE_STR_POINTER			AIO_VALUE_OLD_POINTER+COUNTER_DOUBLE
#define		AIO_MIN_VALUE_POINTER			AIO_VALUE_STR_POINTER+MAX_VALUE_SIZE+1
#define		AIO_MAX_VALUE_POINTER			AIO_MIN_VALUE_POINTER+COUNTER_DOUBLE
#define		AIO_TOGGLE_POINTER				AIO_MAX_VALUE_POINTER+COUNTER_DOUBLE
#define		AIO_HELP_POINTER				AIO_TOGGLE_POINTER+COUNTER_INT
#define		AIO_PLT_DISPLAY_POINTER			AIO_HELP_POINTER+MAX_STRING_SIZE+1
#define		AIO_PLT_START_POINTER			AIO_PLT_DISPLAY_POINTER+COUNTER_INT
#define		AIO_PLT_END_POINTER				AIO_PLT_START_POINTER+COUNTER_INT
#define		AIO_PLT_COUNT_POINTER			AIO_PLT_END_POINTER+COUNTER_INT
#define		AIO_PLT_HERE_POINTER			AIO_PLT_COUNT_POINTER+COUNTER_INT
#define		AIO_PLT_POLL_POINTER			AIO_PLT_HERE_POINTER+COUNTER_INT
#define		AIO_PLT_OLD_POINTER				AIO_PLT_POLL_POINTER+COUNTER_INT
#define		AIO_PLT_OLD2_POINTER			AIO_PLT_OLD_POINTER+COUNTER_INT
#define		AIO_PLT_DEV_POINTER				AIO_PLT_OLD2_POINTER+COUNTER_INT
#define		AIO_PLT_TIME_POINTER			AIO_PLT_DEV_POINTER+COUNTER_INT
#define		AIO_PLT_DATA_POINTER			AIO_PLT_TIME_POINTER+(MAX_PLOTTING_SIZE*COUNTER_LONG)
#define		AIO_ZOOM_POINTER				AIO_PLT_DATA_POINTER+(MAX_PLOTTING_SIZE*COUNTER_DOUBLE)
#define		AIO_MIN_OLD_POINTER				AIO_ZOOM_POINTER+COUNTER_INT
#define		AIO_MAX_OLD_POINTER				AIO_MIN_OLD_POINTER+COUNTER_DOUBLE
#define		AIO_DEV_OLD_POINTER				AIO_MAX_OLD_POINTER+COUNTER_DOUBLE
#define		AIO_TOTAL						AIO_DEV_OLD_POINTER+COUNTER_INT

#define		A_IO_NAME( ind )					      ((char *) A_IOP[ind] + AIO_NAME_POINTER)
#define		A_IO_SHOW( ind )					      ((char *) A_IOP[ind] + AIO_SHOW_POINTER)
#define		A_IO_SAVE( ind )				*(int   *)((char *) A_IOP[ind] + AIO_SAVE_POINTER)
#define		A_IO_SAVE_OLD( ind )			*(int   *)((char *) A_IOP[ind] + AIO_SAVE_OLD_POINTER)
#define		A_IO_TYPE( ind )				*(int   *)((char *) A_IOP[ind] + AIO_TYPE_POINTER)
#define		A_IO_POINT( ind )				*(int   *)((char *) A_IOP[ind] + AIO_POINT_POINTER)
#define		A_IO_UNIT( ind )				  		  ((char *) A_IOP[ind] + AIO_UNIT_POINTER)
#define		A_IO_IN_DEVIATION( ind )		*(int   *)((char *) A_IOP[ind] + AIO_IN_DEVIATION_POINTER)
#define		A_IO_VALUE( ind )				*(double*)((char *) A_IOP[ind] + AIO_VALUE_POINTER)
#define		A_IO_VALUE_OLD( ind )			*(double*)((char *) A_IOP[ind] + AIO_VALUE_OLD_POINTER)
#define		A_IO_VALUE_STR( ind )					  ((char *) A_IOP[ind] + AIO_VALUE_STR_POINTER)
#define		A_IO_MIN( ind )					*(double*)((char *) A_IOP[ind] + AIO_MIN_VALUE_POINTER)
#define		A_IO_MAX( ind )					*(double*)((char *) A_IOP[ind] + AIO_MAX_VALUE_POINTER)
#define		A_IO_TOGGLE( ind )				*(int   *)((char *) A_IOP[ind] + AIO_TOGGLE_POINTER)
#define		A_IO_HELP( ind )					      ((char *) A_IOP[ind] + AIO_HELP_POINTER)
#define		A_IO_PLT_DISPLAY( ind )			*(int   *)((char *) A_IOP[ind] + AIO_PLT_DISPLAY_POINTER)
#define		A_IO_PLT_START( ind )			*(int   *)((char *) A_IOP[ind] + AIO_PLT_START_POINTER)
#define		A_IO_PLT_END( ind )				*(int   *)((char *) A_IOP[ind] + AIO_PLT_END_POINTER)
#define		A_IO_PLT_COUNT( ind )			*(int   *)((char *) A_IOP[ind] + AIO_PLT_COUNT_POINTER)
#define		A_IO_PLT_HERE( ind )			*(int   *)((char *) A_IOP[ind] + AIO_PLT_HERE_POINTER)
#define		A_IO_PLT_POLL( ind )			*(int   *)((char *) A_IOP[ind] + AIO_PLT_POLL_POINTER)
#define		A_IO_PLT_OLD( ind )				*(int   *)((char *) A_IOP[ind] + AIO_PLT_OLD_POINTER)
#define		A_IO_PLT_OLD2( ind )			*(int   *)((char *) A_IOP[ind] + AIO_PLT_OLD2_POINTER)
#define		A_IO_PLT_DEV( ind )				*(int   *)((char *) A_IOP[ind] + AIO_PLT_DEV_POINTER)
#define		A_IO_PLT_TIME( ind , pnt )		*(long	*)((char *) A_IOP[ind] + AIO_PLT_TIME_POINTER + ((COUNTER_LONG)*(pnt)) )
#define		A_IO_PLT_DATA( ind , pnt )		*(double*)((char *) A_IOP[ind] + AIO_PLT_DATA_POINTER + ((COUNTER_DOUBLE)*(pnt)) )

#define		A_IO_ZOOM( ind )				*(int   *)((char *) A_IOP[ind] + AIO_ZOOM_POINTER)
#define		A_IO_MIN_OLD( ind )				*(double*)((char *) A_IOP[ind] + AIO_MIN_OLD_POINTER)
#define		A_IO_MAX_OLD( ind )				*(double*)((char *) A_IOP[ind] + AIO_MAX_OLD_POINTER)
#define		A_IO_DEV_OLD( ind )				*(int   *)((char *) A_IOP[ind] + AIO_DEV_OLD_POINTER)

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//
// Event IO Table
#define		EIO_NAME_POINTER				0
#define		EIO_SHOW_POINTER				EIO_NAME_POINTER+MAX_CHANNEL_NAME_SIZE+1
#define		EIO_TYPE_POINTER				EIO_SHOW_POINTER+MAX_CHANNEL_NAME_SIZE+1
#define		EIO_POINT_POINTER				EIO_TYPE_POINTER+COUNTER_INT
#define		EIO_OLD_POINTER					EIO_POINT_POINTER+COUNTER_INT
#define		EIO_TOTAL						EIO_OLD_POINTER+MAX_STRING_SIZE+1

#define		E_IO_NAME( ind )					      ((char *) E_IOP[ind] + EIO_NAME_POINTER)
#define		E_IO_SHOW( ind )					      ((char *) E_IOP[ind] + EIO_SHOW_POINTER)
#define		E_IO_TYPE( ind )				*(int   *)((char *) E_IOP[ind] + EIO_TYPE_POINTER)
#define		E_IO_POINT( ind )				*(int   *)((char *) E_IOP[ind] + EIO_POINT_POINTER)
#define		E_IO_OLD( ind )							  ((char *) E_IOP[ind] + EIO_OLD_POINTER)

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
extern BOOL REMOTE_MONITOR_MODE; // 2005.03.15
extern char REMOTE_MONITOR_FOLDER[256]; // 2005.03.15

extern long DISPLAY_START_TIME;
extern long DATALOG_START_TIME;

extern int  DISPLAY_CONTROL;

extern int  DATALOG_CONTROL;
extern int  DATALOG_FILE;
extern int  DATALOG_TIME;
extern int  DATALOG_TIME_OLD;
extern int  DATALOG_AUTO_DELETE_TIME;
extern int  DATALOG_AUTO_DELETE_COUNT;

//---> Modify 2008/08/27 HeaderItem 추가 by KSM
//extern int  DATALOG_STRING_IO;
extern int  DATALOG_STRING_IO1;
extern int  DATALOG_STRING_IO2;
//---> End    2008/08/27 HeaderItem 추가 by KSM

//---> Modify 2008/08/27 HeaderItem 추가 by KSM
//extern int  DATALOG_DIGITAL_IO;	
extern int  DATALOG_DIGITAL_IO1;
extern int  DATALOG_DIGITAL_IO2;
extern int  DATALOG_DIGITAL_IO3;
//---> End    2008/08/27 HeaderItem 추가 by KSM

//---> Add    2008/08/27 HeaderItem 추가 by KSM
extern int  DATALOG_ANALOG_IO1;
//---> End    2008/08/27 HeaderItem 추가 by KSM

extern char INFORMATION_FILE[256];

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

extern void	*A_IOP[ MAX_ANALOG_TABLE ];
extern int		ANALOG_CNT;

//----------------------------------------------------------------------------
extern int		DISP_LAST_POINTER[2];
extern int		DISP_LAST_OLD_POINTER[2];
//----------------------------------------------------------------------------

extern void	*E_IOP[ MAX_EVENT_TABLE ];
extern int		EVENT_CNT;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void Draw_Time_Data_Clear( HDC );
void Draw_Time_Data( HDC , int dm , int mode , int , int , int , int color , BOOL );

extern HWND PLOT_MAIN_WINDOW;
//----------------------------------------------------------------------------
extern BOOL A_IO_MONITOR_REFORM;
//----------------------------------------------------------------------------
BOOL Console_Drawing_Zoom_Ready( int SCR , int *x , int *y );
BOOL Console_Drawing_Zoom_Draw_Check( int SCR );

#endif
