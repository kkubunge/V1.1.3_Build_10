#ifndef IODEFINE__H
#define IODEFINE__H

#ifdef _HUGE_FUNCTION
	#define MAX_DRIVER_TABLE		1000
	#define MAX_DIGITAL_TABLE		20000
	#define MAX_ANALOG_TABLE		20000
	#define MAX_STRING_TABLE		15000
#else
	#define MAX_DRIVER_TABLE		200
	#define MAX_DIGITAL_TABLE		5000
	#define MAX_ANALOG_TABLE		5000
	#define MAX_STRING_TABLE		3000
#endif

#define MAX_CHANNEL_NAME_SIZE     30

#define MAX_STRING_SIZE			  99

#define MAX_VALUE_SIZE			  99

//========================================================================
//========================================================================
//enum { UNKNOWN_IO_TYPE , INPUT_IO_TYPE , OUTPUT_IO_TYPE };	// 2003.08.26
enum { UNKNOWN_IO_TYPE , INPUT_IO_TYPE , OUTPUT_IO_TYPE , OUTPUT_VIO_TYPE , OUTPUT_RIO_TYPE };	// 2003.08.26
//
//

#define COUNTER_CHAR		sizeof(char)	//1
#define COUNTER_INT			sizeof(int)		//4
#define COUNTER_LONG		sizeof(long)	//4
#define COUNTER_FLOAT		sizeof(float)	//4
#define COUNTER_DOUBLE		sizeof(double)	//8
#define COUNTER_CLOCK		sizeof(long)
#define COUNTER_BOOL		sizeof(char)

// Driver Table
#define		RIO_NAME_POINTER				0
#define		RIO_TOTAL						RIO_NAME_POINTER+MAX_CHANNEL_NAME_SIZE+1

#define		R_IO_NAME( ind )					        ((char *) R_IOP[ind] + RIO_NAME_POINTER)

// Digital IO Table
#define		DIO_NAME_POINTER				0
#define		DIO_NAMEL_POINTER				DIO_NAME_POINTER+MAX_CHANNEL_NAME_SIZE+1
#define		DIO_TYPE_POINTER				DIO_NAMEL_POINTER+MAX_CHANNEL_NAME_SIZE+1
#define		DIO_IN_VALUE_POINTER			DIO_TYPE_POINTER+COUNTER_INT
#define		DIO_IN_VALUE_STR_POINTER		DIO_IN_VALUE_POINTER+COUNTER_INT
#define		DIO_IN_DRIVER_POINTER			DIO_IN_VALUE_STR_POINTER+MAX_VALUE_SIZE+1
#define		DIO_OUT_VALUE_POINTER			DIO_IN_DRIVER_POINTER+COUNTER_INT
#define		DIO_OUT_VALUE_STR_POINTER		DIO_OUT_VALUE_POINTER+COUNTER_INT
#define		DIO_OUT_DRIVER_POINTER			DIO_OUT_VALUE_STR_POINTER+MAX_VALUE_SIZE+1
#define		DIO_SELECT_POINTER				DIO_OUT_DRIVER_POINTER+COUNTER_INT
#define		DIO_NAMEREAD_POINTER			DIO_SELECT_POINTER+COUNTER_INT
#define		DIO_DATAREAD_POINTER			DIO_NAMEREAD_POINTER+COUNTER_INT
#define		DIO_DRVREAD_POINTER				DIO_DATAREAD_POINTER+COUNTER_INT
#define		DIO_TOTAL						DIO_DRVREAD_POINTER+COUNTER_INT

