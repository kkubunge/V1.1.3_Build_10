#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>
#include <wchar.h>
#include <direct.h>

#include <Kutlstr.h>
#include <Kutlfile.h>
#include <Kutltime.h>
#include <Kutlgui.h>
#include <Kutlanal.h>

#include "resource.h"

#include "cimseqnc.h"
#include "iodefine.h"
#include "ctl_code.h"
#include "plt_code.h"
#include "plt_parm.h"
#include "dat_code.h"

#include <CimSeqncKNFS.h>

int Module_Number = 0;
char LogName[64] ="";
//------------------------------------------------------------------------------------------
BOOL Util_Prepare_Folder( char * );
//------------------------------------------------------------------------------------------

BOOL FindPosFromDataBuffer( int p , int VPos , int *Res ) {
	int Pos;
	if ( A_IO_PLT_COUNT(p) < 0    ) return FALSE;
	if ( A_IO_PLT_COUNT(p) < VPos ) return FALSE;
	if ( VPos < 1 && VPos > MAX_PLOTTING_SIZE ) return FALSE;
	Pos = A_IO_PLT_START(p) + VPos - 1;
	*Res = ( ( Pos >= MAX_PLOTTING_SIZE ) ? ( Pos - MAX_PLOTTING_SIZE ) : Pos );
	return( TRUE );
}

//------------------------------------------------------------------------------------------
void Plotting_Data_Save( int p , int n , long time_data , BOOL RealIO , double Data ) {
	if ( RealIO ) {
		A_IO_PLT_DATA( p , n ) = A_IO_VALUE( p );
	}
	else {
		A_IO_PLT_DATA( p , n ) = Data;
	}
	A_IO_PLT_TIME( p , n ) = time_data;
}
//------------------------------------------------------------------------------------------
void Plotting_Data_Get( int p , long time_data , BOOL RealIO , double Data ) {
	if      ( A_IO_PLT_COUNT(p) == 0 ) {
		A_IO_PLT_START(p)	= 0;
		A_IO_PLT_END(p)		= 0;
		A_IO_PLT_COUNT(p)	= 1;
	}
	else if ( ( A_IO_PLT_START(p) == 0 ) && ( A_IO_PLT_END(p) < MAX_PLOTTING_SIZE-1 ) ) {
		(A_IO_PLT_END(p))++;
		A_IO_PLT_COUNT(p)	= A_IO_PLT_END(p) + 1;
	}
	else if ( ( A_IO_PLT_START(p) == 0 ) && ( A_IO_PLT_END(p) >= MAX_PLOTTING_SIZE-1 ) ) {
		A_IO_PLT_END(p)		= 0;
		A_IO_PLT_START(p)	= 1;
		A_IO_PLT_COUNT(p)	= MAX_PLOTTING_SIZE;
	}
	else if ( ( A_IO_PLT_START(p) != 0 ) && ( A_IO_PLT_START(p) < MAX_PLOTTING_SIZE-1 ) ) {
		(A_IO_PLT_END(p))++;
		A_IO_PLT_START(p)	= A_IO_PLT_END(p)+1;
		A_IO_PLT_COUNT(p)	= MAX_PLOTTING_SIZE;
	}
	else if ( ( A_IO_PLT_START(p) !=  0 ) && ( A_IO_PLT_START(p) >= MAX_PLOTTING_SIZE-1 ) ) {
		(A_IO_PLT_END(p))++;
		A_IO_PLT_START(p)	= 0;
		A_IO_PLT_COUNT(p)	= MAX_PLOTTING_SIZE;
	}
	Plotting_Data_Save( p , A_IO_PLT_END(p) , time_data , RealIO , Data );
}
//------------------------------------------------------------------------------------------
void Plotting_Data_Set_For_Auto_Scale( int trg ) {
	int i , x;
	if ( trg <= 0 ) {
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
//			A_IO_PLT_DEV( i ) = 2; // 2005.05.24
			A_IO_PLT_DEV( i ) = V_AUTO_SCALE_DEFAULT_DEV(); // 2005.05.24
		}
	}
	else {
		if (
			( V_PLOTTER_DRAW_MODE( SCREEN_1 ) == SCR_BIG_DRAW ) ||
			( V_PLOTTER_DRAW_MODE( SCREEN_2 ) == SCR_BIG_DRAW ) ) {
			i = SCR_BIG_DRAW;
		}
		else {
			i = SCR_SMALL_DRAW;
		}
		x = Get_Graph_Plotting_Count( 0 , i );
		if ( ( x - 2 ) < A_IO_PLT_COUNT(0) ) {
			for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
				if ( A_IO_PLT_DEV( i ) > -12 ) (A_IO_PLT_DEV( i ))--;
			}
		}
	}
}
//------------------------------------------------------------------------------------------
BOOL Plotting_Data_Set_For_Zoom_Scale( int scr , int x1 , int x2 , int y1 , int y2 ) {
	int i , s , x , r , m , cs , xs , psx , xc , ct;
	double dymax , dymin , dbmax , dbmin;
	if ( x1 >= x2 ) return FALSE;
	if ( y1 >= y2 ) return FALSE;
	//
	if ( V_PLOTTER_DRAW_MODE( scr ) == SCR_BIG_DRAW ) {
		s = DRAW_SIZE_X_BIG / ( x2 - x1 );
		psx  = x1 - PLT0_X_START;
		//
		dymax = (double) ( DRAW_SIZE_Y_BIG - ( y1 - PLT0_Y_START ) ) / (double) DRAW_SIZE_Y_BIG;
		dymin = (double) ( DRAW_SIZE_Y_BIG - ( y2 - PLT0_Y_START ) ) / (double) DRAW_SIZE_Y_BIG;
	}
	else {
		s = DRAW_SIZE_X_SMALL / ( x2 - x1 );
		if ( scr == SCREEN_1 ) {
			psx = x1 - PLT1_X_START;
			dymax = (double) ( DRAW_SIZE_Y_SMALL - ( y1 - PLT1_Y_START ) ) / (double) DRAW_SIZE_Y_SMALL;
			dymin = (double) ( DRAW_SIZE_Y_SMALL - ( y2 - PLT1_Y_START ) ) / (double) DRAW_SIZE_Y_SMALL;
		}
		else {
			psx = x1 - PLT2_X_START;
			dymax = (double) ( DRAW_SIZE_Y_SMALL - ( y1 - PLT2_Y_START ) ) / (double) DRAW_SIZE_Y_SMALL;
			dymin = (double) ( DRAW_SIZE_Y_SMALL - ( y2 - PLT2_Y_START ) ) / (double) DRAW_SIZE_Y_SMALL;
		}
	}

	if ( s <= 0 ) return FALSE;
	m = -1;
	for ( i = 0 ; ( m == -1 ) && ( i < ANALOG_CNT ) ; i++ ) {
		switch( A_IO_PLT_DISPLAY( i ) ) {
		case DRAW_1_LEFT :
		case DRAW_1_RIGHT :
			if ( scr == SCREEN_1 ) {
				m = i;
			}
			break;
		case DRAW_2_LEFT :
		case DRAW_2_RIGHT :
			if ( scr == SCREEN_2 ) {
				m = i;
			}
			break;
		}
	}
	if ( m == -1 ) return FALSE;
	//
	if ( A_IO_PLT_COUNT( m ) < 0 ) return FALSE;
	if ( A_IO_PLT_COUNT( m ) == 1 ) return FALSE;

	xc = Get_Graph_Plotting_Count( m , V_PLOTTER_DRAW_MODE( scr ) );
	if ( DISP_LAST_POINTER[scr] < 1 ) {
		cs = A_IO_PLT_COUNT( m ) - xc + 1;
		if ( V_START_LEFT_DRAW() )
			xs = 0;
		else
			xs = Get_Graph_Plotting_Draw_Count( m , V_PLOTTER_DRAW_MODE( scr ) , A_IO_PLT_COUNT( m ) );
	}
	else {
		if ( DISP_LAST_POINTER[scr] > A_IO_PLT_COUNT( m ) ) {
			cs = A_IO_PLT_COUNT( m ) - xc + 1;
			if ( V_START_LEFT_DRAW() )
				xs = 0;
			else
				xs = Get_Graph_Plotting_Draw_Count( m , V_PLOTTER_DRAW_MODE( scr ) , A_IO_PLT_COUNT( m ) );
		}
		else {
			if ( DISP_LAST_POINTER[scr] < xc ) {
				if ( xc > A_IO_PLT_COUNT( m ) ) {
					cs = 1;
					if ( V_START_LEFT_DRAW() )
						xs = 0;
					else
						xs = Get_Graph_Plotting_Draw_Count( m , V_PLOTTER_DRAW_MODE( scr ) , A_IO_PLT_COUNT( m ) );
				}
				else {
					cs = 1;
					if ( V_START_LEFT_DRAW() )
						xs = 0;
					else
						xs = Get_Graph_Plotting_Draw_Count( m , V_PLOTTER_DRAW_MODE( scr ) , xc );
				}
			}
			else {
				cs = DISP_LAST_POINTER[scr] - xc + 1;
				if ( V_START_LEFT_DRAW() )
					xs = 0;
				else
					xs = Get_Graph_Plotting_Draw_Count( m , V_PLOTTER_DRAW_MODE( scr ) , DISP_LAST_POINTER[scr] );
			}
		}
	}
	if ( cs < 1 ) cs = 1;
	if ( xs < 0 ) xs = 0;
	//
	if ( psx < xs ) return FALSE;
	psx = psx - xs;
	//
	xc = Get_Graph_Plotting_Dev( m );

	if ( xc > 0 ) {
		psx = psx / xc;
	}
	else {
		psx = psx * ( 2 - xc );
	}

	if ( ( cs + psx ) > A_IO_PLT_COUNT( m ) ) return FALSE;

	x = ( A_IO_PLT_DEV( m ) ) + s;
	if ( x > 12 ) return FALSE;

	if ( V_PLOTTER_DRAW_MODE( scr ) == SCR_BIG_DRAW ) {
		if ( x <= 0 ) {
			ct = DRAW_SIZE_X_BIG * ( 2 - x );
		}
		else {
			ct = DRAW_SIZE_X_BIG / x;
		}
	}
	else {
		if ( x <= 0 ) {
			ct = DRAW_SIZE_X_SMALL * ( 2 - x );
		}
		else {
			ct = DRAW_SIZE_X_SMALL / x;
		}
	}

	if ( ( cs + psx + ct ) > A_IO_PLT_COUNT( m ) ) {
		DISP_LAST_POINTER[scr] = A_IO_PLT_COUNT( m );
	}
	else {
		DISP_LAST_POINTER[scr] = cs + psx + ct;
	}
	r = FALSE;
	for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
		switch( A_IO_PLT_DISPLAY( i ) ) {
		case DRAW_1_LEFT :
		case DRAW_1_RIGHT :
			if ( scr == SCREEN_1 ) {
				if ( A_IO_ZOOM( i ) == 0 ) {
					A_IO_ZOOM( i ) = scr+1;
					A_IO_MIN_OLD( i ) = A_IO_MIN( i );
					A_IO_MAX_OLD( i ) = A_IO_MAX( i );
					A_IO_DEV_OLD( i ) = A_IO_PLT_DEV( i );
				}
				//
				x = ( A_IO_PLT_DEV( i ) ) + s;
				if ( x <= 12 ) A_IO_PLT_DEV( i ) = x;
				//
				dbmax = A_IO_MAX( i ) - A_IO_MIN( i );
				dbmin = A_IO_MIN( i );
				A_IO_MAX( i ) = ( dbmax * dymax ) + dbmin;
				A_IO_MIN( i ) = ( dbmax * dymin ) + dbmin;
				//
				r = TRUE;
			}
			break;
		case DRAW_2_LEFT :
		case DRAW_2_RIGHT :
			if ( scr == SCREEN_2 ) {
				if ( A_IO_ZOOM( i ) == 0 ) {
					A_IO_ZOOM( i ) = scr+1;
					A_IO_MIN_OLD( i ) = A_IO_MIN( i );
					A_IO_MAX_OLD( i ) = A_IO_MAX( i );
					A_IO_DEV_OLD( i ) = A_IO_PLT_DEV( i );
				}
				//
				x = ( A_IO_PLT_DEV( i ) ) + s;
				if ( x <= 12 ) A_IO_PLT_DEV( i ) = x;
				//
				dbmax = A_IO_MAX( i ) - A_IO_MIN( i );
				dbmin = A_IO_MIN( i );
				A_IO_MAX( i ) = ( dbmax * dymax ) + dbmin;
				A_IO_MIN( i ) = ( dbmax * dymin ) + dbmin;
				//
				r = TRUE;
			}
			break;
		}
	}
	return r;
}
//------------------------------------------------------------------------------------------
BOOL Plotting_Zoom_Recover( int scr ) { // 2002.04.11
	int i;
	BOOL Run = FALSE;
	if ( scr == -1 ) {
		DISP_LAST_POINTER[ SCREEN_1 ] = 0;
		DISP_LAST_POINTER[ SCREEN_2 ] = 0;
	}
	else {
		DISP_LAST_POINTER[ scr ] = 0;
	}
	for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
		if ( A_IO_ZOOM( i ) != 0 ) {
			if ( ( scr == -1 ) || ( (scr+1) == A_IO_ZOOM( i ) ) ) {
				Run = TRUE;
				A_IO_ZOOM( i ) = 0;
				A_IO_MIN( i ) = A_IO_MIN_OLD( i );
				A_IO_MAX( i ) = A_IO_MAX_OLD( i );
				A_IO_PLT_DEV( i ) = A_IO_DEV_OLD( i );
			}
		}
	}
	return Run;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL INSIDE_DATALOG_RUNNING = FALSE;
