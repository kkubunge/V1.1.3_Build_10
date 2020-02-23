//--------------------------------------------------------------------
// Application Name
#define APP_NAME	"PM_Simulator"

//--------------------------------------------------------------------
#define SIM_CFG_FILE		".\\Function\\Function_Module_Simulator\\Simulation.cfg"

//--------------------------------------------------------------------
#define		 _MAX_LINE_HEATER		32
//--------------------------------------------------------------------
// MFC Simulation Functions
#define		 _MFC_MAX_NO			20
#define		_MFC_SET_TIME			0.1 /* sec */
#define		_MFC_SCAN_INTERVAL		0.1 /* sec */
#define		_MFC_MAX_FLOW			10000			// 2006.09.07

//--------------------------------------------------------------------
// Chamber Pressure Simulation Functions
#define  _TV_BASE_PRESS				0.001 /* Chamber Base Pressure */
#define  _TV_SET_TIME_POS		1	/* sec */
#define  _TV_SET_TIME_PRS		1	/* sec */
#define  _TV_SCAN_INTERVAL		1	/* sec */
#define  _TV_POS_STEP_GAIN		110	/* >100 */

//--------------------------------------------------------------------
// Dry Pump Simulation Functions
#define  _DP_SET_TIME			1 /* sec */
#define  _DP_SCAN_INTERVAL		1 /* sec */

//--------------------------------------------------------------------
#define _STG_HTR_SCAN_INTERVAL	1 /* sec */


//--------------------------------------------------------------------
#ifndef __ENUM_DEFINE_H__
#define __ENUM_DEFINE_H__

typedef enum
{
	NotUse,
	Use
} NotUseUseEnum;

typedef enum
{
	SetPoint,
	Open, 
	Close
} MFCCmdEnum;

typedef enum
{
	TV_POSITION,
	TV_PRESSURE
} TVControlModeEnum;

typedef enum
{
	TV_HOLD,
	TV_OPEN,
	TV_CLOSE
}M651Vlv;

typedef enum
{ 
	Transition,
	ATM,
	VACUUM,
	Error
} PressSenseEnum;

typedef enum
{
	DP_Unknown	= 0,
	DP_Start,
	DP_Stop,
	DP_Error
} DPStSpEnum;

typedef enum
{
	DP_OFF,
	DP_ON
} DPStatusEnum;

typedef enum {
	VClose= 0,
	VOpen
} CloseOpenEnum;

typedef enum
{
	HTR_OFF,
	HTR_ON
} HTRunEnum;

#endif
//--------------------------------------------------------------------