#define		D_IO_NAME( ind )					        ((char *) D_IOP[ind] + DIO_NAME_POINTER)
#define		D_IO_NAMEL( ind )					        ((char *) D_IOP[ind] + DIO_NAMEL_POINTER)
#define		D_IO_TYPE( ind )				*(int     *)((char *) D_IOP[ind] + DIO_TYPE_POINTER)
#define		D_IO_IN_VALUE( ind )			*(int     *)((char *) D_IOP[ind] + DIO_IN_VALUE_POINTER)
#define		D_IO_IN_VALUE_STR( ind )			        ((char *) D_IOP[ind] + DIO_IN_VALUE_STR_POINTER)
#define		D_IO_IN_DRIVER( ind )			*(int     *)((char *) D_IOP[ind] + DIO_IN_DRIVER_POINTER)
#define		D_IO_OUT_VALUE( ind )			*(int     *)((char *) D_IOP[ind] + DIO_OUT_VALUE_POINTER)
#define		D_IO_OUT_VALUE_STR( ind )			        ((char *) D_IOP[ind] + DIO_OUT_VALUE_STR_POINTER)
#define		D_IO_OUT_DRIVER( ind )			*(int     *)((char *) D_IOP[ind] + DIO_OUT_DRIVER_POINTER)
#define		D_IO_SELECT( ind )				*(int     *)((char *) D_IOP[ind] + DIO_SELECT_POINTER)
#define		D_IO_NAMEREAD( ind )			*(int     *)((char *) D_IOP[ind] + DIO_NAMEREAD_POINTER)
#define		D_IO_DATAREAD( ind )			*(int     *)((char *) D_IOP[ind] + DIO_DATAREAD_POINTER)
#define		D_IO_DRVREAD( ind )				*(int     *)((char *) D_IOP[ind] + DIO_DRVREAD_POINTER)


// Analog IO Table
#define		AIO_NAME_POINTER				0
#define		AIO_NAMEL_POINTER				AIO_NAME_POINTER+MAX_CHANNEL_NAME_SIZE+1
#define		AIO_TYPE_POINTER				AIO_NAMEL_POINTER+MAX_CHANNEL_NAME_SIZE+1
#define		AIO_IN_VALUE_POINTER			AIO_TYPE_POINTER+COUNTER_INT
#define		AIO_IN_VALUE_STR_POINTER		AIO_IN_VALUE_POINTER+COUNTER_DOUBLE
#define		AIO_IN_DRIVER_POINTER			AIO_IN_VALUE_STR_POINTER+MAX_VALUE_SIZE+1
#define		AIO_OUT_VALUE_POINTER			AIO_IN_DRIVER_POINTER+COUNTER_INT
#define		AIO_OUT_VALUE_STR_POINTER		AIO_OUT_VALUE_POINTER+COUNTER_DOUBLE
#define		AIO_OUT_DRIVER_POINTER			AIO_OUT_VALUE_STR_POINTER+MAX_VALUE_SIZE+1
#define		AIO_SELECT_POINTER				AIO_OUT_DRIVER_POINTER+COUNTER_INT
#define		AIO_NAMEREAD_POINTER			AIO_SELECT_POINTER+COUNTER_INT
#define		AIO_DATAREAD_POINTER			AIO_NAMEREAD_POINTER+COUNTER_INT
#define		AIO_DRVREAD_POINTER				AIO_DATAREAD_POINTER+COUNTER_INT
#define		AIO_TOTAL						AIO_DRVREAD_POINTER+COUNTER_INT


#define		A_IO_NAME( ind )						  ((char *) A_IOP[ind] + AIO_NAME_POINTER)
#define		A_IO_NAMEL( ind )						  ((char *) A_IOP[ind] + AIO_NAMEL_POINTER)
#define		A_IO_TYPE( ind )				*(int   *)((char *) A_IOP[ind] + AIO_TYPE_POINTER)
#define		A_IO_IN_VALUE( ind )			*(double*)((char *) A_IOP[ind] + AIO_IN_VALUE_POINTER)
#define		A_IO_IN_VALUE_STR( ind )			      ((char *) A_IOP[ind] + AIO_IN_VALUE_STR_POINTER)
#define		A_IO_IN_DRIVER( ind )			*(int   *)((char *) A_IOP[ind] + AIO_IN_DRIVER_POINTER)
#define		A_IO_OUT_VALUE( ind )			*(double*)((char *) A_IOP[ind] + AIO_OUT_VALUE_POINTER)
#define		A_IO_OUT_VALUE_STR( ind )			      ((char *) A_IOP[ind] + AIO_OUT_VALUE_STR_POINTER)
#define		A_IO_OUT_DRIVER( ind )			*(int   *)((char *) A_IOP[ind] + AIO_OUT_DRIVER_POINTER)
#define		A_IO_SELECT( ind )				*(int   *)((char *) A_IOP[ind] + AIO_SELECT_POINTER)
#define		A_IO_NAMEREAD( ind )			*(int   *)((char *) A_IOP[ind] + AIO_NAMEREAD_POINTER)
#define		A_IO_DATAREAD( ind )			*(int   *)((char *) A_IOP[ind] + AIO_DATAREAD_POINTER)
#define		A_IO_DRVREAD( ind )				*(int   *)((char *) A_IOP[ind] + AIO_DRVREAD_POINTER)


