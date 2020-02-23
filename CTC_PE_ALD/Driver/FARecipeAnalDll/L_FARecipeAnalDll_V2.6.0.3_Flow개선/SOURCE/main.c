#include "FaRecipeAnal.h"

//-----------------------------------------------------------------------------------
extern void _LOG(const char *pFormat, ...);
//-----------------------------------------------------------------------------------
extern BOOL ReadConfigration();		// 2006.10.20
//----------------------------------------------------------------------------------

BOOL WINAPI DllMain( HINSTANCE hinstDLL , DWORD fdwReason , LPVOID lpvReserved ) {
	switch ( fdwReason ) {
	case DLL_PROCESS_ATTACH:
		{
			//_LOG("----------------------------------------------------------");
			//_LOG("FaRecipeAnl.DLL Attached\n");			// 2006.10.20
			ReadConfigration();							// 2006.10.20
			printf(" =========> FaRecipeAnl.DLL Attached\n"); 
			return( TRUE );
		}

	case DLL_PROCESS_DETACH:	break;
	case DLL_THREAD_ATTACH:		break;
	case DLL_THREAD_DETACH:		break;
	default:					break;
	}
	return( FALSE );
}

