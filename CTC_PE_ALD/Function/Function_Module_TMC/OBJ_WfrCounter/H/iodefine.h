/****************************************************************
 * System      : 200mm HDP-CVD
 * Subsystem   : Wafer animation
 * Module      : Wafer animation
 * File        : iodefine.h
 * Description : Wafer animation I/O define header file
 * Author      : Yang/Heejeon
 * Dept.       : Institute of Intelligent System. S-T/F
 * Created     : 2002.11.09
 * Version     : 200mm HDP-CVD (Ver0.0)
 * Revision history
 *   date         rev. by            revised information
 * 2002.11.09     hjeon         - 1st release
 ***************************************************************/

#ifndef IODEFINE_H
#define IODEFINE_H

typedef enum {
	V_WAFER_LOGGING,
	PD_WAFER_COUNT01,
	PD_WAFER_COUNT02,
	PD_WAFER_COUNT03,
	PD_WAFER_COUNT04,
	PD_WAFER_COUNT05,
	PD_WAFER_COUNT06,
	PD_WAFER_COUNT07,
	PD_WAFER_COUNT08,
	PD_WAFER_COUNT09,
	PD_WAFER_COUNT10,
	PS_WAFER_MAINT01,
	PS_WAFER_MAINT02,
	PS_WAFER_MAINT03,
	PS_WAFER_MAINT04,
	PS_WAFER_MAINT05,
	PS_WAFER_MAINT06,
	PS_WAFER_MAINT07,
	PS_WAFER_MAINT08,
	PS_WAFER_MAINT09,
	PS_WAFER_MAINT10
} IOPointIndex;

#endif /* IODEFINE_H */