// String Table
#define		SIO_NAME_POINTER				0
#define		SIO_NAMEL_POINTER				SIO_NAME_POINTER+MAX_CHANNEL_NAME_SIZE+1
#define		SIO_DATA_POINTER				SIO_NAMEL_POINTER+MAX_CHANNEL_NAME_SIZE+1
#define		SIO_IN_DRIVER_POINTER			SIO_DATA_POINTER+MAX_STRING_SIZE+MAX_STRING_SIZE+2 // 2005.05.11
//#define		SIO_IN_DRIVER_POINTER			SIO_DATA_POINTER+MAX_STRING_SIZE+1 // 2005.05.11
#define		SIO_OUT_DRIVER_POINTER			SIO_IN_DRIVER_POINTER+COUNTER_INT
#define		SIO_SELECT_POINTER				SIO_OUT_DRIVER_POINTER+COUNTER_INT
#define		SIO_NAMEREAD_POINTER			SIO_SELECT_POINTER+COUNTER_INT
#define		SIO_DATAREAD_POINTER			SIO_NAMEREAD_POINTER+COUNTER_INT
#define		SIO_DRVREAD_POINTER				SIO_DATAREAD_POINTER+COUNTER_INT
#define		SIO_TOTAL						SIO_DRVREAD_POINTER+COUNTER_INT

#define		S_IO_NAME( ind )				          ((char *) S_IOP[ind] + SIO_NAME_POINTER)
#define		S_IO_NAMEL( ind )				          ((char *) S_IOP[ind] + SIO_NAMEL_POINTER)
#define		S_IO_DATA( ind )				          ((char *) S_IOP[ind] + SIO_DATA_POINTER)
#define		S_IO_IN_DRIVER( ind )			*(int   *)((char *) S_IOP[ind] + SIO_IN_DRIVER_POINTER)
#define		S_IO_OUT_DRIVER( ind )			*(int   *)((char *) S_IOP[ind] + SIO_OUT_DRIVER_POINTER)
#define		S_IO_SELECT( ind )				*(int   *)((char *) S_IOP[ind] + SIO_SELECT_POINTER)
#define		S_IO_NAMEREAD( ind )			*(int   *)((char *) S_IOP[ind] + SIO_NAMEREAD_POINTER)
#define		S_IO_DATAREAD( ind )			*(int   *)((char *) S_IOP[ind] + SIO_DATAREAD_POINTER)
#define		S_IO_DRVREAD( ind )				*(int   *)((char *) S_IOP[ind] + SIO_DRVREAD_POINTER)

//
//
//
extern void	*R_IOP[ MAX_DRIVER_TABLE ];
//extern int	DRIVER_CNT;

extern void	*D_IOP[ MAX_DIGITAL_TABLE ];
//extern int	DIGITAL_CNT;

extern void	*A_IOP[ MAX_ANALOG_TABLE ];
//extern int	ANALOG_CNT;

extern void	*S_IOP[ MAX_STRING_TABLE ];
//extern int	STRING_CNT;

int DIGITAL_SELECT_COUNT();
int ANALOG_SELECT_COUNT();
int STRING_SELECT_COUNT();

int DIGITAL_TOTAL_COUNT();
int ANALOG_TOTAL_COUNT();
int STRING_TOTAL_COUNT();
//==============================================================================================================
BOOL READ_IO_INFORMATION();
void READ_IO_INFORMATION_FOR_DISPLAY_DATA( int mode , int i );
//
void SEARCH_SET_STRING( HWND hdlg , int mode , int btn );
void SEARCH_SET_MODE( HWND hdlg , int mode , int btn );
void SEARCH_SET_DRIVER_IN( HWND hdlg , int mode , int btn );
void SEARCH_SET_DRIVER_OUT( HWND hdlg , int mode , int btn );
void SEARCH_SET_ANDOR( HWND hdlg , int mode , int btn );
//
void SEARCH_CONTROL( HWND hdlg );
//==============================================================================================================
void ConGui_START_POINTER_RESET();
//==============================================================================================================
//==============================================================================================================

typedef enum {
	IOLockDO,
	IO_Driver_Loding,

} IOPointIndex;
#endif
