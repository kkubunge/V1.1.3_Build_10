#ifndef APPPLOTTING_H
#define APPPLOTTING_H

//
void PLOTTING_DATA_CLEAR();
void PLOTTING_TITLE_SET( char *FileName );
void PLOTTING_MAX_ITEM( int Item );
void PLOTTING_ADD_ITEM( int Index , char *Item );
char *PLOTTING_GET_ITEM( int Index );
void PLOTTING_ADD_TIME( char *Time );
void PLOTTING_ADD_DATA( int Index , double Data );
BOOL APIENTRY LogView_Open_Plotting_Proc( HWND hdlg , UINT msg , WPARAM wParam , LPARAM lParam );

//#define MAX_X_TABLE				50		// 2004.10.12
//#define MAX_X_TABLE				60		// 2004.10.12

#ifdef	X_200_SIZE
	#define MAX_X_TABLE				200		// 2014.02.27
#else
	#ifdef	X_160_SIZE
	//	#define MAX_X_TABLE				150		// 2007.05.16 // 2010.05.20
		#define MAX_X_TABLE				160		// 2007.05.16 // 2010.05.20
	#else
		#define MAX_X_TABLE				80		// 2007.05.16
	#endif
#endif

//#define MAX_Y_TABLE				5000 // 2007.05.16
#define MAX_Y_TABLE				4096 // 2007.05.16

#define MAX_ITEM_SIZE			  30
#define MAX_TIME_SIZE			  30


#define COUNTER_CHAR		sizeof(char)	//1
#define COUNTER_INT			sizeof(int)		//4
#define COUNTER_LONG		sizeof(long)	//4
#define COUNTER_FLOAT		sizeof(float)	//4
#define COUNTER_DOUBLE		sizeof(double)	//8
#define COUNTER_CLOCK		sizeof(long)
#define COUNTER_BOOL		sizeof(char)


// Analog IO Table
#define		PIO_PLT_TIME_POINTER			0
#define		PIO_PLT_DATA_POINTER			PIO_PLT_TIME_POINTER+MAX_TIME_SIZE+1
#define		PIO_TOTAL						PIO_PLT_DATA_POINTER+(MAX_X_TABLE*COUNTER_DOUBLE)

#define		P_IO_TIME( ind )						  ((char *) P_IOP[ind] + PIO_PLT_TIME_POINTER)
#define		P_IO_DATA( ind , pnt )			*(double*)((char *) P_IOP[ind] + PIO_PLT_DATA_POINTER + ((COUNTER_DOUBLE)*(pnt)) )

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BOOL COMP_HAS_PLOT_FUNC();
BOOL COMP_CALL_PLOT_FUNC( HWND hWnd , char *msg );

void COMP_INIT();

int COMP_MAKEFILE( char *filename );

BOOL COMP_EDITING( HWND hdlg , HWND hWndPr , int index , char * , char * , char * , char * , int );

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif