//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
// Shared Memory Function
// Create : 2005.04.20 by cskim

#define	S_MEMORY_SIZE	1024*8
#define SM_FILE_NAME	"POST_RECIPE_DATA_FILE"
extern unsigned char g_pSH_DATA[];

extern BOOL CreateSharedMemory();
extern BOOL OpenSharedMemory();
extern BOOL FreeSharedMemory();
extern BOOL ReadFromSharedMemory();
extern BOOL WriteToSharedMemory();
