#ifndef Prg_Param__H
#define Prg_Param__H

#define MFC_BUF_LENGTH		20

#define MAX_VAL_STEP_NO		10
#define MAX_VAL_TIME_STEP	23
#define MAX_VAL_NO			112
#define MAX_HEATER_NO		48

#define VAL_H_MAX			21				// 화면에 나타나는 벨브의 총 수
#define VAL_V_MAX			8				// 화면에 나타나는 벨브의 총 Time Step수

#define	VAL_STATUS_MAX		10
#define MAX_VAL_NAME_LENGTH	8				// 4 - 1 : 3자 임 (1자는 NULL String );
#define NO_OF_RECIPE_TYPE	9				// Recipe Type 갯수 + 1 - File List를 ComboBox에 추가할때 사용

// 2004.09.09 by cskim
extern char g_szModule[10];

#define	INI_PATH			"Function/Function_Module_Recipe_Editor"
#define INI_FILE_NAME		"RecipeConfig.ini"
#define GDF_PATH			"Function/Function_Module_Recipe_Editor"
#define GDF_FILE			"bitmap.grf"

#define HEATER_BEGIN		"HEATER_RECIPE_CONFIG_BEGIN"
#define HEATER_END			"HEATER_RECIPE_CONFIG_END"
#define MFC_BEGIN			"MFC_RECIPE_CONFIG_BEGIN"
#define MFC_END				"MFC_RECIPE_CONFIG_END"
#define VALVE_BEGIN			"VALVE_RECIPE_CONFIG_BEGIN"
#define VALVE_END			"VALVE_RECIPE_CONFIG_END"

#define EXT_CFG_BEGIN		"EXT_CONFIG_BEGIN"
#define EXT_CFG_END			"EXT_CONFIG_END"

#define GDF_CONFIG_BEGIN	"$CONFIGURATION_START$"
#define GDF_CONFIG_END		"$CONFIGURATION_END$"
#define GDF_LOCATION_BEGIN	"$BITMAP_LOCATION_START$"
#define GDF_LOCATION_END	"$BITMAP_LOCATION_END$"


#define RF_ON_COLOR			RGB( 255 ,   0 ,   0 )
#define RF_OFF_COLOR		RGB(  99 ,   0 ,   0 )
#define VAL_NOUSE_COLOR		RGB( 206 , 207 , 206 )
#define VAL_ON_COLOR		RGB(   0 , 255 ,   0 )
#define VAL_OFF_COLOR		RGB(   0 ,   0 , 255 )
#define GUIDE_LINE_COLOR	RGB( 255,	 0 ,   0 )

#define INCREASE_MFC_NO		g_MFCNo++
#define INCREASE_HEATER_NO	g_HeaterNo++
#endif