#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

//==========================================================================================
extern "C"
{

#include <Kutlstr.h>
#include <Kutltime.h>
#include "cimseqnc.h"
#include "iodefine.h"

} // End of [extern "C"]
//==========================================================================================

#include "TextLogDll.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL	SIMULATION_MODE	= FALSE;
//------------------------------------------------------------------------------------------
char *EVENT_STRING_MAPREQ_IN		= "EVENT_ON MAPPING_F PORT%d|0";
char *EVENT_STRING_MAPCOMPLETE_IN	= "EVENT_ON MAPPING_COMPLETE_F PORT%d|0";
char *EVENT_STRING_AT_SOURCE_IN		= "EVENT_ON STS_ATSOURCE_F PORT%d|0";
char *EVENT_STRING_AT_OCCUPY_IN		= "EVENT_ON STS_OCCUPIED_F PORT%d|0";
//
char *EVENT_STRING_MAPREQ_MAN		= "EVENT_ON MAPPING_F PORT%d|1";
char *EVENT_STRING_MAPCOMPLETE_MAN	= "EVENT_ON MAPPING_COMPLETE_F PORT%d|1";
char *EVENT_STRING_AT_SOURCE_MAN	= "EVENT_ON STS_ATSOURCE_F PORT%d|1";
char *EVENT_STRING_AT_OCCUPY_MAN	= "EVENT_ON STS_OCCUPIED_F PORT%d|1";
//
char *EVENT_STRING_MAPREQ_OUT		= "EVENT_ON MAPPING_F PORT%d|2";
char *EVENT_STRING_MAPCOMPLETE_OUT	= "EVENT_ON MAPPING_COMPLETE_F PORT%d|2";


//2014.08.06
char *EVENT_STRING_PICK_TO_LPM1		= "EVENT_ON ATM_ROBOT_PICK_TO_LPM1";
char *EVENT_STRING_PICK_TO_LPM2		= "EVENT_ON ATM_ROBOT_PICK_TO_LPM2";
char *EVENT_STRING_PICK_TO_LPM3		= "EVENT_ON ATM_ROBOT_PICK_TO_LPM3";
char *EVENT_STRING_PLACE_TO_LPM1	= "EVENT_ON ATM_ROBOT_PLACE_TO_LPM1";
char *EVENT_STRING_PLACE_TO_LPM2	= "EVENT_ON ATM_ROBOT_PLACE_TO_LPM2";
char *EVENT_STRING_PLACE_TO_LPM3	= "EVENT_ON ATM_ROBOT_PLACE_TO_LPM3";
char *EVENT_STRING_PICK_TO_LL1		= "EVENT_ON ATM_ROBOT_PICK_TO_LL1";
char *EVENT_STRING_PICK_TO_LL2		= "EVENT_ON ATM_ROBOT_PICK_TO_LL2";
char *EVENT_STRING_PLACE_TO_LL1		= "EVENT_ON ATM_ROBOT_PLACE_TO_LL1";
char *EVENT_STRING_PLACE_TO_LL2		= "EVENT_ON ATM_ROBOT_PLACE_TO_LL2";
char *EVENT_STRING_PICK_TO_AL		= "EVENT_ON ATM_ROBOT_PICK_TO_AL";
char *EVENT_STRING_PLACE_TO_AL		= "EVENT_ON ATM_ROBOT_PLACE_TO_AL";
//
//------------------------------------------------------------------------------------------
#define		ALARM_COMM_ERROR		1
#define		ALARM_FATAL				2
#define		ALARM_NOT_HOME			3
#define		ALARM_MOTION			4
#define		ALARM_OTHER				5
#define		ALARM_HOME_WAFER		11
#define		ALARM_PICK_WAFER		12
#define		ALARM_PLACE_WAFER		13
#define		ALARM_ROTATE_WAFER		14
#define		ALARM_EXTEND_WAFER		15
#define		ALARM_RETRACT_WAFER		16
#define		ALARM_UP_WAFER			17
#define		ALARM_DOWN_WAFER		18
#define		ALARM_MAP_WAFER			21
//------------------------------------------------------------------------------------------
#define		ARM_A		0
#define		ARM_B		1
//------------------------------------------------------------------------------------------
#define		ROBOT_STATUS_DOWN			1
#define		ROBOT_STATUS_UP				2
#define		ROBOT_STATUS_RETRACT		1
#define		ROBOT_STATUS_EXTEND			2
#define     ROBOT_STATUS_ROTATE			10
//------------------------------------------------------------------------------------------
#define DIV 1024
#define DIV2 1024

//------------------------------------------------------------------------------------------
int ALARM_INDEX_POINTER = 4000;
int ALARM_ROBOT_IOFAIL_TIMEOUT = 5500;

//------------------------------------------------------------------------------------------
enum { ROBOT_SYNCH_READY , ROBOT_SYNCH_ROTATE_START , ROBOT_SYNCH_EXTEND_START , ROBOT_SYNCH_UPDATE , ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RESET_REVERSE , ROBOT_SYNCH_RESET_ONLY , ROBOT_SYNCH_SUCCESS };
//------------------------------------------------------------------------------------------
enum { CM1 , CM2 , CM3 , BUFFER_L , BUFFER_R , BM1 , BM2 , BM3 , BM4 , AL , STG1 , STG2 , IC};
enum {WAFER_UNKNOWN , WAFER_ABSENT , WAFER_PRESENT};

//modified by jonghyuk 2011.03.09
enum {NONE = 0, WARNING1_ERROR, WARNING2_ERROR, ALARM1_ERROR, ALARM2_ERROR, FATAL_ERROR};
//2014.10.04
enum { SRC_A, SRC_B, SRC_C, SRC_X, SRC_Y, SRC_D };

//... 2017.03.07
enum { All_Enable,  SEna_VDis,  SDis_VEna,  All_Disable };


//modified by jonghyuk 2011.03.09
#define		WARNING1_ERROR_COUNT		9
#define		WARNING2_ERROR_COUNT		199
#define		ALARM1_ERROR_COUNT			690
#define		ALARM2_ERROR_COUNT			89
#define		FATAL_ERROR_COUNT			45


//------------------------------------------------------------------------------------------
int MODULE_CONTROL_STATION[13] = {  1 ,   //CM1
									2 ,   //CM2
									3 ,   //CM3
									999 , //Buffer Left
									999 , //Buffer Right
									6 ,   // BM1
									7 ,   // BM2
									999 , // BM3
									999 , // BM4
									10 ,  // AL
									999 , //stage1
									999 , //stage2
									999}; // IC