int  INSIDE_DATALOG_POINT   = 0;
int  INSIDE_DATALOG_DRAW    = 0;
char INSIDE_DATALOG_FILENAME[256] = { 0, };
BOOL INSIDE_DISP_GO_READ_MODE = FALSE;

//--------------------------------------------------------------------------------------------------
BOOL DATA_LOGGING_STATUS() {
	return INSIDE_DATALOG_RUNNING;
}
//
char *DATA_LOGGING_FILENAME() {
	return INSIDE_DATALOG_FILENAME;
}
//
//--------------------------------------------------------------------------------------------------
BOOL DISP_GO_READ_MODE_GET() {
	return INSIDE_DISP_GO_READ_MODE;
}
//
void DISP_GO_READ_MODE_SET( BOOL data ) {
	int i;
	INSIDE_DISP_GO_READ_MODE = data;
	if ( data ) {
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
			A_IO_PLT_COUNT( i ) = 0;
			A_IO_SAVE_OLD( i ) = A_IO_SAVE( i );
			A_IO_SAVE( i ) = FALSE;
		}
	}
	else {
		time( &DISPLAY_START_TIME );
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
			A_IO_PLT_COUNT( i ) = 0;
			A_IO_SAVE( i ) = A_IO_SAVE_OLD( i );
		}
	}
	A_IO_MONITOR_REFORM = TRUE;
}
//
void DISP_GO_READ_MODE_INIT() {
	int i;
	for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
		A_IO_PLT_COUNT( i ) = 0;
	}
}
//
void DISP_WAIT_REFORM_COMPLETED() { // 2005.06.15
	while( A_IO_MONITOR_REFORM ) {
		_sleep(100);
	}
}
//
//--------------------------------------------------------------------------------------------------
BOOL Console_Datalog_Read( HWND hdlg , BOOL Clear ) {
	HFILE hFile;
	char FileName[256];
	//---> Modify 2012/04/21 READ Buffer 증가   by KSM
	//char Buffer[ 1025 ] , Buffer2[ 256 ];
	char Buffer[ 3072 ] , Buffer2[ 256 ];

	char chr_return1[ 256 ];
	char chr_return2[ 256 ];
	int  i , j , ReadCnt , Line , ChanPath[ MAX_ANALOG_TABLE ];
	long time_data;
	BOOL FileEnd = TRUE , Start = FALSE , Find = FALSE;
	BOOL Result;

	memset(FileName			,0x00, sizeof(FileName));
	memset(Buffer			,0x00, sizeof(Buffer));
	memset(Buffer2			,0x00, sizeof(Buffer2));
	memset(chr_return1		,0x00, sizeof(chr_return1));
	memset(chr_return2		,0x00, sizeof(chr_return2));



	for ( i = 0 ; i < MAX_ANALOG_TABLE ; i++ ) ChanPath[ i ] = -1;

	//===================================================================================================================
	// 2005.03.17
	//===================================================================================================================
//	strcpy( Buffer2 , "" );
//	if ( _getcwd( Buffer, 1024 ) != NULL ) {
//		strcpy( Buffer2 , DATALOG_DIRECTORY_NAME() );
//		if ( _chdir( Buffer2 ) ) strcpy( Buffer2 , "" );
//		else					 _chdir( Buffer );
//	}
	//===================================================================================================================
	//
	//===================================================================================================================
	// 2005.03.17
	//===================================================================================================================
	strcpy( Buffer2 , DATALOG_DIRECTORY_NAME() );
	//===================================================================================================================

	if ( !REMOTE_MONITOR_MODE ) {
		Result = MODAL_FILE_OPEN_BOX( hdlg , Buffer2 , "Open Datalogged File" , "*.*" , "subdir|newfirst" , FileName );
	}
	else { // 2005.03.17
		//
		MODAL_SET_REMOTE_ACCESS_API_FOR_FINDFILE( _nINF_KNFS_FindFirstFile , _nINF_KNFS_FindNextFile , _nINF_KNFS_FindClose );
		//		
		Result = MODAL_FILE_OPEN_BOX( hdlg , Buffer2 , "Open Datalogged File" , "*.*" , "subdir|newfirst|REMOTE" , Buffer );
		//	
		if ( Result ) {
			sprintf( FileName , "%s/%s" , REMOTE_MONITOR_FOLDER , Buffer );
			if ( !_nINF_KNFS_PrepareLocalFolder_for_Filename( FileName ) ) {
				Result = FALSE;
				MessageBox( hdlg , "Datalog File Get Prepare Error for this System" , Buffer , MB_ICONQUESTION );
			}
			else {
				if ( !_nINF_KNFS_GetFile( Buffer , FileName , FALSE ) ) {
					Result = FALSE;
					MessageBox( hdlg , "Datalog File Get Error from Other System" , Buffer , MB_ICONQUESTION );
				}
			}
		}
	}
	//
	if ( Result ) {
		hFile = _lopen( FileName , OF_READ );
		if ( hFile == -1 ) {
			MessageBox( hdlg , "Datalog File Open Error" , FileName , MB_ICONQUESTION );
			return FALSE;
		}
		//===================================================================================================================
		// 2005.03.17
		//===================================================================================================================
		if ( !REMOTE_MONITOR_MODE ) {
			if ( Clear ) DISP_GO_READ_MODE_SET( TRUE );
		}
		else {
			DISP_GO_READ_MODE_INIT();
		}
		//===================================================================================================================
		for ( Line = 1 ; FileEnd ; Line++ ) {
			FileEnd = H_Get_Line_String_From_File( hFile , Buffer , &ReadCnt );
			if ( Line == 1 ) {
				if ( ReadCnt > 0 ) {
					if ( Get_Data_From_String( Buffer , chr_return1 , 0 ) ) {
						chr_return1[16] = 0x00;
						if ( !STRCMP_L( "!DatalogFileV1.0" , chr_return1 ) ) {
							MessageBox( hdlg , "Datalog File Header Error 1" , FileName , MB_ICONQUESTION );
							_lclose( hFile );
							return TRUE;
						}
					}
					else {
						MessageBox( hdlg , "Datalog File Header Error 2" , FileName , MB_ICONQUESTION );
						_lclose( hFile );
						return TRUE;
					}
					if ( Get_Data_From_String( Buffer , chr_return1 , 1 ) ) {
						DISPLAY_START_TIME = atol( chr_return1 );
					}
					else {
						MessageBox( hdlg , "Datalog File Header Error 3" , FileName , MB_ICONQUESTION );
						_lclose( hFile );
						return TRUE;
					}
				}
				else {
					MessageBox( hdlg , "Datalog File Header Error 4" , FileName , MB_ICONQUESTION );
					_lclose( hFile );
					return TRUE;
				}
				continue;
			}
			if ( ReadCnt > 0 ) {
				if ( !Get_Data_From_String( Buffer , chr_return1 , 0 ) ) {
					MessageBox( hdlg , "Datalog File Error 1" , FileName , MB_ICONQUESTION );
					_lclose( hFile );
					return TRUE;
				}
				if ( STRCMP_L( ">" , chr_return1 ) ) {
					if ( Start ) {
						MessageBox( hdlg , "Datalog File Channel Define Already Error" , FileName , MB_ICONQUESTION );
						_lclose( hFile );
						return TRUE;
					}
					Start = TRUE;

					if ( !Get_Data_From_String( Buffer , chr_return2 , 1 ) ) {
						MessageBox( hdlg , "Datalog File Error 2" , FileName , MB_ICONQUESTION );
						_lclose( hFile );
						return TRUE;
					}
					else {
						if ( !STRCMP_L( "Time" , chr_return2 ) ) {
							MessageBox( hdlg , "Datalog File Channel Info Error" , FileName , MB_ICONQUESTION );
							_lclose( hFile );
							return TRUE;
						}
						Find = FALSE;
						for ( j = 2 ; j < ReadCnt ; j++ ) {
							if ( !Get_Data_From_String( Buffer , chr_return2 , j ) ) {
								MessageBox( hdlg , "Datalog File Error --" , FileName , MB_ICONQUESTION );
								_lclose( hFile );
								return TRUE;
							}
							for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
								if ( STRCMP_L( A_IO_NAME( i ) , chr_return2 ) ) {
									Find = TRUE;
									ChanPath[ j - 2 ] = i;
									break;
								}
								if ( STRCMP_L( A_IO_SHOW( i ) , chr_return2 ) ) {
									Find = TRUE;
									ChanPath[ j - 2 ] = i;
									break;
								}
							}
						}
						if ( !Find ) {
							MessageBox( hdlg , "Datalog File has no matching with IO Error " , FileName , MB_ICONQUESTION );
							_lclose( hFile );
							return TRUE;
						}
						//
						for ( j = 0 ; j < ANALOG_CNT ; j++ )  A_IO_PLT_COUNT(j) = 0;
						//
					}
				}
				else if ( STRCMP_L( "Time" , chr_return1 ) ) { // 2002.03.05
					if ( Start ) {
						MessageBox( hdlg , "Datalog File Channel Define Already Error" , FileName , MB_ICONQUESTION );
						_lclose( hFile );
						return TRUE;
					}
					Start = TRUE;

					Find = FALSE;
					for ( j = 1 ; j < ReadCnt ; j++ ) {
						if ( !Get_Data_From_String( Buffer , chr_return2 , j ) ) {
							MessageBox( hdlg , "Datalog File Error --" , FileName , MB_ICONQUESTION );
							_lclose( hFile );
							return TRUE;
						}
						for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
							if ( STRCMP_L( A_IO_NAME( i ) , chr_return2 ) ) {
								Find = TRUE;
								ChanPath[ j - 1 ] = i;
								break;
							}
							if ( STRCMP_L( A_IO_SHOW( i ) , chr_return2 ) ) {
								Find = TRUE;
								ChanPath[ j - 1 ] = i;
								break;
							}
						}
					}
					if ( !Find ) {
						MessageBox( hdlg , "Datalog File has no matching with IO Error " , FileName , MB_ICONQUESTION );
						_lclose( hFile );
						return TRUE;
					}
					//
					for ( j = 0 ; j < ANALOG_CNT ; j++ )  A_IO_PLT_COUNT(j) = 0;
					//
				}
				else {
					if ( chr_return1[0] == '<' ) continue;
					if ( !Start ) {
						MessageBox( hdlg , "Datalog File Channel not define Error" , FileName , MB_ICONQUESTION );
						_lclose( hFile );
						return TRUE;
					}
					for ( j = 1 ; j < ReadCnt ; j++ ) {
						if ( ChanPath[ j - 1 ] >= 0 ) {
							if ( !Get_Data_From_String( Buffer , chr_return2 , j ) ) {
								MessageBox( hdlg , "Datalog File Error **" , FileName , MB_ICONQUESTION );
								_lclose( hFile );
								return TRUE;
							}
							time_data = Get_Time_Data_From_String( chr_return1 );
							Plotting_Data_Get( ChanPath[ j - 1 ] , time_data , FALSE , atof(chr_return2) );
						}
					}
					if ( V_AUTO_SCALE_DRAW() ) Plotting_Data_Set_For_Auto_Scale( 1 );
				}
			}
		}
		_lclose( hFile );
		return TRUE;
	}
	return FALSE;
}
//========================================================================================
//========================================================================================
//
//
void Console_Datalog_Save_Mode( HWND hdlg ) {
	int iRes;
	iRes = V_PLOTTER_SAVE_TAB_TYPE();
	if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Datalog Control" , "Space Align Save Mode|Tab Align Save Mode|Space Align Save(Wide Time)|Tab Align Save(Wide Time)" , &iRes ) ) {
		S_PLOTTER_SAVE_TAB_TYPE( iRes );
	}
}
//
//
//
//
void Console_Datalog_Time( HWND hdlg ) {
//	int iRes;
//	iRes = DATALOG_xTIME;
	double dRes;
	dRes = DATALOG_TIME;
	if ( DATALOG_CONTROL != -1 ) {
//		if ( MODAL_DIGITAL_BOX2( hdlg , "Select" , "Datalog Inteval Time" , 0 , 999 , &iRes ) ) {
//			DATALOG_TIME = iRes;
//		}
		if ( MODAL_ANALOG_BOX1( hdlg , "Select" , "Datalog Inteval Time" , 0 , 999 , 2 , &dRes ) ) {
			//
			DATALOG_TIME = dRes;
			//
		}
	}
	else {
		MessageBox( hdlg , "Datalog Information not define" , "No Use" , MB_ICONQUESTION );
	}
}
//
//
void Console_Datalog_Control( HWND hdlg ) {
	int iRes , commstatus;
	char Buffer[1025] , Buffer2[256];
	iRes = 1;
	
	memset(Buffer			,0x00, sizeof(Buffer));
	memset(Buffer2			,0x00, sizeof(Buffer2));

	
	
	if ( DATALOG_CONTROL != -1 ) {
		if ( INSIDE_DATALOG_RUNNING ) {
			if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Datalog Control" , "Stop Datalog|Cancel" , &iRes ) ) {
				if ( iRes == 0 ) _dWRITE_DIGITAL( DATALOG_CONTROL , 0 , &commstatus );
			}
		}
		else {
			if ( DATALOG_FILE == -1 ) {
				if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Datalog Control" , "Start Datalog|Cancel" , &iRes ) ) {
					if ( iRes == 0 ) {
						_dWRITE_DIGITAL( DATALOG_CONTROL , 1 , &commstatus );
					}
				}
			}
			else {
				if ( !SYSTEM_LOGSKIP_STATUS() ) { // 2005.03.17
					if ( MODAL_DIGITAL_BOX1( hdlg , "Select" , "Datalog Control" , "Start Datalog|Cancel" , &iRes ) ) {
						if ( iRes == 0 ) {
							//========================================================================================================
							// 2005.03.17
							//========================================================================================================
							if ( Util_Prepare_Folder( DATALOG_DIRECTORY_NAME() ) ) { // 2005.03.17
								strcpy( Buffer2 , DATALOG_DIRECTORY_NAME() );
							}
							else {
								strcpy( Buffer2 , "" );
							}
							//========================================================================================================
							// 2005.03.17
							//========================================================================================================
							//strcpy( Buffer2 , "" );
							//if ( _getcwd( Buffer, 1024 ) != NULL ) {
							//	strcpy( Buffer2 , DATALOG_DIRECTORY_NAME() );
							//	if ( _chdir( Buffer2 ) ) {
							//		if ( _mkdir( Buffer2 ) != 0 ) strcpy( Buffer2 , "" );
							//	}
							//	else {
							//		_chdir( Buffer );
							//	}
							//}
							//========================================================================================================
							strcpy( Buffer , "" );
							if ( MODAL_FILE_SAVE_BOX( hdlg , Buffer2 , "Save Datalog File" , "*.*" , "" , Buffer ) ) {
								_dWRITE_STRING( DATALOG_FILE , Buffer , &commstatus );
								_dWRITE_DIGITAL( DATALOG_CONTROL , 1 , &commstatus );
							}
						}
					}
				}
				else { // 2005.03.17
					MessageBox( hdlg , "Datalog System is Read Only Mode" , "Error" , MB_ICONQUESTION );
				}
			}
		}
	}
	else {
		MessageBox( hdlg , "Datalog Information not define" , "No Use" , MB_ICONQUESTION );
	}
}
//
//
HWND Con_dlog_err_HWND;
void Console_Datalog_Error() {
	MessageBox( Con_dlog_err_HWND , "Datalog Progressing is too long time" , "Stopped.." , MB_ICONQUESTION );
	_endthread();
}
//
//
BOOL DataLog_Header_Saving( char *Date , char *Time , char *File ) {
	FILE *fpt;
	FILE *fp;
	int i , iRes , CommStatus;
	double dRes;
	char uid[65];
	char Buffer[1025] , Buffer2[256];
	char ProcessLogName[128];

	//---> ADD 2008/08/27 HeaderItem 추가 by KSM
	char IO_Name[40];
	double dRes_AnalogIO;
	//---> END 2008/08/27 HeaderItem 추가 by KSM


	memset(Buffer  			,0x00, sizeof(Buffer));
	memset(Buffer2 			,0x00, sizeof(Buffer2));
	memset(ProcessLogName 	,0x00, sizeof(ProcessLogName));
	memset(IO_Name 			,0x00, sizeof(IO_Name));
	memset(uid 				,0x00, sizeof(uid));

	if ( SYSTEM_LOGSKIP_STATUS() ) return TRUE; // 2004.05.21

	//========================================================================================================
	// 2005.03.17
	//========================================================================================================
	if ( Util_Prepare_Folder( DATALOG_DIRECTORY_NAME() ) ) { // 2005.03.17
		strcpy( Buffer2 , DATALOG_DIRECTORY_NAME() );
	}
	else {
		strcpy( Buffer2 , "" );
	}
	//========================================================================================================
	// 2005.03.17
	//========================================================================================================
	//strcpy( Buffer2 , "" );
	//if ( _getcwd( Buffer, 1024 ) != NULL ) {
	//	strcpy( Buffer2 , DATALOG_DIRECTORY_NAME() );
	//	if ( _chdir( Buffer2 ) ) {
	//		if ( _mkdir( Buffer2 ) != 0 ) strcpy( Buffer2 , "" );
	//	}
	//	else {
	//		_chdir( Buffer );
	//	}
	//}
	//========================================================================================================
	if ( DATALOG_FILE == -1 ) {
		if ( strlen( Buffer2 ) == 0 ) sprintf( INSIDE_DATALOG_FILENAME , "%s.log" , File );
		else                          sprintf( INSIDE_DATALOG_FILENAME , "%s/%s.log" , Buffer2 , File );
	}
	else {
		_dREAD_STRING( DATALOG_FILE , Buffer , &CommStatus );
		if ( strlen( Buffer2 ) == 0 ) sprintf( INSIDE_DATALOG_FILENAME , "%s" , Buffer );
		else                          sprintf( INSIDE_DATALOG_FILENAME , "%s/%s" , Buffer2 , Buffer );
	}

	for ( i = 0 ; i < EVENT_CNT ; i++ ) {
		switch( E_IO_TYPE( i ) ) {
			case _K_F_IO :
					_ALARM_EVENT_RECEIVE(); 
					_ALARM_RECV_BUFFER_CLEAR();
					break;
			case _K_D_IO :
					iRes = _dREAD_STR_DIGITAL( E_IO_POINT( i ) , E_IO_OLD( i ) );
					break;
			case _K_A_IO :
					dRes = _dREAD_STR_ANALOG( E_IO_POINT( i ) , E_IO_OLD( i ) );
					break;
			case _K_S_IO :
					_dREAD_STRING( E_IO_POINT( i ) , E_IO_OLD( i ) , &CommStatus );
					break;
		}
	}
	//
	Directory_Make_Prepare_for_Filename( INSIDE_DATALOG_FILENAME );
	//
	if ( ( fpt = fopen( INSIDE_DATALOG_FILENAME , "w" ) ) == NULL ) return FALSE;
	GET_CURRENT_USERNAME( uid );
	fprintf( fpt , "!DatalogFileV1.0 %ld\n" , DATALOG_START_TIME );
	fprintf( fpt , "#\n" );
	fprintf( fpt , "# User : %s           Start : %s %s\n" , uid , Date , Time );

	
	//---> DEL 2008/08/27 HeaderItem 추가 by KSM
	//if ( DATALOG_STRING_IO != -1 ) {
	//	_dREAD_STRING( DATALOG_STRING_IO , Buffer , &CommStatus );
	//	fprintf( fpt , "# Data : %s\n" , Buffer );
	//}
	//---> END 2008/08/27 HeaderItem 추가 by KSM

	//---> ADD 2008/08/27 HeaderItem 추가 by KSM
	if ( DATALOG_STRING_IO3 != -1 ) {

		memset(IO_Name, 0x00, sizeof(IO_Name));
		_dSTRING_NAME( DATALOG_STRING_IO3 , IO_Name );

		_dREAD_STRING( DATALOG_STRING_IO3 , Buffer , &CommStatus );
		fprintf( fpt , "# %s : %s\n" , IO_Name, Buffer );

	}	
	if ( DATALOG_STRING_IO4 != -1 ) 
	{

		memset(IO_Name, 0x00, sizeof(IO_Name));
		_dSTRING_NAME( DATALOG_STRING_IO4 , IO_Name );

		_dREAD_STRING( DATALOG_STRING_IO4 , Buffer , &CommStatus );
		fprintf( fpt , "# %s : %s\n" , IO_Name, Buffer );
		
		//PM 구분
		
		if ( STRCMP_L ( Buffer, "PM1")) sprintf(ProcessLogName, "Parameter/PresentLogName_PM1.log", TRUE);
		else if( STRCMP_L ( Buffer, "PM2") ) sprintf(ProcessLogName, "Parameter/PresentLogName_PM2.log", TRUE);
		else if( STRCMP_L ( Buffer, "PM3") ) sprintf(ProcessLogName, "Parameter/PresentLogName_PM3.log", TRUE);

		//
		if ( ( fp = fopen( ProcessLogName , "w" ) ) == NULL ) 
		{
			printf( "Cannot Open ProcessLogName File\n" , TRUE );
		}
		else
		{
			fprintf( fp , "%s", LogName );
			fclose( fp );
		}
		
	}	
	if ( DATALOG_STRING_IO1 != -1 ) {

		memset(IO_Name, 0x00, sizeof(IO_Name));
		_dSTRING_NAME( DATALOG_STRING_IO1 , IO_Name );

		_dREAD_STRING( DATALOG_STRING_IO1 , Buffer , &CommStatus );
		fprintf( fpt , "# %s : %s\n" , IO_Name, Buffer );

	}
	if ( DATALOG_STRING_IO2 != -1 ) {

		memset(IO_Name, 0x00, sizeof(IO_Name));
		_dSTRING_NAME( DATALOG_STRING_IO2 , IO_Name );

		_dREAD_STRING( DATALOG_STRING_IO2 , Buffer , &CommStatus );
		fprintf( fpt , "# %s : %s\n" , IO_Name, Buffer );

	}

	//---> End    2008/08/27 HeaderItem 추가 by KSM

	
	//---> DEL 2008/08/27 HeaderItem 추가 by KSM
	//if ( DATALOG_DIGITAL_IO != -1 ) {
	//	iRes = _dREAD_DIGITAL( DATALOG_DIGITAL_IO , &CommStatus );
	//	fprintf( fpt , "# Clean Count : %d\n" , iRes );
	//}
	//---> END 2008/08/27 HeaderItem 추가 by KSM

	//---> ADD 2008/08/27 HeaderItem 추가 by KSM
	if ( DATALOG_DIGITAL_IO1 != -1 ) {
		
		memset(IO_Name, 0x00, sizeof(IO_Name));
		_dDIGITAL_NAME( DATALOG_DIGITAL_IO1 , IO_Name );

		iRes = _dREAD_DIGITAL( DATALOG_DIGITAL_IO1 , &CommStatus );

		fprintf( fpt , "# %s : %d\n" , IO_Name, iRes );
	}

	if ( DATALOG_DIGITAL_IO2 != -1 ) {

		memset(IO_Name, 0x00, sizeof(IO_Name));
		_dDIGITAL_NAME( DATALOG_DIGITAL_IO2 , IO_Name );

		iRes = _dREAD_DIGITAL( DATALOG_DIGITAL_IO2 , &CommStatus );
		fprintf( fpt , "# %s : %d\n" , IO_Name, iRes );

	}

	if ( DATALOG_DIGITAL_IO3 != -1 ) {

		memset(IO_Name, 0x00, sizeof(IO_Name));
		_dDIGITAL_NAME( DATALOG_DIGITAL_IO3 , IO_Name );

		iRes = _dREAD_DIGITAL( DATALOG_DIGITAL_IO3 , &CommStatus );
		fprintf( fpt , "# %s : %d\n" , IO_Name, iRes );

	}

	if ( DATALOG_ANALOG_IO1 != -1 ) {

		memset(IO_Name, 0x00, sizeof(IO_Name));
		_dANALOG_NAME( DATALOG_ANALOG_IO1 , IO_Name );

		dRes_AnalogIO = _dREAD_ANALOG( DATALOG_ANALOG_IO1 , &CommStatus );
		fprintf( fpt , "# %s : %f\n" , IO_Name, dRes_AnalogIO );

	}

	//---> End    2008/08/27 HeaderItem 추가 by KSM

	//=========================================================================
	fprintf( fpt , "#\n" );
	//
	switch ( V_PLOTTER_SAVE_TAB_TYPE() ) {
	case 1 :
	case 3 :
		fprintf( fpt , "#-------" );
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) if ( A_IO_SAVE( i ) ) fprintf( fpt , "+--------------------" );
//		fprintf( fpt , "\n> Time" ); // 2002.03.05
		fprintf( fpt , "\nTime" ); // 2002.03.05
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) if ( A_IO_SAVE( i ) ) fprintf( fpt , "%c%s" , 9 , A_IO_SHOW(i) );
		fprintf( fpt , "\n#-------" );
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) if ( A_IO_SAVE( i ) ) fprintf( fpt , "+--------------------" );
		fprintf( fpt , "\n" );
		break;
	case 0 :
	case 2 :
		fprintf( fpt , "#------------" );
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) if ( A_IO_SAVE( i ) ) fprintf( fpt , "+-------------------" );
//		fprintf( fpt , "\n> Time" ); // 2002.03.05
		fprintf( fpt , "\nTime          " ); // 2002.03.05
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) if ( A_IO_SAVE( i ) ) fprintf( fpt , "%-20s" , A_IO_SHOW(i) );
		fprintf( fpt , "\n#------------" );
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) if ( A_IO_SAVE( i ) ) fprintf( fpt , "+-------------------" );
		fprintf( fpt , "\n" );
		break;
	}
	//========================================================================================================
	// 2004.08.18
	//========================================================================================================
	for ( i = 0 ; i < EVENT_CNT ; i++ ) {
		switch( E_IO_TYPE( i ) ) {
			case _K_F_IO :
					break;
			case _K_D_IO :
					fprintf( fpt , "<START DIGITAL:%-8s> %s => %s\n" , Time , E_IO_SHOW( i ) , E_IO_OLD( i ) );
					break;
			case _K_A_IO :
					fprintf( fpt , "<START  ANALOG:%-8s> %s => %s\n" , Time , E_IO_SHOW( i ) , E_IO_OLD( i ) );
					break;
			case _K_S_IO :
					fprintf( fpt , "<START  STRING:%-8s> %s => %s\n" , Time , E_IO_SHOW( i ) , E_IO_OLD( i ) );
					break;
		}
	}
	//========================================================================================================
	fclose(fpt);

	return TRUE;
}
//
BOOL DataLog_Event_Saving( char *Time ) {
	FILE *fpt;
	int i , iRes , CommStatus;
	double dRes;
	char Result[256];

	memset(Result			,0x00, sizeof(Result));

	if ( SYSTEM_LOGSKIP_STATUS() ) return TRUE; // 2004.05.21

	for ( i = 0 ; i < EVENT_CNT ; i++ ) {
		switch( E_IO_TYPE( i ) ) {
			case _K_F_IO :
					while ( TRUE ) {
						if ( _ALARM_RECV_BUFFER_READ( Result , 255 ) ) {
							if ( ( fpt = fopen( INSIDE_DATALOG_FILENAME , "a" ) ) == NULL ) return FALSE;
							fprintf( fpt , "<EVENT ALARM> %s\n" , Result );
							fclose(fpt);
						}
						else {
							break;
						}
					}
					break;
			case _K_D_IO :
					iRes = _dREAD_STR_DIGITAL( E_IO_POINT( i ) , Result );
					if ( !STRCMP_L( Result , E_IO_OLD( i ) ) ) {
						strcpy( E_IO_OLD( i ) , Result );
						if ( ( fpt = fopen( INSIDE_DATALOG_FILENAME , "a" ) ) == NULL ) return FALSE;
						fprintf( fpt , "<EVENT DIGITAL:%-8s> %s => %s\n" , Time , E_IO_SHOW( i ) , E_IO_OLD( i ) );
						fclose(fpt);
					}
					break;
			case _K_A_IO :
					dRes = _dREAD_STR_ANALOG( E_IO_POINT( i ) , Result );
					if ( !STRCMP_L( Result , E_IO_OLD( i ) ) ) {
						strcpy( E_IO_OLD( i ) , Result );
						if ( ( fpt = fopen( INSIDE_DATALOG_FILENAME , "a" ) ) == NULL ) return FALSE;
						fprintf( fpt , "<EVENT  ANALOG:%-8s> %s => %s\n" , Time , E_IO_SHOW( i ) , E_IO_OLD( i ) );
						fclose(fpt);
					}
					break;
			case _K_S_IO :
					_dREAD_STRING( E_IO_POINT( i ) , Result , &CommStatus );
					if ( !STRCMP_L( Result , E_IO_OLD( i ) ) ) {
						strcpy( E_IO_OLD( i ) , Result );
						if ( ( fpt = fopen( INSIDE_DATALOG_FILENAME , "a" ) ) == NULL ) return FALSE;
						fprintf( fpt , "<EVENT  STRING:%-8s> %s => %s\n" , Time , E_IO_SHOW( i ) , E_IO_OLD( i ) );
						fclose(fpt);
					}
					break;
		}
	}
	return TRUE;
}
//------------------------------------------------------------------------

