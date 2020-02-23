#ifndef APPIMC_H
#define APPIMC_H

#if defined(__cplusplus) || defined(c_plusplus)
	extern "C" {
#endif

/* - - - - - - - - - - - -    D E F I N E S    - - - - - - - - - - - - - - */

extern ImcConServiceHdl	hORDBService;
extern ImcConServiceHdl	hEBUSService;
extern ImcConServiceHdl	hIOBUSService;
extern ImcConServiceHdl	hALARMService;

extern IMC_HDL ORDBImcHdl;
extern IMC_HDL IOBUSImcHdl;
extern IMC_HDL EBUSImcHdl;
extern IMC_HDL ALImcHdl;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