//modified by jonghyuk 2011.03.09
int ALARM_NUMBER[1032] = {  0x0f0, 0x900, 0x910, 0x920, 0x930, 0x940, 0x950, 0x960, 0x961, // WARNING1 = 9
							0x020, 0x030, 0x040, 0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057, 0x058, 0x059, 0x05a, 0x05b, 0x05c, // WARNING2 = 199
							0x05d, 0x05e, 0x05f, 0x060, 0x061, 0x062, 0x064, 0x068, 0x069, 0x06a, 0x06b, 0x070, 0x071, 0x072, 0x074, 0x080, 
							0x088, 0x089, 0x08a, 0x08b, 0x08c, 0x08d, 0x08e, 0x0e0, 0x193, 0x293, 0x393, 0x493, 0x593, 0x693, 0x7b0, 0x7b4, 
							0x7b5, 0x7d0, 0x7d1, 0x7d2, 0x7d3, 0x7d4, 0x7d8, 0x7d9, 0x7da, 0xe7f, 0x980, 0x981, 0x990, 0x991, 0x9a0, 0x9a1, 
							0x9a2, 0x9c0, 0x9c1, 0x9c2, 0x9c3, 0x9d0, 0x9e0, 0x9e1, 0xa40, 0xa41, 0xa42, 0xa43, 0xa44, 0xa45, 0xa46, 0xa4f,      
							0xae0, 0xae1, 0xae8, 0xb20, 0xb21, 0xb22, 0xb23, 0xb24, 0xb25, 0xb30, 0xb31, 0xb32, 0xb33, 0xb34, 0xb35, 0xb40, 
							0xb41, 0xb42, 0xb43, 0xb44, 0xb45, 0xb46, 0xb47, 0xb48, 0xb49, 0xb4a, 0xb4b, 0xb4c, 0xb4d, 0xb4e, 0xb4f, 0xb60, 
							0xb61, 0xb62, 0xb63, 0xb64, 0xb65, 0xb66, 0xb68, 0xb70, 0xb80, 0xb81, 0xb82, 0xb83, 0xb88, 0xb89, 0xb8a, 0xb8b, 
							0xb90, 0xb91, 0xb92, 0xb93, 0xb98, 0xb99, 0xb9a, 0xb9b, 0xba0, 0xba1, 0xba8, 0xba9, 0xbaa, 0xbab, 0xbac, 0xbad, 
							0xbb0, 0xbb1, 0xbb8, 0xbb9, 0xbba, 0xbbb, 0xbbc, 0xbe0, 0xbe1, 0xbe2, 0xbe3, 0xbe4, 0xbe5, 0xbe8, 0xbe9, 0xbea, 
							0xbeb, 0xbec, 0xbed, 0xbf0, 0xc80, 0xc90, 0xc91, 0xc92, 0xc94, 0xca0, 0xcb0, 0xcc0, 0xcd0, 0xce0, 0xd00, 0xd10, 
							0xd20, 0xd30, 0xd40, 0xd50, 0xd60, 0xe08, 0xe09, 0xe0a, 0xea0, 0xea1, 0xea4, 0xea5, 0xef0, 0xef1, 0xef2, 0xef3, 
							0xef4, 0xef5, 0xef8, 0xef9, 0xefa, 0xefe, 0xeff, 
							0x171, 0x172, 0x173, 0x174, 0x178, 0x17a, 0x17b, 0x17c, 0x17d, 0xe11, 0x151, 0x152, 0x153, 0x154, 0x15f, 0x1c0, // ALARM1 = 690
							0x1c1, 0x1c2, 0x1c3, 0x1c4, 0x1c5, 0x1c6, 0x1c7, 0x1c8, 0x1c9, 0x1ca, 0x1cb, 0x1cc, 0x1ce, 0x10b, 0x140, 0x141, 
							0x142, 0x143, 0x144, 0x145, 0x146, 0x147, 0x148, 0x149, 0x14a, 0x130, 0x132, 0x133, 0x1d0, 0x1d1, 0x1d3, 0x1d4, 
							0x1f1, 0x1f4, 0x1f5, 0x1f6, 0x110, 0x111, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x10a, 0x181, 
							0x182, 0x187, 0x18c, 0x184, 0x185, 0x186, 0x188, 0x189, 0x18a, 0x18b, 0x18d, 0x18e, 0x190, 0x191, 0x192, 0x194, 
							0x195, 0x196, 0x197, 0x198, 0x19a, 0x19c, 0x19e, 0x19f, 0x1e0, 0x1e1, 0x1e2, 0x1e4, 0x1e5, 0x1e6, 0x1ea, 0x1eb, 
							0x1ec, 0x1ed, 0x1b1, 0x1b2, 0x1b3, 0x1b4, 0x1b6, 0x1bc, 0x1bf, 0xe1e, 0xe1d, 0xe1f, 0x271, 0x272, 0x273, 0x274, 
							0x278, 0x27a, 0x27b, 0x27c, 0x27d, 0xe21, 0x251, 0x252, 0x253, 0x254, 0x25f, 0x2c0, 0x2c1, 0x2c2, 0x2c3, 0x2c4, 
							0x2c5, 0x2c6, 0x2c7, 0x2c8, 0x2c9, 0x2ca, 0x2cb, 0x2cc, 0x2ce, 0x20b, 0x240, 0x241, 0x242, 0x243, 0x244, 0x245, 
							0x246, 0x247, 0x248, 0x249, 0x24a, 0x230, 0x232, 0x233, 0x2d0, 0x2d1, 0x2d3, 0x2d4, 0x2f1, 0x2f4, 0x2f5, 0x2f6, 
							0x210, 0x211, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208, 0x209, 0x20a, 0x281, 0x282, 0x287, 0x28c, 0x284, 
							0x285, 0x286, 0x288, 0x289, 0x28a, 0x28b, 0x28d, 0x28e, 0x290, 0x291, 0x292, 0x294, 0x295, 0x296, 0x297, 0x298, 
							0x29a, 0x29c, 0x29e, 0x29f, 0x2e0, 0x2e1, 0x2e2, 0x2e4, 0x2e5, 0x2e6, 0x2ea, 0x2eb, 0x2ec, 0x2ed, 0x2b1, 0x2b2, 
							0x2b3, 0x2b4, 0x2b6, 0x2bc, 0x2bf, 0xe2e, 0xe2d, 0xe2f, 0x371, 0x372, 0x373, 0x374, 0x378, 0x37a, 0x37b, 0x37c, 
							0x37d, 0xe31, 0x351, 0x352, 0x353, 0x354, 0x35f, 0x3c0, 0x3c1, 0x3c2, 0x3c3, 0x3c4, 0x3c5, 0x3c6, 0x3c7, 0x3c8, 
							0x3c9, 0x3ca, 0x3cb, 0x3cc, 0x3ce, 0x30b, 0x340, 0x341, 0x342, 0x343, 0x344, 0x345, 0x346, 0x347, 0x348, 0x349, 
							0x34a, 0x330, 0x332, 0x333, 0x3d0, 0x3d1, 0x3d3, 0x3d4, 0x3f1, 0x3f4, 0x3f5, 0x3f6, 0x310, 0x311, 0x302, 0x303, 
							0x304, 0x305, 0x306, 0x307, 0x308, 0x309, 0x30a, 0x381, 0x382, 0x387, 0x38c, 0x384, 0x385, 0x386, 0x388, 0x389, 
							0x38a, 0x38b, 0x38d, 0x38e, 0x390, 0x391, 0x392, 0x394, 0x395, 0x396, 0x397, 0x398, 0x39a, 0x39c, 0x39e, 0x39f, 
							0x3e0, 0x3e1, 0x3e2, 0x3e4, 0x3e5, 0x3e6, 0x3ea, 0x3eb, 0x3ec, 0x3ed, 0x3b1, 0x3b2, 0x3b3, 0x3b4, 0x3b6, 0x3bc, 
							0x3bf, 0xe3e, 0xe3d, 0xe3f, 0x471, 0x472, 0x473, 0x474, 0x478, 0x47a, 0x47b, 0x47c, 0x47d, 0xe41, 0x451, 0x452, 
							0x453, 0x454, 0x45f, 0x4c0, 0x4c1, 0x4c2, 0x4c3, 0x4c4, 0x4c5, 0x4c6, 0x4c7, 0x4c8, 0x4c9, 0x4ca, 0x4cb, 0x4cc, 
							0x4ce, 0x40b, 0x440, 0x441, 0x442, 0x443, 0x444, 0x445, 0x446, 0x447, 0x448, 0x449, 0x44a, 0x430, 0x432, 0x433, 
							0x4d0, 0x4d1, 0x4d3, 0x4d4, 0x4f1, 0x4f4, 0x4f5, 0x4f6, 0x410, 0x411, 0x402, 0x403, 0x404, 0x405, 0x406, 0x407, 
							0x408, 0x409, 0x40a, 0x481, 0x482, 0x487, 0x48c, 0x484, 0x485, 0x486, 0x488, 0x489, 0x48a, 0x48b, 0x48d, 0x48e, 
							0x490, 0x491, 0x492, 0x494, 0x495, 0x496, 0x497, 0x498, 0x49a, 0x49c, 0x49e, 0x49f, 0x4e0, 0x4e1, 0x4e2, 0x4e4, 
							0x4e5, 0x4e6, 0x4ea, 0x4eb, 0x4ec, 0x4ed, 0x4b1, 0x4b2, 0x4b3, 0x4b4, 0x4b6, 0x4bc, 0x4bf, 0xe4e, 0xe4d, 0xe4f, 
							0x571, 0x572, 0x573, 0x574, 0x578, 0x57a, 0x57b, 0x57c, 0x57d, 0xe51, 0x551, 0x552, 0x553, 0x554, 0x55f, 0x5c0, 
							0x5c1, 0x5c2, 0x5c3, 0x5c4, 0x5c5, 0x5c6, 0x5c7, 0x5c8, 0x5c9, 0x5ca, 0x5cb, 0x5cc, 0x5ce, 0x50b, 0x540, 0x541, 
							0x542, 0x543, 0x544, 0x545, 0x546, 0x547, 0x548, 0x549, 0x54a, 0x530, 0x532, 0x533, 0x5d0, 0x5d1, 0x5d3, 0x5d4, 
							0x5f1, 0x5f4, 0x5f5, 0x5f6, 0x510, 0x511, 0x502, 0x503, 0x504, 0x505, 0x506, 0x507, 0x508, 0x509, 0x50a, 0x581, 
							0x582, 0x587, 0x58c, 0x584, 0x585, 0x586, 0x588, 0x589, 0x58a, 0x58b, 0x58d, 0x58e, 0x590, 0x591, 0x592, 0x594, 
							0x595, 0x596, 0x597, 0x598, 0x59a, 0x59c, 0x59e, 0x59f, 0x5e0, 0x5e1, 0x5e2, 0x5e4, 0x5e5, 0x5e6, 0x5ea, 0x5eb, 
							0x5ec, 0x5ed, 0x5b1, 0x5b2, 0x5b3, 0x5b4, 0x5b6, 0x5bc, 0x5bf, 0xe5e, 0xe5d, 0xe5f, 0x671, 0x672, 0x673, 0x674, 
							0x678, 0x67a, 0x67b, 0x67c, 0x67d, 0xe61, 0x651, 0x652, 0x653, 0x654, 0x65f, 0x6c0, 0x6c1, 0x6c2, 0x6c3, 0x6c4, 
							0x6c5, 0x6c6, 0x6c7, 0x6c8, 0x6c9, 0x6ca, 0x6cb, 0x6cc, 0x6ce, 0x60b, 0x640, 0x641, 0x642, 0x643, 0x644, 0x645, 
							0x646, 0x647, 0x648, 0x649, 0x64a, 0x630, 0x632, 0x633, 0x6d0, 0x6d1, 0x6d3, 0x6d4, 0x6f1, 0x6f4, 0x6f5, 0x6f6, 
							0x610, 0x611, 0x602, 0x603, 0x604, 0x605, 0x606, 0x607, 0x608, 0x609, 0x60a, 0x681, 0x682, 0x687, 0x68c, 0x684, 
							0x685, 0x686, 0x688, 0x689, 0x68a, 0x68b, 0x68d, 0x68e, 0x690, 0x691, 0x692, 0x694, 0x695, 0x696, 0x697, 0x698, 
							0x69a, 0x69c, 0x69e, 0x69f, 0x6e0, 0x6e1, 0x6e2, 0x6e4, 0x6e5, 0x6e6, 0x6ea, 0x6eb, 0x6ec, 0x6ed, 0x6b1, 0x6b2, 
							0x6b3, 0x6b4, 0x6b6, 0x6bc, 0x6bf, 0xe6e, 0xe6d, 0xe6f, 0x731, 0x732, 0x733, 0x734, 0x735, 0x736, 0x73f, 0xa10, 
							0xa20, 0xa21, 0xa30, 0xa80, 0xac0, 0xac9, 0xad0, 0xad1, 0xad2, 0xad3, 0xad4, 0xad5, 0xb10, 0xb11, 0xb12, 0xb13, 
							0xb14, 0xb15, 0xfd0, 0xfd1, 0xfd2, 0xfd3, 0xfd4, 0xfd5, 0xfd6, 0xfd7, 0xfd8, 0xfd9, 0xfda, 0xfdb, 0xfdc, 0xfdd, 
							0xfde, 0xfdf, 
							0x700, 0x701, 0x702, 0x703, 0x704, 0x705, 0x706, 0x707, 0x708, 0x709, 0x70c, 0x70f, 0x713, 0x714, 0x715, 0x716, //ALARM2 = 89
							0x717, 0x718, 0x719, 0x71a, 0x71b, 0x71c, 0x71d, 0x71e, 0x71f, 0x720, 0x721, 0x722, 0x723, 0x724, 0x725, 0x727, 
							0xa00, 0xa01, 0xa04, 0xa08, 0xa09, 0xa0a, 0xa0c, 0xa60, 0xaf0, 0xaf1, 0xaf2, 0xaf3, 0xaf4, 0xaf5, 0xaf6, 0xaf7, 
							0xaf8, 0xaf9, 0xaff, 0xb50, 0xb51, 0xb52, 0xb53, 0xb54, 0xb55, 0xe90, 0xe91, 0xeb0, 0xeb1, 0xeb4, 0xeb5, 0xec0, 
							0xed0, 0xed1, 0xee0, 0xee1, 0xee2, 0xee3, 0xee4, 0xee5, 0xee8, 0xf10, 0xf11, 0xf12, 0xf14, 0xf20, 0xf21, 0xf22, 
							0xf30, 0xf31, 0xf32, 0xf33, 0xf34, 0xf38, 0xf39, 0xf3b, 0xf3c, 
							0xf50, 0xf61, 0xf62, 0xf82, 0xf83, 0xf84, 0xf86, 0xf88, 0xf90, 0xf91, 0xf92, 0xf93, 0xf94, 0xf95, 0xf98, 0xf99, //FATAL = 45
							0xf9a, 0xf9b, 0xf9c, 0xf9d, 0xfa0, 0xfa1, 0xfa2, 0xfa3, 0xfa4, 0xfa8, 0xfa9, 0xfaa, 0xfab, 0xfb0, 0xfb1, 0xfb8, 
							0xfb9, 0xfc0, 0xfc4, 0xfc5, 0xfc8, 0xfcc, 0xfe0, 0xfe2, 0xfe4, 0xfe6, 0xfe8, 0xfea, 0xfec };