extern CRITICAL_SECTION CR_READ_DATA; // 2016.09.06

//
BOOL DataLog_Data_Saving( char *Time ) {
	FILE *fpt;
	int i;
	char szZipPath[256] = "";
	char szDeleteLogPath[256] = "";


	if ( SYSTEM_LOGSKIP_STATUS() ) return TRUE; // 2004.05.21

	if ( ( fpt = fopen( INSIDE_DATALOG_FILENAME , "a" ) ) == NULL ) return FALSE;
	//
	switch ( V_PLOTTER_SAVE_TAB_TYPE() ) {
	case 1 :
	case 3 :
		fprintf( fpt , "%s" , Time );
		//
		EnterCriticalSection( &CR_READ_DATA ); // 2016.09.06
		//
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
			if ( A_IO_SAVE( i ) ) {
				fprintf( fpt , "%c%s" , 9 , A_IO_VALUE_STR( i ) );
			}
		}
		//
		LeaveCriticalSection( &CR_READ_DATA ); // 2016.09.06
		//
		break;
	case 0 :
	case 2 :
		fprintf( fpt , "[%-12s]" , Time );
		//
		EnterCriticalSection( &CR_READ_DATA ); // 2016.09.06
		//
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
			if ( A_IO_SAVE( i ) ) {
				fprintf( fpt , "%-20s" , A_IO_VALUE_STR( i ) );
			}
		}
		//
		LeaveCriticalSection( &CR_READ_DATA ); // 2016.09.06
		//
		break;
	}
	//
	fprintf( fpt , "\n" );
	fclose(fpt);
	
	return TRUE;
}
//------------------------------------------------------------------------
void Plotting_Data_Polling( HWND hdlg , BOOL disp , BOOL Poll ) {
	int i , commstatus  , cio;
	BOOL Send_1 , Send_2 , chgdata;
	char Buffer1[32] , Buffer2[32] , Buffer3[256] , Buffer4[32];
	struct tm		*Pres_Time;
	HDC hDC;
	long time_data;
	//---> ADD 2012/04/19 PlotNameDetail  by KSM
	char szRcpNameBuf[1025],szLotIDBuf[1025],szDirName[256],EqidBuf[256],ChnameBuf[256] ;
	char szRcpNameBuf2[20],szLotIDBuf2[20],szDirName2[20],szWfrNo[10],EqidBuf2[20],ChnameBuf2[20];
	int	CommStatus;
	char *pt;
	
	//---> END 2012/04/19 PlotNameDetail  by KSM

	//---> ADD 2012/04/19 PlotNameDetail  by KSM
	memset(szRcpNameBuf ,0x00, sizeof(szRcpNameBuf));
	memset(szLotIDBuf   ,0x00, sizeof(szLotIDBuf));
	memset(szRcpNameBuf2,0x00, sizeof(szRcpNameBuf2));
	memset(szLotIDBuf2  ,0x00, sizeof(szLotIDBuf2));
	memset(szDirName    ,0x00, sizeof(szRcpNameBuf2));
	memset(szDirName2   ,0x00, sizeof(szLotIDBuf2));
	memset(szWfrNo      ,0x00, sizeof(szWfrNo));
	memset(EqidBuf      ,0x00, sizeof(EqidBuf));
	memset(EqidBuf2     ,0x00, sizeof(EqidBuf2));
	memset(ChnameBuf    ,0x00, sizeof(ChnameBuf));
	memset(ChnameBuf2   ,0x00, sizeof(ChnameBuf2));
	memset(Buffer1  	,0x00, sizeof(Buffer1));
	memset(Buffer2   	,0x00, sizeof(Buffer2));
	memset(Buffer3	    ,0x00, sizeof(Buffer3));
	memset(Buffer4	    ,0x00, sizeof(Buffer4));
	
	//---> END 2012/04/19 PlotNameDetail  by KSM

	Send_1 = FALSE;
	Send_2 = FALSE;
	chgdata = FALSE;

	if ( Poll && !DISP_GO_READ_MODE_GET() ) { // 2002.04.10
		time_data = Elapsed_Time( Buffer4 );
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
			if ( A_IO_SAVE( i ) ) {
				( A_IO_PLT_OLD( i ) )++;
				if ( A_IO_PLT_OLD( i ) >= ( A_IO_PLT_POLL( i ) * 2 ) ) {
					A_IO_PLT_OLD( i ) = 0;
					Plotting_Data_Get( i , time_data , TRUE , 0 );
					if      ( A_IO_PLT_DISPLAY(i) == DRAW_1_LEFT  ) Send_1 = TRUE;
					else if ( A_IO_PLT_DISPLAY(i) == DRAW_1_RIGHT ) Send_1 = TRUE;
					else if ( A_IO_PLT_DISPLAY(i) == DRAW_2_LEFT  ) Send_2 = TRUE;
					else if ( A_IO_PLT_DISPLAY(i) == DRAW_2_RIGHT ) Send_2 = TRUE;
				}
			}
			if ( A_IO_VALUE( i ) != A_IO_VALUE_OLD( i ) ) {
				A_IO_VALUE_OLD( i ) = A_IO_VALUE( i );
				chgdata = TRUE;
			}
		}
		if ( V_AUTO_SCALE_DRAW() ) Plotting_Data_Set_For_Auto_Scale( 1 );
	}
	else {
		for ( i = 0 ; i < ANALOG_CNT ; i++ ) {
			if      ( A_IO_PLT_DISPLAY(i) == DRAW_1_LEFT  ) Send_1 = TRUE;
			else if ( A_IO_PLT_DISPLAY(i) == DRAW_1_RIGHT ) Send_1 = TRUE;
			else if ( A_IO_PLT_DISPLAY(i) == DRAW_2_LEFT  ) Send_2 = TRUE;
			else if ( A_IO_PLT_DISPLAY(i) == DRAW_2_RIGHT ) Send_2 = TRUE;
		}
	}
	if ( DISP_LAST_POINTER[SCREEN_1] != DISP_LAST_OLD_POINTER[SCREEN_1] ) { // 2002.04.10
		DISP_LAST_OLD_POINTER[SCREEN_1] = DISP_LAST_POINTER[SCREEN_1];
		Send_1 = TRUE;
	}
	if ( DISP_LAST_POINTER[SCREEN_2] != DISP_LAST_OLD_POINTER[SCREEN_2] ) { // 2002.04.10
		DISP_LAST_OLD_POINTER[SCREEN_2] = DISP_LAST_POINTER[SCREEN_2];
		Send_2 = TRUE;
	}

	if ( Console_Drawing_Zoom_Draw_Check( SCREEN_1 ) ) Send_1 = TRUE;
	if ( Console_Drawing_Zoom_Draw_Check( SCREEN_2 ) ) Send_2 = TRUE;

	if ( DATALOG_CONTROL != -1 ) {
		cio = _dREAD_DIGITAL( DATALOG_CONTROL , &commstatus ); // 2002.08.08
		if ( !INSIDE_DATALOG_RUNNING ) {
			if ( ( cio >= 1 ) && ( cio <= 6 ) ) { // 2002.08.08
				//============================================================================================
				// 2005.06.15
				//============================================================================================
				if ( DISP_GO_READ_MODE_GET() ) {
					DISP_GO_READ_MODE_SET( FALSE );
					DISP_WAIT_REFORM_COMPLETED();
				}
				//============================================================================================
				if ( ( cio == 1 ) || ( cio == 3 ) || ( cio == 5 ) ) { // 2002.08.08
					time( &DATALOG_START_TIME );
					Pres_Time = localtime( &DATALOG_START_TIME );
					sprintf( Buffer1 , "%04d/%02d/%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday );
					sprintf( Buffer2 , "%02d:%02d:%02d" , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );
//
//					sprintf( Buffer3 , "%04d%02d%02d%02d%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min ); // 2004.01.09
//
					if ( DATALOG_DIRECTORY_SEPARATE() ) { // 2004.01.09
							sprintf( Buffer3 , "%04d%02d%02d/%04d%02d%02d%02d%02d%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );
					}
					else {
						//---> DEL 2012/04/21 PlotNameDetail  by KSM
						//sprintf( Buffer3 , "%04d%02d%02d%02d%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min );
						//---> END 2012/04/21 PlotNameDetail  by KSM
						
						//---> ADD 2012/04/21 PlotNameDetail  by KSM

						strcpy( szDirName , DATALOG_DIRECTORY_NAME() );
						pt = strstr( szDirName, "PM");
						if  ( pt != NULL )   // "PM" 가 포함되어 있는 경우
						{
							strcpy( szDirName2, "_");
							strncat( szDirName2 , pt+2, 4);
						}
						if ( DATALOG_STRING_IO4 != -1 ) 
						{
							_dREAD_STRING( DATALOG_STRING_IO4 , ChnameBuf, &CommStatus );

							if ( strlen(ChnameBuf ) > 0 )
							{
								strcpy( ChnameBuf2, "_");
								strncat( ChnameBuf2 , ChnameBuf, 10 );
							}
							else
							{
								strcpy( ChnameBuf2, "_Unknown");							
							}
						}
						if ( DATALOG_STRING_IO3 != -1 ) 
						{
							_dREAD_STRING( DATALOG_STRING_IO3 , EqidBuf, &CommStatus );

							if ( strlen(EqidBuf ) > 0 )
							{
								strcpy( EqidBuf2, "_");
								strncat( EqidBuf2 , EqidBuf, 10 );
							}
							else
							{
								strcpy( EqidBuf2, "_Unknown");							
							}
						}
						if ( DATALOG_STRING_IO2 != -1 ) 
						{
							_dREAD_STRING( DATALOG_STRING_IO2 , szLotIDBuf, &CommStatus );

							if ( strlen(szLotIDBuf ) > 0 )
							{
								strcpy( szLotIDBuf2, "_");
								strncat( szLotIDBuf2 , szLotIDBuf, 10 );
							}
							else
							{
								strcpy( szLotIDBuf2, "_Unknown");							
							}
						}

						if ( DATALOG_STRING_IO1 != -1 )
						{
							_dREAD_STRING( DATALOG_STRING_IO1 , szRcpNameBuf, &CommStatus );

							if ( strlen(szRcpNameBuf ) > 0 )
							{
								strcpy( szRcpNameBuf2, "_");
								strncat( szRcpNameBuf2 , szRcpNameBuf, 10 );
							}
							else
							{
								strcpy( szRcpNameBuf2, "_Unknown");							
							}
						}

						if ( DATALOG_DIGITAL_IO1 != -1 && DATALOG_DIGITAL_IO2 != -1 )
						{
							sprintf( szWfrNo , "_%02d%02d" , _dREAD_DIGITAL( DATALOG_DIGITAL_IO1 , &CommStatus )
								                           , _dREAD_DIGITAL( DATALOG_DIGITAL_IO2 , &CommStatus ) );
						}

//						sprintf( Buffer3 , "%04d%02d%02d%02d%02d%s%s%s%s" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min, szDirName2, szLotIDBuf2, szRcpNameBuf2, szWfrNo );
						sprintf( Buffer3 , "%04d-%02d-%02d-%02d-%02d%s%s_SensorData" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min, EqidBuf2, szRcpNameBuf2 );
						sprintf( LogName, Buffer3 );

					
						//---> END 2012/04/21 PlotNameDetail  by KSM
					}
//
					if ( DataLog_Header_Saving( Buffer1 , Buffer2 , Buffer3 ) ) {
						INSIDE_DATALOG_POINT   = 0;
						INSIDE_DATALOG_RUNNING = TRUE;
						DATALOG_TIME_OLD = 9999;
					}
					else {
						INSIDE_DATALOG_RUNNING = FALSE;
						_dWRITE_DIGITAL( DATALOG_CONTROL , 0 , &commstatus );
					}
				}
				else { // 2002.08.08
					if ( strlen( INSIDE_DATALOG_FILENAME ) <= 0 ) {
						time( &DATALOG_START_TIME );
						Pres_Time = localtime( &DATALOG_START_TIME );
						sprintf( Buffer1 , "%04d/%02d/%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday );
						sprintf( Buffer2 , "%02d:%02d:%02d" , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );
//
//						sprintf( Buffer3 , "%04d%02d%02d%02d%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min ); // 2004.01.09
//
						if ( DATALOG_DIRECTORY_SEPARATE() ) { // 2004.01.09
							//sprintf( Buffer3 , "%04d%02d%02d/%04d%02d%02d%02d%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min );
							sprintf( Buffer3 , "%04d%02d%02d/%04d%02d%02d%02d%02d%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );

						}
						else {
							//sprintf( Buffer3 , "%04d%02d%02d%02d%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min );
							sprintf( Buffer3 , "%04d%02d%02d%02d%02d%02d" , Pres_Time->tm_year+1900 , Pres_Time->tm_mon + 1 , Pres_Time->tm_mday , Pres_Time->tm_hour , Pres_Time->tm_min , Pres_Time->tm_sec );

						}
//
						if ( DataLog_Header_Saving( Buffer1 , Buffer2 , Buffer3 ) ) {
							INSIDE_DATALOG_POINT   = 0;
							INSIDE_DATALOG_RUNNING = TRUE;
							DATALOG_TIME_OLD = 9999;
						}
						else {
							INSIDE_DATALOG_RUNNING = FALSE;
							_dWRITE_DIGITAL( DATALOG_CONTROL , 0 , &commstatus );
						}
					}
					else {
						INSIDE_DATALOG_RUNNING = TRUE;
						DATALOG_TIME_OLD = 9999;
					}
				}
			}
		}
		else {
			if ( ( cio < 1 ) || ( cio > 6 ) ) { // 2002.08.08
				INSIDE_DATALOG_RUNNING = FALSE;
			}
			else if ( INSIDE_DATALOG_POINT > 100000 ) {
				INSIDE_DATALOG_RUNNING = FALSE;
				_dWRITE_DIGITAL( DATALOG_CONTROL , 0 , &commstatus );
				if ( hdlg != NULL ) Con_dlog_err_HWND = hdlg;
				else                Con_dlog_err_HWND = GetDesktopWindow();
				_beginthread( (void *) Console_Datalog_Error , 0 , 0 );
			}
		}
		if ( Poll && INSIDE_DATALOG_RUNNING ) {
			if ( !DataLog_Event_Saving( Buffer2 ) ) {
				INSIDE_DATALOG_RUNNING = FALSE;
				_dWRITE_DIGITAL( DATALOG_CONTROL , 0 , &commstatus );
			}
			else {
				DATALOG_TIME_OLD++;
				if ( DATALOG_TIME_OLD >= (int)(DATALOG_TIME * 2 ) ) {
					DATALOG_TIME_OLD = 0;
					Elapsed_Time_with_Datalog( V_PLOTTER_SAVE_TAB_TYPE() , Buffer2 );
					if ( !DataLog_Data_Saving( Buffer2 ) ) {
						INSIDE_DATALOG_RUNNING = FALSE;
						_dWRITE_DIGITAL( DATALOG_CONTROL , 0 , &commstatus );
					}
					INSIDE_DATALOG_POINT++;
				}
			}
		}
	}

	if ( ( disp ) && ( hdlg != NULL ) ) {
		hDC = GetDC( hdlg );	Console_Gui_For_Item( hDC );	ReleaseDC( hdlg , hDC );
		if ( Poll && INSIDE_DATALOG_RUNNING ) {
			if      ( INSIDE_DATALOG_DRAW == 0 ) INSIDE_DATALOG_DRAW = 1;
			else if ( INSIDE_DATALOG_DRAW == 1 ) INSIDE_DATALOG_DRAW = 2;
			else if ( INSIDE_DATALOG_DRAW == 2 ) INSIDE_DATALOG_DRAW = 3;
			else                                 INSIDE_DATALOG_DRAW = 0;
			hDC = GetDC( hdlg ); ConGui_Datalog_Display( hDC , INSIDE_DATALOG_DRAW ); ReleaseDC( hdlg , hDC );
		}
		else {
			if ( INSIDE_DATALOG_DRAW != 0 ) {
				INSIDE_DATALOG_DRAW = 0;
				hDC = GetDC( hdlg ); ConGui_Datalog_Display( hDC , INSIDE_DATALOG_DRAW ); ReleaseDC( hdlg , hDC );
			}
		}

		if ( chgdata ) { hDC = GetDC( hdlg ); ConGui_Display_Data( hDC ); ReleaseDC( hdlg , hDC ); }
		if ( Send_1  ) { hDC = GetDC( hdlg ); Console_Gui_For_Drawing( hDC , SCREEN_1 ); ReleaseDC( hdlg , hDC ); }
		if ( Send_2  ) { hDC = GetDC( hdlg ); Console_Gui_For_Drawing( hDC , SCREEN_2 ); ReleaseDC( hdlg , hDC ); }
	}
}
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
