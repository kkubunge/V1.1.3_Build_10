#ifndef __ENUMDEFINE_H___
#define __ENUMDEFINE_H___

typedef enum {
	EO_OFF = 0,
	EO_ON
} EOffOnEnum; 

typedef enum {
	ENU_NotUse = 0,
	ENU_Use
} ENotUseUseEnum;

typedef enum {
	EMFC_SetPoint = 0,
	EMFC_Open,
	EMFC_Close
} EMFCCmdEnum;

typedef enum {
	EVlv_Close= 0,
	EVlv_Open
} CloseOpenEnum;

#endif