//------------------------------------------------------------------------------------------
IO_Name_String_Map IO_STR_Map_Table[] = {
	{ "IO_Driver_Loding"		, _K_D_IO	,	IO_Driver_Loding	,	0	} ,
	//============================================================================
	{ "e:SCHEDULER"				, _K_S_IO	,	SCHEDULER			,	0	} ,
	//============================================================================
	{ "FC_FM_ROBOT_CONTROL"		, _K_D_IO	,	CONTROL				,	0	} ,

	{ "CTC_EVENT_LOG_SVR"		, _K_F_IO	,	CTC_EVENT_LOG_SVR		,	0	} ,		// 2013.11.15

	//modified by jonghyuk 2011.03.09
	{ "FM.Robot_CommStatus"		, _K_D_IO	,	MTR5_Comm_Status	,	0	} ,
	{ "FM.Robot_Run_Status"		, _K_D_IO	,	MTR5_Run_Status	,	0	} ,
	{ "FM.Robot_ErrorLevel"		, _K_D_IO	,	MTR5_ErrorLevel	,	0	} ,
	{ "FM.Robot_ErrorCode"		, _K_D_IO	,	MTR5_ErrorCode	,	0	} ,
	//2012.05.20 by mgsong
	{ "FM.Robot_SysError"		, _K_D_IO	,	MTR5_SysErrorCode	,	0	} ,
	{ "FM.Robot_SysStsChk"		, _K_D_IO	,	MTR5_SysStsChk	,	0	} ,
	{ "FM.Robot_WfrSts"			, _K_D_IO	,	MTR5_WfrSts		,	0	} ,
	{ "FM.Robot_WfrStsB"			, _K_D_IO	,	MTR5_WfrStsB		,	0	} ,

	{ "FM.Robot_Init"	    	, _K_D_IO	,	MTR5_T_Error_Clear	,	0	} ,
	{ "FM.Robot_Home"			, _K_D_IO	,	MTR5_HOME			,	0	} ,

	{ "FM.Robot_RotG_Robot"		, _K_D_IO	,	MTR5_RotG_Robot		,	0	} ,
	{ "FM.Robot_RotG_RobotB"	, _K_D_IO	,	MTR5_RotG_RobotB	,	0	} ,
	{ "FM.Robot_RotGDB_Robot"	, _K_D_IO	,	MTR5_RotGDB_Robot	,	0	} ,

	{ "FM.Robot_RotP_Robot"		, _K_D_IO	,	MTR5_RotP_Robot		,	0	} ,
	{ "FM.Robot_RotP_RobotB"	, _K_D_IO	,	MTR5_RotP_RobotB	,	0	} ,
	{ "FM.Robot_RotPDB_Robot"	, _K_D_IO	,	MTR5_RotPDB_Robot	,	0	} ,
		
	{ "FM.Robot_Get_Robot"		, _K_D_IO	,	MTR5_Get_Robot		,	0	} ,	
	{ "FM.Robot_Get_RobotB"		, _K_D_IO	,	MTR5_Get_RobotB		,	0	} ,
	{ "FM.Robot_GetDB_Robot"	, _K_D_IO	,	MTR5_GetDB_Robot	,	0	} ,

	{ "FM.Robot_Put_Robot"		, _K_D_IO	,	MTR5_Put_Robot		,	0	} ,
	{ "FM.Robot_Put_RobotB"		, _K_D_IO	,	MTR5_Put_RobotB		,	0	} ,
	{ "FM.Robot_PutDB_Robot"	, _K_D_IO	,	MTR5_PutDB_Robot	,	0	} ,

	{ "FM.Robot_GetAL_Robot"	, _K_D_IO	,	MTR5_GetAL_Robot	,	0	} ,
	{ "FM.Robot_GetAL_RobotB"	, _K_D_IO	,	MTR5_GetAL_RobotB	,	0	} ,
	
	{ "FM.Robot_Slot_Pos"		, _K_D_IO	,	MTR5_T_Slot_Pos		,	0	} ,
		
	{ "FM.RB_SYNCH"				, _K_D_IO	,	ROBOT_SYNCH			,	0	} ,
	{ "FM.RB_RET"				, _K_D_IO	,	ROBOT_RET			,	0	} ,
	{ "FM.RB_STATION"			, _K_D_IO	,	ROBOT_STATION		,	0	} ,
	{ "PRMD_FM_RB_MOVE_TMO"		, _K_D_IO	,	MOVE_WAFER_TIMEOUT	,	0	} ,

	//2008.04.28 BM Auto Pick Retry
	{ "FC_BM_AUTOPICK_CONTROL"	, _K_D_IO	,	BM_AUTOPICK_RETRY_OPTION,	0	} ,
	{ "PRM_BM_AUTOPICK_RETRY"	, _K_D_IO	,	BM_AUTOPICK_RETRY_COUNT	,	0	} ,

    //2009.06.24 ATM Robot Pick/Place Door Sensor Check Retry
	{ "BM1.DoorLXI"			, _K_D_IO	,	BM1LeftDoor 			,	0	} ,
	{ "BM1.DoorRXI"			, _K_D_IO	,	BM1RightDoor 			,	0	} ,
	{ "BM2.DoorLXI"			, _K_D_IO	,	BM2LeftDoor 			,	0	} ,
	{ "BM2.DoorRXI"			, _K_D_IO	,	BM2RightDoor 			,	0	} ,
	// 2014.10.04
	{ "CTC.FA_Wafer_Status"		, _K_D_IO	,	FA_Wafer_Status 	,	0	} ,
	{ "CTC.FB_Wafer_Status"		, _K_D_IO	,	FB_Wafer_Status 	,	0	} ,

	{ "CTC.FA_Wafer_Result"		, _K_D_IO	,	FA_Wafer_Result 	,	0	} ,
	{ "CTC.FB_Wafer_Result"		, _K_D_IO	,	FB_Wafer_Result 	,	0	} ,
	
    { "CTC.FA_Wafer_Source"		, _K_D_IO	,	FA_Wafer_Source		,	0	} ,
    { "CTC.FB_Wafer_Source"		, _K_D_IO	,	FB_Wafer_Source		,	0	} ,
	
	{ "CTC.JOB_NAME"			, _K_S_IO	,	JOB_NAME			,	0	} ,
	{ "CTC.JOB_NAME2"			, _K_S_IO	,	JOB_NAME2			,	0	} ,
	{ "CTC.JOB_NAME3"			, _K_S_IO	,	JOB_NAME3			,	0	} ,
		
	{ "CTC.MID_NAME"			, _K_S_IO	,	MID_NAME			,	0	} ,
	{ "CTC.MID_NAME2"			, _K_S_IO	,	MID_NAME2			,	0	} ,
	{ "CTC.MID_NAME3"			, _K_S_IO	,	MID_NAME3			,	0	} ,

    { "FM_RB_LOTID1"			, _K_S_IO	,	FM_RB_LOTID1		,	0	} ,
	{ "FM_RB_LOTID2"			, _K_S_IO	,	FM_RB_LOTID2		,	0	} ,
    { "FM_RB_MIDID1"			, _K_S_IO	,	FM_RB_MIDID1		,	0	} ,
	{ "FM_RB_MIDID2"			, _K_S_IO	,	FM_RB_MIDID2		,	0	} ,
    { "FM_RB_WAFERID1"			, _K_D_IO	,	FM_RB_WAFERID1		,	0	} ,
	{ "FM_RB_WAFERID2"			, _K_D_IO	,	FM_RB_WAFERID2		,	0	} ,
	//... 2017.03.07
	{ "INTLKS_CONTROL"			, _K_D_IO	,	INTLKS_CONTROL		,	0	} ,
	//... 2014.11.08 Add Aligner Init by YASKAWA Ethernet M124
	{ "FM.Align_Init"	    	, _K_D_IO	,	AL_Error_Clear		,	0	} ,
	{ "FM.Align_Home"	    	, _K_D_IO	,	AL_HOME				,	0	} ,	
	{ "FM.Align_Run"			, _K_D_IO	,	AL_Align_Run		,	0	} ,
	//... 2017.12.23 Add Parameter whether Aligner Use or Not Use
	{ "FC_TM_ALIGNER_CONTROL"	, _K_D_IO	,	FC_TM_ALIGNER_CONTROL,	0	} ,
	""
};

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//2012.05.20 by mgsong
//Robot IO Timeout Recovery Reference
int g_nSystemError  = 0;
int g_nWaferStatusA = 0;
int g_nWaferStatusB = 0;
enum {SYS_NONE = 0, SYS_ERROR_COME = 1, SYS_T_OUT = 2};
int g_nExceptionStatus = SYS_NONE;
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2005.08.19 by cskim, Wafer Sync
enum { RB_IDLE, RB_READY, RB_DOING_STEP1, RB_DOING_STEP2, RB_DONE };
int g_nRobotStatus = RB_IDLE;

//------------------------------------------------------------------------------------------
// 2005.12.20
#include "DynamicIO.h"
CDynamicIO g_DynIO;
//--------------------------------------------------------------------
// Dynamic IO Enum List Definition
static enum {
	CTC_AbortControl,
	TMSIM_ATM_ROBOT,
} IO_INDEX_ENUM;
//--------------------------------------------------------------------
// Dynamic IO List Definition
static IO_LIST	g_pIOList[] =
{
	{"CTC.AbortControl",				_K_D_IO, 0},
	{"TMSIM_ATM_ROBOT",					_K_F_IO, 0},
	""
};
BOOL CheckAbortControl()
{
	BOOL bRet = FALSE;
	BOOL bIOStatus;
	enum { ABC_IDLE, ABC_ABORT, ABC_ABORTING };
	if(g_DynIO.IOValid(CTC_AbortControl))
	{
		if(g_DynIO.dREAD_DIGITAL(CTC_AbortControl, &bIOStatus) != ABC_IDLE) bRet = TRUE;
	}
	return bRet;
}
//------------------------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////
// Global Function
// 2013.11.15
extern void _MLOG(const char *pFormat, ...)
{
	char _szString[1024];
	va_list args;
	va_start(args, pFormat);
	vsprintf(_szString, pFormat, args);
	va_end(args);	
	WRITE_FUNCTION_EVENT(CTC_EVENT_LOG_SVR, _szString);
}

