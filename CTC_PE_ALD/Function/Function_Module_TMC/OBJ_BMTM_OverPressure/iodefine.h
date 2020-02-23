#ifndef IODEFINE_H
#define IODEFINE_H
//

#include <STDIO.H>
extern "C"
{
#include "k:\h\Kutlstr.h"
#include "k:\h\Kutltime.h"
#include "k:\h\cimseqnc.h"
}

typedef enum { DISABLE, ENABLE };
typedef enum { CLOSE, OPEN };
typedef enum
{
	UNKNOWN_SENSOR,
	ATM_SENSOR,
	VAC_SENSOR,
	ERROR_SENSOR
};

typedef enum
{
	PRMD_OVER_PRES_OPT,

	TM_OverPresVvDO,
	TM_RoughVvXO,
	TM_ATMVACSensorXI,

	BM1_OverPresVvDO,
	BM1_ATMVACSensorXI,

	BM2_OverPresVvDO,
	BM2_ATMVACSensorXI,

	//MainBM_OverPresVvDO,
	//MainBM_ATMVACSensorXI,

	TM_UPC_Set,
	PRMA_UPC_SET_FLOW
};

//
#endif
