#ifndef CARRESEVDLL_H
#define CARRESEVDLL_H
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
#define MAX_CARRIER_LOAD_RESERVATION_SIZE		32
#define MAX_CARRIER_UNLOAD_RESERVATION_SIZE		32
//----------------------------------------------------------------------------------------
#define MAX_CARRIER_SLOT_SIZE					30
//----------------------------------------------------------------------------------------
#define MID_STRING_SIZE							32
#define USAGE_STRING_SIZE						32
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
typedef struct {
	int					PORTID;
	int					CAPACITY;
	int					SUBSTRATECOUNT;
	int					SLOTMAP[MAX_CARRIER_SLOT_SIZE];
	char				CARRIERID[MID_STRING_SIZE];
	char				USAGE[USAGE_STRING_SIZE];
	char				LOTID[MAX_CARRIER_SLOT_SIZE][MID_STRING_SIZE];				
	char				SUBSTRATEID[MAX_CARRIER_SLOT_SIZE][MID_STRING_SIZE];
	int					STOCKERID;
	int					USEMODE; // 0 = HOST, 1 = GUI AUTO, 2 = GUI MANUAL
} Carrier_Load_Reservation_Struct;
//----------------------------------------------------------------------------------------
typedef struct {
	int					PORTID;
	char				CARRIERID[MID_STRING_SIZE];
} Carrier_Unload_Reservation_Struct;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
extern int									*Carrier_Load_Reservation_Size;
extern Carrier_Load_Reservation_Struct		*Carrier_Load_Reservation[MAX_CARRIER_LOAD_RESERVATION_SIZE];
extern int									*Carrier_Unload_Reservation_Size;
extern Carrier_Unload_Reservation_Struct	*Carrier_Unload_Reservation[MAX_CARRIER_UNLOAD_RESERVATION_SIZE];
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
BOOL _CARRIER_RESERVE_INTERFACE_INITIALIZING();
BOOL _CARRIER_RESERVE_INTERFACE_DEINITIALIZING();
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
#endif