// 2013.11.15
void LogEvent(char *szModule, char *szRunStr, char *szStatus, char *szDest, int nSlot, int nArmIdx,
				char *szData, BOOL bDualArm)
{
	char szEventID[20];
	char szDeviceID[20];
	char szTemp[40];
	int offset = 0;

	if ( szDest[ 1 ] == '_' ) offset = 2;
	strcpy(szDeviceID, szDest+offset);

	do {
		if(STRCMP_L(szRunStr , "PICK")) strcpy(szEventID, "Get");
		else if(STRCMP_L(szRunStr , "PLACE")) strcpy(szEventID, "Put");
		else if(STRCMP_L(szRunStr , "ROTATE")) strcpy(szEventID, "Rotate");
		else break;

		sprintf( szTemp, "%d %s", nSlot, szData );

		if(! bDualArm) _MLOG("XFR %s %s %s %s %d %c %s", szModule, szEventID, szStatus, szDeviceID, nSlot, 'A'+nArmIdx, szTemp);
		else _MLOG("XFR %s %s %s %s [2,%d,%d] %c %s", szModule, szEventID, szStatus, szDeviceID, nSlot, nSlot+1, 'A'+nArmIdx, szTemp);	// 2013.07.29
	} while(0);
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int READ_DIGITAL_TRUE( int io , int *CommStatus ) {
	int Data;
	Data = READ_DIGITAL( io , CommStatus );
	if ( !(*CommStatus) ) {
		_sleep(250);
		Data = READ_DIGITAL( io , CommStatus );
		if ( !(*CommStatus) ) {
			_sleep(500);
			Data = READ_DIGITAL( io , CommStatus );
			if ( !(*CommStatus) ) {
				_sleep(1000);
				Data = READ_DIGITAL( io , CommStatus );
				if ( !(*CommStatus) ) {
					_sleep(1000);
					return( READ_DIGITAL( io , CommStatus ) );
				}
				else return Data;
			}
			else return Data;
		}
		else return Data;
	}
	else return Data;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void ALARM_MESSAGE_REMAPPING( int id ) {
	//modified by mgsong 2008.01.09
	char Buffer[256] , Buffer2[32];
	int  err = 0;
	int  errlevel = 0;
	int CommStatus;

	//modified by jonghyuk 2011.03.09
	err      = (int) READ_DIGITAL( MTR5_ErrorCode    , &CommStatus );
	errlevel = (int) READ_DIGITAL( MTR5_ErrorLevel , &CommStatus ); 
	if (err > 0) {
		sprintf( Buffer2 , "(ERRLEVEL %d , ERROR=%x)" , errlevel , err ); //... 2016.02.28
		ALARM_MESSAGE_GET( id , Buffer );
		strcat( Buffer , Buffer2 );
		ALARM_MESSAGE_SET( id , Buffer );
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Pre_Check_File_Save2(int data)
{
	FILE *fp3;
	char buf[256], file[256];
	time_t aclock;
	struct tm *newtime;

	sprintf(file, "Datalog/ATR/Pre_Check2.txt" );

	time( &aclock );
	newtime = localtime( &aclock );
	if ( !(fp3 = fopen(file, "a+")) ) return SYS_ABORTED;
	sprintf(buf, "%s ===> Data : %d\n", asctime(newtime), data/DIV2);
	buf[24] = ' ';
	// write message to LOG_FILE
	fputs(buf, fp3);
	fclose(fp3);

	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Pre_Check_File_Save(int data)
{
	FILE *fp2;
	char buf[256], file[256];
	time_t aclock;
	struct tm *newtime;

	sprintf(file, "Datalog/ATR/Pre_Check.txt" );

	time( &aclock );
	newtime = localtime( &aclock );
	if ( !(fp2 = fopen(file, "a+")) ) return SYS_ABORTED;
	sprintf(buf, "%s ===> Data : %d\n", asctime(newtime), data/DIV2);
	buf[24] = ' ';
	// write message to LOG_FILE
	fputs(buf, fp2);
	fclose(fp2);

	return SYS_SUCCESS;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status Run_Status_File_Save(int data)
{
	/*FILE *fp;
	char buf[256], file[256];
	time_t aclock;
	struct tm *newtime;
    int CommStatus;

	sprintf(file, "Datalog/ATR/ATR.log" );

	// get system time
	time( &aclock );
	newtime = localtime( &aclock );
	if ( !(fp = fopen(file, "a+")) ) return SYS_ABORTED;
	sprintf(buf, "%s ===> Run Status :  %d          Vacuum Status : %d		*Data : %d\n", asctime(newtime), 
        READ_DIGITAL_TRUE( MTR5_Run_Status , &CommStatus )/DIV,
        READ_DIGITAL_TRUE( ATR8_VAC_STATUS , &CommStatus )/DIV,
		data/DIV);
	buf[24] = ' ';
	// write message to LOG_FILE
	fputs(buf, fp);
	fclose(fp);*/

	return SYS_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
Module_Status PrepCheckRobot( BOOL HomeMode ) {
	int	Data  , error , i , error_code, errlevel;
	int CommStatus , AlmResult;

	WRITE_DIGITAL(MTR5_T_Error_Clear , 1 , &CommStatus );

	if(0 == READ_DIGITAL(FC_TM_ALIGNER_CONTROL,&CommStatus)) //... 0: Use, 1: NotUse
	{
		//... 2014.11.08 Add Aligner Init by YASKAWA Ethernet M124
		WRITE_DIGITAL(AL_Error_Clear, 1 , &CommStatus );
	}
	//2012.05.20 by mgsong
	g_nSystemError  = 0;
	g_nWaferStatusA = 0;
	g_nWaferStatusB = 0;
	//2012.05.20 by mgsong


	while ( TRUE ) {
		Data = READ_DIGITAL_TRUE(MTR5_Run_Status , &CommStatus );
		if ( !CommStatus ) {
			AlmResult = ALARM_MANAGE( ALARM_INDEX_POINTER + ALARM_COMM_ERROR );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		} else if ( Data == 2 ) { //Got Fault or error occur
			//modified by jonghyuk 2011.03.09
			error    = (int) READ_DIGITAL( MTR5_ErrorCode  , &CommStatus );
			errlevel = (int) READ_DIGITAL( MTR5_ErrorLevel , &CommStatus ); 

			if (1 == errlevel) {
				for( i=0 ; i<WARNING1_ERROR_COUNT ; i++ ) {
					if ( ALARM_NUMBER[i] == error )	{error_code = i+101; break; }
					else							 error_code = ALARM_OTHER;
				}
			} else if (2 == errlevel) {
				for( i=WARNING1_ERROR_COUNT ; i < WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT ; i++ ) {
					if ( ALARM_NUMBER[i] == error )	{error_code = i+101; break; }
					else							 error_code = ALARM_OTHER;
				}
			} else if (3 == errlevel) {
				for( i=WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT  ; i<WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT + ALARM1_ERROR_COUNT ; i++ ) {
					if ( ALARM_NUMBER[i] == error )	{error_code = i+101; break; }
					else							 error_code = ALARM_OTHER;
				}
			} else if (4 == errlevel) {
				for( i=WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT + ALARM1_ERROR_COUNT  ; i<WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT + ALARM1_ERROR_COUNT + ALARM2_ERROR_COUNT ; i++ ) {
					if ( ALARM_NUMBER[i] == error )	{error_code = i+101; break; }
					else							 error_code = ALARM_OTHER;
				}
			} else if (5 == errlevel) {
				for( i=WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT + ALARM1_ERROR_COUNT + ALARM2_ERROR_COUNT ; i<1032 ; i++ ) {
					if ( ALARM_NUMBER[i] == error )	{error_code = i+101; break; }
					else							 error_code = ALARM_OTHER;
				}
			} else error_code = ALARM_OTHER;

			ALARM_MESSAGE_REMAPPING ( ALARM_INDEX_POINTER + error_code );
			AlmResult = ALARM_MANAGE( ALARM_INDEX_POINTER + error_code );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;

		} else if (Data == 3 || Data == 4) { //Got Invalid Data or Disconnected
			WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_READY , &CommStatus );
			AlmResult = ALARM_MANAGE( ALARM_INDEX_POINTER + ALARM_COMM_ERROR );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) return SYS_ABORTED;
		}
		else return SYS_SUCCESS;
	}
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//2012.05.20 by mgsong
BOOL IsValidToContinueNext()
{
	BOOL bRet = FALSE;
	int  nLoop= 0;
	int  CommStatus;
	

	//////////////////////////////////////////////////////////////////////
	//우선 IO Time Out에 의하여 이쪽으로 온것이면 충분한 대기 시간 가지고
	//////////////////////////////////////////////////////////////////////
	if (SYS_T_OUT == g_nExceptionStatus) {
		_LOG("o,.O  Start System Communication Retry Wait after IO Time Out", nLoop);
		if (!WAIT_SECONDS(20)) return FALSE;
	}
	

       for (nLoop = 0 ; nLoop < 3 ; nLoop++) {
		_LOG("o,.O IO Timeout Recovery Sequence[%d] Started after IO Time Out Alarm", nLoop);
		//1. 우선 통신 중단과 관련된 약간의 안정화 시간을 준다.
		if (!WAIT_SECONDS(3)) {
			bRet = FALSE; break;
		}

		//2. IO Write 동작 Timeout이 발생한 상황이니 System Status부터 Check
		WRITE_DIGITAL( MTR5_SysStsChk , 1 , &CommStatus );
		if(CommStatus) {
			g_nSystemError = READ_DIGITAL(MTR5_SysErrorCode , &CommStatus);	
			g_nWaferStatusA= READ_DIGITAL(MTR5_WfrSts       , &CommStatus);
			g_nWaferStatusB= READ_DIGITAL(MTR5_WfrStsB      , &CommStatus);
		}
		else {
			_LOG("o,.O IO Timeout Recovery Sequence[%d] System Status Check Failed due to Timeout again...", nLoop);
			if (!WAIT_SECONDS(5)) return FALSE;
			continue;			
		}

		//3. System Status Check 명령이 정상적으로 수행 되었다.
		bRet = TRUE; break;
	}

	//////////////////////////////////////////////////////////////////////////
	//상위에서 System Error Check가 끝났으면
	//////////////////////////////////////////////////////////////////////////
	char szMessage[64] = {0,};
	if (SYS_T_OUT == g_nExceptionStatus) {
		if (TRUE == bRet) {
			sprintf(szMessage ,"ATMRobot IO_TIME_OUT_LastError(%x)" , g_nSystemError);			
		}
		else {
			sprintf(szMessage ,"ATM Robot IO_TIME_OUT_CommunicationError");
		}
		ALARM_MESSAGE_SET(ALARM_ROBOT_IOFAIL_TIMEOUT , szMessage);
		ALARM_POST       (ALARM_ROBOT_IOFAIL_TIMEOUT);
		
		_LOG("o,.O Checked System Error(%x) after IO Timeout", g_nSystemError);
	}
	else {
		if (TRUE == bRet) {
			_LOG("o,.O Checked System Error(%x)", g_nSystemError);
		}
	}

	return bRet;
}
//2012.05.20 by mgsong


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RunCheckRobot( BOOL HomeMode , int *Data , int retalarm ,int Station,int Arm) {
	int  error , i , error_code, errlevel;
	char szRobot_err[32] = {0};
	int CommStatus , AlmResult;
	*Data = READ_DIGITAL_TRUE( MTR5_Run_Status , &CommStatus );
	printf ("data = %d \n" , *Data);

	if ( !CommStatus ) {
		WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_READY , &CommStatus );
		AlmResult = ALARM_MANAGE( ALARM_INDEX_POINTER + ALARM_COMM_ERROR );
		if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
		else if ( AlmResult != ALM_RETRY  ) 
		{
			g_nRobotStatus = RB_READY;
			return SYS_ABORTED;
		}
	}
	else if ( *Data  == 2 ) { //Got Fault or error occur
			//modified by jonghyuk 2011.03.09
			error	 = (int) READ_DIGITAL( MTR5_ErrorCode  , &CommStatus );
			errlevel = (int) READ_DIGITAL( MTR5_ErrorLevel , &CommStatus ); 

			if (1 == errlevel) {
				for( i=0 ; i<WARNING1_ERROR_COUNT ; i++ ) {
					if ( ALARM_NUMBER[i] == error )	{error_code = i+101; break; }
					else							 error_code = ALARM_OTHER;
				}
			} else if (2 == errlevel) {
				for( i=WARNING1_ERROR_COUNT ; i < WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT ; i++ ) {
					if ( ALARM_NUMBER[i] == error )	{error_code = i+101; break; }
					else							 error_code = ALARM_OTHER;
				}
			} else if (3 == errlevel) {
				for( i=WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT  ; i<WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT + ALARM1_ERROR_COUNT ; i++ ) {
					if ( ALARM_NUMBER[i] == error )	{error_code = i+101; break; }
					else							 error_code = ALARM_OTHER;
				}
			} else if (4 == errlevel) {
				for( i=WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT + ALARM1_ERROR_COUNT  ; i<WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT + ALARM1_ERROR_COUNT + ALARM2_ERROR_COUNT ; i++ ) {
					if ( ALARM_NUMBER[i] == error )	{error_code = i+101; break; }
					else							 error_code = ALARM_OTHER;
				}
			} else if (5 == errlevel) {
				for( i=WARNING1_ERROR_COUNT + WARNING2_ERROR_COUNT + ALARM1_ERROR_COUNT + ALARM2_ERROR_COUNT ; i<1032 ; i++ ) {
					if ( ALARM_NUMBER[i] == error )	{error_code = i+101; break; }
					else							 error_code = ALARM_OTHER;
				}
			} else error_code = ALARM_OTHER;

			ALARM_MESSAGE_REMAPPING( ALARM_INDEX_POINTER  + error_code );
			AlmResult = ALARM_MANAGE( ALARM_INDEX_POINTER + error_code );
			if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
			else if ( AlmResult != ALM_RETRY  ) 
			{
				g_nRobotStatus = RB_READY;
				return SYS_ABORTED;
			}

	} else if (*Data == 3 || *Data == 4) { //Got Invalid Data or Disconnected
		WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_READY , &CommStatus );
		AlmResult = ALARM_MANAGE( ALARM_INDEX_POINTER + ALARM_COMM_ERROR );
		if      ( AlmResult == ALM_IGNORE ) return SYS_SUCCESS;
		else if ( AlmResult != ALM_RETRY  ) 
		{
			g_nRobotStatus = RB_READY;
			return SYS_ABORTED;
		}
	}
	else return SYS_SUCCESS;
	return SYS_RUNNING;	
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RobotMovement_org(
							int Station , int Arm ,
							int A_IOP   , int B_IOP ,
							int ALARM   ,
							int Synch_Start , int Synch_End   , int Last_Retract ,
							int Arm_RetExt  , int Arm_Station , int Arm_Slot     , int Arm_UpDown ,
							int rejectalarm
							)
{
	int Data , Check;
	int CommStatus , AlmResult;
	BOOL bAsyncMove     = FALSE;
	int  nRetryCount    = 0;	
	Module_Status msRet = SYS_RUNNING;


	g_nRobotStatus = RB_DOING_STEP1;
	if ( Last_Retract >= 0 ) WRITE_DIGITAL( ROBOT_RET , 1 , &CommStatus );
	WRITE_DIGITAL( ROBOT_STATION , Station , &CommStatus );
	g_nRobotStatus = RB_DOING_STEP2;

	do {
			// 2003.05.14 yeon add //////////////////////////////////////////////////////////////////////////////
			if ( A_IOP != MTR5_GetAL_Robot ) 
			{
				if ( Synch_Start >= 0 ) WRITE_DIGITAL( ROBOT_SYNCH , Synch_Start , &CommStatus );
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////

			//2009.06.24 ATM Robot Pick/Place Door Open Check Retry 
			//           No Alarm Just Recheck 3 times if not opened interlock occur too
			// BM Station Only
			if (MODULE_CONTROL_STATION[BM1] == Station || MODULE_CONTROL_STATION[BM2] == Station ) {
				
			    while (nRetryCount < 10) {
					//skip sensor check if rotate
					if (10 == Arm_UpDown) break;

					if (MODULE_CONTROL_STATION[BM1] == Station /*BM1*/) {
						 if (1 == READ_DIGITAL(BM1LeftDoor , &CommStatus) && 1 == READ_DIGITAL(BM1RightDoor , &CommStatus))
						  break;
					} 
					else {
						 if (1 == READ_DIGITAL(BM2LeftDoor , &CommStatus) && 1 == READ_DIGITAL(BM2RightDoor , &CommStatus))
						  break;
					}

					printf("BM Door Sensor check retry...\n");
					Sleep(1000);
					nRetryCount++;				     
			    }			           
			}
			//2009.06.24 END

			if ( Arm == ARM_A ) WRITE_DIGITAL( A_IOP , Station , &CommStatus );
			else                WRITE_DIGITAL( B_IOP , Station , &CommStatus );

			// 2005.12.19
			if (CommStatus)
			{
				//여기서 RB_DONE으로
				g_nRobotStatus = RB_DONE;
			}
			else
			{
				//일단 여기서는 그냥 빠져 나간다.
				
				msRet = SYS_ABORTED; g_nRobotStatus = RB_READY;
				g_nExceptionStatus = SYS_T_OUT;
				//ALARM_POST(ALARM_ROBOT_IOFAIL_TIMEOUT); 
				break; 
			}

			S_TIMER_READY();

			do {
					// change atm robot monitoring time gap 0.1 -> 0.02 sec
					// - 2007.09.06 ijbae.
					//if ( !WAIT_SECONDS( 0.1 ) ) 
					if ( !WAIT_SECONDS( 0.02 ) ) 
					{
						_sleep(500);
						WRITE_DIGITAL( MTR5_ErrorCode , 0 , &CommStatus );
						//------------------------------------------

						// 2005.12.21
						WRITE_DIGITAL( ROBOT_RET , 0 , &CommStatus );
						msRet = SYS_ABORTED;
						break;
					}

					// 2003.05.14 yeon add //////////////////////////////////////////////////////////////////////////////
					// 2005.12.21
					if ( S_TIMER_CHECK( 1 , 1.5 ) && A_IOP == MTR5_GetAL_Robot ) {
						if ( Synch_Start >= 0 ) WRITE_DIGITAL( ROBOT_SYNCH , Synch_Start , &CommStatus );
					}
					/////////////////////////////////////////////////////////////////////////////////////////////////////
					Check = RunCheckRobot( ( Station == 0 ) , &Data , rejectalarm , Station , Arm);
					if      ( Check == SYS_ABORTED ) { msRet = SYS_ABORTED; break; }
					else if ( Check == SYS_RUNNING ) break;

					if ( Data == 0) 
					{
						if ( Last_Retract >= 0 ) WRITE_DIGITAL( ROBOT_RET , Last_Retract , &CommStatus );
						if ( Synch_End    >= 0 ) WRITE_DIGITAL( ROBOT_SYNCH , Synch_End , &CommStatus );
						{ msRet = SYS_SUCCESS; break; }
						
					}

					if ( S_TIMER_CHECK( 1 , (double) READ_DIGITAL( MOVE_WAFER_TIMEOUT , &CommStatus ) ) ) 
					{
						_sleep(500);
						WRITE_DIGITAL( MTR5_ErrorCode , 0 , &CommStatus );
						//------------------------------------------
						AlmResult = ALARM_MANAGE( ALARM_INDEX_POINTER + ALARM );
						WRITE_DIGITAL( MTR5_T_Error_Clear , 1 , &CommStatus );
						S_TIMER_READY();
						if      ( AlmResult == ALM_IGNORE ) {
							if ( Last_Retract >= 0 ) WRITE_DIGITAL( ROBOT_RET , Last_Retract , &CommStatus );
							if ( Synch_End    >= 0 ) WRITE_DIGITAL( ROBOT_SYNCH , Synch_End , &CommStatus );
							{ msRet = SYS_SUCCESS; break; }
						}
						else if ( AlmResult != ALM_RETRY  ) { msRet = SYS_ABORTED; break; }
						else                                break;
					}
			} while(1);
	} while(msRet == SYS_RUNNING);

	if(msRet != SYS_SUCCESS)
	{
		// 2005.12.21
		if(A_IOP == MTR5_GetAL_Robot ) 
		{
			if ( g_nRobotStatus == RB_DONE ) WRITE_DIGITAL( ROBOT_SYNCH , Synch_End , &CommStatus );
		}
	}
	return msRet;
}

Module_Status RobotMovement(
							int Station , int Arm ,
							int A_IOP , int B_IOP ,
							int ALARM ,
							int Synch_Start , int Synch_End , int Last_Retract ,
							int Arm_RetExt , int Arm_Station , int Arm_Slot , int Arm_UpDown ,
							int rejectalarm
							)
{
	//우선 이곳에서 사용 되어 질 변수를 초기화 한다.
	//2012.05.20 by mgsong
	g_nSystemError     = 0;
	g_nWaferStatusA    = 0;
	g_nWaferStatusB    = 0;
	g_nExceptionStatus = SYS_NONE;
	//2012.05.20 by mgsong

	return RobotMovement_org(Station , Arm , A_IOP , B_IOP , ALARM ,
							 Synch_Start , Synch_End , Last_Retract ,
							 Arm_RetExt , Arm_Station , Arm_Slot , Arm_UpDown ,	rejectalarm);
}

Module_Status RobotMovement_AL(
							int Station , int Arm ,
							int A_IOP , int B_IOP ,
							int ALARM ,
							int Synch_Start , int Synch_End , int Last_Retract ,
							int Arm_RetExt , int Arm_Station , int Arm_Slot , int Arm_UpDown ,
							int rejectalarm
							)
{
	return RobotMovement_org(Station , Arm , A_IOP , B_IOP , ALARM ,
							 Synch_Start , Synch_End , Last_Retract ,
							 Arm_RetExt , Arm_Station , Arm_Slot , Arm_UpDown ,	rejectalarm);
}

//////////////////////////////////////////////////////////////////////////
Module_Status HomeRobot() {
	Module_Status msRet = SYS_ABORTED;
	int CommStatus;

	do 
	{
		//... Robot Home
		msRet = RobotMovement( 0 , 0 , 
			MTR5_HOME , MTR5_HOME ,
			ALARM_HOME_WAFER ,
			-1 , -1 , 0 ,
			-1 , -1 , -1 , -1 , -1 );

		if(SYS_SUCCESS != msRet)	break;

		if(0 == READ_DIGITAL(FC_TM_ALIGNER_CONTROL,&CommStatus)) //... 0: Use, 1: NotUse
		{
			//... Aligner Home
			WRITE_DIGITAL(AL_HOME, 1 , &CommStatus);
			if(! CommStatus) 			break;
		}
		msRet = SYS_SUCCESS;
	} while (0);

	return msRet;
}

//////////////////////////////////////////////////////////////////////////
Module_Status InitRobot() {
	Module_Status msRet = SYS_SUCCESS;
	int CommStatus;
	
	//... Robot Reset
	WRITE_DIGITAL(MTR5_T_Error_Clear , 1 , &CommStatus);
	if(! CommStatus) 		return SYS_ABORTED;
	if(! WAIT_SECONDS(2))	return SYS_ABORTED;
	if(0 == READ_DIGITAL(FC_TM_ALIGNER_CONTROL,&CommStatus)) //... 0: Use, 1: NotUse
	{
		//... Aligner Reset
		WRITE_DIGITAL(AL_Error_Clear, 1 , &CommStatus);
		if(! CommStatus) 		return SYS_ABORTED;
		if(! WAIT_SECONDS(2))	return SYS_ABORTED;
	}
	return msRet;
}

Module_Status PickRobot( int EStation , int Station , int Arm , BOOL TR, BOOL DB ) {
	int CommStatus;
	Module_Status msRet = SYS_ABORTED;
	do {
		g_nRobotStatus = RB_READY;
		if (TR) {
			if (RobotMovement(Station , Arm ,
							  MTR5_Get_Robot , MTR5_Get_RobotB ,
							  ALARM_PICK_WAFER ,
							  ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
							  -1 , -1 , -1 , -1 , 741 ) == SYS_ABORTED ) break;
		} else {
			if (DB) {
				if (RobotMovement(Station , Arm ,
								  MTR5_GetDB_Robot , -1 ,
								  ALARM_PICK_WAFER ,
								  ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
								  ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_UP , -1 ) == SYS_ABORTED )break;
			} else {
				if (RobotMovement(Station , Arm ,
								  MTR5_Get_Robot , MTR5_Get_RobotB ,
								  ALARM_PICK_WAFER ,
								  ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
								  ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_UP , -1 ) == SYS_ABORTED ) break;
			}
		}
		msRet = SYS_SUCCESS;
	} while(0);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2012.05.20 by mgsong
	//Wafer Sync & IO Timeout Recover
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (msRet != SYS_SUCCESS) {
		//2012.05.20 by mgsong
		//IO에 대한 동작이 실패 했으므로 Grip 상태를 확인하여 Sync Reverse or not 을 결정하고
		//만약 Error등 기본 정보가 맞으면 정상 처리도 가능  3번의 Retry를 한다.
		if (TRUE == IsValidToContinueNext()) 
		{
			//만약 950 Error인 경우 
			printf("system error %x \n" , g_nSystemError);
			if (g_nSystemError == 0x950 || g_nSystemError == 0x00/*ACKTimeout*/) {
				if (TRUE == DB) {
					if (WAFER_PRESENT == g_nWaferStatusA && WAFER_PRESENT == g_nWaferStatusB) {
						WRITE_DIGITAL( MTR5_T_Error_Clear , 1 , &CommStatus );
						if (CommStatus) {
							if (Station != AL) {
								msRet = SYS_SUCCESS;
								_LOG("o,.O System Recovered After Check IO Time Out");
							}
							else {
								_LOG("o,.O No Error but aligner safety After Check IO Time Out");								
							}
						}
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_A == Arm) {
					if (WAFER_PRESENT == g_nWaferStatusA) {
						WRITE_DIGITAL( MTR5_T_Error_Clear , 1 , &CommStatus );
						if (CommStatus) {
							if (Station != AL) {
								msRet = SYS_SUCCESS;
								_LOG("o,.O System Recovered After Check IO Time Out");
							}
							else {
								_LOG("o,.O No Error but aligner safety After Check IO Time Out");								
							}							
						}
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_B == Arm) {
					if (WAFER_PRESENT == g_nWaferStatusB) {
						WRITE_DIGITAL( MTR5_T_Error_Clear , 1 , &CommStatus );
						if (CommStatus) {
							if (Station != AL) {
								msRet = SYS_SUCCESS;
								_LOG("o,.O System Recovered After Check IO Time Out");
							}
							else {
								_LOG("o,.O No Error but aligner safety After Check IO Time Out");								
							}
						}
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else {
					WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
					_sleep(1000);
					_LOG("----------> WaferSync Reversed !");	
				}

			}
			//다른 Error Code이거나 상위에서 이미 Clear되고 나온 것 명령에 따른 Sync만 맞추자
			else {
				if (TRUE == DB) {
					if (WAFER_PRESENT == g_nWaferStatusA && WAFER_PRESENT == g_nWaferStatusB) {
						_sleep(1000);
						_LOG("Wafer Status Valid so Sync Updated!!");
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_A == Arm) {
					if (WAFER_PRESENT == g_nWaferStatusA) {
						_sleep(1000);
						_LOG("Wafer Status Valid so Sync Updated!!");
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_B == Arm) {
					if (WAFER_PRESENT == g_nWaferStatusB) {
						_sleep(1000);
						_LOG("Wafer Status Valid so Sync Updated!!");
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else {
					WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
					_sleep(1000);
					_LOG("----------> WaferSync Reversed !");	
				}
			}
		}
		else {
			//어차피 중단될 것 Wafer Sync는 Reverse한다.
			_LOG("----------> Robot Func Aborted !");
			WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
			_sleep(1000);
			_LOG("----------> WaferSync Reversed !");
		}			
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2012.05.20 by mgsong
	//Wafer Sync & IO Timeout Recover
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return msRet;
}

//////////////////////////////////////////////////////////////////////////
// 2005.12.19
//... 2014.11.08 Add Aligner Align & Vacuum Grip OFF by YASKAWA Ethernet M124
#define ALARM_ALIGN_FAILED		3006
Module_Status Pick_AL_Robot( int Station , int Arm, BOOL DB ) {
	Module_Status msRet = SYS_ABORTED;
	int CommStatus;
	int	AlmResult;

	do {
		//... Aligner Align & Vacuum Grip Release
		WRITE_DIGITAL(AL_Align_Run, 1 , &CommStatus);
		if(! CommStatus)
		{
			AlmResult = ALARM_MANAGE(ALARM_ALIGN_FAILED);
			if(AlmResult == ALM_RETRY)			continue;
			else if(AlmResult == ALM_ABORT)		break;
			//... Case. Ignore
		}

		//... Robot Align Station Pick
		g_nRobotStatus = RB_READY;		
		if(RobotMovement_AL(Station , Arm ,
							 MTR5_GetAL_Robot , MTR5_GetAL_RobotB ,
							 ALARM_PICK_WAFER ,
							 ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
							 -1 , -1 , -1 , -1 , -1 ) == SYS_ABORTED ) break;
		msRet = SYS_SUCCESS;
		break;	//... Not Delete Unlimited Cycle !!!
	} while(1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2012.05.20 by mgsong
	//Wafer Sync & IO Timeout Recover
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (msRet != SYS_SUCCESS) {
		//2012.05.20 by mgsong
		//IO에 대한 동작이 실패 했으므로 Grip 상태를 확인하여 Sync Reverse or not 을 결정하고
		//만약 Error등 기본 정보가 맞으면 정상 처리도 가능  3번의 Retry를 한다.
		if (TRUE == IsValidToContinueNext()) 
		{
			//만약 950 Error인 경우 
			if (g_nSystemError == 0x950/*ACKTimeout*/) {
				if (TRUE == DB) {
					if (WAFER_PRESENT == g_nWaferStatusA && WAFER_PRESENT == g_nWaferStatusB) {
						WRITE_DIGITAL( MTR5_T_Error_Clear , 1 , &CommStatus );
						if (CommStatus) {
							if (Station != AL) {
								msRet = SYS_SUCCESS;
								_LOG("o,.O System Recovered After Check IO Time Out");
							}
							else {
								_LOG("o,.O No Error but aligner safety After Check IO Time Out");								
							}
						}
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_A == Arm) {
					if (WAFER_PRESENT == g_nWaferStatusA) {
						WRITE_DIGITAL( MTR5_T_Error_Clear , 1 , &CommStatus );
						if (CommStatus) {
							if (Station != AL) {
								msRet = SYS_SUCCESS;
								_LOG("o,.O System Recovered After Check IO Time Out");
							}
							else {
								_LOG("o,.O No Error but aligner safety After Check IO Time Out");								
							}
						}
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_B == Arm) {
					if (WAFER_PRESENT == g_nWaferStatusB) {
						WRITE_DIGITAL( MTR5_T_Error_Clear , 1 , &CommStatus );
						if (CommStatus) {
							if (Station != AL) {
								msRet = SYS_SUCCESS;
								_LOG("o,.O System Recovered After Check IO Time Out");
							}
							else {
								_LOG("o,.O No Error but aligner safety After Check IO Time Out");								
							}
						}
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else {
					WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
					_sleep(1000);
					_LOG("----------> WaferSync Reversed !");	
				}

			}
			//다른 Error Code이거나 상위에서 이미 Clear되고 나온 것 명령에 따른 Sync만 맞추자
			else {
				if (TRUE == DB) {
					if (WAFER_PRESENT == g_nWaferStatusA && WAFER_PRESENT == g_nWaferStatusB) {
						_sleep(1000);
						_LOG("Wafer Status Valid so Sync Updated!!");
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_A == Arm) {
					if (WAFER_PRESENT == g_nWaferStatusA) {
						_sleep(1000);
						_LOG("Wafer Status Valid so Sync Updated!!");
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_B == Arm) {
					if (WAFER_PRESENT == g_nWaferStatusB) {
						_sleep(1000);
						_LOG("Wafer Status Valid so Sync Updated!!");
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else {
					WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
					_sleep(1000);
					_LOG("----------> WaferSync Reversed !");	
				}
			}
		}
		else
		{
			_LOG("----------> Robot Func Aborted !");
			WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
			_sleep(1000);
			_LOG("----------> WaferSync Reversed !");
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2012.05.20 by mgsong
	//Wafer Sync & IO Timeout Recover
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// 2005.12.19
Module_Status PlaceRobot( int EStation , int Station , int Arm , BOOL DB ) {
	int CommStatus;
	Module_Status msRet = SYS_ABORTED;
	do {
		g_nRobotStatus = RB_READY;

		if (DB) {
			if (RobotMovement(Station , Arm ,
							  MTR5_PutDB_Robot , -1 ,
							  ALARM_PLACE_WAFER ,
							  ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
							  ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_DOWN , -1 ) == SYS_ABORTED ) break;
		} else {
			if (RobotMovement(Station , Arm ,
							  MTR5_Put_Robot , MTR5_Put_RobotB ,
							  ALARM_PLACE_WAFER ,
							  ROBOT_SYNCH_RETRACT_START , ROBOT_SYNCH_RETRACT_START , 0 ,
							  ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_DOWN , -1 ) == SYS_ABORTED ) break;
		}
		msRet = SYS_SUCCESS;
	} while(0);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2012.05.20 by mgsong
	//Wafer Sync & IO Timeout Recover
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (msRet != SYS_SUCCESS) {
		//2012.05.20 by mgsong
		//IO에 대한 동작이 실패 했으므로 Grip 상태를 확인하여 Sync Reverse or not 을 결정하고
		//만약 Error등 기본 정보가 맞으면 정상 처리도 가능  3번의 Retry를 한다.
		if (TRUE == IsValidToContinueNext()) 
		{
			//만약 950 Error인 경우 
			printf("system error %x \n" , g_nSystemError);
			if (g_nSystemError == 0x950 || g_nSystemError == 0x00/*ACKTimeout*/) {
				if (TRUE == DB) {
					if (WAFER_ABSENT == g_nWaferStatusA && WAFER_ABSENT == g_nWaferStatusB) {
						WRITE_DIGITAL( MTR5_T_Error_Clear , 1 , &CommStatus );
						if (CommStatus) {
							if (Station != AL) {
								msRet = SYS_SUCCESS;
								_LOG("o,.O System Recovered After Check IO Time Out");
							}
							else {
								_LOG("o,.O No Error but aligner safety After Check IO Time Out");								
							}
						}
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_A == Arm) {
					if (WAFER_ABSENT == g_nWaferStatusA) {
						WRITE_DIGITAL( MTR5_T_Error_Clear , 1 , &CommStatus );
						if (CommStatus) {
							if (Station != AL) {
								msRet = SYS_SUCCESS;
								_LOG("o,.O System Recovered After Check IO Time Out");
							}
							else {
								_LOG("o,.O No Error but aligner safety After Check IO Time Out");								
							}
						}
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_B == Arm) {
					if (WAFER_ABSENT == g_nWaferStatusB) {
						WRITE_DIGITAL( MTR5_T_Error_Clear , 1 , &CommStatus );
						if (CommStatus) {
							if (Station != AL) {
								msRet = SYS_SUCCESS;
								_LOG("o,.O System Recovered After Check IO Time Out");
							}
							else {
								_LOG("o,.O No Error but aligner safety After Check IO Time Out");								
							}
						}
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else {
					WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
					_sleep(1000);
					_LOG("----------> WaferSync Reversed !");	
				}

			}
			//다른 Error Code이거나 상위에서 이미 Clear되고 나온 것 명령에 따른 Sync만 맞추자
			else {
				if (TRUE == DB) {
					if (WAFER_ABSENT == g_nWaferStatusA && WAFER_ABSENT == g_nWaferStatusB) {
						_sleep(1000);
						_LOG("Wafer Status Valid so Sync Updated!!");
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_A == Arm) {
					if (WAFER_ABSENT == g_nWaferStatusA) {
						_sleep(1000);
						_LOG("Wafer Status Valid so Sync Updated!!");
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else if (ARM_B == Arm) {
					if (WAFER_ABSENT == g_nWaferStatusB) {
						_sleep(1000);
						_LOG("Wafer Status Valid so Sync Updated!!");
					}
					else {
						WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
						_sleep(1000);
						_LOG("----------> WaferSync Reversed !");						
					}
				}
				else {
					WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
					_sleep(1000);
					_LOG("----------> WaferSync Reversed !");	
				}
			}
		}
		else {
			_LOG("----------> Robot Func Aborted !");
			WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &CommStatus );
			_sleep(1000);
			_LOG("----------> WaferSync Reversed !");
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2012.05.20 by mgsong
	//Wafer Sync & IO Timeout Recover
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return msRet;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RotPicRobot( int Station , int Arm ) {
	return(
		RobotMovement( Station , Arm ,
			MTR5_RotG_Robot , MTR5_RotG_RobotB ,
			ALARM_ROTATE_WAFER ,
			ROBOT_SYNCH_ROTATE_START , ROBOT_SYNCH_SUCCESS , 0 ,
			ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_ROTATE , -1 )
		);
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Module_Status RotPlcRobot( int Station , int Arm ) {
	return(
		RobotMovement( Station , Arm ,
			MTR5_RotP_Robot , MTR5_RotP_RobotB ,
			ALARM_ROTATE_WAFER ,
			ROBOT_SYNCH_ROTATE_START , ROBOT_SYNCH_SUCCESS , 0 ,
			ROBOT_STATUS_RETRACT , Station , -1 , ROBOT_STATUS_ROTATE , -1 )
		);
}


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
BOOL Find_Chamber_Finger_From_String( char *String , int *EStation , int *Station , int *Arm , char *SlotString , int *Slot ) {
	int offset = 0 , GoArm = ARM_A;
	if ( strlen( String ) < 2 ) return FALSE;
	if ( String[ 1 ] == '_' ) {
		offset = 2;
		switch( String[ 0 ] ) {
		case 'S' :	case 's' :	case 'A' :	case 'a' :	GoArm = ARM_A;	break;
		case 'T' :	case 't' :	case 'B' :	case 'b' :	GoArm = ARM_B;	break;
		default  :	return FALSE;	break;
		}
	}
	if      ( STRCMP_L( String + offset , "CM1"  ) ) { *EStation = CM1; }
	else if ( STRCMP_L( String + offset , "CM2"  ) ) { *EStation = CM2; }
	else if ( STRCMP_L( String + offset , "CM3"  ) ) { *EStation = CM3; }
	else if ( STRCMP_L( String + offset , "AL"   ) ) { *EStation = AL; }
	else if ( STRCMP_L( String + offset , "BM1"  ) ) { *EStation = BM1; }
	else if ( STRCMP_L( String + offset , "BM2"  ) ) { *EStation = BM2; }
	else {
		return FALSE;
	}
	*Slot = atoi( SlotString );

	//2011.04.06
	if (*EStation == AL) *Slot = 1;

	// station is BM1 or BM2
	if( (*EStation == BM1) || (*EStation == BM2) )
	{
		if(*Slot <= 0 || *Slot >= 5)
		{
			return FALSE;
		}
	}

	*Station = MODULE_CONTROL_STATION[*EStation];
	*Arm = GoArm;
	return TRUE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//2014.08.06
void PostPickEvent(int Station,int Arm, BOOL DB)
{
	int nComstatus;
	int nWaferID1 = 0, nWaferID2 = 0;
	int nWaferSource1 = 0, nWaferSource2 = 0;
	int nWaferResult1 = 0, nWaferResult2 = 0;
	char szWaferLotID1[40],szWaferLotID2[40];
	char szWaferMID1[40],szWaferMID2[40];

	memset(szWaferLotID1,0,sizeof(szWaferLotID1));
	memset(szWaferLotID2,0,sizeof(szWaferLotID2));
	memset(szWaferMID1	,0,sizeof(szWaferMID1));
	memset(szWaferMID2	,0,sizeof(szWaferMID2));		

	
	if(Arm == ARM_A && DB == FALSE)
	{
		WRITE_STRING(FM_RB_LOTID2,szWaferLotID2,&nComstatus);
		WRITE_STRING(FM_RB_MIDID2,szWaferMID2,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID2,0,&nComstatus);
		
		nWaferResult1 = READ_DIGITAL(FA_Wafer_Result,&nComstatus);
		nWaferResult2 = READ_DIGITAL(FB_Wafer_Result,&nComstatus);

		if((Station == 6 || Station == 7)&&(nWaferResult1 != 0 || nWaferResult2 != 0)){nWaferID1 = READ_DIGITAL(FB_Wafer_Status,&nComstatus);}
		else{nWaferID1 = READ_DIGITAL(FA_Wafer_Status,&nComstatus);}
		nWaferSource1 = READ_DIGITAL(FA_Wafer_Source,&nComstatus);

		switch(nWaferSource1)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID1,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID1,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID1,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID1,"");
				strcpy(szWaferMID1,"");
				nWaferID1 = 0;
				break;
		}
		WRITE_STRING(FM_RB_LOTID1,szWaferLotID1,&nComstatus);
		WRITE_STRING(FM_RB_MIDID1,szWaferMID1,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID1,nWaferID1,&nComstatus);
	}
	else if(Arm == ARM_B && DB == FALSE)
	{
		WRITE_STRING(FM_RB_LOTID2,szWaferLotID2,&nComstatus);
		WRITE_STRING(FM_RB_MIDID2,szWaferMID2,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID2,0,&nComstatus);
		nWaferResult1 = READ_DIGITAL(FA_Wafer_Result,&nComstatus);
		nWaferResult2 = READ_DIGITAL(FB_Wafer_Result,&nComstatus);

		if((Station == 6 || Station == 7)&&(nWaferResult1 != 0 || nWaferResult2 != 0)){nWaferID1 = READ_DIGITAL(FA_Wafer_Status,&nComstatus);}
		else{nWaferID1 = READ_DIGITAL(FB_Wafer_Status,&nComstatus);}
		nWaferSource1 = READ_DIGITAL(FB_Wafer_Source,&nComstatus);

		switch(nWaferSource1)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID1,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID1,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID1,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID1,"");
				strcpy(szWaferMID1,"");
				nWaferID1 = 0;
				break;
		}
		WRITE_STRING(FM_RB_LOTID1,szWaferLotID1,&nComstatus);
		WRITE_STRING(FM_RB_MIDID1,szWaferMID1,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID1,nWaferID1,&nComstatus);
	}
	else if(DB == TRUE)
	{
		WRITE_STRING(FM_RB_LOTID1,szWaferLotID1,&nComstatus);
		WRITE_STRING(FM_RB_MIDID1,szWaferMID1,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID1,0,&nComstatus);
		WRITE_STRING(FM_RB_LOTID2,szWaferLotID2,&nComstatus);
		WRITE_STRING(FM_RB_MIDID2,szWaferMID2,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID2,0,&nComstatus);
		
		nWaferID1 = READ_DIGITAL(FA_Wafer_Status,&nComstatus);
		nWaferSource1 = READ_DIGITAL(FA_Wafer_Source,&nComstatus);
		nWaferID2 = READ_DIGITAL(FB_Wafer_Status,&nComstatus);
		nWaferSource2 = READ_DIGITAL(FB_Wafer_Source,&nComstatus);
		switch(nWaferSource1)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID1,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID1,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID1,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID1,"");
				strcpy(szWaferMID1,"");
				nWaferID1 = 0;
				nWaferID2 = 0;
				break;
		}
		WRITE_STRING(FM_RB_LOTID1,szWaferLotID1,&nComstatus);
		WRITE_STRING(FM_RB_MIDID1,szWaferMID1,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID1,nWaferID1,&nComstatus);
		WRITE_STRING(FM_RB_LOTID2,szWaferLotID1,&nComstatus);
		WRITE_STRING(FM_RB_MIDID2,szWaferMID1,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID2,(nWaferID1 + 1),&nComstatus);
	}

	switch(Station)
	{
		case 1:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_LPM1,&nComstatus);
			break;
		case 2:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_LPM2,&nComstatus);
			break;
		case 3:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_LPM3,&nComstatus);
			break;
		case 6:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_LL1,&nComstatus);
			break;
		case 7:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_LL2,&nComstatus);
			break;
		case 10:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PICK_TO_AL,&nComstatus);
			break;
		default:
			break;
	}
}

void PostPlaceEvent(int Station,int Arm, BOOL DB, BOOL bIOReadOk)
{
	int nComstatus;
	int nWaferID1 = 0, nWaferID2 = 0;
	int nWaferSource1 = 0, nWaferSource2 = 0;
	char szWaferLotID1[40],szWaferLotID2[40];
	char szWaferMID1[40],szWaferMID2[40];

	memset(szWaferLotID1,0,sizeof(szWaferLotID1));
	memset(szWaferLotID2,0,sizeof(szWaferLotID2));
	memset(szWaferMID1	,0,sizeof(szWaferMID1));
	memset(szWaferMID2	,0,sizeof(szWaferMID2));				

	if(Arm == ARM_A && DB == FALSE && bIOReadOk == TRUE)
	{
		WRITE_STRING(FM_RB_LOTID2,szWaferLotID2,&nComstatus);
		WRITE_STRING(FM_RB_MIDID2,szWaferMID2,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID2,0,&nComstatus);

		nWaferID1 = READ_DIGITAL(FA_Wafer_Status,&nComstatus);
		nWaferSource1 = READ_DIGITAL(FA_Wafer_Source,&nComstatus);

		switch(nWaferSource1)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID1,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID1,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID1,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID1,"");
				strcpy(szWaferMID1,"");
				nWaferID1 = 0;
				break;
		}
		WRITE_STRING(FM_RB_LOTID1,szWaferLotID1,&nComstatus);
		WRITE_STRING(FM_RB_MIDID1,szWaferMID1,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID1,nWaferID1,&nComstatus);
	}
	else if(Arm == ARM_B && DB == FALSE && bIOReadOk == TRUE)
	{
		WRITE_STRING(FM_RB_LOTID2,szWaferLotID2,&nComstatus);
		WRITE_STRING(FM_RB_MIDID2,szWaferMID2,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID2,0,&nComstatus);

		nWaferID1 = READ_DIGITAL(FB_Wafer_Status,&nComstatus);
		nWaferSource1 = READ_DIGITAL(FB_Wafer_Source,&nComstatus);

		switch(nWaferSource1)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID1,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID1,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID1,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID1,"");
				strcpy(szWaferMID1,"");
				nWaferID1 = 0;
				break;
		}
		WRITE_STRING(FM_RB_LOTID1,szWaferLotID1,&nComstatus);
		WRITE_STRING(FM_RB_MIDID1,szWaferMID1,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID1,nWaferID1,&nComstatus);
	}
	else if(DB == TRUE && bIOReadOk == TRUE)
	{
		WRITE_STRING(FM_RB_LOTID1,szWaferLotID1,&nComstatus);
		WRITE_STRING(FM_RB_MIDID1,szWaferMID1,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID1,0,&nComstatus);
		WRITE_STRING(FM_RB_LOTID2,szWaferLotID2,&nComstatus);
		WRITE_STRING(FM_RB_MIDID2,szWaferMID2,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID2,0,&nComstatus);

		nWaferID1 = READ_DIGITAL(FA_Wafer_Status,&nComstatus);
		nWaferSource1 = READ_DIGITAL(FA_Wafer_Source,&nComstatus);
		nWaferID2 = READ_DIGITAL(FB_Wafer_Status,&nComstatus);
		nWaferSource2 = READ_DIGITAL(FB_Wafer_Source,&nComstatus);
		switch(nWaferSource1)
		{
			case SRC_A:
				READ_STRING(JOB_NAME,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME,szWaferMID1,&nComstatus);
				break;
			case SRC_B:
				READ_STRING(JOB_NAME2,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME2,szWaferMID1,&nComstatus);
				break;
			case SRC_C:
				READ_STRING(JOB_NAME3,szWaferLotID1,&nComstatus);
				READ_STRING(MID_NAME3,szWaferMID1,&nComstatus);
				break;
			case SRC_X:
			case SRC_Y:
			case SRC_D:
				strcpy(szWaferLotID1,"");
				strcpy(szWaferMID1,"");
				nWaferID1 = 0;
				nWaferID2 = 0;
				break;
		}
		WRITE_STRING(FM_RB_LOTID1,szWaferLotID1,&nComstatus);
		WRITE_STRING(FM_RB_MIDID1,szWaferMID1,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID1,nWaferID1,&nComstatus);
		WRITE_STRING(FM_RB_LOTID2,szWaferLotID1,&nComstatus);
		WRITE_STRING(FM_RB_MIDID2,szWaferMID1,&nComstatus);
		WRITE_DIGITAL(FM_RB_WAFERID2,(nWaferID1 + 1),&nComstatus);
	}
	if(bIOReadOk == TRUE) return;

	switch(Station)
	{
		case 1:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_LPM1,&nComstatus);
			break;
		case 2:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_LPM2,&nComstatus);
			break;
		case 3:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_LPM3,&nComstatus);
			break;
		case 6:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_LL1,&nComstatus);
			break;
		case 7:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_LL2,&nComstatus);
			break;
		case 10:
			WRITE_STRING(SCHEDULER,EVENT_STRING_PLACE_TO_AL,&nComstatus);
			break;
		default:
			break;
	}
}
//////////////////////////////////////////////////////////////////////////
Module_Status Simulation_Main()
{
	Module_Status msRet = SYS_SUCCESS;
	BOOL TR = FALSE, DB = FALSE;
	int	EStation , Station , Arm , sp , CommStatus;
	char RunStr[41] , TempStr[41] , TempStr2[41] , StationStr[41] , SlotStr[41] , ElseStr[41];
	char *pParam;
	char szCmd[80];

	// 2013.11.15
	int nSlot;
	// char szTemp[40];
	char szModule[40];
	char szWaferSts[20];

	pParam = PROGRAM_PARAMETER_READ();
	STR_SEPERATE( pParam	, RunStr     , TempStr	, 40 );
	STR_SEPERATE( TempStr	, StationStr , TempStr2	, 40 );
	STR_SEPERATE( TempStr2	, SlotStr	 , ElseStr	, 40 );

	// 2013.11.15
	if      (STRCMP_L(ElseStr , "TRANSFER2" )) TR = TRUE;
	else if (STRCMP_L(ElseStr , "DB"))         DB = TRUE;
	strcpy(szModule, StationStr);

	// 2013.11.15
	nSlot = atoi(SlotStr);

	_LOG("--------> %s Started in FM RbLow Func(Sim)", pParam);
	// if ( !WAIT_SECONDS( 2 ) ) return SYS_ABORTED;
	WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RETRACT_START , &Station );
	WRITE_DIGITAL( ROBOT_RET , 0 , &Station );

	do {

		if ( !Find_Chamber_Finger_From_String( StationStr , &EStation , &Station , &Arm , SlotStr , &sp ) ) {
			if ( ( !STRCMP_L( RunStr , "READY" ) ) && ( !STRCMP_L( RunStr , "HOME" ) ) ) {
				printf( "---> Robot Control Error - Not Support Station %s\n" , pParam );
				msRet = SYS_ABORTED;
				break;
			}
			sp = 0;
		}
		if ( sp > 0 ) WRITE_DIGITAL( MTR5_T_Slot_Pos , sp , &CommStatus );

		if(EStation == AL) nSlot = 1;			// 2013.11.19

		//2014.06.23 Simulation Code		
		if(STRCMP_L(RunStr , "PICK"))
		{
			sprintf(szWaferSts, "Absent");
		}
		else if(STRCMP_L(RunStr , "PLACE"))
		{
			sprintf(szWaferSts, "Present");
		}
		else
		{
			sprintf(szWaferSts, "");
		}
		//2014.06.23 Simulation Code

		// 2013.11.15
		LogEvent("ATM", RunStr, "Start", szModule, nSlot, Arm, szWaferSts, DB);
		if(STRCMP_L(RunStr,"PLACE")){ PostPlaceEvent(Station,Arm,DB,TRUE); }
		// 2005.12.09
		sprintf(szCmd,"ATM_ROBOT %s %s", RunStr, StationStr);
		if(SYS_SUCCESS != g_DynIO.dRUN_FUNCTION(TMSIM_ATM_ROBOT, szCmd))
		{
			// 2005.12.20
			WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_RESET_REVERSE , &Station );
			_sleep(1000);
			_LOG("-----------> %s Failed in FM RbLow Func(Sim)", pParam);
			msRet = SYS_ABORTED;
			break;
		}
		if(STRCMP_L(RunStr,"PICK")){
			PostPickEvent(Station,Arm,DB);
		}else if(STRCMP_L(RunStr,"PLACE")){
			PostPlaceEvent(Station,Arm,DB,FALSE);
		}
		
		
	} while(0);
	if(SYS_SUCCESS == msRet) WRITE_DIGITAL( ROBOT_SYNCH , ROBOT_SYNCH_SUCCESS , &Station );

	//2014.06.23 Simulation Code		
	if(STRCMP_L(RunStr , "PICK"))
	{
		sprintf(szWaferSts, "Present");
	}
	else if(STRCMP_L(RunStr , "PLACE"))
	{
		sprintf(szWaferSts, "Absent");
	}
	else
	{
		sprintf(szWaferSts, "");
	}
	//2014.06.23 Simulation Code
	
	// 2013.11.15
	LogEvent("ATM", RunStr, "End", szModule, nSlot, Arm, szWaferSts, DB);

	_LOG("--------> %s Finished in FM RbLow Func(Sim)", pParam);
	return msRet;
}

Module_Status Real_Main()
{
	Module_Status msRet = SYS_SUCCESS;
	BOOL TR = FALSE, DB = FALSE;
	int	EStation , Station , Arm , sp , CommStatus;
	char RunStr[41] , TempStr[41] , TempStr2[41] , StationStr[41] , SlotStr[41] , ElseStr[41];
	char *pParam;

	// 2013.11.15
	int nSlot/*, nSlot2*/;
	// char szTemp[40];
	int nWaferStatusA;
	int nWaferStatusB;
	char szModule[40];
	char szWaferSts[40];

	if (READ_DIGITAL( CONTROL , &Station ) == 1) return SYS_SUCCESS;

	//2012.05.20 by mgsong
	g_nSystemError = 0; g_nWaferStatusA = 0; g_nWaferStatusB = 0;
	//2012.05.20 by mgsong

	pParam = PROGRAM_PARAMETER_READ();
	STR_SEPERATE(pParam	   , RunStr     , TempStr	, 40);
	STR_SEPERATE(TempStr   , StationStr , TempStr2	, 40);
	STR_SEPERATE(TempStr2  , SlotStr	, ElseStr	, 40);
	strcpy(szModule, StationStr);

	printf ("FM Robot RunStr %s , Station %s , Slot %s , else %s \n" , RunStr , StationStr , SlotStr , ElseStr);
	do {
		if      (STRCMP_L(ElseStr , "TRANSFER2" )) TR = TRUE;
		else if (STRCMP_L(ElseStr , "DB"))         DB = TRUE;

		// 2013.11.15
		nSlot = atoi(SlotStr);

		printf("---> FEM Robot %s mode\n", RunStr);
		_LOG("--------> %s %s(%d) Started in FM RbLow Func", RunStr, StationStr, Station);
		if (!Find_Chamber_Finger_From_String( StationStr , &EStation , &Station , &Arm , SlotStr , &sp )) {
			if ((!STRCMP_L( RunStr , "READY" ) ) && ( !STRCMP_L( RunStr , "HOME" )) && (!STRCMP_L(RunStr , "INIT"))) {
				printf("---> Robot Control Error - Not Support Station %s\n" , pParam);
				_LOG("--------> Robot Control Error - Not Support Station");
				msRet = SYS_ABORTED;
				break;
			}
			sp = 0;
		}

		if(EStation == AL) nSlot = 1;			// 2013.11.19
		if(STRCMP_L(RunStr,"PLACE")){ PostPlaceEvent(Station,Arm,DB,TRUE); }

		if (sp > 0) WRITE_DIGITAL(MTR5_T_Slot_Pos , sp , &CommStatus);

		/*
		//2014.06.23
		if (Arm == ARM_A) nWaferStatusA = READ_DIGITAL(MTR5_WfrSts , &CommStatus);
		else              nWaferStatusB = READ_DIGITAL(MTR5_WfrStsB, &CommStatus);

		if(DB)
		{
			nWaferStatusA = READ_DIGITAL(MTR5_WfrSts , &CommStatus);
			nWaferStatusB = READ_DIGITAL(MTR5_WfrStsB, &CommStatus);

			if(nWaferStatusA == WAFER_PRESENT && nWaferStatusB == WAFER_PRESENT)
				sprintf(szWaferSts, "Present");
			else 
				sprintf(szWaferSts, "Absent");					
		}
		else
		{
			if (Arm == ARM_A)
			{
				nWaferStatusA = READ_DIGITAL(MTR5_WfrSts , &CommStatus);
				if(nWaferStatusA == WAFER_PRESENT)
					sprintf(szWaferSts, "Present");
				else 
					sprintf(szWaferSts, "Absent");
			}
			else
			{
				nWaferStatusB = READ_DIGITAL(MTR5_WfrStsB , &CommStatus);
				if(nWaferStatusB == WAFER_PRESENT)
					sprintf(szWaferSts, "Present");
				else 
					sprintf(szWaferSts, "Absent");
			}
		}
		//2014.06.23
		*/

		// Driver 에서 Wafer Status 정보를 바로 읽지 못하게 되어 있음... 그냥 쓰자..		
		if(STRCMP_L(RunStr , "PICK"))
		{
			sprintf(szWaferSts, "Absent");
		}
		else if(STRCMP_L(RunStr , "PLACE"))
		{
			sprintf(szWaferSts, "Present");
		}
		else if(STRCMP_L(RunStr , "ROTPIC"))
		{
			sprintf(szWaferSts, "Absent");
		}
		else if(STRCMP_L(RunStr , "ROTPLC"))
		{
			sprintf(szWaferSts, "Present");
		}
		else
		{
			sprintf(szWaferSts, "");
		}


		// 2013.11.15
		LogEvent("ATM", RunStr, "Start", StationStr, nSlot, Arm, szWaferSts, DB);

		if      (STRCMP_L( RunStr , "READY" )) msRet = (PrepCheckRobot( FALSE ) );
		else if (STRCMP_L( RunStr , "HOME"  )) msRet = (HomeRobot() );
		else if (STRCMP_L( RunStr , "INIT"  )) msRet = (InitRobot() );
		else if (STRCMP_L( RunStr , "PICK"  )) {
			if (( EStation == AL) && (!TR)) {
				msRet = (Pick_AL_Robot(Station , Arm, DB ));
				PostPickEvent(Station,Arm,DB);   //2014.08.06
			} else {
				msRet = (PickRobot(EStation , Station , Arm , TR , DB));
				PostPickEvent(Station,Arm,DB);   //2014.08.06
			}
		}
		else if (STRCMP_L( RunStr , "PLACE"   )){
			msRet = PlaceRobot  (EStation , Station , Arm , DB);
			PostPlaceEvent(Station,Arm,DB,FALSE);   //2014.08.06
		} 
		else if (STRCMP_L( RunStr , "ROTPIC"  )) msRet = RotPicRobot (Station , Arm);
		else if (STRCMP_L( RunStr , "ROTPLC"  )) msRet = RotPlcRobot (Station , Arm);
		else {
			printf("---> Robot Control Error - Not Support Code %s\n" , PROGRAM_PARAMETER_READ());
			msRet =  SYS_ABORTED;
		}
	} while(0);

	//2014.06.23
	if (Arm == ARM_A) nWaferStatusA = READ_DIGITAL(MTR5_WfrSts , &CommStatus);
	else              nWaferStatusB = READ_DIGITAL(MTR5_WfrStsB, &CommStatus);

	/*
	if(DB)
	{
		nWaferStatusA = READ_DIGITAL(MTR5_WfrSts , &CommStatus);
		nWaferStatusB = READ_DIGITAL(MTR5_WfrStsB, &CommStatus);
		
		if(nWaferStatusA == WAFER_PRESENT && nWaferStatusB == WAFER_PRESENT)
			sprintf(szWaferSts, "Present");
		else 
			sprintf(szWaferSts, "Absent");					
	}
	else
	{
		if (Arm == ARM_A)
		{
			nWaferStatusA = READ_DIGITAL(MTR5_WfrSts , &CommStatus);
			if(nWaferStatusA == WAFER_PRESENT)
				sprintf(szWaferSts, "Present");
			else 
				sprintf(szWaferSts, "Absent");
		}
		else
		{
			nWaferStatusB = READ_DIGITAL(MTR5_WfrStsB , &CommStatus);
			if(nWaferStatusB == WAFER_PRESENT)
				sprintf(szWaferSts, "Present");
			else 
				sprintf(szWaferSts, "Absent");
		}
	}
	//2014.06.23
	*/

	// Driver 에서 Wafer Status 정보를 바로 읽지 못하게 되어 있음... 그냥 쓰자..		
	if(STRCMP_L(RunStr , "PICK"))
	{
		sprintf(szWaferSts, "Present");
	}
	else if(STRCMP_L(RunStr , "PLACE"))
	{
		sprintf(szWaferSts, "Absent");
	}
	else if(STRCMP_L(RunStr , "ROTPIC"))
	{
		sprintf(szWaferSts, "Present");
	}
	else if(STRCMP_L(RunStr , "ROTPLC"))
	{
		sprintf(szWaferSts, "Absent");
	}
	else
	{
		sprintf(szWaferSts, "");
	}


	// 2013.11.15
	LogEvent("ATM", RunStr, "End", StationStr, nSlot, Arm, szWaferSts, DB);
	
	_LOG("--------> %s %s(%d) Finished in FM RbLow Func", RunStr, StationStr, Station);

	return msRet;
}

//==========================================================================================
extern "C"
{
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

Module_Status Program_Main() {

	int nIOStatus;

	//... 2017.03.07
	if(All_Enable != READ_DIGITAL(INTLKS_CONTROL,&nIOStatus))
	{
		WRITE_DIGITAL(INTLKS_CONTROL,All_Enable,&nIOStatus);
	}

	// 2005.12.20
	if(CheckAbortControl()) return SYS_ABORTED;
	g_nRobotStatus = RB_IDLE;

	if ( SIMULATION_MODE ) return Simulation_Main();
	else return Real_Main();
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Program_Enter_Code() {
	int i;
	char TempStr[256] , TempStr2[256] , TempStr3[256];
	//---------------------------------------------------
	REGIST_FILE_INFO( "Brooks Robot(VTR/MTR/MAG) Control Program(2001.09.01)" );
	//---------------------------------------------------
	if ( strlen( PROGRAM_ARGUMENT_READ() ) <= 0 ) return;
	strcpy( TempStr , PROGRAM_ARGUMENT_READ() );
	for ( i = 0 ; i < 22 ; i++ ) {
		STR_SEPERATE_CHAR( TempStr , '|' , TempStr2 , TempStr3 , 255 );
		MODULE_CONTROL_STATION[i] = atoi( TempStr2 );
		strcpy( TempStr , TempStr3 );
	}
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//
BOOL Program_Init_Code() {
	int CommStatus;
	if ( READ_DIGITAL( IO_Driver_Loding , &CommStatus ) == 0 )	SIMULATION_MODE = TRUE;
	else														SIMULATION_MODE = FALSE;
	// 2005.12.20
	g_DynIO.InitIOList(g_pIOList, false);
	return TRUE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void Screen_Nav_Info( int no ) {
	if(no == -1)
	{
		_gCLOSE_LOG();
	}
}

} // End of [extern "C"]
//==========================================================================================
